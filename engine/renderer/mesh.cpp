#include "mesh.h"
#include "vulkan/device.h"

namespace spark {

// 标准顶点 + 索引
Mesh::Mesh(Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : m_vertexCount(static_cast<uint32_t>(vertices.size())),
      m_indexCount(static_cast<uint32_t>(indices.size())) {

    VkDeviceSize vertexBufferSize = sizeof(Vertex) * vertices.size();
    m_vertexBuffer = std::make_unique<Buffer>(
        device, vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    m_vertexBuffer->copyTo(vertices.data(), vertexBufferSize);

    if (!indices.empty()) {
        VkDeviceSize indexBufferSize = sizeof(uint32_t) * indices.size();
        m_indexBuffer = std::make_unique<Buffer>(
            device, indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        m_indexBuffer->copyTo(indices.data(), indexBufferSize);
    }
}

// 标准顶点，无索引
Mesh::Mesh(Device& device, const std::vector<Vertex>& vertices)
    : m_vertexCount(static_cast<uint32_t>(vertices.size())), m_indexCount(0) {

    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();
    m_vertexBuffer = std::make_unique<Buffer>(
        device, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    m_vertexBuffer->copyTo(vertices.data(), bufferSize);
}

// PBR 顶点 + 索引
Mesh::Mesh(Device& device, const std::vector<PBRVertex>& vertices, const std::vector<uint32_t>& indices)
    : m_vertexCount(static_cast<uint32_t>(vertices.size())),
      m_indexCount(static_cast<uint32_t>(indices.size())) {

    VkDeviceSize vertexBufferSize = sizeof(PBRVertex) * vertices.size();
    m_vertexBuffer = std::make_unique<Buffer>(
        device, vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    m_vertexBuffer->copyTo(vertices.data(), vertexBufferSize);

    if (!indices.empty()) {
        VkDeviceSize indexBufferSize = sizeof(uint32_t) * indices.size();
        m_indexBuffer = std::make_unique<Buffer>(
            device, indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        m_indexBuffer->copyTo(indices.data(), indexBufferSize);
    }
}

// PBR 顶点，无索引
Mesh::Mesh(Device& device, const std::vector<PBRVertex>& vertices)
    : m_vertexCount(static_cast<uint32_t>(vertices.size())), m_indexCount(0) {

    VkDeviceSize bufferSize = sizeof(PBRVertex) * vertices.size();
    m_vertexBuffer = std::make_unique<Buffer>(
        device, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    m_vertexBuffer->copyTo(vertices.data(), bufferSize);
}

Mesh::~Mesh() = default;

void Mesh::draw(VkCommandBuffer commandBuffer) {
    VkBuffer vertexBuffers[] = {m_vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    if (m_indexBuffer && m_indexCount > 0) {
        vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
    } else {
        vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
    }
}

} // namespace spark
