#include "scene_serializer.h"
#include "scene.h"
#include "ecs/components.h"
#include "core/log.h"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace spark {

SceneSerializer::SceneSerializer() = default;
SceneSerializer::~SceneSerializer() = default;

bool SceneSerializer::saveScene(Scene& scene, const std::string& filepath) {
    std::string json = serializeScene(scene);

    std::ofstream file(filepath);
    if (!file.is_open()) {
        SPARK_CORE_ERROR("Failed to save scene: {0}", filepath);
        return false;
    }

    file << json;
    file.close();

    SPARK_CORE_INFO("Scene saved: {0}", filepath);
    return true;
}

std::string SceneSerializer::serializeScene(Scene& scene) {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"version\": \"1.0\",\n";
    ss << "  \"name\": \"Scene\",\n";
    ss << "  \"entities\": [\n";

    bool first = true;
    for (const auto& [id, entity] : scene.getEntities()) {
        if (!first) ss << ",\n";
        first = false;
        ss << "    " << serializeEntity(*entity);
    }

    ss << "\n  ]\n";
    ss << "}\n";

    return ss.str();
}

bool SceneSerializer::loadScene(Scene& scene, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        SPARK_CORE_ERROR("Failed to load scene: {0}", filepath);
        return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    std::string json = ss.str();

    file.close();

    return deserializeScene(scene, json);
}

bool SceneSerializer::deserializeScene(Scene& scene, const std::string& json) {
    // 简化的 JSON 解析
    // 找到 entities 数组
    size_t entitiesStart = json.find("\"entities\"");
    if (entitiesStart == std::string::npos) {
        SPARK_CORE_ERROR("Invalid scene format: no entities array");
        return false;
    }

    size_t arrayStart = json.find("[", entitiesStart);
    size_t arrayEnd = json.find("]", arrayStart);

    if (arrayStart == std::string::npos || arrayEnd == std::string::npos) {
        SPARK_CORE_ERROR("Invalid scene format: malformed entities array");
        return false;
    }

    std::string entitiesJson = json.substr(arrayStart + 1, arrayEnd - arrayStart - 1);

    // 解析每个实体
    size_t pos = 0;
    while (pos < entitiesJson.size()) {
        size_t objStart = entitiesJson.find("{", pos);
        if (objStart == std::string::npos) break;

        int depth = 0;
        size_t objEnd = objStart;
        for (size_t i = objStart; i < entitiesJson.size(); i++) {
            if (entitiesJson[i] == '{') depth++;
            if (entitiesJson[i] == '}') depth--;
            if (depth == 0) {
                objEnd = i;
                break;
            }
        }

        std::string entityJson = entitiesJson.substr(objStart, objEnd - objStart + 1);
        deserializeEntity(scene, entityJson);

        pos = objEnd + 1;
    }

    SPARK_CORE_INFO("Scene loaded with {0} entities", scene.getEntityCount());
    return true;
}

std::string SceneSerializer::serializeEntity(Entity& entity) {
    std::stringstream ss;
    ss << "{\n";

    // ID
    ss << "      \"id\": " << entity.getID() << ",\n";

    // Tag
    if (entity.hasComponent<TagComponent>()) {
        auto& tag = entity.getComponent<TagComponent>();
        ss << "      \"name\": \"" << escapeJson(tag.name) << "\",\n";
    }

    // Components
    ss << "      \"components\": {\n";

    bool first = true;

    // Transform
    if (entity.hasComponent<TransformComponent>()) {
        auto& t = entity.getComponent<TransformComponent>();
        if (!first) ss << ",\n";
        first = false;
        ss << "        \"transform\": " << serializeTransform(t.position, t.rotation, t.scale);
    }

    // MeshRenderer
    if (entity.hasComponent<MeshRendererComponent>()) {
        auto& r = entity.getComponent<MeshRendererComponent>();
        if (!first) ss << ",\n";
        first = false;
        ss << "        \"meshRenderer\": " << serializeMeshRenderer(r.albedo, r.metallic, r.roughness, r.ao);
    }

    // Light
    if (entity.hasComponent<LightComponent>()) {
        auto& l = entity.getComponent<LightComponent>();
        if (!first) ss << ",\n";
        first = false;
        ss << "        \"light\": " << serializeLight(
            static_cast<int>(l.lightType), l.color, l.intensity, l.range);
    }

    // Camera
    if (entity.hasComponent<CameraComponent>()) {
        auto& c = entity.getComponent<CameraComponent>();
        if (!first) ss << ",\n";
        first = false;
        ss << "        \"camera\": " << serializeCamera(
            static_cast<int>(c.projectionType), c.fov, c.nearPlane, c.farPlane);
    }

    ss << "\n      }\n";
    ss << "    }";

    return ss.str();
}

