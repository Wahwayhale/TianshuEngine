#pragma once

#include "math/math_types.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace spark {

class Entity;
class Scene;

// Jolt Physics 前向声明
namespace JPH {
    class PhysicsSystem;
    class BodyInterface;
    class BodyID;
    class TempAllocator;
    class JobSystemThreadPool;
}

// 物理体类型
enum class PhysicsBodyType {
    Static,     // 静态物体（不受力影响）
    Dynamic,    // 动态物体（受力影响）
    Kinematic   // 运动学物体（不受力但可碰撞）
};

// 碰撞形状
enum class PhysicsShapeType {
    Box,
    Sphere,
    Capsule,
    Cylinder,
    Mesh
};

// 物理体创建信息
struct PhysicsBodyDesc {
    PhysicsBodyType type = PhysicsBodyType::Dynamic;
    PhysicsShapeType shapeType = PhysicsShapeType::Box;

    Vec3 position = Vec3(0.0f);
    Vec3 rotation = Vec3(0.0f);  // 欧拉角（度）
    Vec3 scale = Vec3(1.0f);

    // Box 参数
    Vec3 halfExtents = Vec3(0.5f);

    // Sphere 参数
    float radius = 0.5f;

    // Capsule 参数
    float capsuleRadius = 0.5f;
    float capsuleHeight = 1.0f;

    // 物理属性
    float mass = 1.0f;
    float friction = 0.5f;
    float restitution = 0.3f;
    float linearDamping = 0.05f;
    float angularDamping = 0.05f;

    // 是否是触发器
    bool isTrigger = false;

    // 用户数据
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

// Jolt Physics 世界封装
class JoltPhysicsWorld {
public:
    JoltPhysicsWorld();
    ~JoltPhysicsWorld();

    // 初始化/关闭
    bool initialize();
    void shutdown();

    // 更新物理模拟
    void update(float deltaTime);

    // 物体管理
    PhysicsBodyHandle createBody(const PhysicsBodyDesc& desc);
    void destroyBody(PhysicsBodyHandle handle);

    // 物体属性
    void setPosition(PhysicsBodyHandle handle, const Vec3& position);
    void setRotation(PhysicsBodyHandle handle, const Vec3& rotation);
    void setScale(PhysicsBodyHandle handle, const Vec3& scale);
    void setVelocity(PhysicsBodyHandle handle, const Vec3& velocity);
    void setAngularVelocity(PhysicsBodyHandle handle, const Vec3& velocity);
    void setMass(PhysicsBodyHandle handle, float mass);
    void setFriction(PhysicsBodyHandle handle, float friction);
    void setRestitution(PhysicsBodyHandle handle, float restitution);

    Vec3 getPosition(PhysicsBodyHandle handle) const;
    Vec3 getRotation(PhysicsBodyHandle handle) const;
    Vec3 getVelocity(PhysicsBodyHandle handle) const;
    Vec3 getAngularVelocity(PhysicsBodyHandle handle) const;

    // 力和冲量
    void applyForce(PhysicsBodyHandle handle, const Vec3& force);
    void applyImpulse(PhysicsBodyHandle handle, const Vec3& impulse);
    void applyTorque(PhysicsBodyHandle handle, const Vec3& torque);
    void applyAngularImpulse(PhysicsBodyHandle handle, const Vec3& impulse);

    // 射线检测
    struct RaycastHit {
        PhysicsBodyHandle body;
        Vec3 point;
        Vec3 normal;
        float distance;
    };

    bool raycast(const Vec3& origin, const Vec3& direction, float maxDistance, RaycastHit& hit) const;

    // 碰撞事件
    std::vector<CollisionEvent> getCollisionEvents() const;
    std::vector<TriggerEvent> getTriggerEvents() const;

    // 重力
    void setGravity(const Vec3& gravity);
    Vec3 getGravity() const;

    // 调试绘制
    void drawDebug();

    bool isInitialized() const { return m_initialized; }

private:
    bool m_initialized = false;

    // Jolt 内部对象
    std::unique_ptr<JPH::PhysicsSystem> m_physicsSystem;
    std::unique_ptr<JPH::TempAllocator> m_tempAllocator;
    std::unique_ptr<JPH::JobSystemThreadPool> m_jobSystem;

    // 物体映射
    std::unordered_map<PhysicsBodyHandle, uint32_t> m_bodyMap;  // Handle -> Jolt BodyID
    PhysicsBodyHandle m_nextHandle = 1;

    // 事件队列
    std::vector<CollisionEvent> m_collisionEvents;
    std::vector<TriggerEvent> m_triggerEvents;
};

} // namespace spark
