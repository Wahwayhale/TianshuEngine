#include "vulkan/fwd.h"
#include <vulkan/vulkan.h>
#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "math/math_types.h"

namespace spark {


class Buffer;

// GI 设置
struct GISettings {
    bool enabled = true;
    float intensity = 1.0f;
    float radius = 0.5f;
    int samples = 16;
    float bias = 0.01f;
    float maxDistance = 10.0f;
};

// 全局光照系统
class GlobalIllumination {
public:
    GlobalIllumination(Device& device, uint32_t width, uint32_t height);
    ~GlobalIllumination();

    void recreate(uint32_t width, uint32_t height);

    // 渲染 GI
    void render(VkCommandBuffer commandBuffer,
                VkImageView depthView,
                VkImageView normalView,
                VkImageView albedoView);

    // 设置
    void setSettings(const GISettings& settings) { m_settings = settings; }
    GISettings& getSettings() { return m_settings; }

    // 获取输出
    VkImageView getOutputView() const { return m_outputView; }
    VkSampler getSampler() const { return m_sampler; }

private:
    void createResources();
    void cleanupResources();
    void createPipelines();

    Device& m_device;
    uint32_t m_width, m_height;

    // 输出纹理
    VkImage m_outputImage = VK_NULL_HANDLE;
    VkDeviceMemory m_outputMemory = VK_NULL_HANDLE;
    VkImageView m_outputView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;

    // 管线
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

    // 设置
    GISettings m_settings;
};

} // namespace spark
