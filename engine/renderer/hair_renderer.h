#pragma once

#include "math/math_types.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace spark {

class Device;
class Buffer;

// 头发设置
struct HairSettings {
    int strandCount = 10000;
    int segmentsPerStrand = 16;
    float length = 0.3f;
    float lengthVariance = 0.1f;
    float thickness = 0.01f;
    Vec3 baseColor = Vec3(0.3f, 0.2f, 0.1f);
    Vec3 tipColor = Vec3(0.5f, 0.3f, 0.2f);
    float roughness = 0.6f;
    float anisotropy = 0.8f;
};

// 头发渲染器
class HairRenderer {
public:
    HairRenderer(Device& device, VkRenderPass renderPass);
    ~HairRenderer();

    // 生成头发
    void generateHair(const Vec3& position, const HairSettings& settings);

    // 渲染头发
    void render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix);

    // 设置
    void setSettings(const HairSettings& settings) { m_settings = settings; }
    const HairSettings& getSettings() const { return m_settings; }

private:
    void createPipeline(VkRenderPass renderPass);

    Device& m_device;
    HairSettings m_settings;

    struct HairStrand {
        std::vector<Vec3> points;
    };
    std::vector<HairStrand> m_strands;

    std::unique_ptr<Buffer> m_vertexBuffer;
    uint32_t m_vertexCount = 0;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace spark
