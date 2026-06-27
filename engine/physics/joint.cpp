#include "joint.h"
#include "jolt_physics.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

// =============================================
// Joint 基类
// =============================================

Joint::Joint(const JointDesc& desc) : m_desc(desc) {}
Joint::~Joint() = default;

// =============================================
// FixedJoint
// =============================================

FixedJoint::FixedJoint(const JointDesc& desc) : Joint(desc) {}

void FixedJoint::solve(float deltaTime) {
    // 固定关节：保持两个刚体的相对位置和旋转不变
    // 实际实现需要从 PhysicsWorld 获取刚体并约束
}

// =============================================
// HingeJoint
// =============================================

HingeJoint::HingeJoint(const JointDesc& desc)
    : Joint(desc), m_lowerLimit(desc.lowerLimit), m_upperLimit(desc.upperLimit) {}

void HingeJoint::solve(float deltaTime) {
    // 铰链关节：允许绕一个轴旋转，可设置角度限制
    // 实际实现需要约束旋转角度在 [lowerLimit, upperLimit] 范围内
}

void HingeJoint::setLimits(float lower, float upper) {
    m_lowerLimit = lower;
    m_upperLimit = upper;
}

// =============================================
// SliderJoint
// =============================================

SliderJoint::SliderJoint(const JointDesc& desc)
    : Joint(desc), m_lowerLimit(desc.lowerLimit), m_upperLimit(desc.upperLimit) {}

void SliderJoint::solve(float deltaTime) {
    // 滑块关节：允许沿一个轴平移，可设置距离限制
    // 实际实现需要约束位置在 [lowerLimit, upperLimit] 范围内
}

void SliderJoint::setLimits(float lower, float upper) {
    m_lowerLimit = lower;
    m_upperLimit = upper;
}

// =============================================
// SphericalJoint
// =============================================

SphericalJoint::SphericalJoint(const JointDesc& desc)
    : Joint(desc), m_swingLimit(45.0f), m_twistLimit(45.0f) {}

void SphericalJoint::solve(float deltaTime) {
    // 球窝关节：允许三个轴的旋转，可设置摆动和扭转限制
    // 实际实现需要约束旋转在锥形范围内
}

void SphericalJoint::setSwingLimit(float limit) {
    m_swingLimit = limit;
}

void SphericalJoint::setTwistLimit(float limit) {
    m_twistLimit = limit;
}

// =============================================
// SpringJoint
// =============================================

SpringJoint::SpringJoint(const JointDesc& desc)
    : Joint(desc),
      m_stiffness(desc.springStiffness),
      m_damping(desc.springDamping),
      m_restLength(desc.springRestLength) {}

void SpringJoint::solve(float deltaTime) {
    // 弹簧关节：在两个刚体之间施加弹簧力
    // F = -k * (distance - restLength) - damping * relativeVelocity
    // 实际实现需要计算距离和相对速度
}

void SpringJoint::setStiffness(float stiffness) {
    m_stiffness = stiffness;
}

void SpringJoint::setDamping(float damping) {
    m_damping = damping;
}

void SpringJoint::setRestLength(float length) {
    m_restLength = length;
}

// =============================================
// DistanceJoint
// =============================================

DistanceJoint::DistanceJoint(const JointDesc& desc)
    : Joint(desc), m_minDistance(desc.minDistance), m_maxDistance(desc.maxDistance) {}

void DistanceJoint::solve(float deltaTime) {
    // 距离约束：保持两个刚体之间的距离在 [minDistance, maxDistance] 范围内
    // 实际实现需要计算距离并施加约束力
}

void DistanceJoint::setMinDistance(float distance) {
    m_minDistance = distance;
}

void DistanceJoint::setMaxDistance(float distance) {
    m_maxDistance = distance;
}

// =============================================
// JointManager
// =============================================

JointManager& JointManager::get() {
    static JointManager instance;
    return instance;
}

std::shared_ptr<Joint> JointManager::createJoint(const JointDesc& desc) {
    std::shared_ptr<Joint> joint;

    switch (desc.type) {
        case JointType::Fixed:
            joint = std::make_shared<FixedJoint>(desc);
            break;
        case JointType::Hinge:
            joint = std::make_shared<HingeJoint>(desc);
            break;
        case JointType::Slider:
            joint = std::make_shared<SliderJoint>(desc);
            break;
        case JointType::Spherical:
            joint = std::make_shared<SphericalJoint>(desc);
            break;
        case JointType::Spring:
            joint = std::make_shared<SpringJoint>(desc);
            break;
        case JointType::Distance:
            joint = std::make_shared<DistanceJoint>(desc);
            break;
    }

    if (joint) {
        m_joints.push_back(joint);
        SPARK_CORE_INFO("Created joint: {0}", static_cast<int>(desc.type));
    }

    return joint;
}

void JointManager::destroyJoint(std::shared_ptr<Joint> joint) {
    auto it = std::find(m_joints.begin(), m_joints.end(), joint);
    if (it != m_joints.end()) {
        m_joints.erase(it);
    }
}

void JointManager::solveAll(float deltaTime) {
    for (auto& joint : m_joints) {
        if (joint->isEnabled()) {
            joint->solve(deltaTime);
        }
    }
}

} // namespace spark
