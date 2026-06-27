#pragma once

#include "ecs/system.h"
#include "math/math_types.h"
#include "jolt_physics.h"
#include <memory>
#include <unordered_map>

namespace spark {

class JoltPhysicsWorld;

// 物理系统 - 使用 Jolt Physics
class PhysicsSystem : public System {
public:
    PhysicsSystem();
    ~PhysicsSystem() override;

    void update(Scene& scene, float deltaTime) override;

    // 设置
    void setGravity(const Vec3& gravity);
    Vec3 getGravity() const;

    // 射线检测
    bool raycast(const Vec3& origin, const Vec3& direction, float maxDistance, JoltPhysicsWorld::RaycastHit& hit);

    // 获取物理世界
    JoltPhysicsWorld& getWorld() { return *m_world; }

private:
    void syncToPhysics(Scene& scene);
    void syncFromPhysics(Scene& scene);

    std::unique_ptr<JoltPhysicsWorld> m_world;
    std::unordered_map<EntityID, PhysicsBodyHandle> m_bodyHandles;
};

} // namespace spark
