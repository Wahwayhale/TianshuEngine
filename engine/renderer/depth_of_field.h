#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "math/math_types.h"

namespace spark {

class Device;
class Buffer;

// 景深设置
struct DepthOfFieldSettings {
    bool enabled = true;
    float focalDistance = 10.0f;
    float focalRange = 5.0f;
    float blurRadius = 5.0f;
    int samples = 16;
};

// 景深系统
class DepthOfFieldSystem {
public:
    DepthOfFieldSystem(Device& device, uint32_t width, uint32_t height);
    ~DepthOfFieldSystem();

    void recreate(uint32_t width, uint32_t height);

    // 渲染景深
    void render(VkCommandBuffer commandBuffer, VkImageView colorView, VkImageView depthView);

    // 设置
    void setSettings(const DepthOfFieldSettings& settings) { m_settings = settings; }
    const DepthOfFieldSettings& getSettings() const { return m_settings; }

    // 获取输出
    VkImageView getOutputView() const { return m_outputView; }
    VkSampler getSampler() const { return m_sampler; }

private:
    void createResources();
    void cleanupResources();

    Device& m_device;
    uint32_t m_width, m_height;

    DepthOfFieldSettings m_settings;

    VkImage m_outputImage = VK_NULL_HANDLE;
    VkDeviceMemory m_outputMemory = VK_NULL_HANDLE;
    VkImageView m_outputView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;
};

} // namespace spark
