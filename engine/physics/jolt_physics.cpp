#include "jolt_physics.h"
#include "core/log.h"

// Jolt Physics 头文件
// 注意：需要通过 FetchContent 添加 Jolt
// https://github.com/jrouwe/JoltPhysics

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

using namespace JPH;

namespace spark {

// Jolt 回调类
class MyContactListener : public ContactListener {
public:
    void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override {
        CollisionEvent event;
        event.bodyA = static_cast<PhysicsBodyHandle>(inBody1.GetUserData());
        event.bodyB = static_cast<PhysicsBodyHandle>(inBody2.GetUserData());
        event.contactPoint = Vec3(inManifold.mBaseOffset.GetX(), inManifold.mBaseOffset.GetY(), inManifold.mBaseOffset.GetZ());
        event.impulse = 0.0f;
        m_events.push_back(event);
    }

    std::vector<CollisionEvent> getEvents() const { return m_events; }
    void clearEvents() { m_events.clear(); }

private:
    std::vector<CollisionEvent> m_events;
};

class MyBodyActivationListener : public BodyActivationListener {
public:
    void OnBodyActivated(const BodyID& inBodyID, uint64 inBodyUserData) override {}
    void OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData) override {}
};

// =============================================
// JoltPhysicsWorld
// =============================================

JoltPhysicsWorld::JoltPhysicsWorld() = default;

JoltPhysicsWorld::~JoltPhysicsWorld() {
    shutdown();
}

bool JoltPhysicsWorld::initialize() {
    if (m_initialized) return true;

    try {
        // 注册 Jolt 类型
        RegisterDefaultAllocator();
        Factory::sInstance = new Factory();
        RegisterTypes();

        // 创建临时分配器
        m_tempAllocator = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);  // 10MB

        // 创建作业系统
        m_jobSystem = std::make_unique<JobSystemThreadPool>(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

        // 创建物理系统
        const uint cMaxBodies = 1024;
        const uint cMaxBodyPairs = 1024;
        const uint cMaxContactConstraints = 1024;

        m_physicsSystem = std::make_unique<PhysicsSystem>();
        m_physicsSystem->Init(cMaxBodies, 0, cMaxBodyPairs, cMaxContactConstraints,
                              *new BroadPhaseLayer(), *new ObjectLayer(), *new ObjectVsBroadPhaseLayerFilter());

        // 设置重力
        m_physicsSystem->SetGravity(Vec3(0, -9.81f, 0));

        // 设置监听器
        // 注意：实际实现需要创建监听器对象
        // m_physicsSystem->SetContactListener(...);
        // m_physicsSystem->SetBodyActivationListener(...);

        m_initialized = true;
        SPARK_CORE_INFO("Jolt Physics initialized.");
        return true;

    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to initialize Jolt Physics: {0}", e.what());
        return false;
    }
}

void JoltPhysicsWorld::shutdown() {
    if (!m_initialized) return;

    m_physicsSystem.reset();
    m_jobSystem.reset();
    m_tempAllocator.reset();

    // 清理 Jolt
    UnregisterTypes();
    delete Factory::sInstance;
    Factory::sInstance = nullptr;

    m_initialized = false;
    SPARK_CORE_INFO("Jolt Physics shutdown.");
}

void JoltPhysicsWorld::update(float deltaTime) {
    if (!m_initialized) return;

    // 清除事件
    m_collisionEvents.clear();
    m_triggerEvents.clear();

    // 更新物理模拟
    // 注意：实际实现需要调用 m_physicsSystem->Update()
    // m_physicsSystem->Update(deltaTime, 1, m_tempAllocator.get(), m_jobSystem.get());

    // 收集碰撞事件
    // 实际实现会从监听器收集事件
}

PhysicsBodyHandle JoltPhysicsWorld::createBody(const PhysicsBodyDesc& desc) {
    if (!m_initialized) return InvalidBodyHandle;

    PhysicsBodyHandle handle = m_nextHandle++;

    // 创建碰撞形状
    // 注意：实际实现需要创建 Jolt 形状
    // Ref<Shape> shape;
    // switch (desc.shapeType) {
    //     case PhysicsShapeType::Box:
    //         shape = new BoxShape(Vec3(desc.halfExtents.x, desc.halfExtents.y, desc.halfExtents.z));
    //         break;
    //     case PhysicsShapeType::Sphere:
    //         shape = new SphereShape(desc.radius);
    //         break;
    //     // ...
    // }

    // 创建物体
    // BodyCreationSettings settings(shape, Vec3(desc.position.x, desc.position.y, desc.position.z),
    //                                Quat::sEulerAngles(Vec3(desc.rotation.x, desc.rotation.y, desc.rotation.z)),
    //                                static_cast<EMotionType>(desc.type), ...);

    // BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
    // BodyID bodyId = bodyInterface.CreateAndAddBody(settings, EActivation::Activate);

    // m_bodyMap[handle] = bodyId.GetIndexAndSequenceNumber();

    return handle;
}

