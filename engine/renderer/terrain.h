#include "vulkan/fwd.h"
#pragma once

#include "math/math_types.h"
#include "mesh.h"
#include <memory>
#include <string>
#include <vector>

namespace spark {


class Texture;

struct TerrainConfig {
    uint32_t width = 256;
    uint32_t depth = 256;
    float scaleX = 1.0f;
    float scaleY = 50.0f;
    float scaleZ = 1.0f;
    float uvScale = 10.0f;
    std::string heightmapPath;
};

class Terrain {
public:
    Terrain(Device& device, const TerrainConfig& config = TerrainConfig());
    ~Terrain();

    void draw(VkCommandBuffer commandBuffer);

    float getHeight(float x, float z) const;
    Vec3 getNormal(float x, float z) const;

    void setHeightmap(const std::string& filepath);
    void generateFlat(float height = 0.0f);
    void generatePerlin(float scale = 0.02f, int octaves = 6, float persistence = 0.5f);

    const TerrainConfig& getConfig() const { return m_config; }
    uint32_t getWidth() const { return m_config.width; }
    uint32_t getDepth() const { return m_config.depth; }

private:
    void generateMesh();
    void calculateNormals();

    Device& m_device;
    TerrainConfig m_config;
    std::unique_ptr<Mesh> m_mesh;
    std::vector<float> m_heightData;
    std::vector<Vec3> m_normals;

    std::shared_ptr<Texture> m_heightmapTexture;
    std::shared_ptr<Texture> m_diffuseTexture;
    std::shared_ptr<Texture> m_normalTexture;
};

} // namespace spark
