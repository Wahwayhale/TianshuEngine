#pragma once

#include "math/math_types.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace spark {

class Device;
class Texture;

// 玻璃设置
struct GlassSettings {
    float opacity = 0.3f;
    float refractionIndex = 1.5f;
    Vec3 tintColor = Vec3(0.9f, 0.95f, 1.0f);
    float roughness = 0.01f;
    float thickness = 0.01f;
};

// 玻璃渲染器
class GlassRenderer {
public:
    GlassRenderer(Device& device, VkRenderPass renderPass);
    ~GlassRenderer();

    // 渲染玻璃
    void render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix);

    // 设置
    void setSettings(const GlassSettings& settings) { m_settings = settings; }
    const GlassSettings& getSettings() const { return m_settings; }

private:
    void createPipeline(VkRenderPass renderPass);

    Device& m_device;
    GlassSettings m_settings;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace spark