void JoltPhysicsWorld::destroyBody(PhysicsBodyHandle handle) {
    if (!m_initialized) return;

    auto it = m_bodyMap.find(handle);
    if (it != m_bodyMap.end()) {
        // BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
        // bodyInterface.RemoveBody(BodyID(it->second));
        // bodyInterface.DestroyBody(BodyID(it->second));
        m_bodyMap.erase(it);
    }
}

void JoltPhysicsWorld::setPosition(PhysicsBodyHandle handle, const Vec3& position) {
    if (!m_initialized) return;

    auto it = m_bodyMap.find(handle);
    if (it != m_bodyMap.end()) {
        // BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
        // bodyInterface.SetPosition(BodyID(it->second), Vec3(position.x, position.y, position.z), EActivation::Activate);
    }
}

Vec3 JoltPhysicsWorld::getPosition(PhysicsBodyHandle handle) const {
    if (!m_initialized) return Vec3(0.0f);

    auto it = m_bodyMap.find(handle);
    if (it != m_bodyMap.end()) {
        // BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
        // JPH::Vec3 pos = bodyInterface.GetPosition(BodyID(it->second));
        // return Vec3(pos.GetX(), pos.GetY(), pos.GetZ());
    }

    return Vec3(0.0f);
}

void JoltPhysicsWorld::setVelocity(PhysicsBodyHandle handle, const Vec3& velocity) {
    if (!m_initialized) return;

    auto it = m_bodyMap.find(handle);
    if (it != m_bodyMap.end()) {
        // BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
        // bodyInterface.SetLinearVelocity(BodyID(it->second), Vec3(velocity.x, velocity.y, velocity.z));
    }
}

Vec3 JoltPhysicsWorld::getVelocity(PhysicsBodyHandle handle) const {
    if (!m_initialized) return Vec3(0.0f);

    auto it = m_bodyMap.find(handle);
    if (it != m_bodyMap.end()) {
        // BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
        // JPH::Vec3 vel = bodyInterface.GetLinearVelocity(BodyID(it->second));
        // return Vec3(vel.GetX(), vel.GetY(), vel.GetZ());
    }

    return Vec3(0.0f);
}

void JoltPhysicsWorld::applyForce(PhysicsBodyHandle handle, const Vec3& force) {
    if (!m_initialized) return;

    auto it = m_bodyMap.find(handle);
    if (it != m_bodyMap.end()) {
        // BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
        // bodyInterface.AddForce(BodyID(it->second), Vec3(force.x, force.y, force.z));
    }
}

void JoltPhysicsWorld::applyImpulse(PhysicsBodyHandle handle, const Vec3& impulse) {
    if (!m_initialized) return;

    auto it = m_bodyMap.find(handle);
    if (it != m_bodyMap.end()) {
        // BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
        // bodyInterface.AddImpulse(BodyID(it->second), Vec3(impulse.x, impulse.y, impulse.z));
    }
}

bool JoltPhysicsWorld::raycast(const Vec3& origin, const Vec3& direction, float maxDistance, RaycastHit& hit) const {
    if (!m_initialized) return false;

    // 注意：实际实现需要使用 Jolt 的射线检测
    // RRayCast ray(Vec3(origin.x, origin.y, origin.z), Vec3(direction.x, direction.y, direction.z) * maxDistance);
    // RayCastResult result;
    // if (m_physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result)) {
    //     hit.body = static_cast<PhysicsBodyHandle>(m_physicsSystem->GetBodyInterface().GetUserData(result.mBodyID));
    //     hit.point = origin + direction * (result.mFraction * maxDistance);
    //     hit.distance = result.mFraction * maxDistance;
    //     return true;
    // }

    return false;
}

std::vector<CollisionEvent> JoltPhysicsWorld::getCollisionEvents() const {
    return m_collisionEvents;
}

std::vector<TriggerEvent> JoltPhysicsWorld::getTriggerEvents() const {
    return m_triggerEvents;
}

void JoltPhysicsWorld::setGravity(const Vec3& gravity) {
    if (!m_initialized) return;
    // m_physicsSystem->SetGravity(Vec3(gravity.x, gravity.y, gravity.z));
}

Vec3 JoltPhysicsWorld::getGravity() const {
    if (!m_initialized) return Vec3(0, -9.81f, 0);
    // JPH::Vec3 g = m_physicsSystem->GetGravity();
    // return Vec3(g.GetX(), g.GetY(), g.GetZ());
    return Vec3(0, -9.81f, 0);
}

void JoltPhysicsWorld::drawDebug() {
    if (!m_initialized) return;

    // 调试绘制需要实现 Jolt 的 DebugRenderer
    // 这里是占位实现
}

} // namespace spark
