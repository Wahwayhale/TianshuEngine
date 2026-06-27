#include "cloth.h"
#include "core/log.h"
#include <cmath>
#include <algorithm>

namespace spark {

ClothSimulation::ClothSimulation(const ClothSettings& settings) : m_settings(settings) {
    initializeNodes();
    initializeSprings();
    SPARK_CORE_INFO("Cloth simulation created: {0}x{1} nodes", settings.gridSizeX, settings.gridSizeY);
}

ClothSimulation::~ClothSimulation() = default;

void ClothSimulation::update(float deltaTime) {
    // 限制时间步长
    float dt = std::min(deltaTime, 1.0f / 60.0f);

    // 施加力
    applyForces(dt);

    // 求解约束
    solveConstraints(dt);

    // 积分
    integrate(dt);

    // 处理碰撞
    handleCollisions();
}

void ClothSimulation::pinNode(int index) {
    if (index >= 0 && index < static_cast<int>(m_nodes.size())) {
        m_nodes[index].pinned = true;
    }
}

void ClothSimulation::unpinNode(int index) {
    if (index >= 0 && index < static_cast<int>(m_nodes.size())) {
        m_nodes[index].pinned = false;
    }
}

void ClothSimulation::pinTopRow() {
    for (int x = 0; x < m_settings.gridSizeX; x++) {
        pinNode(x);
    }
}

void ClothSimulation::applyForce(const Vec3& force) {
    for (auto& node : m_nodes) {
        if (!node.pinned) {
            node.force += force;
        }
    }
}

void ClothSimulation::setWind(const Vec3& wind) {
    m_settings.windForce = wind;
}

void ClothSimulation::addSphereCollision(Vec3 center, float radius) {
    CollisionSphere sphere;
    sphere.center = center;
    sphere.radius = radius;
    m_collisionSpheres.push_back(sphere);
}

std::vector<Vec3> ClothSimulation::getPositions() const {
    std::vector<Vec3> positions;
    positions.reserve(m_nodes.size());
    for (const auto& node : m_nodes) {
        positions.push_back(node.position);
    }
    return positions;
}

std::vector<Vec3> ClothSimulation::getNormals() const {
    std::vector<Vec3> normals(m_nodes.size(), Vec3(0.0f, 1.0f, 0.0f));

    // 计算法线
    for (int y = 0; y < m_settings.gridSizeY - 1; y++) {
        for (int x = 0; x < m_settings.gridSizeX - 1; x++) {
            int i00 = y * m_settings.gridSizeX + x;
            int i10 = i00 + 1;
            int i01 = i00 + m_settings.gridSizeX;
            int i11 = i01 + 1;

            Vec3 v0 = m_nodes[i00].position;
            Vec3 v1 = m_nodes[i10].position;
            Vec3 v2 = m_nodes[i01].position;

            Vec3 normal = glm::cross(v1 - v0, v2 - v0);

            normals[i00] += normal;
            normals[i10] += normal;
            normals[i01] += normal;
        }
    }

    // 归一化
    for (auto& normal : normals) {
        normal = glm::normalize(normal);
    }

    return normals;
}

std::vector<uint32_t> ClothSimulation::getIndices() const {
    std::vector<uint32_t> indices;

    for (int y = 0; y < m_settings.gridSizeY - 1; y++) {
        for (int x = 0; x < m_settings.gridSizeX - 1; x++) {
            int i00 = y * m_settings.gridSizeX + x;
            int i10 = i00 + 1;
            int i01 = i00 + m_settings.gridSizeX;
            int i11 = i01 + 1;

            // 第一个三角形
            indices.push_back(i00);
            indices.push_back(i01);
            indices.push_back(i10);

            // 第二个三角形
            indices.push_back(i10);
            indices.push_back(i01);
            indices.push_back(i11);
        }
    }

    return indices;
}

void ClothSimulation::initializeNodes() {
    int nodeCount = m_settings.gridSizeX * m_settings.gridSizeY;
    m_nodes.resize(nodeCount);

    for (int y = 0; y < m_settings.gridSizeY; y++) {
        for (int x = 0; x < m_settings.gridSizeX; x++) {
            int index = y * m_settings.gridSizeX + x;

            m_nodes[index].position = Vec3(
                x * m_settings.spacing,
                0.0f,
                y * m_settings.spacing
            );
            m_nodes[index].previousPosition = m_nodes[index].position;
            m_nodes[index].velocity = Vec3(0.0f);
            m_nodes[index].force = Vec3(0.0f);
            m_nodes[index].mass = m_settings.mass;
            m_nodes[index].pinned = false;
        }
    }
}

void ClothSimulation::initializeSprings() {
    // 结构弹簧（水平和垂直）
    for (int y = 0; y < m_settings.gridSizeY; y++) {
        for (int x = 0; x < m_settings.gridSizeX; x++) {
            int index = y * m_settings.gridSizeX + x;

            // 水平弹簧
            if (x < m_settings.gridSizeX - 1) {
                ClothSpring spring;
                spring.nodeA = index;
                spring.nodeB = index + 1;
                spring.restLength = m_settings.spacing;
                spring.stiffness = m_settings.stiffness;
                spring.damping = m_settings.damping;
                m_springs.push_back(spring);
            }

            // 垂直弹簧
            if (y < m_settings.gridSizeY - 1) {
                ClothSpring spring;
                spring.nodeA = index;
                spring.nodeB = index + m_settings.gridSizeX;
                spring.restLength = m_settings.spacing;
                spring.stiffness = m_settings.stiffness;
                spring.damping = m_settings.damping;
                m_springs.push_back(spring);
            }

            // 对角弹簧（增加稳定性）
            if (x < m_settings.gridSizeX - 1 && y < m_settings.gridSizeY - 1) {
                ClothSpring spring;
                spring.nodeA = index;
                spring.nodeB = index + m_settings.gridSizeX + 1;
                spring.restLength = m_settings.spacing * sqrt(2.0f);
                spring.stiffness = m_settings.stiffness * 0.5f;
                spring.damping = m_settings.damping;
                m_springs.push_back(spring);
            }
        }
    }
}

void ClothSimulation::applyForces(float deltaTime) {
    // 重力和风力
    for (auto& node : m_nodes) {
        if (!node.pinned) {
            // 重力
            node.force += Vec3(0.0f, m_settings.gravity * node.mass, 0.0f);

            // 风力
            node.force += m_settings.windForce;
        }
    }
}

void ClothSimulation::solveConstraints(float deltaTime) {
    // 弹簧约束
    for (const auto& spring : m_springs) {
        ClothNode& nodeA = m_nodes[spring.nodeA];
        ClothNode& nodeB = m_nodes[spring.nodeB];

        Vec3 delta = nodeB.position - nodeA.position;
        float distance = glm::length(delta);

        if (distance > 0.0f) {
            Vec3 direction = delta / distance;
            float force = spring.stiffness * (distance - spring.restLength);

            // 阻尼力
            Vec3 relativeVelocity = nodeB.velocity - nodeA.velocity;
            float dampingForce = spring.damping * glm::dot(relativeVelocity, direction);

            Vec3 totalForce = (force + dampingForce) * direction;

            if (!nodeA.pinned) {
                nodeA.force += totalForce;
            }
            if (!nodeB.pinned) {
                nodeB.force -= totalForce;
            }
        }
    }
}

void ClothSimulation::integrate(float deltaTime) {
    for (auto& node : m_nodes) {
        if (node.pinned) continue;

        // Verlet 积分
        Vec3 acceleration = node.force / node.mass;
        Vec3 newPosition = node.position + (node.position - node.previousPosition) * m_settings.damping + acceleration * deltaTime * deltaTime;

        node.previousPosition = node.position;
        node.position = newPosition;

        // 清除力
        node.force = Vec3(0.0f);
    }
}

void ClothSimulation::handleCollisions() {
    for (auto& node : m_nodes) {
        if (node.pinned) continue;

        // 球体碰撞
        for (const auto& sphere : m_collisionSpheres) {
            Vec3 delta = node.position - sphere.center;
            float distance = glm::length(delta);

            if (distance < sphere.radius + m_settings.collisionRadius) {
                Vec3 normal = delta / distance;
                node.position = sphere.center + normal * (sphere.radius + m_settings.collisionRadius);
            }
        }

        // 地面碰撞
        if (node.position.y < 0.0f) {
            node.position.y = 0.0f;
        }
    }
}

} // namespace spark
