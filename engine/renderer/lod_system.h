#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <functional>

namespace spark {

class Mesh;

struct LODLevel {
    std::shared_ptr<Mesh> mesh;
    float distance;  // Distance threshold for this LOD
    float screenSize;  // Screen size threshold (alternative)
};

class LODSystem {
public:
    LODSystem();
    ~LODSystem();

    // Add LOD levels (sorted by distance, closest first)
    void addLODLevel(std::shared_ptr<Mesh> mesh, float distance);

    // Get appropriate LOD based on distance
    const LODLevel& getLODLevel(float distance) const;

    // Get LOD index based on distance
    uint32_t getLODIndex(float distance) const;

    // Create automatic LODs from a mesh
    void generateLODs(std::shared_ptr<Mesh> baseMesh, uint32_t levels = 4);

    // Statistics
    uint32_t getLODCount() const { return static_cast<uint32_t>(m_levels.size()); }
    uint32_t getCurrentLOD() const { return m_currentLOD; }

private:
    std::vector<LODLevel> m_levels;
    uint32_t m_currentLOD = 0;
};

// LOD Manager for managing multiple LOD groups
class LODManager {
public:
    LODManager() = default;
    ~LODManager() = default;

    // Register a LOD group
    uint32_t registerLODGroup(const std::vector<std::shared_ptr<Mesh>>& meshes,
                               const std::vector<float>& distances);

    // Get mesh for a LOD group at given distance
    std::shared_ptr<Mesh> getMesh(uint32_t groupID, float distance) const;

    // Update LODs based on camera position
    void update(const Vec3& cameraPosition);

    // Statistics
    uint32_t getGroupCount() const { return static_cast<uint32_t>(m_groups.size()); }

private:
    struct LODGroup {
        std::vector<std::shared_ptr<Mesh>> meshes;
        std::vector<float> distances;
        uint32_t currentLOD = 0;
    };

    std::vector<LODGroup> m_groups;
};

} // namespace spark
