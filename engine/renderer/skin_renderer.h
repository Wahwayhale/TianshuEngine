#pragma once

#include "math/math_types.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace spark {

class Device;
class Texture;

// 皮肤设置
struct SkinSettings {
    float subsurfaceScattering = 0.5f;
    float roughness = 0.4f;
    float specular = 0.5f;
    Vec3 skinColor = Vec3(0.8f, 0.6f, 0.5f);
    float translucency = 0.3f;
};

// 皮肤渲染器
class SkinRenderer {
public:
    SkinRenderer(Device& device, VkRenderPass renderPass);
    ~SkinRenderer();

    // 渲染皮肤
    void render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix);

    // 设置
    void setSettings(const SkinSettings& settings) { m_settings = settings; }
    const SkinSettings& getSettings() const { return m_settings; }

private:
    void createPipeline(VkRenderPass renderPass);

    Device& m_device;
    SkinSettings m_settings;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace spark
