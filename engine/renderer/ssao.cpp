#include "ssao.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <random>
#include <cmath>

namespace spark {

SSAOSystem::SSAOSystem(Device& device, uint32_t width, uint32_t height)
    : m_device(device), m_width(width), m_height(height) {

    generateKernel();
    createResources();

    SPARK_CORE_INFO("SSAO system initialized: {0}x{1}", width, height);
}

SSAOSystem::~SSAOSystem() {
    vkDeviceWaitIdle(m_device.getDevice());
    cleanupResources();
    SPARK_CORE_INFO("SSAO system destroyed.");
}

void SSAOSystem::recreate(uint32_t width, uint32_t height) {
    vkDeviceWaitIdle(m_device.getDevice());
    m_width = width;
    m_height = height;

    cleanupResources();
    createResources();
}

void SSAOSystem::render(VkCommandBuffer commandBuffer, VkImageView depthView, VkImageView normalView) {
    if (!m_settings.enabled) return;

    // TODO: 实现 SSAO 渲染
    // 需要 SSAO 着色器
}

void SSAOSystem::createResources() {
    // 创建输出纹理
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_width;
    imageInfo.extent.height = m_height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8_UNORM;
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
    viewInfo.format = VK_FORMAT_R8_UNORM;
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

    vkCreateSampler(m_device.getDevice(), &samplerInfo, nullptr, &m_sampler);

    SPARK_CORE_INFO("SSAO resources created.");
}

void SSAOSystem::cleanupResources() {
    vkDestroySampler(m_device.getDevice(), m_sampler, nullptr);
    vkDestroyImageView(m_device.getDevice(), m_outputView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_outputImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_outputMemory, nullptr);

    if (m_noiseView != VK_NULL_HANDLE) vkDestroyImageView(m_device.getDevice(), m_noiseView, nullptr);
    if (m_noiseImage != VK_NULL_HANDLE) vkDestroyImage(m_device.getDevice(), m_noiseImage, nullptr);
    if (m_noiseMemory != VK_NULL_HANDLE) vkFreeMemory(m_device.getDevice(), m_noiseMemory, nullptr);
}

void SSAOSystem::generateKernel() {
    m_kernel.resize(m_settings.kernelSize);

    std::default_random_engine rng;
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> distPos(0.0f, 1.0f);

    for (int i = 0; i < m_settings.kernelSize; i++) {
        Vec3 sample(dist(rng), dist(rng), distPos(rng));
        sample = glm::normalize(sample);
        sample *= distPos(rng);

        // 聚集在原点附近
        float scale = static_cast<float>(i) / m_settings.kernelSize;
        scale = glm::mix(0.1f, 1.0f, scale * scale);
        sample *= scale;

        m_kernel[i] = Vec4(sample, 0.0f);
    }
}

} // namespace spark
