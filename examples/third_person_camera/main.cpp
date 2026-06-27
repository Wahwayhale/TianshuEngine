/**
 * @file main.cpp
 * @brief Third Person Camera Example
 *
 * 第三人称相机示例
 * - 角色控制
 * - 相机跟随
 * - 平滑旋转
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include <memory>

using namespace spark;

struct ThirdPersonCameraComponent : public Component {
    float distance = 5.0f;
    float height = 2.0f;
    float sensitivity = 0.1f;
    float yaw = 0.0f;
    float pitch = 20.0f;
};

class ThirdPersonApp : public Application {
public:
    ThirdPersonApp() : Application("Third Person Camera", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        // 创建角色
        auto& character = m_scene->createEntity("Character");
        character.addComponent<TransformComponent>(Vec3(0.0f, 0.0f, 0.0f));

        // 创建相机
        auto& camera = m_scene->createEntity("Camera");
        camera.addComponent<TransformComponent>(Vec3(0.0f, 2.0f, 5.0f));
        camera.addComponent<ThirdPersonCameraComponent>();

        SPARK_INFO("Third Person Camera started!");
    }

    ~ThirdPersonApp() {
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
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
};

int main() {
    try {
        ThirdPersonApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
