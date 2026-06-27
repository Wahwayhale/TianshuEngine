/**
 * @file main.cpp
 * @brief Scripting Demo Example
 *
 * Demonstrates Lua scripting integration:
 * - Loading Lua scripts
 * - Script components
 * - Entity manipulation from scripts
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include "script/script_system.h"
#include <memory>

using namespace spark;

class ScriptingDemoApp : public Application {
public:
    ScriptingDemoApp() : Application("Scripting Demo", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        // Add script system
        auto& scriptSystem = m_scene->addSystem<ScriptSystem>();

        // Create entity with script
        auto& entity = m_scene->createEntity("Scripted Entity");
        entity.addComponent<TransformComponent>(Vec3(0.0f, 1.0f, 0.0f));

        auto& script = entity.addComponent<LuaScriptComponent>();
        script.scriptPath = "assets/scripts/rotate.lua";

        // Create more scripted entities
        for (int i = 0; i < 5; i++) {
            auto& e = m_scene->createEntity("Entity " + std::to_string(i));
            auto& transform = e.addComponent<TransformComponent>();
            transform.position = Vec3(i * 2.0f - 4.0f, 1.0f, 0.0f);

            auto& s = e.addComponent<LuaScriptComponent>();
            s.scriptPath = "assets/scripts/bounce.lua";
        }

        SPARK_INFO("Scripting Demo started!");
    }

    ~ScriptingDemoApp() {
        m_renderer->waitIdle();
    }

protected:
    void onUpdate(float deltaTime) override {
        m_scene->update(deltaTime);
    }

    void onRender() override {
        // Render scene...
    }

private:
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
};

int main() {
    try {
        ScriptingDemoApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
