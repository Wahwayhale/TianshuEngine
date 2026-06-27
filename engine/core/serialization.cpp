#include "serialization.h"
#include "scene/scene.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include <iomanip>

namespace spark {

Serializer::Serializer() = default;
Serializer::~Serializer() = default;

std::string Serializer::serialize(const JsonValue& value) {
    std::stringstream ss;
    serializeValue(value, ss);
    return ss.str();
}

bool Serializer::serializeToFile(const JsonValue& value, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;

    file << serialize(value);
    return true;
}

JsonValue Serializer::deserialize(const std::string& json) {
    size_t pos = 0;
    return parseValue(json, pos);
}

JsonValue Serializer::deserializeFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return nullptr;

    std::stringstream ss;
    ss << file.rdbuf();
    return deserialize(ss.str());
}

JsonValue Serializer::fromVec2(const Vec2& v) {
    std::vector<JsonValue> arr = {v.x, v.y};
    return arr;
}

JsonValue Serializer::fromVec3(const Vec3& v) {
    std::vector<JsonValue> arr = {v.x, v.y, v.z};
    return arr;
}

JsonValue Serializer::fromVec4(const Vec4& v) {
    std::vector<JsonValue> arr = {v.x, v.y, v.z, v.w};
    return arr;
}

JsonValue Serializer::fromMat4(const Mat4& m) {
    std::vector<JsonValue> arr;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            arr.push_back(m[i][j]);
        }
    }
    return arr;
}

Vec2 Serializer::toVec2(const JsonValue& v) {
    const auto& arr = std::get<std::vector<JsonValue>>(v);
    return Vec2(std::get<double>(arr[0]), std::get<double>(arr[1]));
}

Vec3 Serializer::toVec3(const JsonValue& v) {
    const auto& arr = std::get<std::vector<JsonValue>>(v);
    return Vec3(std::get<double>(arr[0]), std::get<double>(arr[1]), std::get<double>(arr[2]));
}

Vec4 Serializer::toVec4(const JsonValue& v) {
    const auto& arr = std::get<std::vector<JsonValue>>(v);
    return Vec4(std::get<double>(arr[0]), std::get<double>(arr[1]), std::get<double>(arr[2]), std::get<double>(arr[3]));
}

Mat4 Serializer::toMat4(const JsonValue& v) {
    const auto& arr = std::get<std::vector<JsonValue>>(v);
    Mat4 m;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = std::get<double>(arr[i * 4 + j]);
        }
    }
    return m;
}

void Serializer::serializeValue(const JsonValue& value, std::stringstream& ss, int indent) {
    std::string indentStr(indent * 2, ' ');

    if (std::holds_alternative<std::nullptr_t>(value)) {
        ss << "null";
    } else if (std::holds_alternative<bool>(value)) {
        ss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<int64_t>(value)) {
        ss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        ss << std::fixed << std::setprecision(6) << std::get<double>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        ss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<std::vector<JsonValue>>(value)) {
        const auto& arr = std::get<std::vector<JsonValue>>(value);
        ss << "[\n";
        for (size_t i = 0; i < arr.size(); i++) {
            ss << indentStr << "  ";
            serializeValue(arr[i], ss, indent + 1);
            if (i < arr.size() - 1) ss << ",";
            ss << "\n";
        }
        ss << indentStr << "]";
    } else if (std::holds_alternative<std::unordered_map<std::string, JsonValue>>(value)) {
        const auto& obj = std::get<std::unordered_map<std::string, JsonValue>>(value);
        ss << "{\n";
        size_t i = 0;
        for (const auto& pair : obj) {
            ss << indentStr << "  \"" << pair.first << "\": ";
            serializeValue(pair.second, ss, indent + 1);
            if (i < obj.size() - 1) ss << ",";
            ss << "\n";
            i++;
        }
        ss << indentStr << "}";
    }
}

JsonValue Serializer::parseValue(const std::string& json, size_t& pos) {
    skipWhitespace(json, pos);

    if (pos >= json.size()) return nullptr;

    char c = json[pos];
    if (c == 'n') return parseNull(json, pos);
    if (c == 't' || c == 'f') return parseBool(json, pos);
    if (c == '-' || (c >= '0' && c <= '9')) return parseNumber(json, pos);
    if (c == '"') return parseString(json, pos);
    if (c == '[') return parseArray(json, pos);
    if (c == '{') return parseObject(json, pos);

    return nullptr;
}

void Serializer::skipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) {
        pos++;
    }
}

JsonValue Serializer::parseNull(const std::string& json, size_t& pos) {
    if (json.substr(pos, 4) == "null") {
        pos += 4;
        return nullptr;
    }
    return nullptr;
}

JsonValue Serializer::parseBool(const std::string& json, size_t& pos) {
    if (json.substr(pos, 4) == "true") {
        pos += 4;
        return true;
    }
    if (json.substr(pos, 5) == "false") {
        pos += 5;
        return false;
    }
    return false;
}

