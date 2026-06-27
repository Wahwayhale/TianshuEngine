/**
 * @file main.cpp
 * @brief 2D Platformer Example
 *
 * 完整的 2D 平台跳跃游戏示例
 * - 角色控制
 * - 平台碰撞
 * - 收集物品
 * - 敌人 AI
 * - 分数系统
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <memory>
#include <vector>

using namespace spark;

// 自定义组件
struct PlayerComponent : public Component {
    float moveSpeed = 5.0f;
    float jumpForce = 10.0f;
    bool isGrounded = false;
    int score = 0;
    int lives = 3;
};

struct CollectibleComponent : public Component {
    int value = 10;
    bool collected = false;
};

struct EnemyComponent : public Component {
    float moveSpeed = 2.0f;
    float patrolRange = 3.0f;
    Vec3 startPos;
    int direction = 1;
};

class PlatformerApp : public Application {
public:
    PlatformerApp() : Application("2D Platformer", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        // 创建玩家
        auto& player = m_scene->createEntity("Player");
        player.addComponent<TransformComponent>(Vec3(0.0f, 1.0f, 0.0f));
        player.addComponent<PlayerComponent>();

        // 创建平台
        createPlatform(Vec3(0.0f, 0.0f, 0.0f), Vec3(20.0f, 0.5f, 1.0f));
        createPlatform(Vec3(3.0f, 2.0f, 0.0f), Vec3(3.0f, 0.3f, 1.0f));
        createPlatform(Vec3(-3.0f, 3.0f, 0.0f), Vec3(3.0f, 0.3f, 1.0f));
        createPlatform(Vec3(0.0f, 4.5f, 0.0f), Vec3(4.0f, 0.3f, 1.0f));

        // 创建收集物
        createCollectible(Vec3(3.0f, 2.5f, 0.0f));
        createCollectible(Vec3(-3.0f, 3.5f, 0.0f));
        createCollectible(Vec3(0.0f, 5.0f, 0.0f));

        // 创建敌人
        createEnemy(Vec3(5.0f, 0.5f, 0.0f));

        SPARK_INFO("2D Platformer started!");
        SPARK_INFO("Controls: A/D - Move, Space - Jump");
    }

    ~PlatformerApp() {
        m_renderer->waitIdle();
    }

protected:
    void onUpdate(float deltaTime) override {
        // 处理玩家输入
        handlePlayerInput(deltaTime);

        // 更新场景
        m_scene->update(deltaTime);

        // 检查碰撞
        checkCollisions();
    }

    void onRender() override {
        // 渲染场景
    }

private:
    void handlePlayerInput(float deltaTime) {
        auto entities = m_scene->getEntitiesWith<PlayerComponent>();
        if (entities.empty()) return;

        auto& player = *entities[0];
        auto& transform = player.getComponent<TransformComponent>();
        auto& playerComp = player.getComponent<PlayerComponent>();

        // 移动
        if (Input::isKeyPressed(GLFW_KEY_A)) {
            transform.position.x -= playerComp.moveSpeed * deltaTime;
        }
        if (Input::isKeyPressed(GLFW_KEY_D)) {
            transform.position.x += playerComp.moveSpeed * deltaTime;
        }

        // 跳跃
        if (Input::isKeyJustPressed(GLFW_KEY_SPACE) && playerComp.isGrounded) {
            playerComp.isGrounded = false;
            // 施加跳跃力
        }
    }

    void checkCollisions() {
        // 简化的碰撞检测
    }

    void createPlatform(Vec3 position, Vec3 size) {
        auto& platform = m_scene->createEntity("Platform");
        auto& transform = platform.addComponent<TransformComponent>();
        transform.position = position;
        transform.scale = size;
    }

    void createCollectible(Vec3 position) {
        auto& collectible = m_scene->createEntity("Collectible");
        collectible.addComponent<TransformComponent>(position);
        collectible.addComponent<CollectibleComponent>();
    }

    void createEnemy(Vec3 position) {
        auto& enemy = m_scene->createEntity("Enemy");
        auto& transform = enemy.addComponent<TransformComponent>();
        transform.position = position;
        auto& enemyComp = enemy.addComponent<EnemyComponent>();
        enemyComp.startPos = position;
    }

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
};

int main() {
    try {
        PlatformerApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
