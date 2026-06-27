#include "instanced_renderer.h"
#include "buffer.h"
#include "mesh.h"
#include "pipeline.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <cstring>

namespace spark {

InstancedRenderer::InstancedRenderer(Device& device, std::shared_ptr<Mesh> mesh, uint32_t maxInstances)
    : m_device(device), m_mesh(mesh), m_maxInstances(maxInstances) {

    m_instances.reserve(maxInstances);

    // Create instance buffer
    VkDeviceSize bufferSize = sizeof(InstanceData) * maxInstances;
    m_instanceBuffer = std::make_unique<Buffer>(
        device,
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    SPARK_CORE_INFO("Instanced renderer created: max {0} instances", maxInstances);
}

InstancedRenderer::~InstancedRenderer() = default;

void InstancedRenderer::addInstance(const InstanceData& data) {
    if (m_instances.size() < m_maxInstances) {
        m_instances.push_back(data);
        m_bufferDirty = true;
    }
}

void InstancedRenderer::clearInstances() {
    m_instances.clear();
    m_bufferDirty = true;
}

void InstancedRenderer::updateBuffer() {
    if (!m_bufferDirty || m_instances.empty()) return;

    VkDeviceSize bufferSize = sizeof(InstanceData) * m_instances.size();
    m_instanceBuffer->copyTo(m_instances.data(), bufferSize);
    m_bufferDirty = false;
}

void InstancedRenderer::draw(VkCommandBuffer commandBuffer, Pipeline& pipeline) {
    if (m_instances.empty()) return;

    updateBuffer();

    // Bind mesh vertex buffer
    // Bind instance buffer
    // Draw instanced

    // For now, just draw each instance individually
    for (const auto& instance : m_instances) {
        m_mesh->draw(commandBuffer);
    }
}

BatchRenderer::BatchRenderer(Device& device) : m_device(device) {}

BatchRenderer::~BatchRenderer() = default;

void BatchRenderer::beginBatch() {
    m_batches.clear();
    m_totalInstances = 0;
}

void BatchRenderer::addInstance(std::shared_ptr<Mesh> mesh, const InstanceData& data) {
    // Find existing batch or create new one
    for (auto& batch : m_batches) {
        if (batch.mesh == mesh) {
            batch.instances.push_back(data);
            m_totalInstances++;
            return;
        }
    }

    // Create new batch
    Batch newBatch;
    newBatch.mesh = mesh;
    newBatch.instances.push_back(data);
    m_batches.push_back(std::move(newBatch));
    m_totalInstances++;
}

void BatchRenderer::endBatch() {
    // Create instance buffers for each batch
    for (auto& batch : m_batches) {
        VkDeviceSize bufferSize = sizeof(InstanceData) * batch.instances.size();
        batch.instanceBuffer = std::make_unique<Buffer>(
            m_device,
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        batch.instanceBuffer->copyTo(batch.instances.data(), bufferSize);
    }

    SPARK_CORE_INFO("Batch renderer: {0} batches, {1} total instances", m_batches.size(), m_totalInstances);
}

void BatchRenderer::draw(VkCommandBuffer commandBuffer, Pipeline& pipeline) {
    for (const auto& batch : m_batches) {
        if (batch.instances.empty()) continue;

        // Bind pipeline
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipeline());

        // Bind mesh and instance buffers
        // Draw instanced

        // For now, draw each instance individually
        for (size_t i = 0; i < batch.instances.size(); i++) {
            batch.mesh->draw(commandBuffer);
        }
    }
}

} // namespace spark
