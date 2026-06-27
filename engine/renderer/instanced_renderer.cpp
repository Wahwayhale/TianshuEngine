#include "instanced_renderer.h"
#include "buffer.h"
#include "mesh.h"
#include "material.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <cstring>
#include <algorithm>

namespace spark {

InstancedRenderer::InstancedRenderer(Device& device) : m_device(device) {
    SPARK_CORE_INFO("Instanced renderer created.");
}

InstancedRenderer::~InstancedRenderer() {
    clearInstances();
}

void InstancedRenderer::addInstance(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material,
                                     const Mat4& transform, const Vec4& color,
                                     float metallic, float roughness, float ao) {
    // 查找现有批次
    for (auto& batch : m_batches) {
        if (batch.mesh == mesh && batch.material == material) {
            InstanceData data;
            data.transform = transform;
            data.color = color;
            data.metallic = metallic;
            data.roughness = roughness;
            data.ao = ao;
            data.padding = 0.0f;

            batch.instances.push_back(data);
            batch.dirty = true;
            m_totalInstances++;
            return;
        }
    }

    // 创建新批次
    InstanceBatch newBatch;
    newBatch.mesh = mesh;
    newBatch.material = material;

    InstanceData data;
    data.transform = transform;
    data.color = color;
    data.metallic = metallic;
    data.roughness = roughness;
    data.ao = ao;
    data.padding = 0.0f;

    newBatch.instances.push_back(data);
    newBatch.dirty = true;
    m_batches.push_back(std::move(newBatch));
    m_totalInstances++;
}

void InstancedRenderer::clearInstances() {
    m_batches.clear();
    m_totalInstances = 0;
}

void InstancedRenderer::updateInstanceBuffers() {
    for (auto& batch : m_batches) {
        if (!batch.dirty || batch.instances.empty()) continue;

        // 创建或更新实例缓冲
        VkDeviceSize bufferSize = sizeof(InstanceData) * batch.instances.size();

        if (!batch.instanceBuffer) {
            batch.instanceBuffer = std::make_unique<Buffer>(
                m_device,
                bufferSize,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );
        }

        batch.instanceBuffer->copyTo(batch.instances.data(), bufferSize);
        batch.instanceCount = static_cast<uint32_t>(batch.instances.size());
        batch.dirty = false;
    }
}

void InstancedRenderer::render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t frameIndex) {
    if (m_batches.empty()) return;

    // 更新实例缓冲
    updateInstanceBuffers();

    // 渲染每个批次
    for (const auto& batch : m_batches) {
        if (batch.instances.empty() || !batch.instanceBuffer) continue;

        // 绑定顶点缓冲（网格数据）
        VkBuffer vertexBuffers[] = {batch.mesh->getVertexBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        // 绑定实例缓冲
        VkBuffer instanceBuffers[] = {batch.instanceBuffer->getBuffer()};
        VkDeviceSize instanceOffsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 1, 1, instanceBuffers, instanceOffsets);

        // 绘制实例化
        if (batch.mesh->hasIndices()) {
            // 索引绘制
            vkCmdBindIndexBuffer(commandBuffer, batch.mesh->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffer, batch.mesh->getIndexCount(), batch.instanceCount, 0, 0, 0);
        } else {
            // 非索引绘制
            vkCmdDraw(commandBuffer, batch.mesh->getVertexCount(), batch.instanceCount, 0, 0);
        }
    }
}

} // namespace spark
