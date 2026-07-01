#include "deferred_renderer.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <array>

namespace spark {

DeferredRenderer::DeferredRenderer(Device& device, uint32_t width, uint32_t height)
    : m_device(device), m_width(width), m_height(height) {

    createGBuffer();
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

    cleanupGBuffer();

    SPARK_CORE_INFO("Deferred renderer destroyed.");
}

void DeferredRenderer::recreate(uint32_t width, uint32_t height) {
    vkDeviceWaitIdle(m_device.getDevice());

    m_width = width;
    m_height = height;

    vkDestroyFramebuffer(m_device.getDevice(), m_geometryFramebuffer, nullptr);
    vkDestroyFramebuffer(m_device.getDevice(), m_lightingFramebuffer, nullptr);

    cleanupGBuffer();
    createGBuffer();
    createFramebuffers();
}

void DeferredRenderer::createGBuffer() {
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

    // Depth
    createTexture(m_depth, VK_FORMAT_D32_SFLOAT,
                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    // Color Output
    createTexture(m_colorOutput, VK_FORMAT_R16G16B16A16_SFLOAT,
                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    SPARK_CORE_INFO("G-Buffer created.");
}

void DeferredRenderer::cleanupGBuffer() {
    auto cleanupTexture = [&](GBufferTexture& tex) {
        if (tex.view != VK_NULL_HANDLE) vkDestroyImageView(m_device.getDevice(), tex.view, nullptr);
        if (tex.image != VK_NULL_HANDLE) vkDestroyImage(m_device.getDevice(), tex.image, nullptr);
        if (tex.memory != VK_NULL_HANDLE) vkFreeMemory(m_device.getDevice(), tex.memory, nullptr);
    };

    cleanupTexture(m_albedo);
    cleanupTexture(m_normal);
    cleanupTexture(m_position);
    cleanupTexture(m_depth);
    cleanupTexture(m_colorOutput);
}

void DeferredRenderer::createRenderPasses() {
    // 几何通道
    std::array<VkAttachmentDescription, 4> geometryAttachments{};

    // Albedo
    geometryAttachments[0].format = VK_FORMAT_R8G8B8A8_UNORM;
    geometryAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    geometryAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    geometryAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    geometryAttachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    geometryAttachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Normal
    geometryAttachments[1].format = VK_FORMAT_R16G16B16A16_SFLOAT;
    geometryAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    geometryAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    geometryAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    geometryAttachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    geometryAttachments[1].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Position
    geometryAttachments[2].format = VK_FORMAT_R16G16B16A16_SFLOAT;
    geometryAttachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
    geometryAttachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    geometryAttachments[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    geometryAttachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    geometryAttachments[2].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // Depth
    geometryAttachments[3].format = VK_FORMAT_D32_SFLOAT;
    geometryAttachments[3].samples = VK_SAMPLE_COUNT_1_BIT;
    geometryAttachments[3].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    geometryAttachments[3].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    geometryAttachments[3].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    geometryAttachments[3].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

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

    std::array<VkAttachmentDescription, 4> attachments = geometryAttachments;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    vkCreateRenderPass(m_device.getDevice(), &renderPassInfo, nullptr, &m_geometryPass);

    // 光照通道
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
        m_albedo.view,
        m_normal.view,
        m_position.view,
        m_depth.view
    };

    VkFramebufferCreateInfo fbInfo{};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.renderPass = m_geometryPass;
    fbInfo.attachmentCount = static_cast<uint32_t>(geometryAttachments.size());
    fbInfo.pAttachments = geometryAttachments.data();
    fbInfo.width = m_width;
    fbInfo.height = m_height;
    fbInfo.layers = 1;

    vkCreateFramebuffer(m_device.getDevice(), &fbInfo, nullptr, &m_geometryFramebuffer);

    // 光照通道帧缓冲
    VkImageView lightingAttachment = m_colorOutput.view;
    fbInfo.renderPass = m_lightingPass;
    fbInfo.attachmentCount = 1;
    fbInfo.pAttachments = &lightingAttachment;

    vkCreateFramebuffer(m_device.getDevice(), &fbInfo, nullptr, &m_lightingFramebuffer);

    SPARK_CORE_INFO("Deferred framebuffers created.");
}

} // namespace spark
