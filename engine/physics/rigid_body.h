#pragma once

#include "math/math_types.h"

namespace spark {

class RigidBody {
public:
    RigidBody();
    ~RigidBody();

    void applyForce(Vec3 force);
    void applyImpulse(Vec3 impulse);
    void applyTorque(Vec3 torque);

    void update(float deltaTime);

    // Getters/Setters
    Vec3 getPosition() const { return m_position; }
    void setPosition(Vec3 pos) { m_position = pos; }

    Vec3 getVelocity() const { return m_velocity; }
    void setVelocity(Vec3 vel) { m_velocity = vel; }

    Vec3 getAcceleration() const { return m_acceleration; }

    float getMass() const { return m_mass; }
    void setMass(float mass) { m_mass = mass; m_inverseMass = (mass > 0.0f) ? 1.0f / mass : 0.0f; }

    bool isStatic() const { return m_isStatic; }
    void setStatic(bool isStatic) { m_isStatic = isStatic; }

    bool useGravity() const { return m_useGravity; }
    void setUseGravity(bool use) { m_useGravity = use; }

    float getRestitution() const { return m_restitution; }
    void setRestitution(float r) { m_restitution = r; }

    float getFriction() const { return m_friction; }
    void setFriction(float f) { m_friction = f; }

private:
    Vec3 m_position = Vec3(0.0f);
    Vec3 m_velocity = Vec3(0.0f);
    Vec3 m_acceleration = Vec3(0.0f);
    Vec3 m_force = Vec3(0.0f);

    float m_mass = 1.0f;
    float m_inverseMass = 1.0f;
    bool m_isStatic = false;
    bool m_useGravity = true;

    float m_restitution = 0.5f;
    float m_friction = 0.3f;
};

} // namespace spark
