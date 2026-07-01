#pragma once

#include "math/math_types.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace spark {

class Device;
class Mesh;
class Texture;

// 植被实例
struct VegetationInstance {
    Vec3 position;
    Vec3 rotation;
    Vec3 scale;
    float windPhase;
};

// 植被类型
struct VegetationType {
    std::string name;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Texture> texture;
    float density = 1.0f;
    float minScale = 0.8f;
    float maxScale = 1.2f;
    float windStrength = 1.0f;
};

// 植被渲染器
class VegetationRenderer {
public:
    VegetationRenderer(Device& device, VkRenderPass renderPass);
    ~VegetationRenderer();

    // 添加植被类型
    void addVegetationType(const VegetationType& type);

    // 生成植被
    void generateVegetation(const Vec3& center, float radius);

    // 渲染植被
    void render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix);

    // 清除植被
    void clearVegetation();

private:
    void createPipeline(VkRenderPass renderPass);

    Device& m_device;

    std::vector<VegetationType> m_vegetationTypes;
    std::vector<std::vector<VegetationInstance>> m_instances;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace spark
