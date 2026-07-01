#pragma once

#include "math/math_types.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace spark {

class Device;
class Texture;
class Buffer;

// 海洋设置
struct OceanSettings {
    bool enabled = true;
    float waveHeight = 0.5f;
    float waveFrequency = 1.0f;
    float waveSpeed = 1.0f;
    Vec3 waterColor = Vec3(0.0f, 0.3f, 0.5f);
    float transparency = 0.8f;
    float foamThreshold = 0.8f;
    int gridResolution = 256;
};

// 海洋渲染器
class OceanRenderer {
public:
    OceanRenderer(Device& device, VkRenderPass renderPass);
    ~OceanRenderer();

    // 渲染海洋
    void render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix);

    // 设置
    void setSettings(const OceanSettings& settings) { m_settings = settings; }
    const OceanSettings& getSettings() const { return m_settings; }

private:
    void createMesh();
    void createPipeline(VkRenderPass renderPass);
    void generateWaves();

    Device& m_device;
    OceanSettings m_settings;

    std::unique_ptr<Buffer> m_vertexBuffer;
    std::unique_ptr<Buffer> m_indexBuffer;
    uint32_t m_indexCount = 0;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace spark
