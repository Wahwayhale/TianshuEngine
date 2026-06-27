#include "physics_system.h"
#include "jolt_physics.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include "core/log.h"

namespace spark {

PhysicsSystem::PhysicsSystem() {
    m_world = std::make_unique<JoltPhysicsWorld>();
    if (m_world->initialize()) {
        SPARK_CORE_INFO("Physics system initialized with Jolt.");
    } else {
        SPARK_CORE_ERROR("Failed to initialize physics system!");
    }
}

PhysicsSystem::~PhysicsSystem() {
    m_world->shutdown();
}

void PhysicsSystem::update(Scene& scene, float deltaTime) {
    if (!m_world->isInitialized()) return;

    // 同步 ECS 到物理世界
    syncToPhysics(scene);

    // 更新物理模拟
    m_world->update(deltaTime);

    // 同步物理世界到 ECS
    syncFromPhysics(scene);
}

void PhysicsSystem::setGravity(const Vec3& gravity) {
    if (m_world) {
        m_world->setGravity(gravity);
    }
}

Vec3 PhysicsSystem::getGravity() const {
    if (m_world) {
        return m_world->getGravity();
    }
    return Vec3(0, -9.81f, 0);
}

bool PhysicsSystem::raycast(const Vec3& origin, const Vec3& direction, float maxDistance, JoltPhysicsWorld::RaycastHit& hit) {
    if (m_world) {
        return m_world->raycast(origin, direction, maxDistance, hit);
    }
    return false;
}

void PhysicsSystem::syncToPhysics(Scene& scene) {
    // 遍历所有有 TransformComponent + RigidBodyComponent 的实体
    scene.view<TransformComponent, RigidBodyComponent>([&](Entity& entity) {
        auto& transform = entity.getComponent<TransformComponent>();
        auto& rb = entity.getComponent<RigidBodyComponent>();

        EntityID entityId = entity.getID();

        // 检查是否已有物理体
        auto it = m_bodyHandles.find(entityId);
        if (it == m_bodyHandles.end()) {
            // 创建物理体
            PhysicsBodyDesc desc;

            // 设置类型
            if (rb.isStatic) {
                desc.type = PhysicsBodyType::Static;
            } else if (rb.isKinematic) {
                desc.type = PhysicsBodyType::Kinematic;
            } else {
                desc.type = PhysicsBodyType::Dynamic;
            }

            // 设置形状
            if (entity.hasComponent<ColliderComponent>()) {
                auto& collider = entity.getComponent<ColliderComponent>();

                switch (collider.colliderType) {
                    case ColliderComponent::ColliderType::Box:
                        desc.shapeType = PhysicsShapeType::Box;
                        desc.halfExtents = collider.size * transform.scale;
                        break;
                    case ColliderComponent::ColliderType::Sphere:
                        desc.shapeType = PhysicsShapeType::Sphere;
                        desc.radius = collider.radius * glm::max(transform.scale.x, glm::max(transform.scale.y, transform.scale.z));
                        break;
                    case ColliderComponent::ColliderType::Capsule:
                        desc.shapeType = PhysicsShapeType::Capsule;
                        desc.capsuleRadius = collider.radius;
                        desc.capsuleHeight = collider.height;
                        break;
                }

                desc.isTrigger = collider.isTrigger;
            } else {
                // 默认 Box 形状
                desc.shapeType = PhysicsShapeType::Box;
                desc.halfExtents = Vec3(0.5f) * transform.scale;
            }

            // 设置属性
            desc.position = transform.position;
            desc.rotation = transform.rotation;
            desc.mass = rb.mass;
            desc.friction = rb.friction;
            desc.restitution = rb.restitution;
            desc.linearDamping = rb.linearDamping;
            desc.angularDamping = rb.angularDamping;
            desc.userData = entityId;

            // 创建物理体
            PhysicsBodyHandle handle = m_world->createBody(desc);
            if (handle != InvalidBodyHandle) {
                m_bodyHandles[entityId] = handle;

                // 设置初始速度
                if (!rb.isStatic) {
                    m_world->setVelocity(handle, rb.velocity);
                }
            }
        } else {
            // 更新物理体属性
            PhysicsBodyHandle handle = it->second;

            // 如果不是动态物体，同步位置
            if (rb.isStatic || rb.isKinematic) {
                m_world->setPosition(handle, transform.position);
            }

            // 应用力
            if (!rb.isStatic && !rb.isKinematic) {
                if (rb.useGravity) {
                    rb.force += Vec3(0, -9.81f, 0) * rb.mass;
                }

                if (glm::length(rb.force) > 0.0001f) {
                    m_world->applyForce(handle, rb.force);
                }

                if (glm::length(rb.torque) > 0.0001f) {
                    m_world->applyTorque(handle, rb.torque);
                }
            }
        }
    });
}

void PhysicsSystem::syncFromPhysics(Scene& scene) {
    // 遍历所有有物理体的实体
    for (auto& [entityId, handle] : m_bodyHandles) {
        try {
            Entity& entity = scene.getEntity(entityId);
            auto& transform = entity.getComponent<TransformComponent>();
            auto& rb = entity.getComponent<RigidBodyComponent>();

            // 同步位置和旋转
            if (!rb.isStatic) {
                transform.position = m_world->getPosition(handle);
                // 注意：Jolt 使用四元数，需要转换为欧拉角
                // transform.rotation = ...;
            }

            // 同步速度
            rb.velocity = m_world->getVelocity(handle);

            // 清除力
            rb.force = Vec3(0.0f);
            rb.torque = Vec3(0.0f);

        } catch (...) {
            // 实体可能已被删除
        }
    }

    // 处理碰撞事件
    auto collisionEvents = m_world->getCollisionEvents();
    for (const auto& event : collisionEvents) {
        // 可以在这里触发碰撞回调
    }

    // 处理触发器事件
    auto triggerEvents = m_world->getTriggerEvents();
    for (const auto& event : triggerEvents) {
        // 可以在这里触发触发器回调
    }
}

} // namespace spark
