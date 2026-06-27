/**
 * @file main.cpp
 * @brief ECS Demo Example
 *
 * Demonstrates Entity Component System:
 * - Creating entities
 * - Adding components
 * - System processing
 * - Component iteration
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <memory>

using namespace spark;

// 自定义组件
struct RotatorComponent : public Component {
    float speed = 1.0f;
    Vec3 axis = Vec3(0.0f, 1.0f, 0.0f);
};

// 自定义系统
class RotatorSystem : public System {
public:
    void update(Scene& scene, float deltaTime) override {
        scene.view<TransformComponent, RotatorComponent>([&](Entity& entity) {
            auto& transform = entity.getComponent<TransformComponent>();
            auto& rotator = entity.getComponent<RotatorComponent>();

            transform.rotation += rotator.axis * rotator.speed * deltaTime * 57.2958f;
        });
    }
};

class ECSDemoApp : public Application {
public:
    ECSDemoApp() : Application("ECS Demo", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        // 添加旋转系统
        m_scene->addSystem<RotatorSystem>();

        // 创建多个旋转物体
        for (int i = 0; i < 10; i++) {
            auto& entity = m_scene->createEntity("Entity " + std::to_string(i));
            auto& transform = entity.addComponent<TransformComponent>();
            transform.position = Vec3(
                (i % 5) * 2.0f - 4.0f,
                (i / 5) * 2.0f + 1.0f,
                0.0f
            );

            auto& renderer = entity.addComponent<MeshRendererComponent>();
            renderer.albedo = Vec4(
                0.2f + (i % 3) * 0.3f,
                0.5f,
                0.8f - (i % 3) * 0.2f,
                1.0f
            );

            auto& rotator = entity.addComponent<RotatorComponent>();
            rotator.speed = 0.5f + (i * 0.1f);
            rotator.axis = Vec3(
                (i % 2 == 0) ? 1.0f : 0.0f,
                1.0f,
                (i % 3 == 0) ? 1.0f : 0.0f
            );
        }

        SPARK_INFO("ECS Demo started!");
    }

    ~ECSDemoApp() {
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
        ECSDemoApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
