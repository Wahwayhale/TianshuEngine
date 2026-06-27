#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

namespace spark {

class Device;
class Buffer;
class Mesh;
class Pipeline;

struct InstanceData {
    Mat4 transform;
    Vec4 color;
};

class InstancedRenderer {
public:
    InstancedRenderer(Device& device, std::shared_ptr<Mesh> mesh, uint32_t maxInstances = 1000);
    ~InstancedRenderer();

    void addInstance(const InstanceData& data);
    void clearInstances();
    void updateBuffer();

    void draw(VkCommandBuffer commandBuffer, Pipeline& pipeline);

    uint32_t getInstanceCount() const { return static_cast<uint32_t>(m_instances.size()); }
    uint32_t getMaxInstances() const { return m_maxInstances; }

private:
    Device& m_device;
    std::shared_ptr<Mesh> m_mesh;
    std::unique_ptr<Buffer> m_instanceBuffer;
    std::vector<InstanceData> m_instances;
    uint32_t m_maxInstances;
    bool m_bufferDirty = true;
};

// Batch renderer for efficient instanced rendering
class BatchRenderer {
public:
    BatchRenderer(Device& device);
    ~BatchRenderer();

    // Begin a new batch
    void beginBatch();

    // Add an instance to the current batch
    void addInstance(std::shared_ptr<Mesh> mesh, const InstanceData& data);

    // End the batch and prepare for rendering
    void endBatch();

    // Draw all batches
    void draw(VkCommandBuffer commandBuffer, Pipeline& pipeline);

    // Statistics
    uint32_t getBatchCount() const { return static_cast<uint32_t>(m_batches.size()); }
    uint32_t getTotalInstances() const { return m_totalInstances; }

private:
    struct Batch {
        std::shared_ptr<Mesh> mesh;
        std::vector<InstanceData> instances;
        std::unique_ptr<Buffer> instanceBuffer;
    };

    Device& m_device;
    std::vector<Batch> m_batches;
    uint32_t m_totalInstances = 0;
};

} // namespace spark
