#include "spatial_partition.h"
#include "core/log.h"
#include <algorithm>
#include <cmath>

namespace spark {

// =============================================
// SpatialHash
// =============================================

SpatialHash::SpatialHash(float cellSize) : m_cellSize(cellSize), m_invCellSize(1.0f / cellSize) {}

SpatialHash::~SpatialHash() = default;

void SpatialHash::insert(PhysicsBodyHandle handle, const Vec3& position, float radius) {
    m_bodyPositions[handle] = position;
    m_bodyRadii[handle] = radius;

    // 计算受影响的格子
    int minX, minY, minZ, maxX, maxY, maxZ;
    getCellCoords(position - Vec3(radius), minX, minY, minZ);
    getCellCoords(position + Vec3(radius), maxX, maxY, maxZ);

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            for (int z = minZ; z <= maxZ; z++) {
                uint64_t hash = hashCell(x, y, z);
                m_cells[hash].bodies.push_back(handle);
            }
        }
    }

    m_bodyCount++;
}

void SpatialHash::remove(PhysicsBodyHandle handle) {
    auto posIt = m_bodyPositions.find(handle);
    if (posIt == m_bodyPositions.end()) return;

    Vec3 position = posIt->second;
    float radius = m_bodyRadii[handle];

    // 从所有格子中移除
    int minX, minY, minZ, maxX, maxY, maxZ;
    getCellCoords(position - Vec3(radius), minX, minY, minZ);
    getCellCoords(position + Vec3(radius), maxX, maxY, maxZ);

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            for (int z = minZ; z <= maxZ; z++) {
                uint64_t hash = hashCell(x, y, z);
                auto& cell = m_cells[hash];
                cell.bodies.erase(
                    std::remove(cell.bodies.begin(), cell.bodies.end(), handle),
                    cell.bodies.end()
                );
            }
        }
    }

    m_bodyPositions.erase(handle);
    m_bodyRadii.erase(handle);
    m_bodyCount--;
}

void SpatialHash::update(PhysicsBodyHandle handle, const Vec3& position, float radius) {
    remove(handle);
    insert(handle, position, radius);
}

