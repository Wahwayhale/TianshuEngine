#pragma once

#include "math/math_types.h"
#include <memory>

namespace spark {

class Collider {
public:
    enum class Type { Box, Sphere, Capsule, Mesh };

    Collider(Type type = Type::Box);
    ~Collider();

    // Getters/Setters
    Type getType() const { return m_type; }

    Vec3 getSize() const { return m_size; }
    void setSize(Vec3 size) { m_size = size; }

    float getRadius() const { return m_radius; }
    void setRadius(float radius) { m_radius = radius; }

    float getHeight() const { return m_height; }
    void setHeight(float height) { m_height = height; }

    bool isTrigger() const { return m_isTrigger; }
    void setTrigger(bool trigger) { m_isTrigger = trigger; }

    // Collision detection
    bool checkCollision(const Collider& other, Vec3 position, Vec3 otherPosition) const;

private:
    Type m_type;
    Vec3 m_size = Vec3(0.5f);
    float m_radius = 0.5f;
    float m_height = 1.0f;
    bool m_isTrigger = false;
};

} // namespace spark
