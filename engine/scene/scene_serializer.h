#pragma once

#include <string>
#include <memory>
#include "math/math_types.h"

namespace spark {

class Scene;
class Entity;

// 场景序列化器
class SceneSerializer {
public:
    SceneSerializer();
    ~SceneSerializer();

    // 序列化
    bool saveScene(Scene& scene, const std::string& filepath);
    std::string serializeScene(Scene& scene);

    // 反序列化
    bool loadScene(Scene& scene, const std::string& filepath);
    bool deserializeScene(Scene& scene, const std::string& json);

private:
    // 实体序列化
    std::string serializeEntity(Entity& entity);
    bool deserializeEntity(Scene& scene, const std::string& json);

    // 组件序列化
    std::string serializeTransform(const Vec3& position, const Vec3& rotation, const Vec3& scale);
    std::string serializeMeshRenderer(const Vec4& albedo, float metallic, float roughness, float ao);
    std::string serializeLight(int type, const Vec3& color, float intensity, float range);
    std::string serializeCamera(int projType, float fov, float nearPlane, float farPlane);

    // JSON 辅助
    std::string escapeJson(const std::string& s);
    std::string extractJsonValue(const std::string& json, const std::string& key);
    Vec3 extractVec3(const std::string& json, const std::string& key);
    Vec4 extractVec4(const std::string& json, const std::string& key);
    float extractFloat(const std::string& json, const std::string& key);
    int extractInt(const std::string& json, const std::string& key);
    bool extractBool(const std::string& json, const std::string& key);
};

} // namespace spark
