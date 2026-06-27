#pragma once

#include "buffer.h"
#include "pipeline.h"
#include <memory>
#include <vector>

namespace spark {

class Device;

class Mesh {
public:
    // 带索引缓冲的构造函数（标准顶点）
    Mesh(Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    // 无索引缓冲的构造函数（标准顶点）
    Mesh(Device& device, const std::vector<Vertex>& vertices);

    // PBR 顶点构造函数
    Mesh(Device& device, const std::vector<PBRVertex>& vertices, const std::vector<uint32_t>& indices);
    Mesh(Device& device, const std::vector<PBRVertex>& vertices);

    ~Mesh();

    void draw(VkCommandBuffer commandBuffer);

    uint32_t getVertexCount() const { return m_vertexCount; }
    uint32_t getIndexCount() const { return m_indexCount; }
    bool hasIndices() const { return m_indexBuffer != nullptr; }

private:
    std::unique_ptr<Buffer> m_vertexBuffer;
    std::unique_ptr<Buffer> m_indexBuffer;
    uint32_t m_vertexCount;
    uint32_t m_indexCount = 0;
};

} // namespace spark
