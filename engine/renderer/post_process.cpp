#include "post_process.h"
#include "buffer.h"
#include "vulkan/device.h"
#include "core/log.h"

namespace spark {

PostProcessSystem::PostProcessSystem(Device& device, uint32_t width, uint32_t height)
    : m_device(device), m_width(width), m_height(height) {

    createResources();
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

    cleanupResources();
    createResources();
}

void PostProcessSystem::createResources() {
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

    // 创建 UBO
    m_uboBuffer = std::make_unique<Buffer>(m_device, sizeof(PostProcessUBO),
                                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_uboBuffer->map();

    updateUBO();

    SPARK_CORE_INFO("Post-process resources created.");
}

void PostProcessSystem::cleanupResources() {
    vkDestroySampler(m_device.getDevice(), m_sampler, nullptr);
    vkDestroyImageView(m_device.getDevice(), m_outputView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_outputImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_outputMemory, nullptr);
}

void PostProcessSystem::updateUBO() {
    PostProcessUBO ubo{};

    // Bloom
    ubo.bloomThreshold = m_settings.bloom.threshold;
    ubo.bloomIntensity = m_settings.bloom.intensity;
    ubo.bloomIterations = m_settings.bloom.iterations;

    // SSAO
    ubo.ssaoRadius = m_settings.ssao.radius;
    ubo.ssaoBias = m_settings.ssao.bias;
    ubo.ssaoPower = m_settings.ssao.power;
    ubo.ssaoEnabled = m_settings.ssao.enabled ? 1 : 0;

    // 色彩校正
    ubo.exposure = m_settings.colorGrading.exposure;
    ubo.gamma = m_settings.colorGrading.gamma;
    ubo.saturation = m_settings.colorGrading.saturation;
    ubo.contrast = m_settings.colorGrading.contrast;
    ubo.brightness = m_settings.colorGrading.brightness;
    ubo.toneMappingMode = static_cast<int>(m_settings.colorGrading.toneMappingMode);

    // 暗角
    ubo.vignetteIntensity = m_settings.vignetteIntensity;
    ubo.vignetteSmoothness = m_settings.vignetteSmoothness;
    ubo.vignetteEnabled = m_settings.vignetteEnabled ? 1 : 0;

    // FXAA
    ubo.fxaaEnabled = m_settings.fxaaEnabled ? 1 : 0;

    m_uboBuffer->copyTo(&ubo, sizeof(PostProcessUBO));
}

} // namespace spark
