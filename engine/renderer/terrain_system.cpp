#include "terrain_system.h"
#include "texture.h"
#include "mesh.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <cmath>
#include <algorithm>
#include <random>

namespace spark {

// =============================================
// TerrainChunk
// =============================================

TerrainChunk::TerrainChunk(Device& device, int x, int z, int size, float scale, float heightScale)
    : m_device(device), m_x(x), m_z(z), m_size(size), m_scale(scale), m_heightScale(heightScale) {

    generateMesh();
    SPARK_CORE_INFO("Terrain chunk created: ({0}, {1})", x, z);
}

TerrainChunk::~TerrainChunk() = default;

void TerrainChunk::render(VkCommandBuffer commandBuffer) {
    if (m_mesh) {
        m_mesh->draw(commandBuffer);
    }
}

Vec3 TerrainChunk::getWorldPosition() const {
    return Vec3(m_x * m_scale, 0.0f, m_z * m_scale);
}

void TerrainChunk::generateMesh() {
    std::vector<PBRVertex> vertices;
    std::vector<uint32_t> indices;

    float step = m_scale / m_size;

    // 生成顶点
    for (int z = 0; z <= m_size; z++) {
        for (int x = 0; x <= m_size; x++) {
            PBRVertex vertex;

            // 位置
            float worldX = m_x * m_scale + x * step;
            float worldZ = m_z * m_scale + z * step;
            float height = 0.0f;  // TODO: 从高度图获取

            vertex.position[0] = worldX;
            vertex.position[1] = height * m_heightScale;
            vertex.position[2] = worldZ;

            // 法线（默认向上）
            vertex.normal[0] = 0.0f;
            vertex.normal[1] = 1.0f;
            vertex.normal[2] = 0.0f;

            // UV
            vertex.texCoord[0] = (float)x / m_size;
            vertex.texCoord[1] = (float)z / m_size;

            vertices.push_back(vertex);
        }
    }

    // 生成索引
    for (int z = 0; z < m_size; z++) {
        for (int x = 0; x < m_size; x++) {
            int topLeft = z * (m_size + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (m_size + 1) + x;
            int bottomRight = bottomLeft + 1;

            // 第一个三角形
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // 第二个三角形
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    m_mesh = std::make_unique<Mesh>(m_device, vertices, indices);
}

void TerrainChunk::generateLODs() {
    // TODO: 生成不同 LOD 级别的网格
}

// =============================================
// TerrainSystem
// =============================================

TerrainSystem::TerrainSystem(Device& device) : m_device(device) {}

TerrainSystem::~TerrainSystem() {
    shutdown();
}

bool TerrainSystem::initialize(const TerrainSettings& settings) {
    m_settings = settings;

    // 创建默认纹理
    uint32_t whitePixel = 0xFFFFFFFF;
    m_defaultTexture = std::make_shared<Texture>(m_device, 1, 1, &whitePixel);

    // 创建区块
    createChunks();

    SPARK_CORE_INFO("Terrain system initialized: {0}x{0} chunks", settings.size / settings.chunkSize);
    return true;
}

void TerrainSystem::shutdown() {
    m_chunks.clear();
    m_layers.clear();
    m_vegetationTypes.clear();
    m_vegetationInstances.clear();
    SPARK_CORE_INFO("Terrain system shutdown.");
}

void TerrainSystem::render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix) {
    // 渲染所有区块
    for (auto& chunk : m_chunks) {
        chunk->render(commandBuffer);
    }

    // 渲染植被（简化）
    // TODO: 实例化渲染植被
}

void TerrainSystem::loadHeightmap(const std::string& filepath) {
    // TODO: 从文件加载高度图
    SPARK_CORE_INFO("Loading heightmap: {0}", filepath);
}

void TerrainSystem::generateHeightmap(int seed) {
    // 使用 Perlin 噪声生成高度图
    m_heightmapWidth = m_settings.size + 1;
    m_heightmapHeight = m_settings.size + 1;
    m_heightmap.resize(m_heightmapWidth * m_heightmapHeight);

    std::default_random_engine rng(seed);

    // 简化的噪声生成
    for (int z = 0; z < m_heightmapHeight; z++) {
        for (int x = 0; x < m_heightmapWidth; x++) {
            float nx = (float)x / m_settings.size;
            float nz = (float)z / m_settings.size;

            // 多层噪声
            float height = 0.0f;
            height += sin(nx * 3.14159f * 2.0f) * 0.3f;
            height += sin(nz * 3.14159f * 3.0f) * 0.2f;
            height += sin(nx * 3.14159f * 5.0f + nz * 3.14159f * 4.0f) * 0.1f;

            m_heightmap[z * m_heightmapWidth + x] = height;
        }
    }

    SPARK_CORE_INFO("Heightmap generated: {0}x{1}", m_heightmapWidth, m_heightmapHeight);
}

float TerrainSystem::getHeight(float x, float z) const {
    if (m_heightmap.empty()) return 0.0f;

    // 转换到高度图坐标
    float hx = (x / m_settings.scale) * m_heightmapWidth;
    float hz = (z / m_settings.scale) * m_heightmapHeight;

    // 双线性插值
    int ix = (int)hx;
    int iz = (int)hz;
    float fx = hx - ix;
    float fz = hz - iz;

    ix = std::clamp(ix, 0, m_heightmapWidth - 2);
    iz = std::clamp(iz, 0, m_heightmapHeight - 2);

    float h00 = m_heightmap[iz * m_heightmapWidth + ix];
    float h10 = m_heightmap[iz * m_heightmapWidth + ix + 1];
    float h01 = m_heightmap[(iz + 1) * m_heightmapWidth + ix];
    float h11 = m_heightmap[(iz + 1) * m_heightmapWidth + ix + 1];

    float h = h00 * (1 - fx) * (1 - fz) +
              h10 * fx * (1 - fz) +
              h01 * (1 - fx) * fz +
              h11 * fx * fz;

    return h * m_settings.heightScale;
}

Vec3 TerrainSystem::getNormal(float x, float z) const {
    float eps = 0.1f;
    float hL = getHeight(x - eps, z);
    float hR = getHeight(x + eps, z);
    float hD = getHeight(x, z - eps);
    float hU = getHeight(x, z + eps);

    Vec3 normal = Vec3(hL - hR, 2.0f * eps, hD - hU);
    return glm::normalize(normal);
}

void TerrainSystem::addLayer(const TerrainLayer& layer) {
    m_layers.push_back(layer);
    SPARK_CORE_INFO("Added terrain layer: {0}", layer.name);
}

void TerrainSystem::removeLayer(int index) {
    if (index >= 0 && index < static_cast<int>(m_layers.size())) {
        m_layers.erase(m_layers.begin() + index);
    }
}

void TerrainSystem::addVegetationType(const VegetationType& type) {
    m_vegetationTypes.push_back(type);
    m_vegetationInstances.resize(m_vegetationTypes.size());
    SPARK_CORE_INFO("Added vegetation type: {0}", type.name);
}

void TerrainSystem::removeVegetationType(int index) {
    if (index >= 0 && index < static_cast<int>(m_vegetationTypes.size())) {
        m_vegetationTypes.erase(m_vegetationTypes.begin() + index);
        m_vegetationInstances.erase(m_vegetationInstances.begin() + index);
    }
}

void TerrainSystem::generateVegetation() {
    // 为每种植被类型生成实例
    for (size_t typeIdx = 0; typeIdx < m_vegetationTypes.size(); typeIdx++) {
        auto& type = m_vegetationTypes[typeIdx];
        auto& instances = m_vegetationInstances[typeIdx];
        instances.clear();

        // 计算实例数量
        float area = m_settings.scale * m_settings.scale;
        int count = static_cast<int>(area * type.density);

        std::default_random_engine rng(42);
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        for (int i = 0; i < count; i++) {
            float x = dist(rng) * m_settings.scale;
            float z = dist(rng) * m_settings.scale;

            // 检查坡度
            Vec3 normal = getNormal(x, z);
            float slope = glm::degrees(acos(normal.y));
            if (slope < type.minSlope || slope > type.maxSlope) continue;

            // 检查高度
            float height = getHeight(x, z);
            if (height < type.minAltitude || height > type.maxAltitude) continue;

            // 创建实例
            VegetationInstance instance;
            instance.position = Vec3(x, height, z);
            instance.rotation = Vec3(0.0f, dist(rng) * 360.0f, 0.0f);

            float scale = type.minScale + dist(rng) * (type.maxScale - type.minScale);
            instance.scale = Vec3(scale);

            instances.push_back(instance);
        }

        SPARK_CORE_INFO("Generated {0} instances of vegetation: {1}", instances.size(), type.name);
    }
}

void TerrainSystem::clearVegetation() {
    for (auto& instances : m_vegetationInstances) {
        instances.clear();
    }
}

void TerrainSystem::sculpt(float x, float z, float radius, float strength, bool raise) {
    // TODO: 修改地形高度
}

void TerrainSystem::smooth(float x, float z, float radius, float strength) {
    // TODO: 平滑地形
}

void TerrainSystem::paint(float x, float z, float radius, int layerIndex, float strength) {
    // TODO: 绘制地形纹理层
}

void TerrainSystem::createChunks() {
    int chunkCount = m_settings.size / m_settings.chunkSize;

    for (int z = 0; z < chunkCount; z++) {
        for (int x = 0; x < chunkCount; x++) {
            auto chunk = std::make_unique<TerrainChunk>(
                m_device, x, z,
                m_settings.chunkSize,
                m_settings.scale / chunkCount,
                m_settings.heightScale
            );
            m_chunks.push_back(std::move(chunk));
        }
    }
}

void TerrainSystem::updateLODs(const Vec3& cameraPos) {
    // TODO: 根据相机距离更新 LOD
}

} // namespace spark
