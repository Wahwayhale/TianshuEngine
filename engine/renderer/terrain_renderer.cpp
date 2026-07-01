#include "terrain_renderer.h"
#include "buffer.h"
#include "texture.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <cmath>
#include <random>

namespace spark {

TerrainRenderer::TerrainRenderer(Device& device, VkRenderPass renderPass)
    : m_device(device) {
    // TODO: 创建地形渲染管线
    SPARK_CORE_INFO("Terrain renderer initialized.");
}

TerrainRenderer::~TerrainRenderer() {
    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device.getDevice(), m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
    }
}

void TerrainRenderer::generateTerrain(int chunksX, int chunksZ, int chunkSize, float scale) {
    m_chunksX = chunksX;
    m_chunksZ = chunksZ;
    m_chunkSize = chunkSize;
    m_scale = scale;

    // 生成高度图
    int totalSize = (chunksX * chunkSize + 1) * (chunksZ * chunkSize + 1);
    m_heightmap.resize(totalSize);

    std::default_random_engine rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (int z = 0; z <= chunksZ * chunkSize; z++) {
        for (int x = 0; x <= chunksX * chunkSize; x++) {
            float nx = (float)x / (chunksX * chunkSize);
            float nz = (float)z / (chunksZ * chunkSize);

            // 多层噪声
            float height = 0.0f;
            height += sin(nx * 3.14159f * 2.0f) * 0.3f;
            height += sin(nz * 3.14159f * 3.0f) * 0.2f;
            height += sin(nx * 3.14159f * 5.0f + nz * 3.14159f * 4.0f) * 0.1f;

            m_heightmap[z * (chunksX * chunkSize + 1) + x] = height;
        }
    }

    // 生成地形块
    m_chunks.resize(chunksX * chunksZ);
    for (int z = 0; z < chunksZ; z++) {
        for (int x = 0; x < chunksX; x++) {
            auto& chunk = m_chunks[z * chunksX + x];
            chunk.x = x;
            chunk.z = z;
            chunk.size = chunkSize;
            chunk.scale = scale;
            generateChunk(chunk);
            createChunkBuffers(chunk);
        }
    }

    SPARK_CORE_INFO("Terrain generated: {0}x{1} chunks", chunksX, chunksZ);
}

void TerrainRenderer::render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix) {
    // TODO: 渲染地形
}

float TerrainRenderer::getHeight(float x, float z) const {
    if (m_heightmap.empty()) return 0.0f;

    int totalX = m_chunksX * m_chunkSize;
    int totalZ = m_chunksZ * m_chunkSize;

    float hx = (x / m_scale + 0.5f) * totalX;
    float hz = (z / m_scale + 0.5f) * totalZ;

    int ix = static_cast<int>(hx);
    int iz = static_cast<int>(hz);
    float fx = hx - ix;
    float fz = hz - iz;

    ix = std::clamp(ix, 0, totalX - 1);
    iz = std::clamp(iz, 0, totalZ - 1);

    float h00 = m_heightmap[iz * (totalX + 1) + ix];
    float h10 = m_heightmap[iz * (totalX + 1) + ix + 1];
    float h01 = m_heightmap[(iz + 1) * (totalX + 1) + ix];
    float h11 = m_heightmap[(iz + 1) * (totalX + 1) + ix + 1];

    float h = h00 * (1 - fx) * (1 - fz) +
              h10 * fx * (1 - fz) +
              h01 * (1 - fx) * fz +
              h11 * fx * fz;

    return h * m_scale;
}

Vec3 TerrainRenderer::getNormal(float x, float z) const {
    float eps = 0.1f;
    float hL = getHeight(x - eps, z);
    float hR = getHeight(x + eps, z);
    float hD = getHeight(x, z - eps);
    float hU = getHeight(x, z + eps);

    Vec3 normal = Vec3(hL - hR, 2.0f * eps, hD - hU);
    return glm::normalize(normal);
}

void TerrainRenderer::addTextureLayer(std::shared_ptr<Texture> diffuse, std::shared_ptr<Texture> normal,
                                       float minHeight, float maxHeight, float blendRange) {
    TextureLayer layer;
    layer.diffuse = diffuse;
    layer.normal = normal;
    layer.minHeight = minHeight;
    layer.maxHeight = maxHeight;
    layer.blendRange = blendRange;

    m_textureLayers.push_back(layer);
}

void TerrainRenderer::generateChunk(TerrainChunk& chunk) {
    // 从高度图采样
    int totalX = m_chunksX * m_chunkSize;
    int startX = chunk.x * m_chunkSize;
    int startZ = chunk.z * m_chunkSize;

    chunk.heightData.resize((m_chunkSize + 1) * (m_chunkSize + 1));

    for (int z = 0; z <= m_chunkSize; z++) {
        for (int x = 0; x <= m_chunkSize; x++) {
            int globalX = startX + x;
            int globalZ = startZ + z;

            if (globalX <= totalX && globalZ <= m_chunksZ * m_chunkSize) {
                chunk.heightData[z * (m_chunkSize + 1) + x] = m_heightmap[globalZ * (totalX + 1) + globalX];
            }
        }
    }
}

void TerrainRenderer::createChunkBuffers(TerrainChunk& chunk) {
    // 创建顶点
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    float chunkScale = chunk.scale / m_chunksX;

    for (int z = 0; z <= m_chunkSize; z++) {
        for (int x = 0; x <= m_chunkSize; x++) {
            float worldX = chunk.x * chunkScale + (float)x / m_chunkSize * chunkScale - chunk.scale * 0.5f;
            float worldZ = chunk.z * chunkScale + (float)z / m_chunkSize * chunkScale - chunk.scale * 0.5f;
            float height = chunk.heightData[z * (m_chunkSize + 1) + x];

            // 位置
            vertices.push_back(worldX);
            vertices.push_back(height);
            vertices.push_back(worldZ);

            // 法线
            Vec3 normal = getNormal(worldX, worldZ);
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);

            // UV
            vertices.push_back((float)x / m_chunkSize);
            vertices.push_back((float)z / m_chunkSize);
        }
    }

    // 创建索引
    for (int z = 0; z < m_chunkSize; z++) {
        for (int x = 0; x < m_chunkSize; x++) {
            int topLeft = z * (m_chunkSize + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (m_chunkSize + 1) + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    chunk.indexCount = static_cast<uint32_t>(indices.size());

    // 创建缓冲
    VkDeviceSize vertexSize = vertices.size() * sizeof(float);
    chunk.vertexBuffer = std::make_unique<Buffer>(
        m_device, vertexSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    chunk.vertexBuffer->copyTo(vertices.data(), vertexSize);

    VkDeviceSize indexSize = indices.size() * sizeof(uint32_t);
    chunk.indexBuffer = std::make_unique<Buffer>(
        m_device, indexSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    chunk.indexBuffer->copyTo(indices.data(), indexSize);
}

} // namespace spark
