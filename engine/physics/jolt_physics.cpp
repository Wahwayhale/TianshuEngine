#include "jolt_physics.h"
#include "core/log.h"
#include <algorithm>
#include <cmath>

namespace spark {

PhysicsWorld::PhysicsWorld() = default;
PhysicsWorld::~PhysicsWorld() = default;

bool PhysicsWorld::initialize() {
    if (m_initialized) return true;

    m_initialized = true;
    SPARK_CORE_INFO("Physics World initialized.");
    return true;
}

void PhysicsWorld::shutdown() {
    if (!m_initialized) return;

    m_bodies.clear();
    m_collisionEvents.clear();
    m_triggerEvents.clear();

    m_initialized = false;
    SPARK_CORE_INFO("Physics World shutdown.");
}

void PhysicsWorld::update(float deltaTime) {
    if (!m_initialized) return;

    // 限制时间步长
    float dt = std::min(deltaTime, 1.0f / 30.0f);

    // 清除事件
    m_collisionEvents.clear();
    m_triggerEvents.clear();

    // 1. 应用重力
    for (auto& [handle, body] : m_bodies) {
        if (!body.isStatic && body.desc.type == PhysicsBodyType::Dynamic) {
            body.force += m_gravity * body.desc.mass;
        }
    }

    // 2. 积分速度
    integrateVelocities(dt);

    // 3. 碰撞检测
    detectCollisions();

    // 4. 碰撞响应
    resolveCollisions();

    // 5. 积分位置
    integratePositions(dt);

    // 6. 清除力
    for (auto& [handle, body] : m_bodies) {
        body.force = Vec3(0.0f);
        body.torque = Vec3(0.0f);
    }
}

PhysicsBodyHandle PhysicsWorld::createBody(const PhysicsBodyDesc& desc) {
    PhysicsBodyHandle handle = m_nextHandle++;

    PhysicsBody body;
    body.desc = desc;
    body.position = desc.position;
    body.rotation = desc.rotation;
    body.velocity = Vec3(0.0f);
    body.angularVelocity = Vec3(0.0f);
    body.force = Vec3(0.0f);
    body.torque = Vec3(0.0f);
    body.isStatic = (desc.type == PhysicsBodyType::Static);

    m_bodies[handle] = body;

    SPARK_CORE_INFO("Physics body created: {0}", handle);
    return handle;
}

void PhysicsWorld::destroyBody(PhysicsBodyHandle handle) {
    m_bodies.erase(handle);
}

void PhysicsWorld::setPosition(PhysicsBodyHandle handle, const Vec3& position) {
    auto it = m_bodies.find(handle);
    if (it != m_bodies.end()) {
        it->second.position = position;
    }
}

void PhysicsWorld::setRotation(PhysicsBodyHandle handle, const Vec3& rotation) {
    auto it = m_bodies.find(handle);
    if (it != m_bodies.end()) {
        it->second.rotation = rotation;
    }
}

void PhysicsWorld::setVelocity(PhysicsBodyHandle handle, const Vec3& velocity) {
    auto it = m_bodies.find(handle);
    if (it != m_bodies.end()) {
        it->second.velocity = velocity;
    }
}

void PhysicsWorld::setAngularVelocity(PhysicsBodyHandle handle, const Vec3& velocity) {
    auto it = m_bodies.find(handle);
    if (it != m_bodies.end()) {
        it->second.angularVelocity = velocity;
    }
}

Vec3 PhysicsWorld::getPosition(PhysicsBodyHandle handle) const {
    auto it = m_bodies.find(handle);
    if (it != m_bodies.end()) {
        return it->second.position;
    }
    return Vec3(0.0f);
}

Vec3 PhysicsWorld::getRotation(PhysicsBodyHandle handle) const {
    auto it = m_bodies.find(handle);
    if (it != m_bodies.end()) {
        return it->second.rotation;
    }
    return Vec3(0.0f);
}

Vec3 PhysicsWorld::getVelocity(PhysicsBodyHandle handle) const {
    auto it = m_bodies.find(handle);
    if (it != m_bodies.end()) {
        return it->second.velocity;
    }
    return Vec3(0.0f);
}

Vec3 PhysicsWorld::getAngularVelocity(PhysicsBodyHandle handle) const {
    auto it = m_bodies.find(handle);
    if (it != m_bodies.end()) {
        return it->second.angularVelocity;
    }
    return Vec3(0.0f);
}

void PhysicsWorld::applyForce(PhysicsBodyHandle handle, const Vec3& force) {
    auto it = m_bodies.find(handle);
    if (it != m_bodies.end() && !it->second.isStatic) {
        it->second.force += force;
    }
}

void PhysicsWorld::applyImpulse(PhysicsBodyHandle handle, const Vec3& impulse) {
    auto it = m_bodies.find(handle);
    if (it != m_bodies.end() && !it->second.isStatic) {
        it->second.velocity += impulse / it->second.desc.mass;
    }
}

void PhysicsWorld::applyTorque(PhysicsBodyHandle handle, const Vec3& torque) {
    auto it = m_bodies.find(handle);
    if (it != m_bodies.end() && !it->second.isStatic) {
        it->second.torque += torque;
    }
}

bool PhysicsWorld::raycast(const Vec3& origin, const Vec3& direction, float maxDistance, RaycastHit& hit) const {
    bool found = false;
    float closestDist = maxDistance;

    for (const auto& [handle, body] : m_bodies) {
        // 简化的 AABB 射线检测
        Vec3 halfSize = body.desc.halfExtents * body.desc.scale;
        Vec3 boxMin = body.position - halfSize;
        Vec3 boxMax = body.position + halfSize;

        // 射线-AABB 相交
        Vec3 invDir = 1.0f / direction;
        Vec3 t1 = (boxMin - origin) * invDir;
        Vec3 t2 = (boxMax - origin) * invDir;

        Vec3 tMin = glm::min(t1, t2);
        Vec3 tMax = glm::max(t1, t2);

        float tmin = std::max({tMin.x, tMin.y, tMin.z});
        float tmax = std::min({tMax.x, tMax.y, tMax.z});

        if (tmax >= tmin && tmax > 0 && tmin < closestDist) {
            closestDist = tmin;
            hit.body = handle;
            hit.point = origin + direction * tmin;
            hit.distance = tmin;

            // 计算法线
            Vec3 localHit = hit.point - body.position;
            Vec3 absLocal = glm::abs(localHit);
            if (absLocal.x > absLocal.y && absLocal.x > absLocal.z) {
                hit.normal = Vec3(localHit.x > 0 ? 1.0f : -1.0f, 0.0f, 0.0f);
            } else if (absLocal.y > absLocal.z) {
                hit.normal = Vec3(0.0f, localHit.y > 0 ? 1.0f : -1.0f, 0.0f);
            } else {
                hit.normal = Vec3(0.0f, 0.0f, localHit.z > 0 ? 1.0f : -1.0f);
            }

            found = true;
        }
    }

    return found;
}

std::vector<CollisionEvent> PhysicsWorld::getCollisionEvents() const {
    return m_collisionEvents;
}

std::vector<TriggerEvent> PhysicsWorld::getTriggerEvents() const {
    return m_triggerEvents;
}

void PhysicsWorld::integrateVelocities(float deltaTime) {
    for (auto& [handle, body] : m_bodies) {
        if (body.isStatic) continue;

        // 计算加速度
        Vec3 acceleration = body.force / body.desc.mass;

        // 更新速度
        body.velocity += acceleration * deltaTime;

        // 应用阻尼
        body.velocity *= (1.0f - body.desc.linearDamping * deltaTime);
        body.angularVelocity *= (1.0f - body.desc.angularDamping * deltaTime);
    }
}

void PhysicsWorld::detectCollisions() {
    // 简化的碰撞检测（O(n²)）
    std::vector<PhysicsBodyHandle> handles;
    for (const auto& [handle, body] : m_bodies) {
        handles.push_back(handle);
    }

    for (size_t i = 0; i < handles.size(); i++) {
        for (size_t j = i + 1; j < handles.size(); j++) {
            auto& bodyA = m_bodies[handles[i]];
            auto& bodyB = m_bodies[handles[j]];

            // 跳过两个静态物体
            if (bodyA.isStatic && bodyB.isStatic) continue;

            // 简化的 AABB 碰撞检测
            Vec3 halfSizeA = bodyA.desc.halfExtents * bodyA.desc.scale;
            Vec3 halfSizeB = bodyB.desc.halfExtents * bodyB.desc.scale;

            Vec3 minA = bodyA.position - halfSizeA;
            Vec3 maxA = bodyA.position + halfSizeA;
            Vec3 minB = bodyB.position - halfSizeB;
            Vec3 maxB = bodyB.position + halfSizeB;

            // AABB 重叠检测
            if (minA.x <= maxB.x && maxA.x >= minB.x &&
                minA.y <= maxB.y && maxA.y >= minB.y &&
                minA.z <= maxB.z && maxA.z >= minB.z) {

                // 计算穿透深度
                float overlapX = std::min(maxA.x - minB.x, maxB.x - minA.x);
                float overlapY = std::min(maxA.y - minB.y, maxB.y - minA.y);
                float overlapZ = std::min(maxA.z - minB.z, maxB.z - minA.z);

                float penetration = std::min({overlapX, overlapY, overlapZ});

                // 计算碰撞法线
                Vec3 normal;
                if (penetration == overlapX) {
                    normal = Vec3(bodyA.position.x < bodyB.position.x ? -1.0f : 1.0f, 0.0f, 0.0f);
                } else if (penetration == overlapY) {
                    normal = Vec3(0.0f, bodyA.position.y < bodyB.position.y ? -1.0f : 1.0f, 0.0f);
                } else {
                    normal = Vec3(0.0f, 0.0f, bodyA.position.z < bodyB.position.z ? -1.0f : 1.0f);
                }

                // 记录碰撞事件
                CollisionEvent event;
                event.bodyA = handles[i];
                event.bodyB = handles[j];
                event.contactPoint = (bodyA.position + bodyB.position) * 0.5f;
                event.contactNormal = normal;
                event.impulse = penetration;
                m_collisionEvents.push_back(event);
            }
        }
    }
}

void PhysicsWorld::resolveCollisions() {
    for (const auto& event : m_collisionEvents) {
        auto& bodyA = m_bodies[event.bodyA];
        auto& bodyB = m_bodies[event.bodyB];

        // 分离物体
        float totalMass = 0.0f;
        float invMassA = bodyA.isStatic ? 0.0f : 1.0f / bodyA.desc.mass;
        float invMassB = bodyB.isStatic ? 0.0f : 1.0f / bodyB.desc.mass;
        totalMass = invMassA + invMassB;

        if (totalMass <= 0.0f) continue;

        float ratioA = invMassA / totalMass;
        float ratioB = invMassB / totalMass;

        bodyA.position -= event.contactNormal * event.impulse * ratioA;
        bodyB.position += event.contactNormal * event.impulse * ratioB;

        // 计算相对速度
        Vec3 relativeVel = bodyB.velocity - bodyA.velocity;
        float velAlongNormal = glm::dot(relativeVel, event.contactNormal);

        // 如果物体已经在分离，不处理
        if (velAlongNormal > 0) continue;

        // 计算恢复系数
        float restitution = std::min(bodyA.desc.restitution, bodyB.desc.restitution);

        // 计算冲量标量
        float j = -(1.0f + restitution) * velAlongNormal / totalMass;

        // 应用冲量
        Vec3 impulse = j * event.contactNormal;

        if (!bodyA.isStatic) {
            bodyA.velocity -= impulse * invMassA;
        }
        if (!bodyB.isStatic) {
            bodyB.velocity += impulse * invMassB;
        }
    }
}

void PhysicsWorld::integratePositions(float deltaTime) {
    for (auto& [handle, body] : m_bodies) {
        if (body.isStatic) continue;

        // 更新位置
        body.position += body.velocity * deltaTime;

        // 更新旋转
        body.rotation += body.angularVelocity * deltaTime * (180.0f / 3.14159f);

        // 地面碰撞（简化）
        if (body.position.y < 0.0f) {
            body.position.y = 0.0f;
            if (body.velocity.y < 0.0f) {
                body.velocity.y = -body.velocity.y * body.desc.restitution;
                if (std::abs(body.velocity.y) < 0.1f) {
                    body.velocity.y = 0.0f;
                }
            }
        }
    }
}

} // namespace spark
