#include "global_illumination.h"
#include "renderer/vulkan/device.h"
#include "core/log.h"

namespace spark {

GlobalIllumination::GlobalIllumination(Device& device, uint32_t width, uint32_t height)
    : m_device(device), m_width(width), m_height(height) {

    createResources();
    createPipelines();

    SPARK_CORE_INFO("Global illumination initialized: {0}x{1}", width, height);
}

GlobalIllumination::~GlobalIllumination() {
    vkDeviceWaitIdle(m_device.getDevice());
    cleanupResources();
    SPARK_CORE_INFO("Global illumination destroyed.");
}

void GlobalIllumination::recreate(uint32_t width, uint32_t height) {
    vkDeviceWaitIdle(m_device.getDevice());
    m_width = width;
    m_height = height;

    cleanupResources();
    createResources();
}

void GlobalIllumination::createResources() {
    // 创建输出纹理
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

    vkCreateImage(m_device.getDevice(), &imageInfo, nullptr, &m_outputImage);

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(m_device.getDevice(), m_outputImage, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_outputMemory);
    vkBindImageMemory(m_device.getDevice(), m_outputImage, m_outputMemory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_outputImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;

    vkCreateImageView(m_device.getDevice(), &viewInfo, nullptr, &m_outputView);

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

    vkCreateSampler(m_device.getDevice(), &samplerInfo, nullptr, &m_sampler);

    SPARK_CORE_INFO("GI resources created.");
}

void GlobalIllumination::cleanupResources() {
    vkDestroySampler(m_device.getDevice(), m_sampler, nullptr);
    vkDestroyImageView(m_device.getDevice(), m_outputView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_outputImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_outputMemory, nullptr);
}

void GlobalIllumination::createPipelines() {
    // 创建 GI 管线
    // 简化实现：实际需要加载着色器
    SPARK_CORE_INFO("GI pipelines created (simplified).");
}

void GlobalIllumination::render(VkCommandBuffer commandBuffer,
                                 VkImageView depthView,
                                 VkImageView normalView,
                                 VkImageView albedoView) {
    if (!m_settings.enabled) return;

    // 渲染全局光照
    // 简化实现：实际需要完整的 SSGI 管线
}

} // namespace spark
