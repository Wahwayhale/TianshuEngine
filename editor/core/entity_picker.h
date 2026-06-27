#pragma once

#include "math/math_types.h"
#include <cstdint>

namespace spark {

class Scene;
class Camera;
class Entity;

using EntityID = uint64_t;

// 实体拾取系统
class EntityPicker {
public:
    EntityPicker();
    ~EntityPicker();

    // 从屏幕坐标拾取实体
    EntityID pickEntity(Scene& scene, Camera& camera,
                        float screenX, float screenY,
                        float viewportWidth, float viewportHeight);

    // 射线拾取
    EntityID raycastPick(Scene& scene, const Vec3& origin, const Vec3& direction, float maxDistance);

    // 屏幕坐标转世界射线
    void screenToWorldRay(Camera& camera,
                          float screenX, float screenY,
                          float viewportWidth, float viewportHeight,
                          Vec3& outOrigin, Vec3& outDirection);

private:
    // AABB 射线相交测试
    bool rayIntersectAABB(const Vec3& origin, const Vec3& direction,
                          const Vec3& boxMin, const Vec3& boxMax,
                          float& tMin, float& tMax);

    // 球体射线相交测试
    bool rayIntersectSphere(const Vec3& origin, const Vec3& direction,
                            const Vec3& center, float radius,
                            float& t);
};

} // namespace spark
