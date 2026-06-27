#include "terrain.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <stb_image.h>
#include <cmath>
#include <noise/noise.h>

namespace spark {

Terrain::Terrain(Device& device, const TerrainConfig& config)
    : m_device(device), m_config(config) {

    m_heightData.resize(config.width * config.depth);
    m_normals.resize(config.width * config.depth);

    if (!config.heightmapPath.empty()) {
        setHeightmap(config.heightmapPath);
    } else {
        generateFlat();
    }

    generateMesh();
    calculateNormals();

    SPARK_CORE_INFO("Terrain created: {0}x{1}", config.width, config.depth);
}

Terrain::~Terrain() = default;

void Terrain::draw(VkCommandBuffer commandBuffer) {
    if (m_mesh) {
        m_mesh->draw(commandBuffer);
    }
}

float Terrain::getHeight(float x, float z) const {
    // Convert world coordinates to terrain coordinates
    float tx = (x / m_config.scaleX + m_config.width / 2.0f);
    float tz = (z / m_config.scaleZ + m_config.depth / 2.0f);

    // Clamp to terrain bounds
    tx = std::clamp(tx, 0.0f, static_cast<float>(m_config.width - 1));
    tz = std::clamp(tz, 0.0f, static_cast<float>(m_config.depth - 1));

    // Get integer and fractional parts
    int ix = static_cast<int>(tx);
    int iz = static_cast<int>(tz);
    float fx = tx - ix;
    float fz = tz - iz;

    // Bilinear interpolation
    float h00 = m_heightData[iz * m_config.width + ix];
    float h10 = m_heightData[iz * m_config.width + std::min(ix + 1, static_cast<int>(m_config.width - 1))];
    float h01 = m_heightData[std::min(iz + 1, static_cast<int>(m_config.depth - 1)) * m_config.width + ix];
    float h11 = m_heightData[std::min(iz + 1, static_cast<int>(m_config.depth - 1)) * m_config.width + std::min(ix + 1, static_cast<int>(m_config.width - 1))];

    float h0 = h00 * (1.0f - fx) + h10 * fx;
    float h1 = h01 * (1.0f - fx) + h11 * fx;

    return (h0 * (1.0f - fz) + h1 * fz) * m_config.scaleY;
}

Vec3 Terrain::getNormal(float x, float z) const {
    // Convert world coordinates to terrain coordinates
    float tx = (x / m_config.scaleX + m_config.width / 2.0f);
    float tz = (z / m_config.scaleZ + m_config.depth / 2.0f);

    int ix = std::clamp(static_cast<int>(tx), 0, static_cast<int>(m_config.width - 1));
    int iz = std::clamp(static_cast<int>(tz), 0, static_cast<int>(m_config.depth - 1));

    return m_normals[iz * m_config.width + ix];
}

void Terrain::setHeightmap(const std::string& filepath) {
    int width, height, channels;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 1);

    if (!data) {
        SPARK_CORE_ERROR("Failed to load heightmap: {0}", filepath);
        generateFlat();
        return;
    }

    // Scale heightmap to terrain size
    for (uint32_t z = 0; z < m_config.depth; z++) {
        for (uint32_t x = 0; x < m_config.width; x++) {
            int sx = static_cast<int>(x * width / m_config.width);
            int sz = static_cast<int>(z * height / m_config.depth);
            m_heightData[z * m_config.width + x] = data[sz * width + sx] / 255.0f;
        }
    }

    stbi_image_free(data);
    SPARK_CORE_INFO("Heightmap loaded: {0}", filepath);
}

void Terrain::generateFlat(float height) {
    std::fill(m_heightData.begin(), m_heightData.end(), height);
}

void Terrain::generatePerlin(float scale, int octaves, float persistence) {
    for (uint32_t z = 0; z < m_config.depth; z++) {
        for (uint32_t x = 0; x < m_config.width; x++) {
            float nx = x * scale;
            float nz = z * scale;

            float height = 0.0f;
            float amplitude = 1.0f;
            float frequency = 1.0f;

            for (int i = 0; i < octaves; i++) {
                // Simple noise approximation (would use proper Perlin noise library)
                float noise = sin(nx * frequency) * cos(nz * frequency) * 0.5f + 0.5f;
                height += noise * amplitude;
                amplitude *= persistence;
                frequency *= 2.0f;
            }

            m_heightData[z * m_config.width + x] = height / octaves;
        }
    }
}

void Terrain::generateMesh() {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(m_config.width * m_config.depth);
    indices.reserve((m_config.width - 1) * (m_config.depth - 1) * 6);

    // Generate vertices
    for (uint32_t z = 0; z < m_config.depth; z++) {
        for (uint32_t x = 0; x < m_config.width; x++) {
            Vertex vertex{};

            float px = (x - m_config.width / 2.0f) * m_config.scaleX;
            float pz = (z - m_config.depth / 2.0f) * m_config.scaleZ;
            float py = m_heightData[z * m_config.width + x] * m_config.scaleY;

            vertex.position[0] = px;
            vertex.position[1] = py;
            vertex.position[2] = pz;

            vertex.texCoord[0] = x * m_config.uvScale / m_config.width;
            vertex.texCoord[1] = z * m_config.uvScale / m_config.depth;

            // Color based on height
            float height = m_heightData[z * m_config.width + x];
            if (height < 0.3f) {
                vertex.color[0] = 0.2f; vertex.color[1] = 0.5f; vertex.color[2] = 0.2f; // Green
            } else if (height < 0.6f) {
                vertex.color[0] = 0.5f; vertex.color[1] = 0.4f; vertex.color[2] = 0.3f; // Brown
            } else {
                vertex.color[0] = 0.8f; vertex.color[1] = 0.8f; vertex.color[2] = 0.8f; // White
            }

            vertices.push_back(vertex);
        }
    }

    // Generate indices
    for (uint32_t z = 0; z < m_config.depth - 1; z++) {
        for (uint32_t x = 0; x < m_config.width - 1; x++) {
            uint32_t topLeft = z * m_config.width + x;
            uint32_t topRight = topLeft + 1;
            uint32_t bottomLeft = (z + 1) * m_config.width + x;
            uint32_t bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    m_mesh = std::make_unique<Mesh>(m_device, vertices);
}

void Terrain::calculateNormals() {
    for (uint32_t z = 0; z < m_config.depth; z++) {
        for (uint32_t x = 0; x < m_config.width; x++) {
            // Get heights of neighboring vertices
            float hL = (x > 0) ? m_heightData[z * m_config.width + (x - 1)] : m_heightData[z * m_config.width + x];
            float hR = (x < m_config.width - 1) ? m_heightData[z * m_config.width + (x + 1)] : m_heightData[z * m_config.width + x];
            float hD = (z > 0) ? m_heightData[(z - 1) * m_config.width + x] : m_heightData[z * m_config.width + x];
            float hU = (z < m_config.depth - 1) ? m_heightData[(z + 1) * m_config.width + x] : m_heightData[z * m_config.width + x];

            // Calculate normal
            Vec3 normal;
            normal.x = (hL - hR) * m_config.scaleY;
            normal.y = 2.0f;
            normal.z = (hD - hU) * m_config.scaleY;
            normal = glm::normalize(normal);

            m_normals[z * m_config.width + x] = normal;
        }
    }
}

} // namespace spark
