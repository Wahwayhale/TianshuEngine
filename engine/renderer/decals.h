#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

namespace spark {

class Device;
class Texture;

// 贴花类型
enum class DecalType {
    Diffuse,      // 漫反射贴花
    Normal,       // 法线贴花
    Combined      // 组合贴花
};

// 贴花数据
struct DecalData {
    Vec3 position;
    Vec3 normal;
    Vec3 up;
    Vec2 size;
    float opacity = 1.0f;
    int materialIndex = 0;
};

// 贴花渲染器
class DecalRenderer {
public:
    DecalRenderer(Device& device, VkRenderPass renderPass);
    ~DecalRenderer();

    // 添加贴花
    void addDecal(const DecalData& decal);

    // 清除贴花
    void clearDecals();

    // 渲染贴花
    void render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix);

    // 获取贴花数量
    size_t getDecalCount() const { return m_decals.size(); }

private:
    void createPipeline(VkRenderPass renderPass);

    Device& m_device;

    std::vector<DecalData> m_decals;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace spark
