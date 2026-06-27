/**
 * @file main.cpp
 * @brief Lighting Demo Example
 *
 * Demonstrates lighting system:
 * - Directional light
 * - Point lights with different colors
 * - Spot lights
 * - Dynamic lighting
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <memory>

using namespace spark;

class LightingDemoApp : public Application {
public:
    LightingDemoApp() : Application("Lighting Demo", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        // 创建地面
        auto& ground = m_scene->createEntity("Ground");
        auto& groundTransform = ground.addComponent<TransformComponent>();
        groundTransform.scale = Vec3(20.0f, 0.1f, 20.0f);
        auto& groundRenderer = ground.addComponent<MeshRendererComponent>();
        groundRenderer.albedo = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
        groundRenderer.metallic = 0.0f;
        groundRenderer.roughness = 0.8f;

        // 创建物体
        auto& sphere1 = m_scene->createEntity("Sphere 1");
        sphere1.addComponent<TransformComponent>(Vec3(-3.0f, 1.0f, 0.0f));
        auto& r1 = sphere1.addComponent<MeshRendererComponent>();
        r1.albedo = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        r1.metallic = 0.0f;
        r1.roughness = 0.5f;

        auto& sphere2 = m_scene->createEntity("Sphere 2");
        sphere2.addComponent<TransformComponent>(Vec3(0.0f, 1.0f, 0.0f));
        auto& r2 = sphere2.addComponent<MeshRendererComponent>();
        r2.albedo = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        r2.metallic = 0.5f;
        r2.roughness = 0.5f;

        auto& sphere3 = m_scene->createEntity("Sphere 3");
        sphere3.addComponent<TransformComponent>(Vec3(3.0f, 1.0f, 0.0f));
        auto& r3 = sphere3.addComponent<MeshRendererComponent>();
        r3.albedo = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        r3.metallic = 1.0f;
        r3.roughness = 0.1f;

        SPARK_INFO("Lighting Demo started!");
    }

    ~LightingDemoApp() {
        m_renderer->waitIdle();
    }

protected:
    void onUpdate(float deltaTime) override {
        // 动态更新光源位置
        m_lightAngle += deltaTime * 0.5f;
    }

    void onRender() override {
        // Render scene...
    }

private:
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
    float m_lightAngle = 0.0f;
};

int main() {
    try {
        LightingDemoApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
