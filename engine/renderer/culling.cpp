#include "culling.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

// =============================================
// Frustum
// =============================================

void Frustum::extractFromMatrix(const Mat4& viewProj) {
    // 从视图投影矩阵提取 6 个平面
    // 使用 Gribb-Hartmann 方法

    // 左平面
    m_planes[0].normal.x = viewProj[0][3] + viewProj[0][0];
    m_planes[0].normal.y = viewProj[1][3] + viewProj[1][0];
    m_planes[0].normal.z = viewProj[2][3] + viewProj[2][0];
    m_planes[0].distance = viewProj[3][3] + viewProj[3][0];

    // 右平面
    m_planes[1].normal.x = viewProj[0][3] - viewProj[0][0];
    m_planes[1].normal.y = viewProj[1][3] - viewProj[1][0];
    m_planes[1].normal.z = viewProj[2][3] - viewProj[2][0];
    m_planes[1].distance = viewProj[3][3] - viewProj[3][0];

    // 上平面
    m_planes[2].normal.x = viewProj[0][3] - viewProj[0][1];
    m_planes[2].normal.y = viewProj[1][3] - viewProj[1][1];
    m_planes[2].normal.z = viewProj[2][3] - viewProj[2][1];
    m_planes[2].distance = viewProj[3][3] - viewProj[3][1];

    // 下平面
    m_planes[3].normal.x = viewProj[0][3] + viewProj[0][1];
    m_planes[3].normal.y = viewProj[1][3] + viewProj[1][1];
    m_planes[3].normal.z = viewProj[2][3] + viewProj[2][1];
    m_planes[3].distance = viewProj[3][3] + viewProj[3][1];

    // 近平面
    m_planes[4].normal.x = viewProj[0][3] + viewProj[0][2];
    m_planes[4].normal.y = viewProj[1][3] + viewProj[1][2];
    m_planes[4].normal.z = viewProj[2][3] + viewProj[2][2];
    m_planes[4].distance = viewProj[3][3] + viewProj[3][2];

    // 远平面
    m_planes[5].normal.x = viewProj[0][3] - viewProj[0][2];
    m_planes[5].normal.y = viewProj[1][3] - viewProj[1][2];
    m_planes[5].normal.z = viewProj[2][3] - viewProj[2][2];
    m_planes[5].distance = viewProj[3][3] - viewProj[3][2];

    // 归一化平面
    for (auto& plane : m_planes) {
        float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.distance /= length;
    }
}

bool Frustum::containsPoint(const Vec3& point) const {
    for (const auto& plane : m_planes) {
        if (plane.distanceToPoint(point) < 0) {
            return false;
        }
    }
    return true;
}

bool Frustum::containsSphere(const BoundingSphere& sphere) const {
    for (const auto& plane : m_planes) {
        float distance = plane.distanceToPoint(sphere.center);
        if (distance < -sphere.radius) {
            return false;
        }
    }
    return true;
}

bool Frustum::containsAABB(const BoundingBox& bbox) const {
    for (const auto& plane : m_planes) {
        // 找到法线方向最远的顶点
        Vec3 positiveVertex;
        positiveVertex.x = (plane.normal.x >= 0) ? bbox.max.x : bbox.min.x;
        positiveVertex.y = (plane.normal.y >= 0) ? bbox.max.y : bbox.min.y;
        positiveVertex.z = (plane.normal.z >= 0) ? bbox.max.z : bbox.min.z;

        if (plane.distanceToPoint(positiveVertex) < 0) {
            return false;
        }
    }
    return true;
}

// =============================================
// FrustumCuller
// =============================================

FrustumCuller::FrustumCuller() = default;

void FrustumCuller::update(const Mat4& viewMatrix, const Mat4& projMatrix) {
    Mat4 viewProj = projMatrix * viewMatrix;
    m_frustum.extractFromMatrix(viewProj);

    // 提取相机位置
    Mat4 invView = glm::inverse(viewMatrix);
    m_cameraPos = Vec3(invView[3]);
}

FrustumCuller::CullResult FrustumCuller::testBoundingBox(const BoundingBox& bbox) const {
    CullResult result;
    result.visible = m_frustum.containsAABB(bbox);
    result.distance = glm::length(bbox.getCenter() - m_cameraPos);
    return result;
}

FrustumCuller::CullResult FrustumCuller::testBoundingSphere(const BoundingSphere& sphere) const {
    CullResult result;
    result.visible = m_frustum.containsSphere(sphere);
    result.distance = glm::length(sphere.center - m_cameraPos);
    return result;
}

std::vector<FrustumCuller::VisibleObject> FrustumCuller::cullObjects(
    const std::vector<BoundingBox>& bboxes,
    const std::vector<int>& indices) const {

    std::vector<VisibleObject> visibleObjects;
    m_lastCulledCount = 0;
    m_lastVisibleCount = 0;

    for (int index : indices) {
        if (index >= 0 && index < static_cast<int>(bboxes.size())) {
            auto result = testBoundingBox(bboxes[index]);

            if (result.visible) {
                VisibleObject obj;
                obj.index = index;
                obj.distance = result.distance;
                visibleObjects.push_back(obj);
                m_lastVisibleCount++;
            } else {
                m_lastCulledCount++;
            }
        }
    }

    // 按距离排序（从前到后）
    std::sort(visibleObjects.begin(), visibleObjects.end(),
        [](const VisibleObject& a, const VisibleObject& b) {
            return a.distance < b.distance;
        });

    return visibleObjects;
}

} // namespace spark
