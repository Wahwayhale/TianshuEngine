#include "bloom.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <cmath>

namespace spark {

BloomSystem::BloomSystem(Device& device, uint32_t width, uint32_t height)
    : m_device(device), m_width(width), m_height(height) {

    createResources();
    createMipChain();

    SPARK_CORE_INFO("Bloom system initialized: {0}x{1}", width, height);
}

BloomSystem::~BloomSystem() {
    vkDeviceWaitIdle(m_device.getDevice());
    cleanupResources();
    SPARK_CORE_INFO("Bloom system destroyed.");
}

void BloomSystem::recreate(uint32_t width, uint32_t height) {
    vkDeviceWaitIdle(m_device.getDevice());
    m_width = width;
    m_height = height;

    cleanupResources();
    createResources();
    createMipChain();
}

void BloomSystem::render(VkCommandBuffer commandBuffer, VkImageView hdrInput) {
    if (!m_settings.enabled) return;

    // TODO: 实现 Bloom 渲染
    // 1. 提取高亮区域
    // 2. 多级高斯模糊
    // 3. 合成
}

void BloomSystem::createResources() {
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

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

    vkCreateSampler(m_device.getDevice(), &samplerInfo, nullptr, &m_sampler);

    SPARK_CORE_INFO("Bloom resources created.");
}

void BloomSystem::cleanupResources() {
    vkDestroySampler(m_device.getDevice(), m_sampler, nullptr);
    vkDestroyImageView(m_device.getDevice(), m_outputView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_outputImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_outputMemory, nullptr);

    for (auto& mip : m_mipChain) {
        if (mip.view != VK_NULL_HANDLE) vkDestroyImageView(m_device.getDevice(), mip.view, nullptr);
        if (mip.image != VK_NULL_HANDLE) vkDestroyImage(m_device.getDevice(), mip.image, nullptr);
        if (mip.memory != VK_NULL_HANDLE) vkFreeMemory(m_device.getDevice(), mip.memory, nullptr);
    }
    m_mipChain.clear();
}

void BloomSystem::createMipChain() {
    m_mipChain.resize(m_mipLevels);

    uint32_t mipWidth = m_width / 2;
    uint32_t mipHeight = m_height / 2;

    for (int i = 0; i < m_mipLevels; i++) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = mipWidth;
        imageInfo.extent.height = mipHeight;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

        vkCreateImage(m_device.getDevice(), &imageInfo, nullptr, &m_mipChain[i].image);

        VkMemoryRequirements memReq;
        vkGetImageMemoryRequirements(m_device.getDevice(), m_mipChain[i].image, &memReq);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_mipChain[i].memory);
        vkBindImageMemory(m_device.getDevice(), m_mipChain[i].image, m_mipChain[i].memory, 0);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_mipChain[i].image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(m_device.getDevice(), &viewInfo, nullptr, &m_mipChain[i].view);

        mipWidth /= 2;
        mipHeight /= 2;
    }

    SPARK_CORE_INFO("Bloom mip chain created: {0} levels", m_mipLevels);
}

} // namespace spark
