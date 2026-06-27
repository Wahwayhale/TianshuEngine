#include "script_engine.h"
#include "core/log.h"
#include "core/input.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <fstream>
#include <sstream>

// sol2 头文件
// 注意：需要将 sol2 和 lua 头文件添加到项目
// sol2: https://github.com/ThePhD/sol2
// Lua: https://www.lua.org/

#include <sol/sol.hpp>

namespace spark {

ScriptEngine& ScriptEngine::get() {
    static ScriptEngine instance;
    return instance;
}

bool ScriptEngine::initialize() {
    if (m_initialized) return true;

    try {
        m_lua = std::make_unique<sol::state>();

        // 加载 Lua 标准库
        m_lua->open_libraries(
            sol::lib::base,
            sol::lib::math,
            sol::lib::string,
            sol::lib::table,
            sol::lib::io,
            sol::lib::os
        );

        // 绑定引擎 API
        bindAPI();

        m_initialized = true;
        SPARK_CORE_INFO("Script engine initialized with sol2.");
        return true;

    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to initialize script engine: {0}", e.what());
        return false;
    }
}

void ScriptEngine::shutdown() {
    if (!m_initialized) return;

    m_lua.reset();
    m_initialized = false;
    SPARK_CORE_INFO("Script engine shutdown.");
}

bool ScriptEngine::loadScript(const std::string& filepath) {
    if (!m_initialized) return false;

    try {
        auto result = m_lua->script_file(filepath);
        if (!result.valid()) {
            sol::error err = result;
            SPARK_CORE_ERROR("Script error in {0}: {1}", filepath, err.what());
            if (m_errorCallback) {
                m_errorCallback(err.what());
            }
            return false;
        }
        return true;

    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to load script {0}: {1}", filepath, e.what());
        if (m_errorCallback) {
            m_errorCallback(e.what());
        }
        return false;
    }
}

bool ScriptEngine::executeString(const std::string& code) {
    if (!m_initialized) return false;

    try {
        auto result = m_lua->script(code);
        if (!result.valid()) {
            sol::error err = result;
            SPARK_CORE_ERROR("Script error: {0}", err.what());
            if (m_errorCallback) {
                m_errorCallback(err.what());
            }
            return false;
        }
        return true;

    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to execute script: {0}", e.what());
        if (m_errorCallback) {
            m_errorCallback(e.what());
        }
        return false;
    }
}

void ScriptEngine::bindAPI() {
    if (!m_initialized) return;

    bindMathTypes();
    bindInput();
    bindLog();
    bindScene();
    bindEntity();

    SPARK_CORE_INFO("Lua API bindings registered.");
}

void ScriptEngine::bindMathTypes() {
    auto& lua = *m_lua;

    // Vec2
    lua.new_usertype<Vec2>("Vec2",
        sol::constructors<Vec2(), Vec2(float), Vec2(float, float)>(),
        "x", &Vec2::x,
        "y", &Vec2::y,
        sol::meta_function::addition, sol::overload(
            [](const Vec2& a, const Vec2& b) { return a + b; }
        ),
        sol::meta_function::subtraction, sol::overload(
            [](const Vec2& a, const Vec2& b) { return a - b; }
        ),
        sol::meta_function::multiplication, sol::overload(
            [](const Vec2& a, float b) { return a * b; },
            [](float a, const Vec2& b) { return a * b; }
        )
    );

    // Vec3
    lua.new_usertype<Vec3>("Vec3",
        sol::constructors<Vec3(), Vec3(float), Vec3(float, float, float)>(),
        "x", &Vec3::x,
        "y", &Vec3::y,
        "z", &Vec3::z,
        sol::meta_function::addition, sol::overload(
            [](const Vec3& a, const Vec3& b) { return a + b; }
        ),
        sol::meta_function::subtraction, sol::overload(
            [](const Vec3& a, const Vec3& b) { return a - b; }
        ),
        sol::meta_function::multiplication, sol::overload(
            [](const Vec3& a, float b) { return a * b; },
            [](float a, const Vec3& b) { return a * b; }
        )
    );

    // Vec4
    lua.new_usertype<Vec4>("Vec4",
        sol::constructors<Vec4(), Vec4(float), Vec4(float, float, float, float)>(),
        "x", &Vec4::x,
        "y", &Vec4::y,
        "z", &Vec4::z,
        "w", &Vec4::w
    );

    // 数学函数
    lua["math"]["length"] = [](const Vec3& v) { return glm::length(v); };
    lua["math"]["normalize"] = [](const Vec3& v) { return glm::normalize(v); };
    lua["math"]["dot"] = [](const Vec3& a, const Vec3& b) { return glm::dot(a, b); };
    lua["math"]["cross"] = [](const Vec3& a, const Vec3& b) { return glm::cross(a, b); };
    lua["math"]["lerp"] = [](float a, float b, float t) { return glm::mix(a, b, t); };
    lua["math"]["clamp"] = [](float x, float min, float max) { return glm::clamp(x, min, max); };
}

void ScriptEngine::bindInput() {
    auto& lua = *m_lua;

    // 输入系统绑定
    lua["Input"] = lua.create_table();

    lua["Input"]["isKeyPressed"] = [](int key) {
        return Input::isKeyPressed(key);
    };

    lua["Input"]["isKeyJustPressed"] = [](int key) {
        return Input::isKeyJustPressed(key);
    };

    lua["Input"]["isMouseButtonPressed"] = [](int button) {
        return Input::isMouseButtonPressed(button);
    };

    lua["Input"]["getMousePosition"] = []() {
        auto pos = Input::getMousePosition();
        return std::make_tuple(pos.x, pos.y);
    };

    lua["Input"]["getMouseDelta"] = []() {
        auto delta = Input::getMouseDelta();
        return std::make_tuple(delta.x, delta.y);
    };

    // 常用键码
    lua["Key"] = lua.create_table();
    lua["Key"]["A"] = 65;
    lua["Key"]["B"] = 66;
    lua["Key"]["C"] = 67;
    lua["Key"]["D"] = 68;
    lua["Key"]["E"] = 69;
    lua["Key"]["F"] = 70;
    lua["Key"]["G"] = 71;
    lua["Key"]["H"] = 72;
    lua["Key"]["I"] = 73;
    lua["Key"]["J"] = 74;
    lua["Key"]["K"] = 75;
    lua["Key"]["L"] = 76;
    lua["Key"]["M"] = 77;
    lua["Key"]["N"] = 78;
    lua["Key"]["O"] = 79;
    lua["Key"]["P"] = 80;
    lua["Key"]["Q"] = 81;
    lua["Key"]["R"] = 82;
    lua["Key"]["S"] = 83;
    lua["Key"]["T"] = 84;
    lua["Key"]["U"] = 85;
    lua["Key"]["V"] = 86;
    lua["Key"]["W"] = 87;
    lua["Key"]["X"] = 88;
    lua["Key"]["Y"] = 89;
    lua["Key"]["Z"] = 90;
    lua["Key"]["Space"] = 32;
    lua["Key"]["Escape"] = 256;
    lua["Key"]["Enter"] = 257;
    lua["Key"]["Tab"] = 258;
    lua["Key"]["Backspace"] = 259;
    lua["Key"]["Left"] = 263;
    lua["Key"]["Right"] = 262;
    lua["Key"]["Up"] = 265;
    lua["Key"]["Down"] = 264;
    lua["Key"]["LeftShift"] = 340;
    lua["Key"]["LeftControl"] = 341;
    lua["Key"]["LeftAlt"] = 342;
}

void ScriptEngine::bindLog() {
    auto& lua = *m_lua;

    lua["Log"] = lua.create_table();

    lua["Log"]["info"] = [](const std::string& msg) {
        SPARK_INFO("[Lua] {0}", msg);
    };

    lua["Log"]["warn"] = [](const std::string& msg) {
        SPARK_WARN("[Lua] {0}", msg);
    };

    lua["Log"]["error"] = [](const std::string& msg) {
        SPARK_ERROR("[Lua] {0}", msg);
    };
}

void ScriptEngine::bindScene() {
    auto& lua = *m_lua;

    // 场景绑定（简化版本）
    lua["Scene"] = lua.create_table();

    lua["Scene"]["getEntityCount"] = [this]() -> int {
        if (m_currentScene) {
            return static_cast<int>(m_currentScene->getEntityCount());
        }
        return 0;
    };
}

void ScriptEngine::bindEntity() {
    auto& lua = *m_lua;

    // TransformComponent 绑定
    lua.new_usertype<TransformComponent>("TransformComponent",
        "position", sol::property(
            [](TransformComponent& t) -> Vec3& { return t.position; },
            [](TransformComponent& t, const Vec3& v) { t.position = v; }
        ),
        "rotation", sol::property(
            [](TransformComponent& t) -> Vec3& { return t.rotation; },
            [](TransformComponent& t, const Vec3& v) { t.rotation = v; }
        ),
        "scale", sol::property(
            [](TransformComponent& t) -> Vec3& { return t.scale; },
            [](TransformComponent& t, const Vec3& v) { t.scale = v; }
        ),
        "translate", [](TransformComponent& t, const Vec3& offset) {
            t.position += offset;
        }
    );

    // TagComponent 绑定
    lua.new_usertype<TagComponent>("TagComponent",
        "name", sol::property(
            [](TagComponent& t) -> std::string& { return t.name; },
            [](TagComponent& t, const std::string& n) { t.name = n; }
        )
    );

    // RigidBodyComponent 绑定
    lua.new_usertype<RigidBodyComponent>("RigidBodyComponent",
        "velocity", sol::property(
            [](RigidBodyComponent& rb) -> Vec3& { return rb.velocity; },
            [](RigidBodyComponent& rb, const Vec3& v) { rb.velocity = v; }
        ),
        "applyForce", &RigidBodyComponent::applyForce,
        "applyImpulse", &RigidBodyComponent::applyImpulse,
        "mass", sol::property(
            [](RigidBodyComponent& rb) { return rb.mass; },
            [](RigidBodyComponent& rb, float m) { rb.setMass(m); }
        ),
        "useGravity", &RigidBodyComponent::useGravity,
        "isStatic", &RigidBodyComponent::isStatic
    );

    // MeshRendererComponent 绑定
    lua.new_usertype<MeshRendererComponent>("MeshRendererComponent",
        "visible", &MeshRendererComponent::visible,
        "castShadow", &MeshRendererComponent::castShadow,
        "metallic", &MeshRendererComponent::metallic,
        "roughness", &MeshRendererComponent::roughness
    );

    // Entity 绑定（简化版本，通过 ID 访问）
    lua["Entity"] = lua.create_table();

    // 全局时间
    lua["Time"] = lua.create_table();
    lua["Time"]["deltaTime"] = 0.0f;
    lua["Time"]["totalTime"] = 0.0f;
}

} // namespace spark
