#pragma once

#include "math/math_types.h"
#include <string>
#include <fstream>
#include <sstream>
#include <variant>
#include <vector>
#include <unordered_map>
#include <memory>

namespace spark {

// JSON-like value type (using shared_ptr for recursive structure)
struct JsonValue;
using JsonValuePtr = std::shared_ptr<JsonValue>;

struct JsonValue {
    enum Type {
        Null,
        Bool,
        Int,
        Float,
        String,
        Array,
        Object
    };

    Type type = Null;
    bool boolValue = false;
    int64_t intValue = 0;
    double floatValue = 0.0;
    std::string stringValue;
    std::vector<JsonValuePtr> arrayValue;
    std::unordered_map<std::string, JsonValuePtr> objectValue;
};

class Serializer {
public:
    Serializer();
    ~Serializer();

    // Serialization
    std::string serialize(const JsonValue& value);

    // Deserialization
    JsonValue deserialize(const std::string& json);

    // File I/O
    bool saveToFile(const std::string& path, const JsonValue& value);
    JsonValue loadFromFile(const std::string& path);
};

// Scene serializer
class Scene;
class SceneSerializer {
public:
    static bool saveScene(Scene& scene, const std::string& path);
    static bool loadScene(Scene& scene, const std::string& path);
};

} // namespace spark
