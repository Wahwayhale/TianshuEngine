#include "vulkan/fwd.h"
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "math/math_types.h"

namespace spark {


class Buffer;
class Mesh;
class Material;

// 实例数据
struct InstanceData {
    Mat4 transform;
    Vec4 color;
    float metallic;
    float roughness;
    float ao;
    float padding;
};

// 实例批次
struct InstanceBatch {
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    std::vector<InstanceData> instances;
    std::unique_ptr<Buffer> instanceBuffer;
    uint32_t instanceCount = 0;
    bool dirty = true;
};

// 实例化渲染器
class InstancedRenderer {
public:
    InstancedRenderer(Device& device);
    ~InstancedRenderer();

    // 添加实例
    void addInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
                     const Mat4& transform, const Vec4& color = Vec4(1.0f),
                     float metallic = 0.0f, float roughness = 0.5f, float ao = 1.0f);

    // 清除所有实例
    void clearInstances();

    // 渲染所有实例
    void render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t frameIndex);

    // 获取统计
    int getInstanceCount() const { return m_totalInstances; }
    int getBatchCount() const { return static_cast<int>(m_batches.size()); }
    int getDrawCalls() const { return static_cast<int>(m_batches.size()); }

private:
    void updateInstanceBuffers();

    Device& m_device;

    // 实例批次（按材质和网格分组）
    std::vector<InstanceBatch> m_batches;

    // 统计
    int m_totalInstances = 0;
};

} // namespace spark
