#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "math/math_types.h"

namespace spark {

class Device;
class Buffer;

// 运动模糊设置
struct MotionBlurSettings {
    bool enabled = true;
    float strength = 0.5f;
    int samples = 8;
    float maxVelocity = 50.0f;
};

// 运动模糊系统
class MotionBlurSystem {
public:
    MotionBlurSystem(Device& device, uint32_t width, uint32_t height);
    ~MotionBlurSystem();

    void recreate(uint32_t width, uint32_t height);

    // 渲染运动模糊
    void render(VkCommandBuffer commandBuffer, VkImageView colorView, VkImageView velocityView);

    // 设置
    void setSettings(const MotionBlurSettings& settings) { m_settings = settings; }
    const MotionBlurSettings& getSettings() const { return m_settings; }

    // 获取输出
    VkImageView getOutputView() const { return m_outputView; }
    VkSampler getSampler() const { return m_sampler; }

private:
    void createResources();
    void cleanupResources();

    Device& m_device;
    uint32_t m_width, m_height;

    MotionBlurSettings m_settings;

    VkImage m_outputImage = VK_NULL_HANDLE;
    VkDeviceMemory m_outputMemory = VK_NULL_HANDLE;
    VkImageView m_outputView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;
};

} // namespace spark
