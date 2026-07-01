/**
 * 天枢引擎 - 最小可运行版本
 *
 * 这个版本只包含核心渲染功能，不依赖复杂系统
 */

#include "core/application.h"
#include "core/log.h"
#include "renderer/renderer.h"
#include "renderer/pipeline.h"
#include "renderer/shader.h"
#include "renderer/mesh.h"
#include "renderer/camera.h"
#include "renderer/descriptor_set.h"
#include <memory>

using namespace spark;

class MinimalApp : public Application {
public:
    MinimalApp() : Application("Tianshu Engine - Minimal", 1280, 720) {
        // 初始化渲染器
        m_renderer = std::make_unique<Renderer>(
            getWindow().getNativeWindow(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );

        // 创建相机
        m_camera = std::make_unique<Camera>(Vec3(0.0f, 2.0f, 5.0f));

        SPARK_INFO("Minimal demo started!");
        SPARK_INFO("Controls: WASD - Move, Mouse - Look");
    }

    ~MinimalApp() {
        m_renderer->waitIdle();
    }

protected:
    void onUpdate(float deltaTime) override {
        m_camera->update(deltaTime);
    }

    void onRender() override {
        m_renderer->beginFrame();
        // 渲染逻辑将在这里添加
        m_renderer->endFrame();
    }

    void onEvent(Event& event) override {
        if (auto* resizeEvent = dynamic_cast<WindowResizeEvent*>(&event)) {
            m_renderer->onResize(resizeEvent->width, resizeEvent->height);
        }
    }

private:
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Camera> m_camera;
};

int main() {
    try {
        MinimalApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
