#include "frustum_cull.h"
#include <cmath>

namespace spark {

void Frustum::update(const Mat4& viewProjection) {
    // Extract frustum planes from view-projection matrix
    // Left plane
    m_planes[0].normal.x = viewProjection[0][3] + viewProjection[0][0];
    m_planes[0].normal.y = viewProjection[1][3] + viewProjection[1][0];
    m_planes[0].normal.z = viewProjection[2][3] + viewProjection[2][0];
    m_planes[0].distance = viewProjection[3][3] + viewProjection[3][0];

    // Right plane
    m_planes[1].normal.x = viewProjection[0][3] - viewProjection[0][0];
    m_planes[1].normal.y = viewProjection[1][3] - viewProjection[1][0];
    m_planes[1].normal.z = viewProjection[2][3] - viewProjection[2][0];
    m_planes[1].distance = viewProjection[3][3] - viewProjection[3][0];

    // Bottom plane
    m_planes[2].normal.x = viewProjection[0][3] + viewProjection[0][1];
    m_planes[2].normal.y = viewProjection[1][3] + viewProjection[1][1];
    m_planes[2].normal.z = viewProjection[2][3] + viewProjection[2][1];
    m_planes[2].distance = viewProjection[3][3] + viewProjection[3][1];

    // Top plane
    m_planes[3].normal.x = viewProjection[0][3] - viewProjection[0][1];
    m_planes[3].normal.y = viewProjection[1][3] - viewProjection[1][1];
    m_planes[3].normal.z = viewProjection[2][3] - viewProjection[2][1];
    m_planes[3].distance = viewProjection[3][3] - viewProjection[3][1];

    // Near plane
    m_planes[4].normal.x = viewProjection[0][3] + viewProjection[0][2];
    m_planes[4].normal.y = viewProjection[1][3] + viewProjection[1][2];
    m_planes[4].normal.z = viewProjection[2][3] + viewProjection[2][2];
    m_planes[4].distance = viewProjection[3][3] + viewProjection[3][2];

    // Far plane
    m_planes[5].normal.x = viewProjection[0][3] - viewProjection[0][2];
    m_planes[5].normal.y = viewProjection[1][3] - viewProjection[1][2];
    m_planes[5].normal.z = viewProjection[2][3] - viewProjection[2][2];
    m_planes[5].distance = viewProjection[3][3] - viewProjection[3][2];

    // Normalize planes
    for (auto& plane : m_planes) {
        float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.distance /= length;
    }
}

bool Frustum::isPointVisible(const Vec3& point) const {
    for (const auto& plane : m_planes) {
        if (plane.distanceToPoint(point) < 0) {
            return false;
        }
    }
    return true;
}

bool Frustum::isSphereVisible(const BoundingSphere& sphere) const {
    for (const auto& plane : m_planes) {
        float distance = plane.distanceToPoint(sphere.center);
        if (distance < -sphere.radius) {
            return false;
        }
    }
    return true;
}

bool Frustum::isBoxVisible(const BoundingBox& box) const {
    for (const auto& plane : m_planes) {
        Vec3 positiveVertex = box.min;
        if (plane.normal.x >= 0) positiveVertex.x = box.max.x;
        if (plane.normal.y >= 0) positiveVertex.y = box.max.y;
        if (plane.normal.z >= 0) positiveVertex.z = box.max.z;

        if (plane.distanceToPoint(positiveVertex) < 0) {
            return false;
        }
    }
    return true;
}

void FrustumCuller::update(const Mat4& viewProjection) {
    m_frustum.update(viewProjection);
    m_visibleCount = 0;
    m_culledCount = 0;
}

bool FrustumCuller::isVisible(const BoundingBox& box) const {
    if (m_frustum.isBoxVisible(box)) {
        m_visibleCount++;
        return true;
    } else {
        m_culledCount++;
        return false;
    }
}

bool FrustumCuller::isVisible(const BoundingSphere& sphere) const {
    if (m_frustum.isSphereVisible(sphere)) {
        m_visibleCount++;
        return true;
    } else {
        m_culledCount++;
        return false;
    }
}

} // namespace spark
