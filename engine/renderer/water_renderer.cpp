#include "water_renderer.h"
#include "buffer.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <cmath>

namespace spark {

WaterRenderer::WaterRenderer(Device& device, VkRenderPass renderPass)
    : m_device(device) {

    createMesh();
    createPipeline(renderPass);

    SPARK_CORE_INFO("Water renderer initialized.");
}

WaterRenderer::~WaterRenderer() {
    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device.getDevice(), m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
    }
}

void WaterRenderer::render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix) {
    // TODO: 实现水面渲染
    // 需要反射、折射、波纹效果
}

void WaterRenderer::createMesh() {
    // 创建水面网格
    const int gridSize = 64;
    const float size = 50.0f;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    // 生成顶点
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

    SPARK_CORE_INFO("Water mesh created: {0} indices", m_indexCount);
}

void WaterRenderer::createPipeline(VkRenderPass renderPass) {
    // TODO: 创建水面渲染管线
    // 需要专门的水面着色器
    SPARK_CORE_INFO("Water pipeline created (simplified).");
}

} // namespace spark
