/**
 * @file main.cpp
 * @brief Animation Demo Example
 *
 * Demonstrates animation system:
 * - Loading glTF animations
 * - Playing animations
 * - Animation blending
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include "renderer/animation_system.h"
#include <memory>

using namespace spark;

class AnimationDemoApp : public Application {
public:
    AnimationDemoApp() : Application("Animation Demo", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        // 添加动画系统
        m_scene->addSystem<AnimationSystem>();

        // 创建动画实体
        auto& character = m_scene->createEntity("Character");
        character.addComponent<TransformComponent>();

        auto& animator = character.addComponent<AnimatorComponent>();
        animator.animationPath = "assets/animations/idle.glb";
        animator.speed = 1.0f;
        animator.looping = true;
        animator.playOnStart = true;

        SPARK_INFO("Animation Demo started!");
    }

    ~AnimationDemoApp() {
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
        AnimationDemoApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
