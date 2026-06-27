#include "post_process.h"
#include "buffer.h"
#include "shader.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <stdexcept>
#include <array>
#include <cstring>

namespace spark {

// 全屏四边形顶点（两个三角形覆盖整个屏幕）
struct FullscreenVertex {
    float position[2];
    float texCoord[2];
};

static const FullscreenVertex fullscreenQuadVertices[] = {
    {{-1.0f, -1.0f}, {0.0f, 0.0f}},
    {{ 1.0f, -1.0f}, {1.0f, 0.0f}},
    {{ 1.0f,  1.0f}, {1.0f, 1.0f}},
    {{-1.0f, -1.0f}, {0.0f, 0.0f}},
    {{ 1.0f,  1.0f}, {1.0f, 1.0f}},
    {{-1.0f,  1.0f}, {0.0f, 1.0f}},
};

PostProcess::PostProcess(Device& device, VkFormat swapchainFormat, uint32_t width, uint32_t height)
    : m_device(device), m_swapchainFormat(swapchainFormat), m_width(width), m_height(height) {
    m_extent = {width, height};

    createOffscreenResources();
    createRenderPass();
    createFramebuffer();
    createDescriptorSetLayout();
    createDescriptorPool();
    createDescriptorSets();
    createPipeline(swapchainFormat);
    createFullscreenQuad();
    createUniformBuffer();

    SPARK_CORE_INFO("Post-processing system initialized: {0}x{1}", width, height);
}

PostProcess::~PostProcess() {
    vkDeviceWaitIdle(m_device.getDevice());

    if (m_uboBuffer) {
        if (m_uboMapped) {
            m_uboBuffer->unmap();
        }
        m_uboBuffer.reset();
    }

    if (m_quadVertexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_device.getDevice(), m_quadVertexBuffer, nullptr);
        vkFreeMemory(m_device.getDevice(), m_quadVertexMemory, nullptr);
    }

    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device.getDevice(), m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
    }

    if (m_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_device.getDevice(), m_descriptorPool, nullptr);
    }
    if (m_descriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_device.getDevice(), m_descriptorSetLayout, nullptr);
    }

    cleanupOffscreenResources();

    SPARK_CORE_INFO("Post-processing system destroyed.");
}

void PostProcess::recreate(uint32_t width, uint32_t height) {
    vkDeviceWaitIdle(m_device.getDevice());
    m_width = width;
    m_height = height;
    m_extent = {width, height};

    cleanupOffscreenResources();
    createOffscreenResources();

    // 重建帧缓冲
    vkDestroyFramebuffer(m_device.getDevice(), m_framebuffer, nullptr);
    createFramebuffer();

    // 重建描述符集
    vkDestroyDescriptorPool(m_device.getDevice(), m_descriptorPool, nullptr);
    createDescriptorPool();
    createDescriptorSets();
}

void PostProcess::createOffscreenResources() {
    // 颜色图像
    VkImageCreateInfo colorImageInfo{};
    colorImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    colorImageInfo.imageType = VK_IMAGE_TYPE_2D;
    colorImageInfo.extent.width = m_width;
    colorImageInfo.extent.height = m_height;
    colorImageInfo.extent.depth = 1;
    colorImageInfo.mipLevels = 1;
    colorImageInfo.arrayLayers = 1;
    colorImageInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;  // HDR 格式
    colorImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    colorImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorImageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    colorImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    colorImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(m_device.getDevice(), &colorImageInfo, nullptr, &m_colorImage) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create offscreen color image!");
    }

    VkMemoryRequirements colorMemReq;
    vkGetImageMemoryRequirements(m_device.getDevice(), m_colorImage, &colorMemReq);

    VkMemoryAllocateInfo colorAllocInfo{};
    colorAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    colorAllocInfo.allocationSize = colorMemReq.size;
    colorAllocInfo.memoryTypeIndex = m_device.findMemoryType(colorMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(m_device.getDevice(), &colorAllocInfo, nullptr, &m_colorMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate offscreen color memory!");
    }
    vkBindImageMemory(m_device.getDevice(), m_colorImage, m_colorMemory, 0);

    // 颜色图像视图
    VkImageViewCreateInfo colorViewInfo{};
    colorViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    colorViewInfo.image = m_colorImage;
    colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorViewInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    colorViewInfo.subresourceRange.baseMipLevel = 0;
    colorViewInfo.subresourceRange.levelCount = 1;
    colorViewInfo.subresourceRange.baseArrayLayer = 0;
    colorViewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_device.getDevice(), &colorViewInfo, nullptr, &m_colorImageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create offscreen color image view!");
    }

    // 颜色采样器
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

    if (vkCreateSampler(m_device.getDevice(), &samplerInfo, nullptr, &m_colorSampler) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create offscreen color sampler!");
    }

    // 深度图像
    VkImageCreateInfo depthImageInfo{};
    depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
    depthImageInfo.extent.width = m_width;
    depthImageInfo.extent.height = m_height;
    depthImageInfo.extent.depth = 1;
    depthImageInfo.mipLevels = 1;
    depthImageInfo.arrayLayers = 1;
    depthImageInfo.format = m_device.findDepthFormat();
    depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    if (vkCreateImage(m_device.getDevice(), &depthImageInfo, nullptr, &m_depthImage) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create offscreen depth image!");
    }

    VkMemoryRequirements depthMemReq;
    vkGetImageMemoryRequirements(m_device.getDevice(), m_depthImage, &depthMemReq);

    VkMemoryAllocateInfo depthAllocInfo{};
    depthAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    depthAllocInfo.allocationSize = depthMemReq.size;
    depthAllocInfo.memoryTypeIndex = m_device.findMemoryType(depthMemReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(m_device.getDevice(), &depthAllocInfo, nullptr, &m_depthMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate offscreen depth memory!");
    }
    vkBindImageMemory(m_device.getDevice(), m_depthImage, m_depthMemory, 0);

    VkImageViewCreateInfo depthViewInfo{};
    depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depthViewInfo.image = m_depthImage;
    depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthViewInfo.format = m_device.findDepthFormat();
    depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    depthViewInfo.subresourceRange.baseMipLevel = 0;
    depthViewInfo.subresourceRange.levelCount = 1;
    depthViewInfo.subresourceRange.baseArrayLayer = 0;
    depthViewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_device.getDevice(), &depthViewInfo, nullptr, &m_depthImageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create offscreen depth image view!");
    }

    SPARK_CORE_INFO("Offscreen resources created.");
}