JsonValue Serializer::parseNumber(const std::string& json, size_t& pos) {
    size_t start = pos;
    bool isFloat = false;

    if (json[pos] == '-') pos++;

    while (pos < json.size() && ((json[pos] >= '0' && json[pos] <= '9') || json[pos] == '.')) {
        if (json[pos] == '.') isFloat = true;
        pos++;
    }

    std::string numStr = json.substr(start, pos - start);
    if (isFloat) {
        return std::stod(numStr);
    } else {
        return static_cast<int64_t>(std::stoll(numStr));
    }
}

JsonValue Serializer::parseString(const std::string& json, size_t& pos) {
    pos++;  // Skip opening quote
    size_t start = pos;

    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\') pos++;  // Skip escaped char
        pos++;
    }

    std::string str = json.substr(start, pos - start);
    pos++;  // Skip closing quote
    return str;
}

JsonValue Serializer::parseArray(const std::string& json, size_t& pos) {
    pos++;  // Skip opening bracket
    std::vector<JsonValue> arr;

    skipWhitespace(json, pos);
    if (pos < json.size() && json[pos] == ']') {
        pos++;
        return arr;
    }

    while (pos < json.size()) {
        arr.push_back(parseValue(json, pos));
        skipWhitespace(json, pos);

        if (pos < json.size() && json[pos] == ',') {
            pos++;
        } else {
            break;
        }
    }

    if (pos < json.size() && json[pos] == ']') {
        pos++;
    }

    return arr;
}

JsonValue Serializer::parseObject(const std::string& json, size_t& pos) {
    pos++;  // Skip opening brace
    std::unordered_map<std::string, JsonValue> obj;

    skipWhitespace(json, pos);
    if (pos < json.size() && json[pos] == '}') {
        pos++;
        return obj;
    }

    while (pos < json.size()) {
        skipWhitespace(json, pos);
        std::string key = std::get<std::string>(parseString(json, pos));

        skipWhitespace(json, pos);
        pos++;  // Skip colon

        obj[key] = parseValue(json, pos);

        skipWhitespace(json, pos);
        if (pos < json.size() && json[pos] == ',') {
            pos++;
        } else {
            break;
        }
    }

    if (pos < json.size() && json[pos] == '}') {
        pos++;
    }

    return obj;
}

SceneSerializer::SceneSerializer() = default;
SceneSerializer::~SceneSerializer() = default;

bool SceneSerializer::saveScene(Scene& scene, const std::string& filepath) {
    std::unordered_map<std::string, JsonValue> sceneData;
    std::vector<JsonValue> entities;

    for (const auto& [id, entity] : scene.getEntities()) {
        entities.push_back(serializeEntity(*entity));
    }

    sceneData["entities"] = entities;
    sceneData["version"] = static_cast<int64_t>(1);

    return m_serializer.serializeToFile(sceneData, filepath);
}

bool SceneSerializer::loadScene(Scene& scene, const std::string& filepath) {
    auto data = m_serializer.deserializeFromFile(filepath);
    if (std::holds_alternative<std::nullptr_t>(data)) return false;

    const auto& sceneData = std::get<std::unordered_map<std::string, JsonValue>>(data);
    const auto& entities = std::get<std::vector<JsonValue>>(sceneData.at("entities"));

    for (const auto& entityData : entities) {
        deserializeEntity(scene, entityData);
    }

    return true;
}

JsonValue SceneSerializer::serializeEntity(Entity& entity) {
    std::unordered_map<std::string, JsonValue> entityData;

    entityData["id"] = static_cast<int64_t>(entity.getID());

    if (entity.hasComponent<TagComponent>()) {
        entityData["tag"] = entity.getComponent<TagComponent>().name;
    }

    if (entity.hasComponent<TransformComponent>()) {
        auto& transform = entity.getComponent<TransformComponent>();
        std::unordered_map<std::string, JsonValue> transformData;
        transformData["position"] = Serializer::fromVec3(transform.position);
        transformData["rotation"] = Serializer::fromVec3(transform.rotation);
        transformData["scale"] = Serializer::fromVec3(transform.scale);
        entityData["transform"] = transformData;
    }

    return entityData;
}

void SceneSerializer::deserializeEntity(Scene& scene, const JsonValue& value) {
    const auto& entityData = std::get<std::unordered_map<std::string, JsonValue>>(value);

    auto& entity = scene.createEntity();

    if (entityData.find("tag") != entityData.end()) {
        auto& tag = entity.addComponent<TagComponent>();
        tag.name = std::get<std::string>(entityData.at("tag"));
    }

    if (entityData.find("transform") != entityData.end()) {
        auto& transform = entity.addComponent<TransformComponent>();
        const auto& transformData = std::get<std::unordered_map<std::string, JsonValue>>(entityData.at("transform"));
        transform.position = Serializer::toVec3(transformData.at("position"));
        transform.rotation = Serializer::toVec3(transformData.at("rotation"));
        transform.scale = Serializer::toVec3(transformData.at("scale"));
    }
}

} // namespace spark
