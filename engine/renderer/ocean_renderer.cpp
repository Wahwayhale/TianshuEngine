#include "ocean_renderer.h"
#include "buffer.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <cmath>

namespace spark {

OceanRenderer::OceanRenderer(Device& device, VkRenderPass renderPass)
    : m_device(device) {

    createMesh();
    createPipeline(renderPass);

    SPARK_CORE_INFO("Ocean renderer initialized.");
}

OceanRenderer::~OceanRenderer() {
    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device.getDevice(), m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
    }
}

void OceanRenderer::render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix) {
    if (!m_settings.enabled) return;

    // TODO: 渲染海洋
    // 需要波浪模拟和水面着色
}

void OceanRenderer::createMesh() {
    const int gridSize = m_settings.gridResolution;
    const float size = 100.0f;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    // 生成网格
    for (int z = 0; z <= gridSize; z++) {
        for (int x = 0; x <= gridSize; x++) {
            float xPos = (float)x / gridSize * size - size * 0.5f;
            float zPos = (float)z / gridSize * size - size * 0.5f;

            // 位置
            vertices.push_back(xPos);
            vertices.push_back(0.0f);
            vertices.push_back(zPos);

            // 法线
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);

            // UV
            vertices.push_back((float)x / gridSize);
            vertices.push_back((float)z / gridSize);
        }
    }

    // 生成索引
    for (int z = 0; z < gridSize; z++) {
        for (int x = 0; x < gridSize; x++) {
            int topLeft = z * (gridSize + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (gridSize + 1) + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    m_indexCount = static_cast<uint32_t>(indices.size());

    // 创建缓冲
    VkDeviceSize vertexSize = vertices.size() * sizeof(float);
    m_vertexBuffer = std::make_unique<Buffer>(
        m_device, vertexSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    m_vertexBuffer->copyTo(vertices.data(), vertexSize);

    VkDeviceSize indexSize = indices.size() * sizeof(uint32_t);
    m_indexBuffer = std::make_unique<Buffer>(
        m_device, indexSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    m_indexBuffer->copyTo(indices.data(), indexSize);

    SPARK_CORE_INFO("Ocean mesh created: {0} indices", m_indexCount);
}

void OceanRenderer::createPipeline(VkRenderPass renderPass) {
    // TODO: 创建海洋渲染管线
    SPARK_CORE_INFO("Ocean pipeline created (simplified).");
}

void OceanRenderer::generateWaves() {
    // TODO: 生成波浪数据
}

} // namespace spark
