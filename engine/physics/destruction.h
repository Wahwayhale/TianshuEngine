#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <cstdint>

namespace spark {

using PhysicsBodyHandle = uint32_t;

// 破碎模式
enum class DestructionMode {
    Voronoi,        // Voronoi 破碎
    Radial,         // 径向破碎
    Planar,         // 平面破碎
    Shatter         // 粉碎
};

// 破碎设置
struct DestructionSettings {
    DestructionMode mode = DestructionMode::Voronoi;
    int fragmentCount = 10;            // 碎片数量
    float impactForce = 500.0f;        // 冲击力
    float fragmentMass = 1.0f;         // 碎片质量
    float fragmentLifetime = 5.0f;     // 碎片生命周期（秒）
    bool usePhysics = true;            // 碎片是否使用物理
    float debrisThreshold = 0.1f;      // 碎片大小阈值
};

// 碎片
struct Fragment {
    PhysicsBodyHandle body;
    Vec3 position;
    Vec3 velocity;
    Vec3 angularVelocity;
    float lifetime;
    float size;
};

// 破碎系统
class DestructionSystem {
public:
    DestructionSystem();
    ~DestructionSystem();

    // 初始化
    bool initialize();
    void shutdown();

    // 破碎物体
    std::vector<Fragment> destroyObject(PhysicsBodyHandle body, const Vec3& impactPoint,
                                         const Vec3& impactDirection, const DestructionSettings& settings);

    // 更新碎片
    void update(float deltaTime);

    // 获取活跃碎片
    const std::vector<Fragment>& getFragments() const { return m_fragments; }

    // 清除所有碎片
    void clearFragments();

    // 设置
    void setMaxFragments(int max) { m_maxFragments = max; }
    int getMaxFragments() const { return m_maxFragments; }

private:
    // Voronoi 破碎
    std::vector<Fragment> voronoiFracture(PhysicsBodyHandle body, const Vec3& impactPoint,
                                           const Vec3& impactDirection, int fragmentCount);

    // 径向破碎
    std::vector<Fragment> radialFracture(PhysicsBodyHandle body, const Vec3& impactPoint,
                                          const Vec3& impactDirection, int fragmentCount);

    // 平面破碎
    std::vector<Fragment> planarFracture(PhysicsBodyHandle body, const Vec3& impactPoint,
                                          const Vec3& impactDirection, int fragmentCount);

    std::vector<Fragment> m_fragments;
    int m_maxFragments = 1000;
};

} // namespace spark
