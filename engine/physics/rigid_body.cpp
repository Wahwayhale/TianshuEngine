#include "rigid_body.h"

namespace spark {

RigidBody::RigidBody() = default;
RigidBody::~RigidBody() = default;

void RigidBody::applyForce(Vec3 force) {
    m_force += force;
}

void RigidBody::applyImpulse(Vec3 impulse) {
    m_velocity += impulse * m_inverseMass;
}

void RigidBody::applyTorque(Vec3 torque) {
    // Torque implementation (rotation)
}

void RigidBody::update(float deltaTime) {
    if (m_isStatic) return;

    // Apply gravity
    if (m_useGravity) {
        m_force += Vec3(0.0f, -9.81f, 0.0f) * m_mass;
    }

    // Calculate acceleration
    m_acceleration = m_force * m_inverseMass;

    // Update velocity
    m_velocity += m_acceleration * deltaTime;

    // Update position
    m_position += m_velocity * deltaTime;

    // Clear forces
    m_force = Vec3(0.0f);
}

} // namespace spark
