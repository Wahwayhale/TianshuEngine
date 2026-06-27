#include "post_process.h"
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

PostProcessSystem::PostProcessSystem(Device& device, VkFormat swapchainFormat, uint32_t width, uint32_t height)
    : m_device(device), m_swapchainFormat(swapchainFormat), m_width(width), m_height(height) {
    m_extent = {width, height};

    createResources();
    createPipelines();
    createDescriptorSets();

    SPARK_CORE_INFO("Post-process system initialized: {0}x{1}", width, height);
}

PostProcessSystem::~PostProcessSystem() {
    vkDeviceWaitIdle(m_device.getDevice());
    cleanupResources();
    SPARK_CORE_INFO("Post-process system destroyed.");
}

void PostProcessSystem::recreate(uint32_t width, uint32_t height) {
    vkDeviceWaitIdle(m_device.getDevice());
    m_width = width;
    m_height = height;
    m_extent = {width, height};

    cleanupResources();
    createResources();
    createDescriptorSets();
}

void PostProcessSystem::createResources() {
    // 创建颜色输出纹理
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_width;
    imageInfo.extent.height = m_height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    vkCreateImage(m_device.getDevice(), &imageInfo, nullptr, &m_colorOutputImage);

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(m_device.getDevice(), m_colorOutputImage, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_colorOutputMemory);
    vkBindImageMemory(m_device.getDevice(), m_colorOutputImage, m_colorOutputMemory, 0);

    // 创建图像视图
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_colorOutputImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;

    vkCreateImageView(m_device.getDevice(), &viewInfo, nullptr, &m_colorOutputView);

    // 创建采样器
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    vkCreateSampler(m_device.getDevice(), &samplerInfo, nullptr, &m_colorSampler);

    // 创建渲染通道
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    vkCreateRenderPass(m_device.getDevice(), &renderPassInfo, nullptr, &m_renderPass);

    // 创建帧缓冲
    VkFramebufferCreateInfo fbInfo{};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.renderPass = m_renderPass;
    fbInfo.attachmentCount = 1;
    fbInfo.pAttachments = &m_colorOutputView;
    fbInfo.width = m_width;
    fbInfo.height = m_height;
    fbInfo.layers = 1;

    vkCreateFramebuffer(m_device.getDevice(), &fbInfo, nullptr, &m_framebuffer);

    // 创建全屏四边形顶点缓冲
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(quadVertices);
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    vkCreateBuffer(m_device.getDevice(), &bufferInfo, nullptr, &m_quadVertexBuffer);

    vkGetBufferMemoryRequirements(m_device.getDevice(), m_quadVertexBuffer, &memReq);
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits,
                                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_quadVertexMemory);
    vkBindBufferMemory(m_device.getDevice(), m_quadVertexBuffer, m_quadVertexMemory, 0);

    void* data;
    vkMapMemory(m_device.getDevice(), m_quadVertexMemory, 0, sizeof(quadVertices), 0, &data);
    memcpy(data, quadVertices, sizeof(quadVertices));
    vkUnmapMemory(m_device.getDevice(), m_quadVertexMemory);

    // 创建 UBO
    m_uboBuffer = std::make_unique<Buffer>(m_device, sizeof(PostProcessUBO),
                                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_uboBuffer->map();

    SPARK_CORE_INFO("Post-process resources created.");
}

void PostProcessSystem::cleanupResources() {
    vkDestroySampler(m_device.getDevice(), m_colorSampler, nullptr);
    vkDestroyImageView(m_device.getDevice(), m_colorOutputView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_colorOutputImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_colorOutputMemory, nullptr);

    vkDestroyFramebuffer(m_device.getDevice(), m_framebuffer, nullptr);
    vkDestroyRenderPass(m_device.getDevice(), m_renderPass, nullptr);

    vkDestroyBuffer(m_device.getDevice(), m_quadVertexBuffer, nullptr);
    vkFreeMemory(m_device.getDevice(), m_quadVertexMemory, nullptr);
}

void PostProcessSystem::createPipelines() {
    // 创建后处理管线
    // 简化实现：实际需要加载着色器并创建完整的图形管线
    SPARK_CORE_INFO("Post-process pipelines created (simplified).");
}

