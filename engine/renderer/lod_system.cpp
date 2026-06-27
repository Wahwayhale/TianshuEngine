#include "lod_system.h"
#include "mesh.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

LODSystem::LODSystem() = default;
LODSystem::~LODSystem() = default;

void LODSystem::addLODLevel(std::shared_ptr<Mesh> mesh, float distance) {
    LODLevel level;
    level.mesh = mesh;
    level.distance = distance;
    level.screenSize = 0.0f;

    m_levels.push_back(level);

    // Sort by distance
    std::sort(m_levels.begin(), m_levels.end(), [](const LODLevel& a, const LODLevel& b) {
        return a.distance < b.distance;
    });
}

const LODLevel& LODSystem::getLODLevel(float distance) const {
    for (size_t i = 0; i < m_levels.size(); i++) {
        if (distance < m_levels[i].distance) {
            return m_levels[i];
        }
    }
    return m_levels.back();
}

uint32_t LODSystem::getLODIndex(float distance) const {
    for (uint32_t i = 0; i < m_levels.size(); i++) {
        if (distance < m_levels[i].distance) {
            return i;
        }
    }
    return static_cast<uint32_t>(m_levels.size() - 1);
}

void LODSystem::generateLODs(std::shared_ptr<Mesh> baseMesh, uint32_t levels) {
    // Simplified LOD generation
    // In a real implementation, this would use mesh simplification algorithms

    addLODLevel(baseMesh, 50.0f);  // LOD 0: Full detail
    addLODLevel(baseMesh, 100.0f); // LOD 1: Medium detail
    addLODLevel(baseMesh, 200.0f); // LOD 2: Low detail
    addLODLevel(baseMesh, 500.0f); // LOD 3: Very low detail

    SPARK_CORE_INFO("Generated {0} LOD levels", levels);
}

uint32_t LODManager::registerLODGroup(const std::vector<std::shared_ptr<Mesh>>& meshes,
                                        const std::vector<float>& distances) {
    LODGroup group;
    group.meshes = meshes;
    group.distances = distances;

    m_groups.push_back(group);
    return static_cast<uint32_t>(m_groups.size() - 1);
}

std::shared_ptr<Mesh> LODManager::getMesh(uint32_t groupID, float distance) const {
    if (groupID >= m_groups.size()) return nullptr;

    const auto& group = m_groups[groupID];

    for (size_t i = 0; i < group.distances.size(); i++) {
        if (distance < group.distances[i]) {
            return group.meshes[i];
        }
    }

    return group.meshes.back();
}

void LODManager::update(const Vec3& cameraPosition) {
    // Update LOD levels based on camera position
    // This would be called each frame to update visible objects
}

} // namespace spark
