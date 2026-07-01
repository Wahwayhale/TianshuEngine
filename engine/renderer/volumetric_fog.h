#pragma once

#include "math/math_types.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace spark {

class Device;
class Buffer;

// 体积雾设置
struct VolumetricFogSettings {
    bool enabled = true;
    float density = 0.02f;
    float heightFalloff = 0.1f;
    float height = 0.0f;
    Vec3 color = Vec3(0.7f, 0.8f, 0.9f);
    float maxDistance = 100.0f;
    int sampleCount = 64;
};

// 体积雾渲染器
class VolumetricFogRenderer {
public:
    VolumetricFogRenderer(Device& device, uint32_t width, uint32_t height);
    ~VolumetricFogRenderer();

    void recreate(uint32_t width, uint32_t height);

    // 渲染体积雾
    void render(VkCommandBuffer commandBuffer, VkImageView depthView, const Mat4& viewMatrix, const Mat4& projMatrix);

    // 设置
    void setSettings(const VolumetricFogSettings& settings) { m_settings = settings; }
    const VolumetricFogSettings& getSettings() const { return m_settings; }

    // 获取输出
    VkImageView getOutputView() const { return m_outputView; }
    VkSampler getSampler() const { return m_sampler; }

private:
    void createResources();
    void cleanupResources();

    Device& m_device;
    uint32_t m_width, m_height;

    VolumetricFogSettings m_settings;

    // 输出纹理
    VkImage m_outputImage = VK_NULL_HANDLE;
    VkDeviceMemory m_outputMemory = VK_NULL_HANDLE;
    VkImageView m_outputView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;

    // UBO
    std::unique_ptr<Buffer> m_uboBuffer;
};

} // namespace spark
