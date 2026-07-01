/**
 * 测试 Scene 初始化
 */

#include "core/application.h"
#include "core/log.h"
#include "renderer/renderer.h"
#include "renderer/camera.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <iostream>

using namespace spark;

class TestApp : public Application {
public:
    TestApp() : Application("Test Scene", 800, 600) {
        std::cout << "Creating renderer..." << std::endl;
        m_renderer = std::make_unique<Renderer>(
            getWindow().getNativeWindow(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );

        std::cout << "Creating camera..." << std::endl;
        m_camera = std::make_unique<Camera>(Vec3(0.0f, 5.0f, 10.0f));

        std::cout << "Creating scene..." << std::endl;
        m_scene = std::make_unique<Scene>();

        std::cout << "Creating entities..." << std::endl;

        // 创建地面
        auto& ground = m_scene->createEntity("Ground");
        auto& groundTransform = ground.addComponent<TransformComponent>();
        groundTransform.scale = Vec3(20.0f, 0.1f, 20.0f);
        auto& groundRenderer = ground.addComponent<MeshRendererComponent>();
        groundRenderer.albedo = Vec4(0.3f, 0.3f, 0.35f, 1.0f);

        // 创建立方体
        auto& cube = m_scene->createEntity("Cube");
        cube.addComponent<TransformComponent>(Vec3(0.0f, 1.0f, 0.0f));
        auto& cubeRenderer = cube.addComponent<MeshRendererComponent>();
        cubeRenderer.albedo = Vec4(0.8f, 0.2f, 0.2f, 1.0f);

        std::cout << "All created!" << std::endl;
        std::cout << "Entities: " << m_scene->getEntityCount() << std::endl;
    }

    ~TestApp() {
        if (m_renderer) {
            m_renderer->waitIdle();
        }
    }

protected:
    void onUpdate(float deltaTime) override {
        m_camera->update(deltaTime);
    }

    void onRender() override {
        m_renderer->beginFrame();
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
    std::unique_ptr<Scene> m_scene;
};

int main() {
    std::cout << "Starting test..." << std::endl;
    try {
        TestApp app;
        std::cout << "Running..." << std::endl;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
