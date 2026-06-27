#include "destruction.h"
#include "jolt_physics.h"
#include "core/log.h"
#include <algorithm>
#include <random>

namespace spark {

DestructionSystem::DestructionSystem() = default;
DestructionSystem::~DestructionSystem() = default;

bool DestructionSystem::initialize() {
    SPARK_CORE_INFO("Destruction system initialized.");
    return true;
}

void DestructionSystem::shutdown() {
    clearFragments();
    SPARK_CORE_INFO("Destruction system shutdown.");
}

std::vector<Fragment> DestructionSystem::destroyObject(PhysicsBodyHandle body, const Vec3& impactPoint,
                                                        const Vec3& impactDirection, const DestructionSettings& settings) {
    std::vector<Fragment> fragments;

    switch (settings.mode) {
        case DestructionMode::Voronoi:
            fragments = voronoiFracture(body, impactPoint, impactDirection, settings.fragmentCount);
            break;
        case DestructionMode::Radial:
            fragments = radialFracture(body, impactPoint, impactDirection, settings.fragmentCount);
            break;
        case DestructionMode::Planar:
            fragments = planarFracture(body, impactPoint, impactDirection, settings.fragmentCount);
            break;
        case DestructionMode::Shatter:
            fragments = voronoiFracture(body, impactPoint, impactDirection, settings.fragmentCount * 2);
            break;
    }

    // 设置碎片属性
    for (auto& fragment : fragments) {
        fragment.lifetime = settings.fragmentLifetime;

        // 应用冲击力
        if (settings.usePhysics) {
            Vec3 forceDir = glm::normalize(impactDirection + Vec3(
                (rand() % 100 - 50) / 100.0f,
                (rand() % 100) / 100.0f,
                (rand() % 100 - 50) / 100.0f
            ));
            fragment.velocity = forceDir * settings.impactForce / settings.fragmentMass;

            // 随机角速度
            fragment.angularVelocity = Vec3(
                (rand() % 100 - 50) / 10.0f,
                (rand() % 100 - 50) / 10.0f,
                (rand() % 100 - 50) / 10.0f
            );
        }
    }

    // 添加到活跃碎片
    m_fragments.insert(m_fragments.end(), fragments.begin(), fragments.end());

    // 限制碎片数量
    while (m_fragments.size() > m_maxFragments) {
        m_fragments.erase(m_fragments.begin());
    }

    SPARK_CORE_INFO("Object destroyed: {0} fragments created", fragments.size());
    return fragments;
}

void DestructionSystem::update(float deltaTime) {
    // 更新碎片生命周期
    for (auto it = m_fragments.begin(); it != m_fragments.end();) {
        it->lifetime -= deltaTime;

        if (it->lifetime <= 0.0f) {
            it = m_fragments.erase(it);
        } else {
            ++it;
        }
    }
}

void DestructionSystem::clearFragments() {
    m_fragments.clear();
}

std::vector<Fragment> DestructionSystem::voronoiFracture(PhysicsBodyHandle body, const Vec3& impactPoint,
                                                          const Vec3& impactDirection, int fragmentCount) {
    std::vector<Fragment> fragments;

    // 简化的 Voronoi 破碎实现
    // 实际实现需要 Voronoi 图计算
    std::default_random_engine rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (int i = 0; i < fragmentCount; i++) {
        Fragment fragment;
        fragment.position = impactPoint + Vec3(dist(rng), dist(rng), dist(rng)) * 0.5f;
        fragment.size = 0.1f + dist(rng) * 0.1f;
        fragments.push_back(fragment);
    }

    return fragments;
}

std::vector<Fragment> DestructionSystem::radialFracture(PhysicsBodyHandle body, const Vec3& impactPoint,
                                                         const Vec3& impactDirection, int fragmentCount) {
    std::vector<Fragment> fragments;

    // 径向破碎：从冲击点向外辐射
    for (int i = 0; i < fragmentCount; i++) {
        float angle = (2.0f * 3.14159f * i) / fragmentCount;

        Fragment fragment;
        fragment.position = impactPoint + Vec3(cos(angle), 0.0f, sin(angle)) * 0.5f;
        fragment.size = 0.15f;
        fragments.push_back(fragment);
    }

    return fragments;
}

std::vector<Fragment> DestructionSystem::planarFracture(PhysicsBodyHandle body, const Vec3& impactPoint,
                                                         const Vec3& impactDirection, int fragmentCount) {
    std::vector<Fragment> fragments;

    // 平面破碎：沿冲击方向切割
    Vec3 normal = glm::normalize(impactDirection);
    Vec3 tangent = glm::cross(normal, Vec3(0.0f, 1.0f, 0.0f));
    if (glm::length(tangent) < 0.001f) {
        tangent = glm::cross(normal, Vec3(1.0f, 0.0f, 0.0f));
    }
    tangent = glm::normalize(tangent);
    Vec3 bitangent = glm::cross(normal, tangent);

    for (int i = 0; i < fragmentCount; i++) {
        float offset = (i - fragmentCount / 2.0f) * 0.2f;

        Fragment fragment;
        fragment.position = impactPoint + tangent * offset;
        fragment.size = 0.15f;
        fragments.push_back(fragment);
    }

    return fragments;
}

} // namespace spark
