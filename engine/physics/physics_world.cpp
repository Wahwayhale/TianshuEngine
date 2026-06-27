#include "physics_world.h"
#include "core/log.h"

namespace spark {

PhysicsWorld::PhysicsWorld(Vec3 gravity)
    : m_gravity(gravity) {
    SPARK_CORE_INFO("Physics world initialized with gravity: ({0}, {1}, {2})",
                    gravity.x, gravity.y, gravity.z);
}

PhysicsWorld::~PhysicsWorld() = default;

void PhysicsWorld::update(float deltaTime) {
    // Physics simulation will be implemented here
    // For now, this is a placeholder

    if (m_debugDraw) {
        // Debug rendering
    }
}

} // namespace spark
