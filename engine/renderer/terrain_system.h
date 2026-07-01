#include <vulkan/vulkan.h>
#include "vulkan/fwd.h"
#pragma once

#include "math/math_types.h"
#include <memory>
#include <vector>
#include <string>

namespace spark {


class Texture;
class Mesh;

// 地形设置
struct TerrainSettings {
    int size = 256;                    // 地形大小（顶点数）
    float scale = 100.0f;              // 世界空间缩放
    float heightScale = 50.0f;         // 高度缩放
    int chunkSize = 64;                // 区块大小
    float lodDistance = 100.0f;        // LOD 切换距离
};

// 地形层
struct TerrainLayer {
    std::string name;
    std::shared_ptr<Texture> diffuseTexture;
    std::shared_ptr<Texture> normalTexture;
    float tiling = 10.0f;
    float minHeight = 0.0f;
    float maxHeight = 1.0f;
    float blendRange = 0.1f;
};

// 植被类型
struct VegetationType {
    std::string name;
    std::shared_ptr<Mesh> mesh;
    float density = 1.0f;             // 每平方米密度
    float minScale = 0.8f;
    float maxScale = 1.2f;
    float minSlope = 0.0f;            // 最小坡度（度）
    float maxSlope = 45.0f;           // 最大地形坡度（度）
    float minAltitude = 0.0f;
    float maxAltitude = 100.0f;
};

// 植被实例
struct VegetationInstance {
    Vec3 position;
    Vec3 rotation;
    Vec3 scale;
};

// 地形区块
class TerrainChunk {
public:
    TerrainChunk(Device& device, int x, int z, int size, float scale, float heightScale);
    ~TerrainChunk();

    void render(VkCommandBuffer commandBuffer);

    int getX() const { return m_x; }
    int getZ() const { return m_z; }
    Vec3 getWorldPosition() const;

private:
    void generateMesh();
    void generateLODs();

    Device& m_device;
    int m_x, m_z;
    int m_size;
    float m_scale;
    float m_heightScale;

    std::unique_ptr<Mesh> m_mesh;
    std::vector<std::unique_ptr<Mesh>> m_lodMeshes;

    // 高度数据
    std::vector<float> m_heightData;
};

// 地形系统
class TerrainSystem {
public:
    TerrainSystem(Device& device);
    ~TerrainSystem();

    // 初始化
    bool initialize(const TerrainSettings& settings);
    void shutdown();

    // 渲染
    void render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix);

    // 高度图
    void loadHeightmap(const std::string& filepath);
    void generateHeightmap(int seed);
    float getHeight(float x, float z) const;
    Vec3 getNormal(float x, float z) const;

    // 地形层
    void addLayer(const TerrainLayer& layer);
    void removeLayer(int index);
    int getLayerCount() const { return static_cast<int>(m_layers.size()); }

    // 植被
    void addVegetationType(const VegetationType& type);
    void removeVegetationType(int index);
    void generateVegetation();
    void clearVegetation();

    // 设置
    void setSettings(const TerrainSettings& settings) { m_settings = settings; }
    const TerrainSettings& getSettings() const { return m_settings; }

    // 地形修改
    void sculpt(float x, float z, float radius, float strength, bool raise);
    void smooth(float x, float z, float radius, float strength);
    void paint(float x, float z, float radius, int layerIndex, float strength);

private:
    void createChunks();
    void updateLODs(const Vec3& cameraPos);

    Device& m_device;
    TerrainSettings m_settings;

    // 区块
    std::vector<std::unique_ptr<TerrainChunk>> m_chunks;

    // 高度图数据
    std::vector<float> m_heightmap;
    int m_heightmapWidth = 0;
    int m_heightmapHeight = 0;

    // 地形层
    std::vector<TerrainLayer> m_layers;

    // 植被
    std::vector<VegetationType> m_vegetationTypes;
    std::vector<std::vector<VegetationInstance>> m_vegetationInstances;

    // 渲染资源
    std::shared_ptr<Texture> m_defaultTexture;
};

} // namespace spark
