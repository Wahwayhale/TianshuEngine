/**
 * @file main.cpp
 * @brief Space Shooter Example
 *
 * 太空射击游戏示例
 * - 飞船控制
 * - 子弹系统
 * - 敌人生成
 * - 碰撞检测
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <memory>

using namespace spark;

struct BulletComponent : public Component {
    float speed = 20.0f;
    float lifetime = 3.0f;
    float currentTime = 0.0f;
};

struct EnemyComponent : public Component {
    float health = 100.0f;
    float moveSpeed = 5.0f;
};

class SpaceShooterApp : public Application {
public:
    SpaceShooterApp() : Application("Space Shooter", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        // 创建玩家飞船
        auto& player = m_scene->createEntity("Player Ship");
        player.addComponent<TransformComponent>(Vec3(0.0f, 0.0f, 0.0f));

        SPARK_INFO("Space Shooter started!");
        SPARK_INFO("Controls: WASD - Move, Space - Shoot");
    }

    ~SpaceShooterApp() {
        m_renderer->waitIdle();
    }

protected:
    void onUpdate(float deltaTime) override {
        handleInput(deltaTime);
        m_scene->update(deltaTime);
    }

    void onRender() override {
        // 渲染场景
    }

private:
    void handleInput(float deltaTime) {
        // 处理玩家输入
    }

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
};

int main() {
    try {
        SpaceShooterApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
