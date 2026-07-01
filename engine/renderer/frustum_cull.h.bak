#pragma once

#include "math/math_types.h"
#include <array>

namespace spark {

struct BoundingBox {
    Vec3 min;
    Vec3 max;

    BoundingBox() : min(Vec3(FLT_MAX)), max(Vec3(-FLT_MAX)) {}
    BoundingBox(Vec3 min, Vec3 max) : min(min), max(max) {}

    Vec3 getCenter() const { return (min + max) * 0.5f; }
    Vec3 getSize() const { return max - min; }
    float getRadius() const { return glm::length(getSize()) * 0.5f; }

    void expand(const Vec3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }

    void expand(const BoundingBox& other) {
        min = glm::min(min, other.min);
        max = glm::max(max, other.max);
    }
};

struct BoundingSphere {
    Vec3 center;
    float radius;

    BoundingSphere() : center(Vec3(0.0f)), radius(0.0f) {}
    BoundingSphere(Vec3 center, float radius) : center(center), radius(radius) {}
};

struct Plane {
    Vec3 normal;
    float distance;

    Plane() : normal(Vec3(0.0f, 1.0f, 0.0f)), distance(0.0f) {}
    Plane(Vec3 normal, float distance) : normal(normal), distance(distance) {}

    float distanceToPoint(const Vec3& point) const {
        return glm::dot(normal, point) + distance;
    }
};

class Frustum {
public:
    Frustum() = default;

    void update(const Mat4& viewProjection);

    bool isPointVisible(const Vec3& point) const;
    bool isSphereVisible(const BoundingSphere& sphere) const;
    bool isBoxVisible(const BoundingBox& box) const;

private:
    std::array<Plane, 6> m_planes; // Left, Right, Bottom, Top, Near, Far
};

class FrustumCuller {
public:
    FrustumCuller() = default;

    void update(const Mat4& viewProjection);
    bool isVisible(const BoundingBox& box) const;
    bool isVisible(const BoundingSphere& sphere) const;

    uint32_t getVisibleCount() const { return m_visibleCount; }
    uint32_t getCulledCount() const { return m_culledCount; }

private:
    Frustum m_frustum;
    uint32_t m_visibleCount = 0;
    uint32_t m_culledCount = 0;
};

} // namespace spark
