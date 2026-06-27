#pragma once

#include "ecs/system.h"
#include "ecs/components.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace spark {

class ScriptEngine;

// 脚本组件 - 每个实体可以附加 Lua 脚本
struct LuaScriptComponent : public Component {
    std::string scriptPath;        // 脚本文件路径
    bool loaded = false;           // 是否已加载
    bool hasOnInit = false;        // 是否有 OnInit 函数
    bool hasOnUpdate = false;      // 是否有 OnUpdate 函数
    bool hasOnDestroy = false;     // 是否有 OnDestroy 函数
    bool hasOnCollision = false;   // 是否有 OnCollision 函数

    // 脚本环境（每个实体独立的 Lua 环境）
    void* scriptEnv = nullptr;

    LuaScriptComponent(const std::string& path = "") : scriptPath(path) {}
};

// 脚本系统 - 管理所有脚本的生命周期
class ScriptSystem : public System {
public:
    ScriptSystem();
    ~ScriptSystem() override;

    void update(Scene& scene, float deltaTime) override;

    // 设置时间（供脚本访问）
    void setTotalTime(float time) { m_totalTime = time; }

private:
    void loadScript(Entity& entity, LuaScriptComponent& script);
    void callOnInit(Entity& entity, LuaScriptComponent& script);
    void callOnUpdate(Entity& entity, LuaScriptComponent& script, float deltaTime);
    void callOnDestroy(Entity& entity, LuaScriptComponent& script);

    float m_totalTime = 0.0f;
};

} // namespace spark
