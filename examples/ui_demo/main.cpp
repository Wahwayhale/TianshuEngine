/**
 * @file main.cpp
 * @brief UI Demo Example
 *
 * 游戏 UI 示例
 * - 主菜单
 * - HUD
 * - 设置界面
 * - 对话框
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <memory>

using namespace spark;

class UIDemoApp : public Application {
public:
    UIDemoApp() : Application("UI Demo", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        // 创建 UI 元素
        createMainMenu();

        SPARK_INFO("UI Demo started!");
    }

    ~UIDemoApp() {
        m_renderer->waitIdle();
    }

protected:
    void onUpdate(float deltaTime) override {
        m_scene->update(deltaTime);
    }

    void onRender() override {
        // 渲染场景和 UI
    }

private:
    void createMainMenu() {
        // 创建主菜单 UI
    }

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
};

int main() {
    try {
        UIDemoApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
