#include "script_system.h"
#include "script_engine.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include "core/log.h"

#include <sol/sol.hpp>

namespace spark {

ScriptSystem::ScriptSystem() {
    // 初始化脚本引擎
    ScriptEngine::get().initialize();
    SPARK_CORE_INFO("Script system initialized.");
}

ScriptSystem::~ScriptSystem() {
    // 清理脚本环境
    ScriptEngine::get().shutdown();
}

void ScriptSystem::update(Scene& scene, float deltaTime) {
    m_totalTime += deltaTime;

    // 更新全局时间
    auto& lua = ScriptEngine::get().getLua();
    lua["Time"]["deltaTime"] = deltaTime;
    lua["Time"]["totalTime"] = m_totalTime;

    // 设置当前场景
    ScriptEngine::get().setCurrentScene(&scene);

    // 遍历所有有 LuaScriptComponent 的实体
    scene.view<TransformComponent, LuaScriptComponent>([&](Entity& entity) {
        auto& script = entity.getComponent<LuaScriptComponent>();

        // 首次加载脚本
        if (!script.loaded && !script.scriptPath.empty()) {
            loadScript(entity, script);
        }

        // 调用 OnUpdate
        if (script.loaded && script.hasOnUpdate) {
            callOnUpdate(entity, script, deltaTime);
        }
    });
}

void ScriptSystem::loadScript(Entity& entity, LuaScriptComponent& script) {
    if (script.scriptPath.empty()) {
        return;
    }

    auto& lua = ScriptEngine::get().getLua();

    try {
        // 创建脚本环境
        sol::environment env(lua, sol::create, lua.globals());
        script.scriptEnv = new sol::environment(std::move(env));
        auto& envRef = *static_cast<sol::environment*>(script.scriptEnv);

        // 设置实体引用
        envRef["entity"] = &entity;

        // 加载脚本文件
        auto result = lua.script_file(script.scriptPath, envRef);
        if (!result.valid()) {
            sol::error err = result;
            SPARK_CORE_ERROR("Failed to load script {0}: {1}", script.scriptPath, err.what());
            delete static_cast<sol::environment*>(script.scriptEnv);
            script.scriptEnv = nullptr;
            return;
        }

        // 检查是否有生命周期函数
        script.hasOnInit = envRef["OnInit"].valid();
        script.hasOnUpdate = envRef["OnUpdate"].valid();
        script.hasOnDestroy = envRef["OnDestroy"].valid();
        script.hasOnCollision = envRef["OnCollision"].valid();

        script.loaded = true;

        // 调用 OnInit
        if (script.hasOnInit) {
            callOnInit(entity, script);
        }

        SPARK_CORE_INFO("Script loaded: {0}", script.scriptPath);

    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to load script {0}: {1}", script.scriptPath, e.what());
        if (script.scriptEnv) {
            delete static_cast<sol::environment*>(script.scriptEnv);
            script.scriptEnv = nullptr;
        }
    }
}

void ScriptSystem::callOnInit(Entity& entity, LuaScriptComponent& script) {
    if (!script.scriptEnv) return;

    try {
        auto& envRef = *static_cast<sol::environment*>(script.scriptEnv);
        sol::protected_function initFunc = envRef["OnInit"];
        if (initFunc.valid()) {
            auto result = initFunc();
            if (!result.valid()) {
                sol::error err = result;
                SPARK_CORE_ERROR("OnInit error in {0}: {1}", script.scriptPath, err.what());
            }
        }
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("OnInit exception in {0}: {1}", script.scriptPath, e.what());
    }
}

void ScriptSystem::callOnUpdate(Entity& entity, LuaScriptComponent& script, float deltaTime) {
    if (!script.scriptEnv) return;

    try {
        auto& envRef = *static_cast<sol::environment*>(script.scriptEnv);
        sol::protected_function updateFunc = envRef["OnUpdate"];
        if (updateFunc.valid()) {
            auto result = updateFunc(deltaTime);
            if (!result.valid()) {
                sol::error err = result;
                SPARK_CORE_ERROR("OnUpdate error in {0}: {1}", script.scriptPath, err.what());
            }
        }
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("OnUpdate exception in {0}: {1}", script.scriptPath, e.what());
    }
}

void ScriptSystem::callOnDestroy(Entity& entity, LuaScriptComponent& script) {
    if (!script.scriptEnv) return;

    try {
        auto& envRef = *static_cast<sol::environment*>(script.scriptEnv);
        sol::protected_function destroyFunc = envRef["OnDestroy"];
        if (destroyFunc.valid()) {
            auto result = destroyFunc();
            if (!result.valid()) {
                sol::error err = result;
                SPARK_CORE_ERROR("OnDestroy error in {0}: {1}", script.scriptPath, err.what());
            }
        }
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("OnDestroy exception in {0}: {1}", script.scriptPath, e.what());
    }
}

} // namespace spark
