#pragma once

#include "math/math_types.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace spark {

class Device;
class Texture;

// 云层设置
struct CloudSettings {
    bool enabled = true;
    float coverage = 0.5f;
    float thickness = 1.0f;
    float height = 100.0f;
    float scale = 0.001f;
    Vec3 color = Vec3(1.0f);
    float absorption = 0.5f;
    float scattering = 0.5f;
    float windSpeed = 1.0f;
    Vec3 windDirection = Vec3(1.0f, 0.0f, 0.0f);
};

// 云渲染器
class CloudRenderer {
public:
    CloudRenderer(Device& device, VkRenderPass renderPass);
    ~CloudRenderer();

    // 渲染云层
    void render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix);

    // 设置
    void setSettings(const CloudSettings& settings) { m_settings = settings; }
    const CloudSettings& getSettings() const { return m_settings; }

private:
    void createNoiseTexture();
    void createPipeline(VkRenderPass renderPass);

    Device& m_device;
    CloudSettings m_settings;

    std::unique_ptr<Texture> m_noiseTexture;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace spark
