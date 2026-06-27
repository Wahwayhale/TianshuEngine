#pragma once

#include "math/math_types.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace spark {

// 物理体类型
enum class PhysicsBodyType {
    Static,
    Dynamic,
    Kinematic
};

// 碰撞形状
enum class PhysicsShapeType {
    Box,
    Sphere,
    Capsule
};

// 物理体创建信息
struct PhysicsBodyDesc {
    PhysicsBodyType type = PhysicsBodyType::Dynamic;
    PhysicsShapeType shapeType = PhysicsShapeType::Box;

    Vec3 position = Vec3(0.0f);
    Vec3 rotation = Vec3(0.0f);
    Vec3 scale = Vec3(1.0f);

    Vec3 halfExtents = Vec3(0.5f);
    float radius = 0.5f;
    float capsuleRadius = 0.5f;
    float capsuleHeight = 1.0f;

    float mass = 1.0f;
    float friction = 0.5f;
    float restitution = 0.3f;
    float linearDamping = 0.05f;
    float angularDamping = 0.05f;

    bool isTrigger = false;
    uint64_t userData = 0;
};

// 物理体句柄
using PhysicsBodyHandle = uint32_t;
constexpr PhysicsBodyHandle InvalidBodyHandle = 0;

// 碰撞事件
struct CollisionEvent {
    PhysicsBodyHandle bodyA;
    PhysicsBodyHandle bodyB;
    Vec3 contactPoint;
    Vec3 contactNormal;
    float impulse;
};

// 触发器事件
struct TriggerEvent {
    PhysicsBodyHandle triggerBody;
    PhysicsBodyHandle otherBody;
    bool isEntering;
};

// 射线检测结果
struct RaycastHit {
    PhysicsBodyHandle body;
    Vec3 point;
    Vec3 normal;
    float distance;
};

// 简化的物理世界（不依赖 Jolt，使用自研实现）
class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();

    bool initialize();
    void shutdown();

    void update(float deltaTime);

    // 物体管理
    PhysicsBodyHandle createBody(const PhysicsBodyDesc& desc);
    void destroyBody(PhysicsBodyHandle handle);

    // 物体属性
    void setPosition(PhysicsBodyHandle handle, const Vec3& position);
    void setRotation(PhysicsBodyHandle handle, const Vec3& rotation);
    void setVelocity(PhysicsBodyHandle handle, const Vec3& velocity);
    void setAngularVelocity(PhysicsBodyHandle handle, const Vec3& velocity);

    Vec3 getPosition(PhysicsBodyHandle handle) const;
    Vec3 getRotation(PhysicsBodyHandle handle) const;
    Vec3 getVelocity(PhysicsBodyHandle handle) const;
    Vec3 getAngularVelocity(PhysicsBodyHandle handle) const;

    // 力和冲量
    void applyForce(PhysicsBodyHandle handle, const Vec3& force);
    void applyImpulse(PhysicsBodyHandle handle, const Vec3& impulse);
    void applyTorque(PhysicsBodyHandle handle, const Vec3& torque);

    // 射线检测
    bool raycast(const Vec3& origin, const Vec3& direction, float maxDistance, RaycastHit& hit) const;

    // 碰撞事件
    std::vector<CollisionEvent> getCollisionEvents() const;
    std::vector<TriggerEvent> getTriggerEvents() const;

    // 重力
    void setGravity(const Vec3& gravity) { m_gravity = gravity; }
    Vec3 getGravity() const { return m_gravity; }

    bool isInitialized() const { return m_initialized; }

private:
    struct PhysicsBody {
        PhysicsBodyDesc desc;
        Vec3 position;
        Vec3 rotation;
        Vec3 velocity;
        Vec3 angularVelocity;
        Vec3 force;
        Vec3 torque;
        bool isStatic;
    };

    void integrateVelocities(float deltaTime);
    void detectCollisions();
    void resolveCollisions();
    void integratePositions(float deltaTime);

    bool m_initialized = false;
    Vec3 m_gravity = Vec3(0.0f, -9.81f, 0.0f);

    std::unordered_map<PhysicsBodyHandle, PhysicsBody> m_bodies;
    PhysicsBodyHandle m_nextHandle = 1;

    std::vector<CollisionEvent> m_collisionEvents;
    std::vector<TriggerEvent> m_triggerEvents;
};

} // namespace spark
