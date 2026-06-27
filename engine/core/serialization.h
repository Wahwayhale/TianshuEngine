#pragma once

#include "math/math_types.h"
#include <string>
#include <fstream>
#include <sstream>
#include <variant>
#include <vector>
#include <unordered_map>

namespace spark {

// JSON-like value type
using JsonValue = std::variant<
    std::nullptr_t,
    bool,
    int64_t,
    double,
    std::string,
    std::vector<JsonValue>,
    std::unordered_map<std::string, JsonValue>
>;

class Serializer {
public:
    Serializer();
    ~Serializer();

    // Serialization
    std::string serialize(const JsonValue& value);
    bool serializeToFile(const JsonValue& value, const std::string& filepath);

    // Deserialization
    JsonValue deserialize(const std::string& json);
    JsonValue deserializeFromFile(const std::string& filepath);

    // Helper functions for common types
    static JsonValue fromVec2(const Vec2& v);
    static JsonValue fromVec3(const Vec3& v);
    static JsonValue fromVec4(const Vec4& v);
    static JsonValue fromMat4(const Mat4& m);

    static Vec2 toVec2(const JsonValue& v);
    static Vec3 toVec3(const JsonValue& v);
    static Vec4 toVec4(const JsonValue& v);
    static Mat4 toMat4(const JsonValue& v);

private:
    void serializeValue(const JsonValue& value, std::stringstream& ss, int indent = 0);
    JsonValue parseValue(const std::string& json, size_t& pos);

    // Parser helpers
    void skipWhitespace(const std::string& json, size_t& pos);
    JsonValue parseNull(const std::string& json, size_t& pos);
    JsonValue parseBool(const std::string& json, size_t& pos);
    JsonValue parseNumber(const std::string& json, size_t& pos);
    JsonValue parseString(const std::string& json, size_t& pos);
    JsonValue parseArray(const std::string& json, size_t& pos);
    JsonValue parseObject(const std::string& json, size_t& pos);
};

// Scene serialization
class SceneSerializer {
public:
    SceneSerializer();
    ~SceneSerializer();

    // Save scene to file
    bool saveScene(class Scene& scene, const std::string& filepath);

    // Load scene from file
    bool loadScene(Scene& scene, const std::string& filepath);

private:
    JsonValue serializeEntity(class Entity& entity);
    void deserializeEntity(Scene& scene, const JsonValue& value);

    Serializer m_serializer;
};

} // namespace spark
