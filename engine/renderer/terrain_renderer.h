#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

namespace spark {

class Device;
class Texture;
class Buffer;

// 地形块
struct TerrainChunk {
    int x, z;
    int size;
    float scale;
    std::vector<float> heightData;
    std::unique_ptr<Buffer> vertexBuffer;
    std::unique_ptr<Buffer> indexBuffer;
    uint32_t indexCount;
};

// 地形渲染器
class TerrainRenderer {
public:
    TerrainRenderer(Device& device, VkRenderPass renderPass);
    ~TerrainRenderer();

    // 生成地形
    void generateTerrain(int chunksX, int chunksZ, int chunkSize, float scale);

    // 渲染地形
    void render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix);

    // 高度查询
    float getHeight(float x, float z) const;
    Vec3 getNormal(float x, float z) const;

    // 纹理层
    void addTextureLayer(std::shared_ptr<Texture> diffuse, std::shared_ptr<Texture> normal,
                         float minHeight, float maxHeight, float blendRange);

private:
    void generateChunk(TerrainChunk& chunk);
    void createChunkBuffers(TerrainChunk& chunk);

    Device& m_device;

    std::vector<TerrainChunk> m_chunks;
    std::vector<float> m_heightmap;

    int m_chunksX = 0;
    int m_chunksZ = 0;
    int m_chunkSize = 64;
    float m_scale = 1.0f;

    struct TextureLayer {
        std::shared_ptr<Texture> diffuse;
        std::shared_ptr<Texture> normal;
        float minHeight;
        float maxHeight;
        float blendRange;
    };
    std::vector<TextureLayer> m_textureLayers;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace spark