bool SceneSerializer::deserializeEntity(Scene& scene, const std::string& json) {
    // 提取名称
    std::string name = extractJsonValue(json, "name");
    if (name.empty()) name = "Entity";

    auto& entity = scene.createEntity(name);

    // 提取 Transform
    if (json.find("\"transform\"") != std::string::npos) {
        Vec3 pos = extractVec3(json, "position");
        Vec3 rot = extractVec3(json, "rotation");
        Vec3 scale = extractVec3(json, "scale");

        auto& t = entity.addComponent<TransformComponent>();
        t.position = pos;
        t.rotation = rot;
        t.scale = scale;
    }

    // 提取 MeshRenderer
    if (json.find("\"meshRenderer\"") != std::string::npos) {
        Vec4 albedo = extractVec4(json, "albedo");
        float metallic = extractFloat(json, "metallic");
        float roughness = extractFloat(json, "roughness");
        float ao = extractFloat(json, "ao");

        auto& r = entity.addComponent<MeshRendererComponent>();
        r.albedo = albedo;
        r.metallic = metallic;
        r.roughness = roughness;
        r.ao = ao;
    }

    // 提取 Light
    if (json.find("\"light\"") != std::string::npos) {
        int type = extractInt(json, "type");
        Vec3 color = extractVec3(json, "color");
        float intensity = extractFloat(json, "intensity");
        float range = extractFloat(json, "range");

        auto& l = entity.addComponent<LightComponent>();
        l.lightType = static_cast<LightComponent::LightType>(type);
        l.color = color;
        l.intensity = intensity;
        l.range = range;
    }

    // 提取 Camera
    if (json.find("\"camera\"") != std::string::npos) {
        int projType = extractInt(json, "projectionType");
        float fov = extractFloat(json, "fov");
        float nearPlane = extractFloat(json, "nearPlane");
        float farPlane = extractFloat(json, "farPlane");

        auto& c = entity.addComponent<CameraComponent>();
        c.projectionType = static_cast<CameraComponent::ProjectionType>(projType);
        c.fov = fov;
        c.nearPlane = nearPlane;
        c.farPlane = farPlane;
    }

    return true;
}

std::string SceneSerializer::serializeTransform(const Vec3& position, const Vec3& rotation, const Vec3& scale) {
    std::stringstream ss;
    ss << "{\n";
    ss << "          \"position\": {\"x\": " << position.x << ", \"y\": " << position.y << ", \"z\": " << position.z << "},\n";
    ss << "          \"rotation\": {\"x\": " << rotation.x << ", \"y\": " << rotation.y << ", \"z\": " << rotation.z << "},\n";
    ss << "          \"scale\": {\"x\": " << scale.x << ", \"y\": " << scale.y << ", \"z\": " << scale.z << "}\n";
    ss << "        }";
    return ss.str();
}

std::string SceneSerializer::serializeMeshRenderer(const Vec4& albedo, float metallic, float roughness, float ao) {
    std::stringstream ss;
    ss << "{\n";
    ss << "          \"albedo\": {\"x\": " << albedo.x << ", \"y\": " << albedo.y << ", \"z\": " << albedo.z << ", \"w\": " << albedo.w << "},\n";
    ss << "          \"metallic\": " << metallic << ",\n";
    ss << "          \"roughness\": " << roughness << ",\n";
    ss << "          \"ao\": " << ao << "\n";
    ss << "        }";
    return ss.str();
}

