#include "collider.h"
#include <cmath>

namespace spark {

Collider::Collider(Type type)
    : m_type(type) {
}

Collider::~Collider() = default;

bool Collider::checkCollision(const Collider& other, Vec3 position, Vec3 otherPosition) const {
    if (m_type == Type::Box && other.m_type == Type::Box) {
        // AABB collision
        Vec3 minA = position - m_size;
        Vec3 maxA = position + m_size;
        Vec3 minB = otherPosition - other.m_size;
        Vec3 maxB = otherPosition + other.m_size;

        return (minA.x <= maxB.x && maxA.x >= minB.x) &&
               (minA.y <= maxB.y && maxA.y >= minB.y) &&
               (minA.z <= maxB.z && maxA.z >= minB.z);
    }

    if (m_type == Type::Sphere && other.m_type == Type::Sphere) {
        // Sphere-sphere collision
        float distance = glm::length(position - otherPosition);
        return distance < (m_radius + other.m_radius);
    }

    // Box-Sphere collision
    if (m_type == Type::Box && other.m_type == Type::Sphere) {
        Vec3 closest = glm::clamp(otherPosition, position - m_size, position + m_size);
        float distance = glm::length(otherPosition - closest);
        return distance < other.m_radius;
    }

    if (m_type == Type::Sphere && other.m_type == Type::Box) {
        Vec3 closest = glm::clamp(position, otherPosition - other.m_size, otherPosition + other.m_size);
        float distance = glm::length(position - closest);
        return distance < m_radius;
    }

    return false;
}

} // namespace spark