void PostProcessSystem::createDescriptorSets() {
    // 创建描述符集布局
    VkDescriptorSetLayoutBinding samplerBinding{};
    samplerBinding.binding = 0;
    samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerBinding.descriptorCount = 1;
    samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding uboBinding{};
    uboBinding.binding = 1;
    uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboBinding.descriptorCount = 1;
    uboBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {samplerBinding, uboBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    vkCreateDescriptorSetLayout(m_device.getDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout);

    // 创建描述符池
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[0].descriptorCount = m_frameCount;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = m_frameCount;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = m_frameCount;

    vkCreateDescriptorPool(m_device.getDevice(), &poolInfo, nullptr, &m_descriptorPool);

    // 分配描述符集
    std::vector<VkDescriptorSetLayout> layouts(m_frameCount, m_descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = m_frameCount;
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorSets.resize(m_frameCount);
    vkAllocateDescriptorSets(m_device.getDevice(), &allocInfo, m_descriptorSets.data());

    // 更新描述符集
    for (uint32_t i = 0; i < m_frameCount; i++) {
        // 绑定颜色纹理
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_colorOutputView;
        imageInfo.sampler = m_colorSampler;

        VkWriteDescriptorSet imageWrite{};
        imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        imageWrite.dstSet = m_descriptorSets[i];
        imageWrite.dstBinding = 0;
        imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        imageWrite.descriptorCount = 1;
        imageWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_device.getDevice(), 1, &imageWrite, 0, nullptr);

        // 绑定 UBO
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_uboBuffer->getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(PostProcessUBO);

        VkWriteDescriptorSet uboWrite{};
        uboWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uboWrite.dstSet = m_descriptorSets[i];
        uboWrite.dstBinding = 1;
        uboWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboWrite.descriptorCount = 1;
        uboWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(m_device.getDevice(), 1, &uboWrite, 0, nullptr);
    }

    SPARK_CORE_INFO("Post-process descriptor sets created.");
}

void PostProcessSystem::updateUBO(uint32_t frameIndex) {
    PostProcessUBO ubo{};

    // Bloom
    ubo.bloomThreshold = m_settings.bloom.threshold;
    ubo.bloomIntensity = m_settings.bloom.intensity;
    ubo.bloomSoftKnee = m_settings.bloom.softKnee;
    ubo.bloomIterations = m_settings.bloom.iterations;

    // SSAO
    ubo.ssaoRadius = m_settings.ssao.radius;
    ubo.ssaoBias = m_settings.ssao.bias;
    ubo.ssaoPower = m_settings.ssao.power;
    ubo.ssaoKernelSize = m_settings.ssao.kernelSize;

    // SSR
    ubo.ssrMaxDistance = m_settings.ssr.maxDistance;
    ubo.ssrStride = m_settings.ssr.stride;
    ubo.ssrMaxSteps = m_settings.ssr.maxSteps;
    ubo.ssrThickness = m_settings.ssr.thickness;

    // TAA
    ubo.taaJitterScale = m_settings.taa.jitterScale;
    ubo.taaFeedback = m_settings.taa.feedback;

    // 色彩校正
    ubo.exposure = m_settings.colorGrading.exposure;
    ubo.gamma = m_settings.colorGrading.gamma;
    ubo.saturation = m_settings.colorGrading.saturation;
    ubo.contrast = m_settings.colorGrading.contrast;
    ubo.brightness = m_settings.colorGrading.brightness;
    ubo.temperature = m_settings.colorGrading.temperature;
    ubo.tint = m_settings.colorGrading.tint;
    ubo.toneMappingMode = static_cast<int>(m_settings.colorGrading.toneMappingMode);

    // 景深
    ubo.dofFocalDistance = m_settings.focalDistance;
    ubo.dofFocalRange = m_settings.focalRange;
    ubo.dofBlurRadius = m_settings.blurRadius;
    ubo.dofEnabled = m_settings.depthOfFieldEnabled ? 1 : 0;

    // 运动模糊
    ubo.motionBlurStrength = m_settings.motionBlurStrength;
    ubo.motionBlurSamples = m_settings.motionBlurSamples;
    ubo.motionBlurEnabled = m_settings.motionBlurEnabled ? 1 : 0;

    // 暗角
    ubo.vignetteIntensity = m_settings.vignetteIntensity;
    ubo.vignetteSmoothness = m_settings.vignetteSmoothness;
    ubo.vignetteEnabled = m_settings.vignetteEnabled ? 1 : 0;

    // 色差
    ubo.chromaticAberrationIntensity = m_settings.chromaticAberrationIntensity;
    ubo.chromaticAberrationEnabled = m_settings.chromaticAberrationEnabled ? 1 : 0;

    m_uboBuffer->copyTo(&ubo, sizeof(PostProcessUBO));
}

void PostProcessSystem::render(VkCommandBuffer commandBuffer, uint32_t frameIndex) {
    // 更新 UBO
    updateUBO(frameIndex);

    // 绑定后处理管线
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    // 绑定描述符集
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_pipelineLayout, 0, 1, &m_descriptorSets[frameIndex], 0, nullptr);

    // 绑定全屏四边形
    VkBuffer vertexBuffers[] = {m_quadVertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    // 绘制
    vkCmdDraw(commandBuffer, 6, 1, 0, 0);
}

} // namespace spark
