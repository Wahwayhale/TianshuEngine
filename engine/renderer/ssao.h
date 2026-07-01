#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "math/math_types.h"

namespace spark {

class Device;
class Buffer;

// SSAO 设置
struct SSAOSettings {
    bool enabled = true;
    float radius = 0.5f;
    float bias = 0.025f;
    float power = 2.0f;
    int kernelSize = 64;
    int noiseSize = 4;
};

// SSAO 系统
class SSAOSystem {
public:
    SSAOSystem(Device& device, uint32_t width, uint32_t height);
    ~SSAOSystem();

    void recreate(uint32_t width, uint32_t height);

    // 渲染 SSAO
    void render(VkCommandBuffer commandBuffer, VkImageView depthView, VkImageView normalView);

    // 设置
    void setSettings(const SSAOSettings& settings) { m_settings = settings; }
    const SSAOSettings& getSettings() const { return m_settings; }

    // 获取输出
    VkImageView getOutputView() const { return m_outputView; }
    VkSampler getSampler() const { return m_sampler; }

private:
    void createResources();
    void cleanupResources();
    void generateKernel();

    Device& m_device;
    uint32_t m_width, m_height;

    SSAOSettings m_settings;

    // 输出纹理
    VkImage m_outputImage = VK_NULL_HANDLE;
    VkDeviceMemory m_outputMemory = VK_NULL_HANDLE;
    VkImageView m_outputView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;

    // 噪声纹理
    VkImage m_noiseImage = VK_NULL_HANDLE;
    VkDeviceMemory m_noiseMemory = VK_NULL_HANDLE;
    VkImageView m_noiseView = VK_NULL_HANDLE;

    // 采样核
    std::vector<Vec4> m_kernel;
};

} // namespace spark
