#include "deferred_renderer.h"
#include "renderer/vulkan/device.h"
#include "core/log.h"
#include <array>
#include <stdexcept>

namespace spark {

DeferredRenderer::DeferredRenderer(Device& device, uint32_t width, uint32_t height)
    : m_device(device), m_extent({width, height}) {

    createGBuffer();
    createColorOutput();
    createSampler();
    createRenderPasses();
    createFramebuffers();

    SPARK_CORE_INFO("Deferred renderer initialized: {0}x{1}", width, height);
}

DeferredRenderer::~DeferredRenderer() {
    vkDeviceWaitIdle(m_device.getDevice());

    vkDestroyFramebuffer(m_device.getDevice(), m_geometryFramebuffer, nullptr);
    vkDestroyFramebuffer(m_device.getDevice(), m_lightingFramebuffer, nullptr);
    vkDestroyRenderPass(m_device.getDevice(), m_geometryPass, nullptr);
    vkDestroyRenderPass(m_device.getDevice(), m_lightingPass, nullptr);
    vkDestroySampler(m_device.getDevice(), m_sampler, nullptr);

    cleanupGBuffer();

    vkDestroyImageView(m_device.getDevice(), m_colorOutputView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_colorOutputImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_colorOutputMemory, nullptr);

    SPARK_CORE_INFO("Deferred renderer destroyed.");
}

void DeferredRenderer::recreate(uint32_t width, uint32_t height) {
    vkDeviceWaitIdle(m_device.getDevice());
    m_extent = {width, height};

    vkDestroyFramebuffer(m_device.getDevice(), m_geometryFramebuffer, nullptr);
    vkDestroyFramebuffer(m_device.getDevice(), m_lightingFramebuffer, nullptr);

    cleanupGBuffer();

    vkDestroyImageView(m_device.getDevice(), m_colorOutputView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_colorOutputImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_colorOutputMemory, nullptr);

    createGBuffer();
    createColorOutput();
    createFramebuffers();
}

void DeferredRenderer::createGBuffer() {
    // Albedo + Metallic (RGBA8)
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_extent.width;
    imageInfo.extent.height = m_extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    vkCreateImage(m_device.getDevice(), &imageInfo, nullptr, &m_gbuffer.albedoImage);

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(m_device.getDevice(), m_gbuffer.albedoImage, &memReq);
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_gbuffer.albedoMemory);
    vkBindImageMemory(m_device.getDevice(), m_gbuffer.albedoImage, m_gbuffer.albedoMemory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_gbuffer.albedoImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;
    vkCreateImageView(m_device.getDevice(), &viewInfo, nullptr, &m_gbuffer.albedoView);

    // Normal + Roughness (RGBA16F)
    imageInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    vkCreateImage(m_device.getDevice(), &imageInfo, nullptr, &m_gbuffer.normalImage);
    vkGetImageMemoryRequirements(m_device.getDevice(), m_gbuffer.normalImage, &memReq);
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_gbuffer.normalMemory);
    vkBindImageMemory(m_device.getDevice(), m_gbuffer.normalImage, m_gbuffer.normalMemory, 0);

    viewInfo.image = m_gbuffer.normalImage;
    viewInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    vkCreateImageView(m_device.getDevice(), &viewInfo, nullptr, &m_gbuffer.normalView);

    // World Position (RGBA16F)
    vkCreateImage(m_device.getDevice(), &imageInfo, nullptr, &m_gbuffer.positionImage);
    vkGetImageMemoryRequirements(m_device.getDevice(), m_gbuffer.positionImage, &memReq);
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_gbuffer.positionMemory);
    vkBindImageMemory(m_device.getDevice(), m_gbuffer.positionImage, m_gbuffer.positionMemory, 0);

    viewInfo.image = m_gbuffer.positionImage;
    vkCreateImageView(m_device.getDevice(), &viewInfo, nullptr, &m_gbuffer.positionView);

    // Depth (D32_SFLOAT)
    imageInfo.format = m_device.findDepthFormat();
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    vkCreateImage(m_device.getDevice(), &imageInfo, nullptr, &m_gbuffer.depthImage);
    vkGetImageMemoryRequirements(m_device.getDevice(), m_gbuffer.depthImage, &memReq);
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_gbuffer.depthMemory);
    vkBindImageMemory(m_device.getDevice(), m_gbuffer.depthImage, m_gbuffer.depthMemory, 0);

    viewInfo.image = m_gbuffer.depthImage;
    viewInfo.format = m_device.findDepthFormat();
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    vkCreateImageView(m_device.getDevice(), &viewInfo, nullptr, &m_gbuffer.depthView);

    SPARK_CORE_INFO("G-Buffer created.");
}

