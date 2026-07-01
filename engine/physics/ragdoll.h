#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace spark {

class PhysicsWorld;

// 骨骼节点
struct RagdollBone {
    std::string name;
    Vec3 position;
    Vec3 rotation;
    Vec3 scale;
    int parentIndex = -1;
    std::vector<int> children;
};

// 关节约点
struct RagdollJoint {
    int boneA;
    int boneB;
    Vec3 anchorA;
    Vec3 anchorB;
    Vec3 axis;
    float lowerLimit;
    float upperLimit;
};

// 布娃娃设置
struct RagdollSettings {
    float totalMass = 70.0f;
    float boneRadius = 0.05f;
    float jointStiffness = 100.0f;
    float jointDamping = 10.0f;
    Vec3 gravity = Vec3(0.0f, -9.81f, 0.0f);
};

// 布娃娃
class Ragdoll {
public:
    Ragdoll(PhysicsWorld& world, const RagdollSettings& settings = {});
    ~Ragdoll();

    // 创建布娃娃
    void create(const std::vector<RagdollBone>& bones, const std::vector<RagdollJoint>& joints);

    // 更新
    void update(float deltaTime);

    // 应用力
    void applyForce(const Vec3& force);
    void applyImpulse(const Vec3& impulse, const Vec3& position);

    // 设置
    void setSettings(const RagdollSettings& settings) { m_settings = settings; }
    const RagdollSettings& getSettings() const { return m_settings; }

    // 获取骨骼变换
    std::vector<Mat4> getBoneTransforms() const;

    // 状态
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

private:
    PhysicsWorld& m_world;
    RagdollSettings m_settings;

    struct RagdollBody {
        PhysicsBodyHandle handle;
        RagdollBone bone;
    };

    std::vector<RagdollBody> m_bodies;
    std::vector<PhysicsBodyHandle> m_joints;
    bool m_active = true;
};

} // namespace spark