void PostProcess::cleanupOffscreenResources() {
    vkDestroySampler(m_device.getDevice(), m_colorSampler, nullptr);
    vkDestroyImageView(m_device.getDevice(), m_colorImageView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_colorImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_colorMemory, nullptr);

    vkDestroyImageView(m_device.getDevice(), m_depthImageView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_depthImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_depthMemory, nullptr);

    if (m_framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(m_device.getDevice(), m_framebuffer, nullptr);
        m_framebuffer = VK_NULL_HANDLE;
    }
}

void PostProcess::createRenderPass() {
    // 颜色附件
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // 深度附件
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = m_device.findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthRef{};
    depthRef.attachment = 1;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;
    subpass.pDepthStencilAttachment = &depthRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_device.getDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create offscreen render pass!");
    }

    SPARK_CORE_INFO("Offscreen render pass created.");
}

void PostProcess::createFramebuffer() {
    std::array<VkImageView, 2> attachments = {m_colorImageView, m_depthImageView};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = m_width;
    framebufferInfo.height = m_height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_device.getDevice(), &framebufferInfo, nullptr, &m_framebuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create offscreen framebuffer!");
    }
}

void PostProcess::createDescriptorSetLayout() {
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

    if (vkCreateDescriptorSetLayout(m_device.getDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create post-process descriptor set layout!");
    }
}

void PostProcess::createDescriptorPool() {
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

    if (vkCreateDescriptorPool(m_device.getDevice(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create post-process descriptor pool!");
    }
}

void PostProcess::createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(m_frameCount, m_descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = m_frameCount;
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorSets.resize(m_frameCount);
    if (vkAllocateDescriptorSets(m_device.getDevice(), &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate post-process descriptor sets!");
    }

    // 绑定离屏颜色图像
    for (uint32_t i = 0; i < m_frameCount; i++) {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_colorImageView;
        imageInfo.sampler = m_colorSampler;

        VkWriteDescriptorSet imageWrite{};
        imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        imageWrite.dstSet = m_descriptorSets[i];
        imageWrite.dstBinding = 0;
        imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        imageWrite.descriptorCount = 1;
        imageWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_device.getDevice(), 1, &imageWrite, 0, nullptr);
    }
}

void PostProcess::createPipeline(VkFormat swapchainFormat) {
    // 加载着色器
    Shader vertShader(m_device, "shaders/fullscreen_vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    Shader fragShader(m_device, "shaders/post_process_frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    VkPipelineShaderStageCreateInfo shaderStages[2] = {};

    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = vertShader.getModule();
    shaderStages[0].pName = "main";

    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = fragShader.getModule();
    shaderStages[1].pName = "main";

    // 顶点输入（全屏四边形）
    VkVertexInputBindingDescription bindingDesc{};
    bindingDesc.binding = 0;
    bindingDesc.stride = sizeof(FullscreenVertex);
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::array<VkVertexInputAttributeDescription, 2> attrDescs{};
    attrDescs[0].binding = 0;
    attrDescs[0].location = 0;
    attrDescs[0].format = VK_FORMAT_R32G32_SFLOAT;
    attrDescs[0].offset = offsetof(FullscreenVertex, position);

    attrDescs[1].binding = 0;
    attrDescs[1].location = 1;
    attrDescs[1].format = VK_FORMAT_R32G32_SFLOAT;
    attrDescs[1].offset = offsetof(FullscreenVertex, texCoord);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attrDescs.size());
    vertexInputInfo.pVertexAttributeDescriptions = attrDescs.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(m_device.getDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create post-process pipeline layout!");
    }

    // 使用 swapchain 格式作为输出
    VkPipelineRenderingCreateInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachmentFormats = &swapchainFormat;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = VK_NULL_HANDLE;  // 使用 dynamic rendering
    pipelineInfo.pNext = &renderingInfo;

    if (vkCreateGraphicsPipelines(m_device.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create post-process pipeline!");
    }

    SPARK_CORE_INFO("Post-process pipeline created.");
}

void PostProcess::createFullscreenQuad() {
    VkDeviceSize bufferSize = sizeof(fullscreenQuadVertices);

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device.getDevice(), &bufferInfo, nullptr, &m_quadVertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create fullscreen quad vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device.getDevice(), m_quadVertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memRequirements.memoryTypeBits,
                                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_quadVertexMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate fullscreen quad vertex memory!");
    }

    vkBindBufferMemory(m_device.getDevice(), m_quadVertexBuffer, m_quadVertexMemory, 0);

    void* data;
    vkMapMemory(m_device.getDevice(), m_quadVertexMemory, 0, bufferSize, 0, &data);
    memcpy(data, fullscreenQuadVertices, bufferSize);
    vkUnmapMemory(m_device.getDevice(), m_quadVertexMemory);
}

void PostProcess::createUniformBuffer() {
    VkDeviceSize bufferSize = sizeof(PostProcessUBO);

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer;
    VkDeviceMemory memory;

    if (vkCreateBuffer(m_device.getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create post-process UBO!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device.getDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memRequirements.memoryTypeBits,
                                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &memory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate post-process UBO memory!");
    }

    vkBindBufferMemory(m_device.getDevice(), buffer, memory, 0);
    vkMapMemory(m_device.getDevice(), memory, 0, bufferSize, 0, &m_uboMapped);

    // 创建 Buffer 对象（简化处理，直接使用原始句柄）
    m_uboBuffer = std::make_unique<Buffer>(m_device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_uboBuffer->map();

    // 绑定 UBO 到描述符集
    for (uint32_t i = 0; i < m_frameCount; i++) {
        VkDescriptorBufferInfo bufferDescInfo{};
        bufferDescInfo.buffer = m_uboBuffer->getBuffer();
        bufferDescInfo.offset = 0;
        bufferDescInfo.range = sizeof(PostProcessUBO);

        VkWriteDescriptorSet uboWrite{};
        uboWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uboWrite.dstSet = m_descriptorSets[i];
        uboWrite.dstBinding = 1;
        uboWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboWrite.descriptorCount = 1;
        uboWrite.pBufferInfo = &bufferDescInfo;

        vkUpdateDescriptorSets(m_device.getDevice(), 1, &uboWrite, 0, nullptr);
    }

    // 设置默认值
    PostProcessSettings defaultSettings;
    updateSettings(0, defaultSettings);
    updateSettings(1, defaultSettings);
}

void PostProcess::updateSettings(uint32_t frameIndex, const PostProcessSettings& settings) {
    PostProcessUBO ubo{};
    ubo.exposure = settings.exposure;
    ubo.bloomThreshold = settings.bloomThreshold;
    ubo.bloomIntensity = settings.bloomIntensity;
    ubo.gamma = settings.gamma;
    ubo.saturation = settings.saturation;
    ubo.contrast = settings.contrast;
    ubo.brightness = settings.brightness;

    m_uboBuffer->copyTo(&ubo, sizeof(PostProcessUBO));
}

void PostProcess::setSwapchainImageView(VkImageView imageView, uint32_t imageIndex) {
    // 重建描述符集绑定新的交换链图像视图
    // 注意：这里简化处理，实际应该为每个交换链图像创建独立的帧缓冲
}

void PostProcess::draw(VkCommandBuffer commandBuffer, uint32_t frameIndex) {
    // 绑定后处理管线
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    // 绑定描述符集
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_pipelineLayout, 0, 1, &m_descriptorSets[frameIndex], 0, nullptr);

    // 绑定全屏四边形顶点缓冲
    VkBuffer vertexBuffers[] = {m_quadVertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    // 绘制全屏四边形
    vkCmdDraw(commandBuffer, 6, 1, 0, 0);
}

} // namespace spark
