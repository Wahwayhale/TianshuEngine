#pragma once

#include "math/math_types.h"
#include <cstdint>

namespace spark {

using PhysicsBodyHandle = uint32_t;

// 关节类型
enum class JointType {
    Fixed,          // 固定关节
    Hinge,          // 铰链关节
    Slider,         // 滑块关节
    Spherical,      // 球窝关节
    Spring,         // 弹簧关节
    Distance        // 距离约束
};

// 关节描述
struct JointDesc {
    JointType type = JointType::Fixed;
    PhysicsBodyHandle bodyA = 0;
    PhysicsBodyHandle bodyB = 0;

    // 关节锚点（相对于各自刚体）
    Vec3 anchorA = Vec3(0.0f);
    Vec3 anchorB = Vec3(0.0f);

    // 关节轴（用于铰链和滑块）
    Vec3 axis = Vec3(0.0f, 1.0f, 0.0f);

    // 限制参数
    float lowerLimit = 0.0f;
    float upperLimit = 0.0f;

    // 弹簧参数
    float springStiffness = 100.0f;
    float springDamping = 10.0f;
    float springRestLength = 1.0f;

    // 距离约束参数
    float minDistance = 0.0f;
    float maxDistance = 1.0f;

    // 是否启用碰撞
    bool enableCollision = false;
};

// 关节基类
class Joint {
public:
    Joint(const JointDesc& desc);
    virtual ~Joint();

    JointType getType() const { return m_desc.type; }
    PhysicsBodyHandle getBodyA() const { return m_desc.bodyA; }
    PhysicsBodyHandle getBodyB() const { return m_desc.bodyB; }

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }

    // 更新约束
    virtual void solve(float deltaTime) = 0;

protected:
    JointDesc m_desc;
    bool m_enabled = true;
};

// 固定关节
class FixedJoint : public Joint {
public:
    FixedJoint(const JointDesc& desc);
    void solve(float deltaTime) override;
};

// 铰链关节
class HingeJoint : public Joint {
public:
    HingeJoint(const JointDesc& desc);
    void solve(float deltaTime) override;

    void setLimits(float lower, float upper);

private:
    float m_lowerLimit;
    float m_upperLimit;
};

// 滑块关节
class SliderJoint : public Joint {
public:
    SliderJoint(const JointDesc& desc);
    void solve(float deltaTime) override;

    void setLimits(float lower, float upper);

private:
    float m_lowerLimit;
    float m_upperLimit;
};

// 球窝关节
class SphericalJoint : public Joint {
public:
    SphericalJoint(const JointDesc& desc);
    void solve(float deltaTime) override;

    void setSwingLimit(float limit);
    void setTwistLimit(float limit);

private:
    float m_swingLimit;
    float m_twistLimit;
};

// 弹簧关节
class SpringJoint : public Joint {
public:
    SpringJoint(const JointDesc& desc);
    void solve(float deltaTime) override;

    void setStiffness(float stiffness);
    void setDamping(float damping);
    void setRestLength(float length);

private:
    float m_stiffness;
    float m_damping;
    float m_restLength;
};

// 距离约束
class DistanceJoint : public Joint {
public:
    DistanceJoint(const JointDesc& desc);
    void solve(float deltaTime) override;

    void setMinDistance(float distance);
    void setMaxDistance(float distance);

private:
    float m_minDistance;
    float m_maxDistance;
};

// 关节管理器
class JointManager {
public:
    static JointManager& get();

    // 创建关节
    std::shared_ptr<Joint> createJoint(const JointDesc& desc);

    // 删除关节
    void destroyJoint(std::shared_ptr<Joint> joint);

    // 更新所有关节
    void solveAll(float deltaTime);

    // 获取关节数量
    int getJointCount() const { return static_cast<int>(m_joints.size()); }

private:
    JointManager() = default;

    std::vector<std::shared_ptr<Joint>> m_joints;
};

} // namespace spark
