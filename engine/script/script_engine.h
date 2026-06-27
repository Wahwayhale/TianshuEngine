#pragma once

#include "math/math_types.h"
#include <string>
#include <memory>
#include <functional>

// 前向声明 sol2 类型
namespace sol {
    class state;
}

namespace spark {

class Scene;
class Entity;

class ScriptEngine {
public:
    static ScriptEngine& get();

    bool initialize();
    void shutdown();

    // 加载并执行脚本文件
    bool loadScript(const std::string& filepath);
    bool executeString(const std::string& code);

    // 获取 Lua 状态
    sol::state& getLua() { return *m_lua; }

    // 绑定引擎 API
    void bindAPI();

    // 设置当前场景（供脚本访问）
    void setCurrentScene(Scene* scene) { m_currentScene = scene; }
    Scene* getCurrentScene() const { return m_currentScene; }

    // 脚本错误处理
    using ErrorCallback = std::function<void(const std::string&)>;
    void setErrorCallback(ErrorCallback callback) { m_errorCallback = callback; }

private:
    ScriptEngine() = default;
    ~ScriptEngine() = default;

    void bindMathTypes();
    void bindInput();
    void bindLog();
    void bindScene();
    void bindEntity();

    std::unique_ptr<sol::state> m_lua;
    Scene* m_currentScene = nullptr;
    ErrorCallback m_errorCallback;
    bool m_initialized = false;
};

} // namespace spark
