#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <functional>

namespace spark {

struct CollisionInfo {
    Vec3 contactPoint;
    Vec3 contactNormal;
    float penetrationDepth;
};

class PhysicsWorld {
public:
    PhysicsWorld(Vec3 gravity = Vec3(0.0f, -9.81f, 0.0f));
    ~PhysicsWorld();

    void update(float deltaTime);

    // Collision detection
    using CollisionCallback = std::function<void(const CollisionInfo&)>;
    void setCollisionCallback(CollisionCallback callback) { m_collisionCallback = callback; }

    // Debug
    void setDebugDraw(bool enable) { m_debugDraw = enable; }
    bool isDebugDrawEnabled() const { return m_debugDraw; }

    Vec3 getGravity() const { return m_gravity; }
    void setGravity(Vec3 gravity) { m_gravity = gravity; }

private:
    Vec3 m_gravity;
    bool m_debugDraw = false;
    CollisionCallback m_collisionCallback;
};

} // namespace spark
