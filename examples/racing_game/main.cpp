/**
 * @file main.cpp
 * @brief Racing Game Example
 *
 * 竞速游戏示例
 * - 车辆物理
 * - 赛道生成
 * - 计时系统
 * - 排行榜
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include "physics/vehicle.h"
#include <memory>

using namespace spark;

struct VehicleControlComponent : public Component {
    float throttle = 0.0f;
    float brake = 0.0f;
    float steering = 0.0f;
};

struct CheckpointComponent : public Component {
    int index = 0;
    bool passed = false;
};

class RacingGameApp : public Application {
public:
    RacingGameApp() : Application("Racing Game", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        // 创建赛道
        createTrack();

        // 创建车辆
        createVehicle();

        // 创建检查点
        createCheckpoints();

        SPARK_INFO("Racing Game started!");
        SPARK_INFO("Controls: W/S - Throttle/Brake, A/D - Steer");
    }

    ~RacingGameApp() {
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
        auto vehicles = m_scene->getEntitiesWith<VehicleControlComponent>();
        if (vehicles.empty()) return;

        auto& control = vehicles[0]->getComponent<VehicleControlComponent>();

        control.throttle = Input::isKeyPressed(GLFW_KEY_W) ? 1.0f : 0.0f;
        control.brake = Input::isKeyPressed(GLFW_KEY_S) ? 1.0f : 0.0f;
        control.steering = 0.0f;
        if (Input::isKeyPressed(GLFW_KEY_A)) control.steering = -1.0f;
        if (Input::isKeyPressed(GLFW_KEY_D)) control.steering = 1.0f;
    }

    void createTrack() {
        // 创建赛道地面
        auto& track = m_scene->createEntity("Track");
        auto& transform = track.addComponent<TransformComponent>();
        transform.scale = Vec3(100.0f, 0.1f, 100.0f);
    }

    void createVehicle() {
        auto& vehicle = m_scene->createEntity("Vehicle");
        vehicle.addComponent<TransformComponent>(Vec3(0.0f, 0.5f, 0.0f));
        vehicle.addComponent<VehicleControlComponent>();
    }

    void createCheckpoints() {
        for (int i = 0; i < 4; i++) {
            auto& checkpoint = m_scene->createEntity("Checkpoint " + std::to_string(i));
            checkpoint.addComponent<TransformComponent>(Vec3(i * 10.0f, 0.0f, 0.0f));
            auto& cp = checkpoint.addComponent<CheckpointComponent>();
            cp.index = i;
        }
    }

    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
};

int main() {
    try {
        RacingGameApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
