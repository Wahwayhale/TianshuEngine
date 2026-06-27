/**
 * @file main.cpp
 * @brief PBR Materials Example
 *
 * Demonstrates PBR material system:
 * - Different material properties (metallic, roughness)
 * - Multiple light sources
 * - Material spheres showcase
 */

#include "core/application.h"
#include "core/log.h"
#include "renderer/renderer.h"
#include "renderer/pipeline.h"
#include "renderer/shader.h"
#include "renderer/mesh.h"
#include "renderer/camera.h"
#include "renderer/descriptor_set.h"
#include "renderer/texture.h"
#include "renderer/render_system.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <memory>

using namespace spark;

class PBRMaterialsApp : public Application {
public:
    PBRMaterialsApp() : Application("PBR Materials", 1280, 720) {
        m_renderer = std::make_unique<Renderer>(
            getWindow().getNativeWindow(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );

        setupRendering();
        createScene();

        SPARK_INFO("PBR Materials Example started!");
    }

    ~PBRMaterialsApp() {
        m_renderer->waitIdle();
    }

protected:
    void onUpdate(float deltaTime) override {
        m_camera->update(deltaTime);
    }

    void onRender() override {
        m_renderer->beginFrame();
        // Render scene...
        m_renderer->endFrame();
    }

private:
    void setupRendering() {
        // Setup descriptor sets, pipeline, etc.
    }

    void createScene() {
        m_scene = std::make_unique<Scene>();
        m_camera = std::make_unique<Camera>(Vec3(0.0f, 3.0f, 8.0f));

        // 创建不同材质的球体
        // 行 1: 金属度变化
        for (int i = 0; i < 5; i++) {
            auto& sphere = m_scene->createEntity("Metal " + std::to_string(i));
            auto& transform = sphere.addComponent<TransformComponent>();
            transform.position = Vec3(i * 2.5f - 5.0f, 1.0f, 0.0f);

            auto& renderer = sphere.addComponent<MeshRendererComponent>();
            renderer.albedo = Vec4(0.8f, 0.2f, 0.2f, 1.0f);
            renderer.metallic = i * 0.25f;  // 0.0 到 1.0
            renderer.roughness = 0.3f;
        }

        // 行 2: 粗糙度变化
        for (int i = 0; i < 5; i++) {
            auto& sphere = m_scene->createEntity("Rough " + std::to_string(i));
            auto& transform = sphere.addComponent<TransformComponent>();
            transform.position = Vec3(i * 2.5f - 5.0f, 1.0f, 3.0f);

            auto& renderer = sphere.addComponent<MeshRendererComponent>();
            renderer.albedo = Vec4(0.2f, 0.5f, 0.8f, 1.0f);
            renderer.metallic = 0.5f;
            renderer.roughness = i * 0.25f;  // 0.0 到 1.0
        }

        // 不同颜色的金属
        auto& gold = m_scene->createEntity("Gold");
        gold.addComponent<TransformComponent>(Vec3(-3.0f, 1.0f, -3.0f));
        auto& goldRenderer = gold.addComponent<MeshRendererComponent>();
        goldRenderer.albedo = Vec4(1.0f, 0.84f, 0.0f, 1.0f);
        goldRenderer.metallic = 1.0f;
        goldRenderer.roughness = 0.1f;

        auto& copper = m_scene->createEntity("Copper");
        copper.addComponent<TransformComponent>(Vec3(0.0f, 1.0f, -3.0f));
        auto& copperRenderer = copper.addComponent<MeshRendererComponent>();
        copperRenderer.albedo = Vec4(0.95f, 0.64f, 0.54f, 1.0f);
        copperRenderer.metallic = 1.0f;
        copperRenderer.roughness = 0.2f;

        auto& chrome = m_scene->createEntity("Chrome");
        chrome.addComponent<TransformComponent>(Vec3(3.0f, 1.0f, -3.0f));
        auto& chromeRenderer = chrome.addComponent<MeshRendererComponent>();
        chromeRenderer.albedo = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
        chromeRenderer.metallic = 1.0f;
        chromeRenderer.roughness = 0.05f;
    }

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<Camera> m_camera;
};

int main() {
    try {
        PBRMaterialsApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