std::vector<PhysicsBodyHandle> SpatialHash::query(const Vec3& position, float radius) const {
    std::vector<PhysicsBodyHandle> result;
    std::unordered_map<PhysicsBodyHandle, bool> seen;

    int minX, minY, minZ, maxX, maxY, maxZ;
    getCellCoords(position - Vec3(radius), minX, minY, minZ);
    getCellCoords(position + Vec3(radius), maxX, maxY, maxZ);

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            for (int z = minZ; z <= maxZ; z++) {
                uint64_t hash = hashCell(x, y, z);
                auto it = m_cells.find(hash);
                if (it != m_cells.end()) {
                    for (PhysicsBodyHandle handle : it->second.bodies) {
                        if (!seen[handle]) {
                            seen[handle] = true;

                            // 检查距离
                            auto posIt = m_bodyPositions.find(handle);
                            if (posIt != m_bodyPositions.end()) {
                                float dist = glm::length(posIt->second - position);
                                float combinedRadius = radius + m_bodyRadii.at(handle);
                                if (dist <= combinedRadius) {
                                    result.push_back(handle);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

std::vector<PhysicsBodyHandle> SpatialHash::queryBox(const Vec3& min, const Vec3& max) const {
    std::vector<PhysicsBodyHandle> result;
    std::unordered_map<PhysicsBodyHandle, bool> seen;

    int minX, minY, minZ, maxX, maxY, maxZ;
    getCellCoords(min, minX, minY, minZ);
    getCellCoords(max, maxX, maxY, maxZ);

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            for (int z = minZ; z <= maxZ; z++) {
                uint64_t hash = hashCell(x, y, z);
                auto it = m_cells.find(hash);
                if (it != m_cells.end()) {
                    for (PhysicsBodyHandle handle : it->second.bodies) {
                        if (!seen[handle]) {
                            seen[handle] = true;
                            result.push_back(handle);
                        }
                    }
                }
            }
        }
    }

    return result;
}

void SpatialHash::clear() {
    m_cells.clear();
    m_bodyPositions.clear();
    m_bodyRadii.clear();
    m_bodyCount = 0;
}

uint64_t SpatialHash::hashCell(int x, int y, int z) const {
    // 简单的哈希函数
    uint64_t h = static_cast<uint64_t>(x) * 73856093;
    h ^= static_cast<uint64_t>(y) * 19349663;
    h ^= static_cast<uint64_t>(z) * 83492791;
    return h;
}

void SpatialHash::getCellCoords(const Vec3& position, int& x, int& y, int& z) const {
    x = static_cast<int>(std::floor(position.x * m_invCellSize));
    y = static_cast<int>(std::floor(position.y * m_invCellSize));
    z = static_cast<int>(std::floor(position.z * m_invCellSize));
}

// =============================================
// BVH
// =============================================

BVH::BVH() = default;
BVH::~BVH() = default;

void BVH::build(const std::vector<Vec3>& positions, const std::vector<float>& radii) {
    m_nodes.clear();

    if (positions.empty()) {
        m_rootIndex = -1;
        return;
    }

    // 创建叶节点
    std::vector<int> indices(positions.size());
    for (size_t i = 0; i < positions.size(); i++) {
        indices[i] = static_cast<int>(i);
    }

    m_rootIndex = buildRecursive(indices, 0, static_cast<int>(indices.size()), positions, radii);
}

void BVH::update(const std::vector<Vec3>& positions, const std::vector<float>& radii) {
    build(positions, radii);
}

int BVH::buildRecursive(std::vector<int>& indices, int start, int end,
                         const std::vector<Vec3>& positions, const std::vector<float>& radii) {
    int nodeIndex = static_cast<int>(m_nodes.size());
    m_nodes.push_back(BVHNode());

    // 计算包围盒
    Vec3 min = Vec3(1e30f);
    Vec3 max = Vec3(-1e30f);

    for (int i = start; i < end; i++) {
        int idx = indices[i];
        Vec3 pos = positions[idx];
        float r = radii[idx];

        min = glm::min(min, pos - Vec3(r));
        max = glm::max(max, pos + Vec3(r));
    }

    m_nodes[nodeIndex].min = min;
    m_nodes[nodeIndex].max = max;

    // 叶节点
    if (end - start <= 1) {
        m_nodes[nodeIndex].bodyIndex = indices[start];
        m_nodes[nodeIndex].isLeaf = true;
        return nodeIndex;
    }

    // 选择最长轴分割
    Vec3 size = max - min;
    int axis = 0;
    if (size.y > size.x) axis = 1;
    if (size.z > size[axis]) axis = 2;

    // 按中心排序
    int mid = (start + end) / 2;
    std::nth_element(indices.begin() + start, indices.begin() + mid, indices.begin() + end,
        [&](int a, int b) {
            return positions[a][axis] < positions[b][axis];
        });

    // 递归构建子树
    m_nodes[nodeIndex].left = buildRecursive(indices, start, mid, positions, radii);
    m_nodes[nodeIndex].right = buildRecursive(indices, mid, end, positions, radii);

    return nodeIndex;
}

std::vector<int> BVH::query(const Vec3& position, float radius) const {
    std::vector<int> results;
    if (m_rootIndex >= 0) {
        queryRecursive(m_rootIndex, position, radius, results);
    }
    return results;
}

void BVH::queryRecursive(int nodeIndex, const Vec3& position, float radius,
                          std::vector<int>& results) const {
    const auto& node = m_nodes[nodeIndex];

    // 检查包围盒相交
    Vec3 closest = glm::clamp(position, node.min, node.max);
    float dist = glm::length(closest - position);

    if (dist > radius) return;

    if (node.isLeaf) {
        results.push_back(node.bodyIndex);
        return;
    }

    if (node.left >= 0) {
        queryRecursive(node.left, position, radius, results);
    }
    if (node.right >= 0) {
        queryRecursive(node.right, position, radius, results);
    }
}

std::vector<int> BVH::queryBox(const Vec3& min, const Vec3& max) const {
    std::vector<int> results;
    if (m_rootIndex >= 0) {
        queryBoxRecursive(m_rootIndex, min, max, results);
    }
    return results;
}

void BVH::queryBoxRecursive(int nodeIndex, const Vec3& min, const Vec3& max,
                             std::vector<int>& results) const {
    const auto& node = m_nodes[nodeIndex];

    // 检查包围盒相交
    if (node.min.x > max.x || node.max.x < min.x ||
        node.min.y > max.y || node.max.y < min.y ||
        node.min.z > max.z || node.max.z < min.z) {
        return;
    }

    if (node.isLeaf) {
        results.push_back(node.bodyIndex);
        return;
    }

    if (node.left >= 0) {
        queryBoxRecursive(node.left, min, max, results);
    }
    if (node.right >= 0) {
        queryBoxRecursive(node.right, min, max, results);
    }
}

std::vector<BVH::CollisionPair> BVH::findAllCollisions(const std::vector<Vec3>& positions,
                                                         const std::vector<float>& radii) const {
    std::vector<CollisionPair> pairs;

    // 简化实现：O(n²) 检测
    // 实际应该使用 BVH 加速
    for (size_t i = 0; i < positions.size(); i++) {
        for (size_t j = i + 1; j < positions.size(); j++) {
            float dist = glm::length(positions[i] - positions[j]);
            float combinedRadius = radii[i] + radii[j];

            if (dist < combinedRadius) {
                CollisionPair pair;
                pair.indexA = static_cast<int>(i);
                pair.indexB = static_cast<int>(j);
                pairs.push_back(pair);
            }
        }
    }

    return pairs;
}

} // namespace spark