void DeferredRenderer::cleanupGBuffer() {
    vkDestroyImageView(m_device.getDevice(), m_gbuffer.albedoView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_gbuffer.albedoImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_gbuffer.albedoMemory, nullptr);

    vkDestroyImageView(m_device.getDevice(), m_gbuffer.normalView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_gbuffer.normalImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_gbuffer.normalMemory, nullptr);

    vkDestroyImageView(m_device.getDevice(), m_gbuffer.positionView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_gbuffer.positionImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_gbuffer.positionMemory, nullptr);

    vkDestroyImageView(m_device.getDevice(), m_gbuffer.depthView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_gbuffer.depthImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_gbuffer.depthMemory, nullptr);
}

void DeferredRenderer::createColorOutput() {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_extent.width;
    imageInfo.extent.height = m_extent.height;
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

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_colorOutputImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;
    vkCreateImageView(m_device.getDevice(), &viewInfo, nullptr, &m_colorOutputView);
}

void DeferredRenderer::createSampler() {
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

    vkCreateSampler(m_device.getDevice(), &samplerInfo, nullptr, &m_sampler);
}

void DeferredRenderer::createRenderPasses() {
    // 几何通道 (写入 G-Buffer)
    std::array<VkAttachmentDescription, 4> attachments{};

    // Albedo
    attachments[0].format = VK_FORMAT_R8G8B8A8_UNORM;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Normal
    attachments[1].format = VK_FORMAT_R16G16B16A16_SFLOAT;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Position
    attachments[2].format = VK_FORMAT_R16G16B16A16_SFLOAT;
    attachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[2].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Depth
    attachments[3].format = m_device.findDepthFormat();
    attachments[3].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[3].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[3].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[3].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[3].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    std::array<VkAttachmentReference, 3> colorRefs{};
    colorRefs[0] = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    colorRefs[1] = {1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    colorRefs[2] = {2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    VkAttachmentReference depthRef{};
    depthRef.attachment = 3;
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
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    vkCreateRenderPass(m_device.getDevice(), &renderPassInfo, nullptr, &m_geometryPass);

    // 光照通道 (读取 G-Buffer，输出最终颜色)
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
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

    vkCreateRenderPass(m_device.getDevice(), &lightingPassInfo, nullptr, &m_lightingPass);

    SPARK_CORE_INFO("Deferred render passes created.");
}

void DeferredRenderer::createFramebuffers() {
    // 几何通道帧缓冲
    std::array<VkImageView, 4> geometryAttachments = {
        m_gbuffer.albedoView,
        m_gbuffer.normalView,
        m_gbuffer.positionView,
        m_gbuffer.depthView
    };

    VkFramebufferCreateInfo fbInfo{};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.renderPass = m_geometryPass;
    fbInfo.attachmentCount = static_cast<uint32_t>(geometryAttachments.size());
    fbInfo.pAttachments = geometryAttachments.data();
    fbInfo.width = m_extent.width;
    fbInfo.height = m_extent.height;
    fbInfo.layers = 1;

    vkCreateFramebuffer(m_device.getDevice(), &fbInfo, nullptr, &m_geometryFramebuffer);

    // 光照通道帧缓冲
    VkImageView lightingAttachment = m_colorOutputView;
    fbInfo.renderPass = m_lightingPass;
    fbInfo.attachmentCount = 1;
    fbInfo.pAttachments = &lightingAttachment;

    vkCreateFramebuffer(m_device.getDevice(), &fbInfo, nullptr, &m_lightingFramebuffer);

    SPARK_CORE_INFO("Deferred framebuffers created.");
}

} // namespace spark
