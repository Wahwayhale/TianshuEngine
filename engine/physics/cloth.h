#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>

namespace spark {

// 布料设置
struct ClothSettings {
    int gridSizeX = 20;                // 网格大小 X
    int gridSizeY = 20;                // 网格大小 Y
    float spacing = 0.1f;              // 节点间距
    float mass = 0.1f;                 // 节点质量
    float stiffness = 100.0f;          // 弹簧刚度
    float damping = 0.9f;              // 阻尼
    float gravity = -9.81f;            // 重力
    Vec3 windForce = Vec3(0.0f);       // 风力
    float collisionRadius = 0.05f;     // 碰撞半径
};

// 布料节点
struct ClothNode {
    Vec3 position;
    Vec3 previousPosition;
    Vec3 velocity;
    Vec3 force;
    float mass;
    bool pinned;                       // 是否固定
};

// 布料弹簧
struct ClothSpring {
    int nodeA;
    int nodeB;
    float restLength;
    float stiffness;
    float damping;
};

// 布料模拟
class ClothSimulation {
public:
    ClothSimulation(const ClothSettings& settings);
    ~ClothSimulation();

    // 更新模拟
    void update(float deltaTime);

    // 设置固定点
    void pinNode(int index);
    void unpinNode(int index);
    void pinTopRow();

    // 施加力
    void applyForce(const Vec3& force);
    void setWind(const Vec3& wind);

    // 碰撞
    void addSphereCollision(Vec3 center, float radius);

    // 获取数据
    const std::vector<ClothNode>& getNodes() const { return m_nodes; }
    const std::vector<ClothSpring>& getSprings() const { return m_springs; }
    int getGridSizeX() const { return m_settings.gridSizeX; }
    int getGridSizeY() const { return m_settings.gridSizeY; }

    // 渲染数据
    std::vector<Vec3> getPositions() const;
    std::vector<Vec3> getNormals() const;
    std::vector<uint32_t> getIndices() const;

private:
    void initializeNodes();
    void initializeSprings();
    void applyForces(float deltaTime);
    void solveConstraints(float deltaTime);
    void integrate(float deltaTime);
    void handleCollisions();

    ClothSettings m_settings;
    std::vector<ClothNode> m_nodes;
    std::vector<ClothSpring> m_springs;

    // 碰撞球体
    struct CollisionSphere {
        Vec3 center;
        float radius;
    };
    std::vector<CollisionSphere> m_collisionSpheres;
};

} // namespace spark
