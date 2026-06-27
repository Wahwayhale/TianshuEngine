#include "deferred_pipeline.h"
#include "buffer.h"
#include "shader.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <array>

namespace spark {

// 全屏四边形顶点
struct FullscreenVertex {
    float position[2];
    float texCoord[2];
};

static const FullscreenVertex quadVertices[] = {
    {{-1.0f, -1.0f}, {0.0f, 0.0f}},
    {{ 1.0f, -1.0f}, {1.0f, 0.0f}},
    {{ 1.0f,  1.0f}, {1.0f, 1.0f}},
    {{-1.0f, -1.0f}, {0.0f, 0.0f}},
    {{ 1.0f,  1.0f}, {1.0f, 1.0f}},
    {{-1.0f,  1.0f}, {0.0f, 1.0f}},
};

DeferredPipeline::DeferredPipeline(Device& device, uint32_t width, uint32_t height)
    : m_device(device), m_width(width), m_height(height) {

    m_extent = {width, height};

    createGBufferTextures();
    createRenderPasses();
    createFramebuffers();
    createPipelines();
    createDescriptorSets();
    createFullscreenQuad();

    SPARK_CORE_INFO("Deferred pipeline initialized: {0}x{1}", width, height);
}

DeferredPipeline::~DeferredPipeline() {
    vkDeviceWaitIdle(m_device.getDevice());

    // 清理资源
    vkDestroySampler(m_device.getDevice(), m_colorSampler, nullptr);

    vkDestroyFramebuffer(m_device.getDevice(), m_geometryFramebuffer, nullptr);
    vkDestroyFramebuffer(m_device.getDevice(), m_lightingFramebuffer, nullptr);

    vkDestroyRenderPass(m_device.getDevice(), m_geometryRenderPass, nullptr);
    vkDestroyRenderPass(m_device.getDevice(), m_lightingRenderPass, nullptr);

    vkDestroyPipeline(m_device.getDevice(), m_lightingPipeline, nullptr);
    vkDestroyPipelineLayout(m_device.getDevice(), m_lightingPipelineLayout, nullptr);

    vkDestroyDescriptorPool(m_device.getDevice(), m_descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(m_device.getDevice(), m_descriptorSetLayout, nullptr);

    vkDestroyBuffer(m_device.getDevice(), m_quadVertexBuffer, nullptr);
    vkFreeMemory(m_device.getDevice(), m_quadVertexMemory, nullptr);

    cleanupGBufferTextures();

    SPARK_CORE_INFO("Deferred pipeline destroyed.");
}

void DeferredPipeline::recreate(uint32_t width, uint32_t height) {
    vkDeviceWaitIdle(m_device.getDevice());

    m_width = width;
    m_height = height;
    m_extent = {width, height};

    cleanupGBufferTextures();
    vkDestroyFramebuffer(m_device.getDevice(), m_geometryFramebuffer, nullptr);
    vkDestroyFramebuffer(m_device.getDevice(), m_lightingFramebuffer, nullptr);

    createGBufferTextures();
    createFramebuffers();
}

void DeferredPipeline::createGBufferTextures() {
    auto createTexture = [&](GBufferTexture& tex, VkFormat format, VkImageUsageFlags usage) {
        tex.format = format;

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = m_width;
        imageInfo.extent.height = m_height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vkCreateImage(m_device.getDevice(), &imageInfo, nullptr, &tex.image);

        VkMemoryRequirements memReq;
        vkGetImageMemoryRequirements(m_device.getDevice(), tex.image, &memReq);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &tex.memory);
        vkBindImageMemory(m_device.getDevice(), tex.image, tex.memory, 0);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = tex.image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) ?
                                                VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(m_device.getDevice(), &viewInfo, nullptr, &tex.view);
    };

