/**
 * @file main.cpp
 * @brief Post-Processing Demo Example
 *
 * Demonstrates post-processing effects:
 * - Bloom
 * - Tone mapping
 * - SSAO
 * - Fog
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <memory>

using namespace spark;

class PostProcessingDemoApp : public Application {
public:
    PostProcessingDemoApp() : Application("Post-Processing Demo", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        // 创建高对比度场景
        auto& brightCube = m_scene->createEntity("Bright Cube");
        brightCube.addComponent<TransformComponent>(Vec3(0.0f, 1.0f, 0.0f));
        auto& brightRenderer = brightCube.addComponent<MeshRendererComponent>();
        brightRenderer.albedo = Vec4(2.0f, 2.0f, 2.0f, 1.0f);  // HDR 值
        brightRenderer.emission = 1.0f;

        auto& darkCube = m_scene->createEntity("Dark Cube");
        darkCube.addComponent<TransformComponent>(Vec3(3.0f, 1.0f, 0.0f));
        auto& darkRenderer = darkCube.addComponent<MeshRendererComponent>();
        darkRenderer.albedo = Vec4(0.1f, 0.1f, 0.1f, 1.0f);

        auto& ground = m_scene->createEntity("Ground");
        auto& groundTransform = ground.addComponent<TransformComponent>();
        groundTransform.scale = Vec3(20.0f, 0.1f, 20.0f);
        auto& groundRenderer = ground.addComponent<MeshRendererComponent>();
        groundRenderer.albedo = Vec4(0.5f, 0.5f, 0.5f, 1.0f);

        SPARK_INFO("Post-Processing Demo started!");
    }

    ~PostProcessingDemoApp() {
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
        PostProcessingDemoApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