std::string SceneSerializer::serializeLight(int type, const Vec3& color, float intensity, float range) {
    std::stringstream ss;
    ss << "{\n";
    ss << "          \"type\": " << type << ",\n";
    ss << "          \"color\": {\"x\": " << color.x << ", \"y\": " << color.y << ", \"z\": " << color.z << "},\n";
    ss << "          \"intensity\": " << intensity << ",\n";
    ss << "          \"range\": " << range << "\n";
    ss << "        }";
    return ss.str();
}

std::string SceneSerializer::serializeCamera(int projType, float fov, float nearPlane, float farPlane) {
    std::stringstream ss;
    ss << "{\n";
    ss << "          \"projectionType\": " << projType << ",\n";
    ss << "          \"fov\": " << fov << ",\n";
    ss << "          \"nearPlane\": " << nearPlane << ",\n";
    ss << "          \"farPlane\": " << farPlane << "\n";
    ss << "        }";
    return ss.str();
}

std::string SceneSerializer::escapeJson(const std::string& s) {
    std::string result;
    for (char c : s) {
        switch (c) {
            case '"':  result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:   result += c;
        }
    }
    return result;
}

std::string SceneSerializer::extractJsonValue(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return "";

    size_t colonPos = json.find(":", pos);
    if (colonPos == std::string::npos) return "";

    size_t valueStart = json.find("\"", colonPos + 1);
    if (valueStart == std::string::npos) return "";

    size_t valueEnd = json.find("\"", valueStart + 1);
    if (valueEnd == std::string::npos) return "";

    return json.substr(valueStart + 1, valueEnd - valueStart - 1);
}

Vec3 SceneSerializer::extractVec3(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return Vec3(0.0f);

    size_t objStart = json.find("{", pos);
    size_t objEnd = json.find("}", objStart);
    if (objStart == std::string::npos || objEnd == std::string::npos) return Vec3(0.0f);

    std::string obj = json.substr(objStart, objEnd - objStart + 1);

    Vec3 result;
    result.x = extractFloat(obj, "x");
    result.y = extractFloat(obj, "y");
    result.z = extractFloat(obj, "z");

    return result;
}

Vec4 SceneSerializer::extractVec4(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return Vec4(0.0f);

    size_t objStart = json.find("{", pos);
    size_t objEnd = json.find("}", objStart);
    if (objStart == std::string::npos || objEnd == std::string::npos) return Vec4(0.0f);

    std::string obj = json.substr(objStart, objEnd - objStart + 1);

    Vec4 result;
    result.x = extractFloat(obj, "x");
    result.y = extractFloat(obj, "y");
    result.z = extractFloat(obj, "z");
    result.w = extractFloat(obj, "w");

    return result;
}

float SceneSerializer::extractFloat(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return 0.0f;

    size_t colonPos = json.find(":", pos);
    if (colonPos == std::string::npos) return 0.0f;

    size_t valueStart = colonPos + 1;
    while (valueStart < json.size() && (json[valueStart] == ' ' || json[valueStart] == '\t')) {
        valueStart++;
    }

    size_t valueEnd = valueStart;
    while (valueEnd < json.size() && (json[valueEnd] >= '0' && json[valueEnd] <= '9' || json[valueEnd] == '.' || json[valueEnd] == '-')) {
        valueEnd++;
    }

    try {
        return std::stof(json.substr(valueStart, valueEnd - valueStart));
    } catch (...) {
        return 0.0f;
    }
}

int SceneSerializer::extractInt(const std::string& json, const std::string& key) {
    return static_cast<int>(extractFloat(json, key));
}

bool SceneSerializer::extractBool(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return false;

    size_t colonPos = json.find(":", pos);
    if (colonPos == std::string::npos) return false;

    size_t valueStart = colonPos + 1;
    while (valueStart < json.size() && json[valueStart] == ' ') {
        valueStart++;
    }

    return json.substr(valueStart, 4) == "true";
}

} // namespace spark
