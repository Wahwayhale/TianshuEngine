#pragma once

#include "math/math_types.h"
#include <vector>
#include <array>

namespace spark {

// 视锥平面
struct Plane {
    Vec3 normal;
    float distance;

    float distanceToPoint(const Vec3& point) const {
        return glm::dot(normal, point) + distance;
    }
};

// 包围盒
struct BoundingBox {
    Vec3 min;
    Vec3 max;

    Vec3 getCenter() const { return (min + max) * 0.5f; }
    Vec3 getSize() const { return max - min; }
    Vec3 getExtents() const { return getSize() * 0.5f; }

    // 获取 8 个顶点
    std::array<Vec3, 8> getCorners() const {
        return {{
            Vec3(min.x, min.y, min.z),
            Vec3(max.x, min.y, min.z),
            Vec3(min.x, max.y, min.z),
            Vec3(max.x, max.y, min.z),
            Vec3(min.x, min.y, max.z),
            Vec3(max.x, min.y, max.z),
            Vec3(min.x, max.y, max.z),
            Vec3(max.x, max.y, max.z)
        }};
    }

    // 变换包围盒
    BoundingBox transform(const Mat4& matrix) const {
        BoundingBox result;
        result.min = Vec3(1e30f);
        result.max = Vec3(-1e30f);

        for (const auto& corner : getCorners()) {
            Vec4 transformed = matrix * Vec4(corner, 1.0f);
            Vec3 point = Vec3(transformed);
            result.min = glm::min(result.min, point);
            result.max = glm::max(result.max, point);
        }

        return result;
    }
};

// 包围球
struct BoundingSphere {
    Vec3 center;
    float radius;
};

// 视锥体
class Frustum {
public:
    // 从视图投影矩阵提取视锥平面
    void extractFromMatrix(const Mat4& viewProj);

    // 点是否在视锥内
    bool containsPoint(const Vec3& point) const;

    // 球体是否在视锥内
    bool containsSphere(const BoundingSphere& sphere) const;

    // AABB 是否在视锥内
    bool containsAABB(const BoundingBox& bbox) const;

    // 获取 6 个平面
    const std::array<Plane, 6>& getPlanes() const { return m_planes; }

private:
    std::array<Plane, 6> m_planes;  // 左、右、上、下、近、远
};

// 视锥剔除器
class FrustumCuller {
public:
    FrustumCuller();

    // 更新视锥
    void update(const Mat4& viewMatrix, const Mat4& projMatrix);

    // 剔除实体
    struct CullResult {
        bool visible;
        float distance;  // 到相机的距离
    };

    CullResult testBoundingBox(const BoundingBox& bbox) const;
    CullResult testBoundingSphere(const BoundingSphere& sphere) const;

    // 批量剔除
    struct VisibleObject {
        int index;
        float distance;
    };

    std::vector<VisibleObject> cullObjects(
        const std::vector<BoundingBox>& bboxes,
        const std::vector<int>& indices
    ) const;

    // 获取统计
    int getLastCulledCount() const { return m_lastCulledCount; }
    int getLastVisibleCount() const { return m_lastVisibleCount; }

private:
    Frustum m_frustum;
    Vec3 m_cameraPos;

    mutable int m_lastCulledCount = 0;
    mutable int m_lastVisibleCount = 0;
};

} // namespace spark