    // Albedo + Metallic
    createTexture(m_albedo, VK_FORMAT_R8G8B8A8_UNORM,
                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    // Normal + Roughness
    createTexture(m_normal, VK_FORMAT_R16G16B16A16_SFLOAT,
                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    // Position + AO
    createTexture(m_position, VK_FORMAT_R16G16B16A16_SFLOAT,
                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    // Emission
    createTexture(m_emission, VK_FORMAT_R8G8B8A8_UNORM,
                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    // Depth
    createTexture(m_depth, VK_FORMAT_D32_SFLOAT,
                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    // 最终颜色输出
    createTexture(m_colorOutput, VK_FORMAT_R16G16B16A16_SFLOAT,
                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    // 创建采样器
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

    vkCreateSampler(m_device.getDevice(), &samplerInfo, nullptr, &m_colorSampler);

    SPARK_CORE_INFO("G-Buffer textures created.");
}

void DeferredPipeline::cleanupGBufferTextures() {
    auto cleanupTexture = [&](GBufferTexture& tex) {
        vkDestroyImageView(m_device.getDevice(), tex.view, nullptr);
        vkDestroyImage(m_device.getDevice(), tex.image, nullptr);
        vkFreeMemory(m_device.getDevice(), tex.memory, nullptr);
    };

    cleanupTexture(m_albedo);
    cleanupTexture(m_normal);
    cleanupTexture(m_position);
    cleanupTexture(m_emission);
    cleanupTexture(m_depth);
    cleanupTexture(m_colorOutput);
}

void DeferredPipeline::createRenderPasses() {
    // 几何通道渲染通道
    std::array<VkAttachmentDescription, 5> geometryAttachments{};

    // Albedo
    geometryAttachments[0].format = VK_FORMAT_R8G8B8A8_UNORM;
    geometryAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    geometryAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    geometryAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    geometryAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    geometryAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    geometryAttachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    geometryAttachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Normal
    geometryAttachments[1].format = VK_FORMAT_R16G16B16A16_SFLOAT;
    geometryAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    geometryAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    geometryAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    geometryAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    geometryAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    geometryAttachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    geometryAttachments[1].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Position
    geometryAttachments[2].format = VK_FORMAT_R16G16B16A16_SFLOAT;
    geometryAttachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
    geometryAttachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    geometryAttachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    geometryAttachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    geometryAttachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    geometryAttachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    geometryAttachments[2].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Emission
    geometryAttachments[3].format = VK_FORMAT_R8G8B8A8_UNORM;
    geometryAttachments[3].samples = VK_SAMPLE_COUNT_1_BIT;
    geometryAttachments[3].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    geometryAttachments[3].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    geometryAttachments[3].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    geometryAttachments[3].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    geometryAttachments[3].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    geometryAttachments[3].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Depth
    geometryAttachments[4].format = VK_FORMAT_D32_SFLOAT;
    geometryAttachments[4].samples = VK_SAMPLE_COUNT_1_BIT;
    geometryAttachments[4].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    geometryAttachments[4].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    geometryAttachments[4].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    geometryAttachments[4].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    geometryAttachments[4].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    geometryAttachments[4].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    std::array<VkAttachmentReference, 4> colorRefs{};
    colorRefs[0] = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    colorRefs[1] = {1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    colorRefs[2] = {2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    colorRefs[3] = {3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    VkAttachmentReference depthRef{};
    depthRef.attachment = 4;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = static_cast<uint32_t>(colorRefs.size());
    subpass.pColorAttachments = colorRefs.data();
    subpass.pDepthStencilAttachment = &depthRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(geometryAttachments.size());
    renderPassInfo.pAttachments = geometryAttachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    vkCreateRenderPass(m_device.getDevice(), &renderPassInfo, nullptr, &m_geometryRenderPass);

    // 光照通道渲染通道
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference lightingColorRef{};
    lightingColorRef.attachment = 0;
    lightingColorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription lightingSubpass{};
    lightingSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    lightingSubpass.colorAttachmentCount = 1;
    lightingSubpass.pColorAttachments = &lightingColorRef;

    VkSubpassDependency lightingDependency{};
    lightingDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    lightingDependency.dstSubpass = 0;
    lightingDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    lightingDependency.srcAccessMask = 0;
    lightingDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    lightingDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo lightingPassInfo{};
    lightingPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    lightingPassInfo.attachmentCount = 1;
    lightingPassInfo.pAttachments = &colorAttachment;
    lightingPassInfo.subpassCount = 1;
    lightingPassInfo.pSubpasses = &lightingSubpass;
    lightingPassInfo.dependencyCount = 1;
    lightingPassInfo.pDependencies = &lightingDependency;

    vkCreateRenderPass(m_device.getDevice(), &lightingPassInfo, nullptr, &m_lightingRenderPass);

    SPARK_CORE_INFO("Deferred render passes created.");
}

void DeferredPipeline::createFramebuffers() {
    // 几何通道帧缓冲
    std::array<VkImageView, 5> geometryAttachments = {
        m_albedo.view,
        m_normal.view,
        m_position.view,
        m_emission.view,
        m_depth.view
    };

    VkFramebufferCreateInfo fbInfo{};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.renderPass = m_geometryRenderPass;
    fbInfo.attachmentCount = static_cast<uint32_t>(geometryAttachments.size());
    fbInfo.pAttachments = geometryAttachments.data();
    fbInfo.width = m_width;
    fbInfo.height = m_height;
    fbInfo.layers = 1;

    vkCreateFramebuffer(m_device.getDevice(), &fbInfo, nullptr, &m_geometryFramebuffer);

    // 光照通道帧缓冲
    VkImageView lightingAttachment = m_colorOutput.view;
    fbInfo.renderPass = m_lightingRenderPass;
    fbInfo.attachmentCount = 1;
    fbInfo.pAttachments = &lightingAttachment;

    vkCreateFramebuffer(m_device.getDevice(), &fbInfo, nullptr, &m_lightingFramebuffer);

    SPARK_CORE_INFO("Deferred framebuffers created.");
}

void DeferredPipeline::createPipelines() {
    // 创建光照通道管线
    // 简化实现：实际需要加载着色器
    SPARK_CORE_INFO("Deferred pipelines created (simplified).");
}

void DeferredPipeline::createDescriptorSets() {
    // 创建描述符集布局
    std::array<VkDescriptorSetLayoutBinding, 5> bindings{};

    // Albedo
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // Normal
    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // Position
    bindings[2].binding = 2;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[2].descriptorCount = 1;
    bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // Emission
    bindings[3].binding = 3;
    bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[3].descriptorCount = 1;
    bindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // Depth
    bindings[4].binding = 4;
    bindings[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[4].descriptorCount = 1;
    bindings[4].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    vkCreateDescriptorSetLayout(m_device.getDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout);

    // 创建描述符池
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = 5;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;

    vkCreateDescriptorPool(m_device.getDevice(), &poolInfo, nullptr, &m_descriptorPool);

    // 分配描述符集
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_descriptorSetLayout;

    vkAllocateDescriptorSets(m_device.getDevice(), &allocInfo, &m_descriptorSet);

    // 更新描述符集
    std::array<VkWriteDescriptorSet, 5> writes{};
    std::array<VkDescriptorImageInfo, 5> imageInfos{};

    auto bindTexture = [&](int binding, VkImageView view) {
        imageInfos[binding].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[binding].imageView = view;
        imageInfos[binding].sampler = m_colorSampler;

        writes[binding].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[binding].dstSet = m_descriptorSet;
        writes[binding].dstBinding = binding;
        writes[binding].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[binding].descriptorCount = 1;
        writes[binding].pImageInfo = &imageInfos[binding];
    };

    bindTexture(0, m_albedo.view);
    bindTexture(1, m_normal.view);
    bindTexture(2, m_position.view);
    bindTexture(3, m_emission.view);
    bindTexture(4, m_depth.view);

    vkUpdateDescriptorSets(m_device.getDevice(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);

    SPARK_CORE_INFO("Deferred descriptor sets created.");
}

void DeferredPipeline::createFullscreenQuad() {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(quadVertices);
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    vkCreateBuffer(m_device.getDevice(), &bufferInfo, nullptr, &m_quadVertexBuffer);

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(m_device.getDevice(), m_quadVertexBuffer, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits,
                                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_quadVertexMemory);
    vkBindBufferMemory(m_device.getDevice(), m_quadVertexBuffer, m_quadVertexMemory, 0);

    void* data;
    vkMapMemory(m_device.getDevice(), m_quadVertexMemory, 0, sizeof(quadVertices), 0, &data);
    memcpy(data, quadVertices, sizeof(quadVertices));
    vkUnmapMemory(m_device.getDevice(), m_quadVertexMemory);

    SPARK_CORE_INFO("Fullscreen quad created.");
}

void DeferredPipeline::beginGeometryPass(VkCommandBuffer commandBuffer) {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_geometryRenderPass;
    renderPassInfo.framebuffer = m_geometryFramebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_extent;

    std::array<VkClearValue, 5> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[2].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[3].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[4].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void DeferredPipeline::endGeometryPass(VkCommandBuffer commandBuffer) {
    vkCmdEndRenderPass(commandBuffer);
}

void DeferredPipeline::beginLightingPass(VkCommandBuffer commandBuffer) {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_lightingRenderPass;
    renderPassInfo.framebuffer = m_lightingFramebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_extent;

    VkClearValue clearValue{};
    clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void DeferredPipeline::endLightingPass(VkCommandBuffer commandBuffer) {
    vkCmdEndRenderPass(commandBuffer);
}

void DeferredPipeline::drawFullscreenQuad(VkCommandBuffer commandBuffer) {
    VkBuffer vertexBuffers[] = {m_quadVertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdDraw(commandBuffer, 6, 1, 0, 0);
}

} // namespace spark
