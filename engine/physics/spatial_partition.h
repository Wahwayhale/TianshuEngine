#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <cstdint>

namespace spark {

using PhysicsBodyHandle = uint32_t;

// 空间哈希格子
struct SpatialCell {
    std::vector<PhysicsBodyHandle> bodies;
};

// 空间哈希
class SpatialHash {
public:
    SpatialHash(float cellSize = 10.0f);
    ~SpatialHash();

    // 插入/移除
    void insert(PhysicsBodyHandle handle, const Vec3& position, float radius);
    void remove(PhysicsBodyHandle handle);
    void update(PhysicsBodyHandle handle, const Vec3& position, float radius);

    // 查询
    std::vector<PhysicsBodyHandle> query(const Vec3& position, float radius) const;
    std::vector<PhysicsBodyHandle> queryBox(const Vec3& min, const Vec3& max) const;

    // 清除
    void clear();

    // 统计
    int getBodyCount() const { return m_bodyCount; }
    int getCellCount() const { return static_cast<int>(m_cells.size()); }

private:
    uint64_t hashCell(int x, int y, int z) const;
    void getCellCoords(const Vec3& position, int& x, int& y, int& z) const;

    float m_cellSize;
    float m_invCellSize;
    int m_bodyCount = 0;

    std::unordered_map<uint64_t, SpatialCell> m_cells;
    std::unordered_map<PhysicsBodyHandle, Vec3> m_bodyPositions;
    std::unordered_map<PhysicsBodyHandle, float> m_bodyRadii;
};

// BVH 节点
struct BVHNode {
    Vec3 min;
    Vec3 max;
    int left = -1;
    int right = -1;
    int bodyIndex = -1;
    bool isLeaf = false;
};

// BVH 加速结构
class BVH {
public:
    BVH();
    ~BVH();

    // 构建
    void build(const std::vector<Vec3>& positions, const std::vector<float>& radii);
    void update(const std::vector<Vec3>& positions, const std::vector<float>& radii);

    // 查询
    std::vector<int> query(const Vec3& position, float radius) const;
    std::vector<int> queryBox(const Vec3& min, const Vec3& max) const;

    // 碰撞对
    struct CollisionPair {
        int indexA;
        int indexB;
    };

    std::vector<CollisionPair> findAllCollisions(const std::vector<Vec3>& positions,
                                                  const std::vector<float>& radii) const;

    // 统计
    int getNodeCount() const { return static_cast<int>(m_nodes.size()); }

private:
    int buildRecursive(std::vector<int>& indices, int start, int end,
                       const std::vector<Vec3>& positions, const std::vector<float>& radii);

    void queryRecursive(int nodeIndex, const Vec3& position, float radius,
                        std::vector<int>& results) const;

    void queryBoxRecursive(int nodeIndex, const Vec3& min, const Vec3& max,
                           std::vector<int>& results) const;

    std::vector<BVHNode> m_nodes;
    int m_rootIndex = -1;
};

} // namespace spark
