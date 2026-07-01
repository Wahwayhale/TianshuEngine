#pragma once

#include "math/math_types.h"
#include <memory>
#include <vulkan/vulkan.h>

namespace spark {

class Device;
class Texture;
class Buffer;

// 水面设置
struct WaterSettings {
    float waveSpeed = 1.0f;
    float waveHeight = 0.1f;
    float waveFrequency = 1.0f;
    Vec3 waterColor = Vec3(0.1f, 0.3f, 0.5f);
    float opacity = 0.8f;
    float reflectivity = 0.5f;
    float refractionStrength = 0.1f;
    float foamThreshold = 0.8f;
};

// 水面渲染器
class WaterRenderer {
public:
    WaterRenderer(Device& device, VkRenderPass renderPass);
    ~WaterRenderer();

    // 渲染水面
    void render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix);

    // 设置
    void setSettings(const WaterSettings& settings) { m_settings = settings; }
    const WaterSettings& getSettings() const { return m_settings; }

private:
    void createMesh();
    void createPipeline(VkRenderPass renderPass);

    Device& m_device;
    WaterSettings m_settings;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

    std::unique_ptr<Buffer> m_vertexBuffer;
    std::unique_ptr<Buffer> m_indexBuffer;
    uint32_t m_indexCount = 0;
};

} // namespace spark
