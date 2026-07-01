#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "math/math_types.h"

namespace spark {

class Device;
class Buffer;

// Bloom 设置
struct BloomSettings {
    bool enabled = true;
    float threshold = 1.0f;
    float intensity = 0.04f;
    int iterations = 5;
    float softKnee = 0.5f;
};

// Bloom 系统
class BloomSystem {
public:
    BloomSystem(Device& device, uint32_t width, uint32_t height);
    ~BloomSystem();

    void recreate(uint32_t width, uint32_t height);

    // 渲染 Bloom
    void render(VkCommandBuffer commandBuffer, VkImageView hdrInput);

    // 设置
    void setSettings(const BloomSettings& settings) { m_settings = settings; }
    const BloomSettings& getSettings() const { return m_settings; }

    // 获取输出
    VkImageView getOutputView() const { return m_outputView; }
    VkSampler getSampler() const { return m_sampler; }

private:
    void createResources();
    void cleanupResources();
    void createMipChain();

    Device& m_device;
    uint32_t m_width, m_height;

    BloomSettings m_settings;

    // 输出纹理
    VkImage m_outputImage = VK_NULL_HANDLE;
    VkDeviceMemory m_outputMemory = VK_NULL_HANDLE;
    VkImageView m_outputView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;

    // Mip 链（用于多级模糊）
    struct MipLevel {
        VkImage image;
        VkDeviceMemory memory;
        VkImageView view;
    };
    std::vector<MipLevel> m_mipChain;
    int m_mipLevels = 5;
};

} // namespace spark
