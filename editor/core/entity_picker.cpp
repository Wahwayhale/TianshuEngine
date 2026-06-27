#include "entity_picker.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include "renderer/camera.h"
#include "core/log.h"
#include <algorithm>
#include <cmath>

namespace spark {

EntityPicker::EntityPicker() = default;
EntityPicker::~EntityPicker() = default;

EntityID EntityPicker::pickEntity(Scene& scene, Camera& camera,
                                   float screenX, float screenY,
                                   float viewportWidth, float viewportHeight) {
    Vec3 origin, direction;
    screenToWorldRay(camera, screenX, screenY, viewportWidth, viewportHeight, origin, direction);

    return raycastPick(scene, origin, direction, 1000.0f);
}

EntityID EntityPicker::raycastPick(Scene& scene, const Vec3& origin, const Vec3& direction, float maxDistance) {
    EntityID closestEntity = 0;
    float closestDistance = maxDistance;

    // 遍历所有有 TransformComponent 的实体
    scene.view<TransformComponent>([&](Entity& entity) {
        auto& transform = entity.getComponent<TransformComponent>();

        // 计算 AABB
        Vec3 halfSize = transform.scale * 0.5f;
        Vec3 boxMin = transform.position - halfSize;
        Vec3 boxMax = transform.position + halfSize;

        // 射线-AABB 相交测试
        float tMin, tMax;
        if (rayIntersectAABB(origin, direction, boxMin, boxMax, tMin, tMax)) {
            if (tMin > 0 && tMin < closestDistance) {
                closestDistance = tMin;
                closestEntity = entity.getID();
            }
        }
    });

    return closestEntity;
}

void EntityPicker::screenToWorldRay(Camera& camera,
                                     float screenX, float screenY,
                                     float viewportWidth, float viewportHeight,
                                     Vec3& outOrigin, Vec3& outDirection) {
    // 将屏幕坐标转换为 NDC (-1 到 1)
    float ndcX = (2.0f * screenX / viewportWidth) - 1.0f;
    float ndcY = 1.0f - (2.0f * screenY / viewportHeight);

    // 获取相机矩阵
    Mat4 viewMatrix = camera.getViewMatrix();
    Mat4 projMatrix = camera.getProjectionMatrix(viewportWidth / viewportHeight);

    // 计算逆矩阵
    Mat4 invViewProj = glm::inverse(projMatrix * viewMatrix);

    // 近平面和远平面的点
    Vec4 nearPoint = invViewProj * Vec4(ndcX, ndcY, 0.0f, 1.0f);
    Vec4 farPoint = invViewProj * Vec4(ndcX, ndcY, 1.0f, 1.0f);

    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;

    // 射线原点和方向
    outOrigin = Vec3(nearPoint);
    outDirection = glm::normalize(Vec3(farPoint) - Vec3(nearPoint));
}

bool EntityPicker::rayIntersectAABB(const Vec3& origin, const Vec3& direction,
                                     const Vec3& boxMin, const Vec3& boxMax,
                                     float& tMin, float& tMax) {
    Vec3 invDir = 1.0f / direction;

    Vec3 t1 = (boxMin - origin) * invDir;
    Vec3 t2 = (boxMax - origin) * invDir;

    Vec3 tMinVec = glm::min(t1, t2);
    Vec3 tMaxVec = glm::max(t1, t2);

    tMin = std::max({tMinVec.x, tMinVec.y, tMinVec.z});
    tMax = std::min({tMaxVec.x, tMaxVec.y, tMaxVec.z});

    return tMax >= tMin && tMax > 0;
}

bool EntityPicker::rayIntersectSphere(const Vec3& origin, const Vec3& direction,
                                       const Vec3& center, float radius,
                                       float& t) {
    Vec3 oc = origin - center;
    float a = glm::dot(direction, direction);
    float b = 2.0f * glm::dot(oc, direction);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return false;
    }

    t = (-b - std::sqrt(discriminant)) / (2.0f * a);
    return t > 0;
}

} // namespace spark
