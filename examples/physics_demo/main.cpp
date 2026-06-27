/**
 * @file main.cpp
 * @brief Physics Demo Example
 *
 * Demonstrates physics system:
 * - Rigid body dynamics
 * - Collision detection
 * - Gravity simulation
 * - Stacking objects
 */

#include "core/application.h"
#include "core/log.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include "physics/physics_system.h"
#include <memory>

using namespace spark;

class PhysicsDemoApp : public Application {
public:
    PhysicsDemoApp() : Application("Physics Demo", 1280, 720) {
        m_scene = std::make_unique<Scene>();

        // Add physics system
        auto& physics = m_scene->addSystem<PhysicsSystem>();
        physics.setGravity(Vec3(0.0f, -9.81f, 0.0f));

        // Create ground
        auto& ground = m_scene->createEntity("Ground");
        auto& groundTransform = ground.addComponent<TransformComponent>();
        groundTransform.scale = Vec3(20.0f, 0.1f, 20.0f);
        auto& groundCollider = ground.addComponent<ColliderComponent>();
        groundCollider.colliderType = ColliderComponent::ColliderType::Box;
        groundCollider.size = Vec3(10.0f, 0.05f, 10.0f);
        auto& groundRb = ground.addComponent<RigidBodyComponent>();
        groundRb.isStatic = true;

        // Create falling cubes
        for (int i = 0; i < 5; i++) {
            auto& cube = m_scene->createEntity("Cube " + std::to_string(i));
            auto& transform = cube.addComponent<TransformComponent>();
            transform.position = Vec3(0.0f, 2.0f + i * 1.5f, 0.0f);

            auto& renderer = cube.addComponent<MeshRendererComponent>();
            renderer.albedo = Vec4(0.2f + i * 0.15f, 0.5f, 0.8f - i * 0.1f, 1.0f);

            auto& collider = cube.addComponent<ColliderComponent>();
            collider.colliderType = ColliderComponent::ColliderType::Box;
            collider.size = Vec3(0.5f);

            auto& rb = cube.addComponent<RigidBodyComponent>();
            rb.mass = 1.0f;
            rb.restitution = 0.3f;
        }

        // Create bouncing balls
        for (int i = 0; i < 3; i++) {
            auto& ball = m_scene->createEntity("Ball " + std::to_string(i));
            auto& transform = ball.addComponent<TransformComponent>();
            transform.position = Vec3(3.0f + i * 2.0f, 5.0f + i * 3.0f, 0.0f);

            auto& renderer = ball.addComponent<MeshRendererComponent>();
            renderer.albedo = Vec4(0.9f, 0.3f, 0.2f + i * 0.2f, 1.0f);

            auto& collider = ball.addComponent<ColliderComponent>();
            collider.colliderType = ColliderComponent::ColliderType::Sphere;
            collider.radius = 0.5f;

            auto& rb = ball.addComponent<RigidBodyComponent>();
            rb.mass = 2.0f;
            rb.restitution = 0.6f;  // High bounce
        }

        SPARK_INFO("Physics Demo started!");
    }

    ~PhysicsDemoApp() {
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
        PhysicsDemoApp app;
        app.run();
    } catch (const std::exception& e) {
        SPARK_CRITICAL("Error: {0}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
