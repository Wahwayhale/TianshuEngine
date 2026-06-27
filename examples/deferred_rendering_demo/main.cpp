/**
 * @file main.cpp
 * @brief Deferred Rendering Demo Example
 *
 * Demonstrates deferred rendering:
 * - G-Buffer visualization
 * - Multiple lights
 * - SSAO
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <memory>

using namespace spark;

class DeferredRenderingDemoApp : public Application {
public:
    DeferredRenderingDemoApp() : Application("Deferred Rendering Demo", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        // 创建多个物体
        for (int i = 0; i < 20; i++) {
            auto& entity = m_scene->createEntity("Object " + std::to_string(i));
            auto& transform = entity.addComponent<TransformComponent>();
            transform.position = Vec3(
                (i % 5) * 3.0f - 6.0f,
                1.0f,
                (i / 5) * 3.0f - 3.0f
            );

            auto& renderer = entity.addComponent<MeshRendererComponent>();
            renderer.albedo = Vec4(
                0.2f + (i % 3) * 0.3f,
                0.5f,
                0.8f - (i % 3) * 0.2f,
                1.0f
            );
            renderer.metallic = (i % 4) * 0.25f;
            renderer.roughness = 0.3f + (i % 3) * 0.2f;
        }

        // 创建地面
        auto& ground = m_scene->createEntity("Ground");
        auto& groundTransform = ground.addComponent<TransformComponent>();
        groundTransform.scale = Vec3(30.0f, 0.1f, 30.0f);
        auto& groundRenderer = ground.addComponent<MeshRendererComponent>();
        groundRenderer.albedo = Vec4(0.4f, 0.4f, 0.4f, 1.0f);
        groundRenderer.metallic = 0.0f;
        groundRenderer.roughness = 0.9f;

        SPARK_INFO("Deferred Rendering Demo started!");
    }

    ~DeferredRenderingDemoApp() {
        m_renderer->waitIdle();
    }

protected:
    void onUpdate(float deltaTime) override {
        // 更新场景
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
        DeferredRenderingDemoApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
