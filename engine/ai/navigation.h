#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace spark {

// 导航网格节点
struct NavNode {
    Vec3 position;
    Vec3 normal;
    std::vector<int> neighbors;
    float cost = 1.0f;
    bool walkable = true;
};

// 导航网格
class NavigationMesh {
public:
    NavigationMesh();
    ~NavigationMesh();

    // 生成导航网格
    void generate(const std::vector<Vec3>& vertices, const std::vector<uint32_t>& indices);

    // 路径查找
    std::vector<Vec3> findPath(const Vec3& start, const Vec3& end);

    // 查询
    Vec3 getClosestPoint(const Vec3& position) const;
    Vec3 getRandomPoint() const;
    bool isWalkable(const Vec3& position) const;

    // 节点
    const std::vector<NavNode>& getNodes() const { return m_nodes; }

private:
    int getClosestNode(const Vec3& position) const;
    std::vector<int> findPathNodes(int startNode, int endNode);

    std::vector<NavNode> m_nodes;
};

// 导航系统
class NavigationSystem {
public:
    static NavigationSystem& get();

    // 初始化
    bool initialize();
    void shutdown();

    // 导航网格管理
    std::shared_ptr<NavigationMesh> createNavMesh(const std::string& name);
    std::shared_ptr<NavigationMesh> getNavMesh(const std::string& name) const;

    // 路径查找
    std::vector<Vec3> findPath(const std::string& navMeshName, const Vec3& start, const Vec3& end);

private:
    NavigationSystem() = default;

    std::unordered_map<std::string, std::shared_ptr<NavigationMesh>> m_navMeshes;
};

} // namespace spark
