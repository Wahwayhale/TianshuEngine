/**
 * @file main.cpp
 * @brief Basic Cube Example
 *
 * Demonstrates basic engine functionality:
 * - Creating a window
 * - Setting up a renderer
 * - Creating a scene with entities
 * - Basic PBR rendering
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

class BasicCubeApp : public Application {
public:
    BasicCubeApp() : Application("Basic Cube Example", 1280, 720) {
        // Initialize renderer
        m_renderer = std::make_unique<Renderer>(
            getWindow().getNativeWindow(),
            getWindow().getWidth(),
            getWindow().getHeight()
        );

        // Setup rendering pipeline
        setupPipeline();

        // Create scene
        createScene();

        SPARK_INFO("Basic Cube Example started!");
    }

    ~BasicCubeApp() {
        m_renderer->waitIdle();
    }

protected:
    void onUpdate(float deltaTime) override {
        m_camera->update(deltaTime);
        m_scene->update(deltaTime);
    }

    void onRender() override {
        m_renderer->beginFrame();

        // Render scene...

        m_renderer->endFrame();
    }

private:
    void setupPipeline() {
        // Create descriptor set layout, shaders, pipeline, etc.
    }

    void createScene() {
        m_scene = std::make_unique<Scene>();
        m_camera = std::make_unique<Camera>(Vec3(0.0f, 2.0f, 5.0f));

        // Create a cube entity
        auto& cube = m_scene->createEntity("Cube");
        cube.addComponent<TransformComponent>();
        auto& renderer = cube.addComponent<MeshRendererComponent>();
        renderer.albedo = Vec4(0.8f, 0.2f, 0.2f, 1.0f);
    }

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<Camera> m_camera;
};

int main() {
    try {
        BasicCubeApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
