#include "navigation.h"
#include "core/log.h"
#include <algorithm>
#include <queue>
#include <cmath>

namespace spark {

// =============================================
// NavigationMesh
// =============================================

NavigationMesh::NavigationMesh() = default;
NavigationMesh::~NavigationMesh() = default;

void NavigationMesh::generate(const std::vector<Vec3>& vertices, const std::vector<uint32_t>& indices) {
    m_nodes.clear();

    // 创建节点
    for (const auto& vertex : vertices) {
        NavNode node;
        node.position = vertex;
        node.normal = Vec3(0.0f, 1.0f, 0.0f);
        node.walkable = true;
        m_nodes.push_back(node);
    }

    // 建立邻居关系
    for (size_t i = 0; i < indices.size(); i += 3) {
        int i0 = indices[i];
        int i1 = indices[i + 1];
        int i2 = indices[i + 2];

        if (i0 < m_nodes.size() && i1 < m_nodes.size() && i2 < m_nodes.size()) {
            // 添加邻居
            auto addNeighbor = [&](int from, int to) {
                auto& neighbors = m_nodes[from].neighbors;
                if (std::find(neighbors.begin(), neighbors.end(), to) == neighbors.end()) {
                    neighbors.push_back(to);
                }
            };

            addNeighbor(i0, i1);
            addNeighbor(i1, i0);
            addNeighbor(i1, i2);
            addNeighbor(i2, i1);
            addNeighbor(i2, i0);
            addNeighbor(i0, i2);
        }
    }

    SPARK_CORE_INFO("Navigation mesh generated: {0} nodes", m_nodes.size());
}

std::vector<Vec3> NavigationMesh::findPath(const Vec3& start, const Vec3& end) {
    std::vector<Vec3> path;

    int startNode = getClosestNode(start);
    int endNode = getClosestNode(end);

    if (startNode < 0 || endNode < 0) {
        return path;
    }

    // A* 寻路
    std::vector<int> nodePath = findPathNodes(startNode, endNode);

    // 转换为世界坐标
    for (int nodeId : nodePath) {
        if (nodeId >= 0 && nodeId < m_nodes.size()) {
            path.push_back(m_nodes[nodeId].position);
        }
    }

    return path;
}

Vec3 NavigationMesh::getClosestPoint(const Vec3& position) const {
    int nodeIndex = getClosestNode(position);
    if (nodeIndex >= 0 && nodeIndex < m_nodes.size()) {
        return m_nodes[nodeIndex].position;
    }
    return position;
}

Vec3 NavigationMesh::getRandomPoint() const {
    if (m_nodes.empty()) return Vec3(0.0f);

    // 简化：返回随机节点位置
    int index = rand() % m_nodes.size();
    return m_nodes[index].position;
}

bool NavigationMesh::isWalkable(const Vec3& position) const {
    int nodeIndex = getClosestNode(position);
    if (nodeIndex >= 0 && nodeIndex < m_nodes.size()) {
        return m_nodes[nodeIndex].walkable;
    }
    return false;
}

int NavigationMesh::getClosestNode(const Vec3& position) const {
    if (m_nodes.empty()) return -1;

    int closestIndex = 0;
    float closestDist = glm::length(m_nodes[0].position - position);

    for (size_t i = 1; i < m_nodes.size(); i++) {
        float dist = glm::length(m_nodes[i].position - position);
        if (dist < closestDist) {
            closestDist = dist;
            closestIndex = static_cast<int>(i);
        }
    }

    return closestIndex;
}

std::vector<int> NavigationMesh::findPathNodes(int startNode, int endNode) {
    std::vector<int> path;

    if (startNode < 0 || endNode < 0 || startNode >= m_nodes.size() || endNode >= m_nodes.size()) {
        return path;
    }

    // A* 算法
    struct NodeCost {
        int node;
        float f;  // g + h
        float g;  // 实际代价
        float h;  // 启发式代价
    };

    auto compare = [](const NodeCost& a, const NodeCost& b) {
        return a.f > b.f;
    };

    std::priority_queue<NodeCost, std::vector<NodeCost>, decltype(compare)> openSet(compare);
    std::unordered_map<int, int> cameFrom;
    std::unordered_map<int, float> gScore;

    // 初始化
    gScore[startNode] = 0.0f;
    float h = glm::length(m_nodes[startNode].position - m_nodes[endNode].position);
    openSet.push({startNode, h, 0.0f, h});

    while (!openSet.empty()) {
        NodeCost current = openSet.top();
        openSet.pop();

        if (current.node == endNode) {
            // 重建路径
            int node = endNode;
            while (node != startNode) {
                path.push_back(node);
                node = cameFrom[node];
            }
            path.push_back(startNode);
            std::reverse(path.begin(), path.end());
            return path;
        }

        // 检查邻居
        for (int neighbor : m_nodes[current.node].neighbors) {
            if (!m_nodes[neighbor].walkable) continue;

            float tentativeG = current.g + glm::length(m_nodes[current.node].position - m_nodes[neighbor].position);

            auto it = gScore.find(neighbor);
            if (it == gScore.end() || tentativeG < it->second) {
                gScore[neighbor] = tentativeG;
                float h = glm::length(m_nodes[neighbor].position - m_nodes[endNode].position);
                float f = tentativeG + h;

                cameFrom[neighbor] = current.node;
                openSet.push({neighbor, f, tentativeG, h});
            }
        }
    }

    return path;  // 无路径
}

// =============================================
// NavigationSystem
// =============================================

NavigationSystem& NavigationSystem::get() {
    static NavigationSystem instance;
    return instance;
}

bool NavigationSystem::initialize() {
    SPARK_CORE_INFO("Navigation system initialized.");
    return true;
}

void NavigationSystem::shutdown() {
    m_navMeshes.clear();
    SPARK_CORE_INFO("Navigation system shutdown.");
}

std::shared_ptr<NavigationMesh> NavigationSystem::createNavMesh(const std::string& name) {
    auto navMesh = std::make_shared<NavigationMesh>();
    m_navMeshes[name] = navMesh;
    SPARK_CORE_INFO("Navigation mesh created: {0}", name);
    return navMesh;
}

std::shared_ptr<NavigationMesh> NavigationSystem::getNavMesh(const std::string& name) const {
    auto it = m_navMeshes.find(name);
    if (it != m_navMeshes.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<Vec3> NavigationSystem::findPath(const std::string& navMeshName, const Vec3& start, const Vec3& end) {
    auto navMesh = getNavMesh(navMeshName);
    if (navMesh) {
        return navMesh->findPath(start, end);
    }
    return {};
}

} // namespace spark
