#pragma once

#include "math/math_types.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace spark {

class Device;
class Buffer;

// 大气散射设置
struct AtmosphereSettings {
    Vec3 sunDirection = glm::normalize(Vec3(0.5f, -0.5f, 0.3f));
    Vec3 sunColor = Vec3(1.0f, 0.95f, 0.9f);
    float sunIntensity = 10.0f;

    float rayleighScattering = 0.01f;
    float mieScattering = 0.01f;
    float mieG = 0.8f;

    float atmosphereHeight = 100.0f;
    float planetRadius = 6371000.0f;

    Vec3 rayleighCoefficients = Vec3(5.5e-6f, 13.0e-6f, 22.4e-6f);
    float mieCoefficient = 21e-6f;
};

// 大气渲染器
class AtmosphereRenderer {
public:
    AtmosphereRenderer(Device& device, VkRenderPass renderPass);
    ~AtmosphereRenderer();

    // 渲染大气
    void render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix);

    // 设置
    void setSettings(const AtmosphereSettings& settings) { m_settings = settings; }
    const AtmosphereSettings& getSettings() const { return m_settings; }

private:
    void createPipeline(VkRenderPass renderPass);

    Device& m_device;
    AtmosphereSettings m_settings;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace spark
