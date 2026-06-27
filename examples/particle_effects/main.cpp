/**
 * @file main.cpp
 * @brief Particle Effects Example
 *
 * 粒子效果示例
 * - 火焰效果
 * - 烟雾效果
 * - 烟花效果
 * - 雨雪效果
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <memory>

using namespace spark;

class ParticleEffectsApp : public Application {
public:
    ParticleEffectsApp() : Application("Particle Effects", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        // 创建粒子发射器
        createEmitter("Fire", Vec3(-3.0f, 0.0f, 0.0f));
        createEmitter("Smoke", Vec3(0.0f, 0.0f, 0.0f));
        createEmitter("Sparks", Vec3(3.0f, 0.0f, 0.0f));

        SPARK_INFO("Particle Effects started!");
    }

    ~ParticleEffectsApp() {
        m_renderer->waitIdle();
    }

protected:
    void onUpdate(float deltaTime) override {
        m_scene->update(deltaTime);
    }

    void onRender() override {
        // 渲染场景
    }

private:
    void createEmitter(const std::string& name, Vec3 position) {
        auto& emitter = m_scene->createEntity(name);
        emitter.addComponent<TransformComponent>(position);
    }

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
};

int main() {
    try {
        ParticleEffectsApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
