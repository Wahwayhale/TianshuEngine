#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <cstdint>
#include <functional>

namespace spark {

// 物理体类型
enum class PhysicsBodyType {
    Static,
    Dynamic,
    Kinematic
};

// 碰撞形状类型
enum class CollisionShapeType {
    Box,
    Sphere,
    Capsule
};

// 物理体描述
struct PhysicsBodyDesc {
    PhysicsBodyType type = PhysicsBodyType::Dynamic;
    CollisionShapeType shapeType = CollisionShapeType::Box;

    Vec3 position = Vec3(0.0f);
    Vec3 rotation = Vec3(0.0f);
    Vec3 scale = Vec3(1.0f);

    Vec3 halfExtents = Vec3(0.5f);
    float radius = 0.5f;

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

// 碰撞结果
struct CollisionResult {
    PhysicsBodyHandle bodyA;
    PhysicsBodyHandle bodyB;
    Vec3 contactPoint;
    Vec3 contactNormal;
    float penetration;
};

// 射线检测结果
struct RaycastHit {
    PhysicsBodyHandle body;
    Vec3 point;
    Vec3 normal;
    float distance;
};

// 碰撞回调
using CollisionCallback = std::function<void(const CollisionResult&)>;

// 物理世界
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

    // 属性设置
    void setPosition(PhysicsBodyHandle handle, const Vec3& position);
    void setRotation(PhysicsBodyHandle handle, const Vec3& rotation);
    void setVelocity(PhysicsBodyHandle handle, const Vec3& velocity);

    Vec3 getPosition(PhysicsBodyHandle handle) const;
    Vec3 getVelocity(PhysicsBodyHandle handle) const;

    // 力和冲量
    void applyForce(PhysicsBodyHandle handle, const Vec3& force);
    void applyImpulse(PhysicsBodyHandle handle, const Vec3& impulse);

    // 射线检测
    bool raycast(const Vec3& origin, const Vec3& direction, float maxDistance, RaycastHit& hit) const;

    // 碰撞回调
    void setCollisionCallback(CollisionCallback callback) { m_collisionCallback = callback; }

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

    std::vector<CollisionResult> m_collisionEvents;
    CollisionCallback m_collisionCallback;
};

} // namespace spark
