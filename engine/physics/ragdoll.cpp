#include "ragdoll.h"
#include "jolt_physics.h"
#include "core/log.h"

namespace spark {

Ragdoll::Ragdoll(PhysicsWorld& world, const RagdollSettings& settings)
    : m_world(world), m_settings(settings) {}

Ragdoll::~Ragdoll() {
    // 清理刚体和关节
    for (auto& body : m_bodies) {
        m_world.destroyBody(body.handle);
    }
}

void Ragdoll::create(const std::vector<RagdollBone>& bones, const std::vector<RagdollJoint>& joints) {
    // 为每个骨骼创建刚体
    for (const auto& bone : bones) {
        PhysicsBodyDesc desc;
        desc.type = PhysicsBodyType::Dynamic;
        desc.shapeType = PhysicsShapeType::Capsule;
        desc.position = bone.position;
        desc.rotation = bone.rotation;
        desc.capsuleRadius = m_settings.boneRadius;
        desc.capsuleHeight = 0.3f;
        desc.mass = m_settings.totalMass / bones.size();

        auto handle = m_world.createBody(desc);
        m_bodies.push_back({handle, bone});
    }

    // 创建关节约束
    for (const auto& joint : joints) {
        if (joint.boneA < m_bodies.size() && joint.boneB < m_bodies.size()) {
            JointDesc jointDesc;
            jointDesc.type = JointType::Spherical;
            jointDesc.bodyA = m_bodies[joint.boneA].handle;
            jointDesc.bodyB = m_bodies[joint.boneB].handle;
            jointDesc.anchorA = joint.anchorA;
            jointDesc.anchorB = joint.anchorB;

            // TODO: 创建关节
        }
    }

    SPARK_CORE_INFO("Ragdoll created with {0} bones", bones.size());
}

void Ragdoll::update(float deltaTime) {
    if (!m_active) return;

    // 布娃娃物理由 PhysicsWorld 自动更新
}

void Ragdoll::applyForce(const Vec3& force) {
    for (auto& body : m_bodies) {
        m_world.applyForce(body.handle, force / m_bodies.size());
    }
}

void Ragdoll::applyImpulse(const Vec3& impulse, const Vec3& position) {
    // 应用冲量到最近的骨骼
    float closestDist = 1e10f;
    int closestIndex = 0;

    for (size_t i = 0; i < m_bodies.size(); i++) {
        Vec3 bodyPos = m_world.getPosition(m_bodies[i].handle);
        float dist = glm::length(bodyPos - position);
        if (dist < closestDist) {
            closestDist = dist;
            closestIndex = static_cast<int>(i);
        }
    }

    m_world.applyImpulse(m_bodies[closestIndex].handle, impulse);
}

std::vector<Mat4> Ragdoll::getBoneTransforms() const {
    std::vector<Mat4> transforms;

    for (const auto& body : m_bodies) {
        Vec3 pos = m_world.getPosition(body.handle);
        Vec3 rot = m_world.getRotation(body.handle);

        Mat4 transform = glm::translate(Mat4(1.0f), pos);
        transform = glm::rotate(transform, glm::radians(rot.x), Vec3(1, 0, 0));
        transform = glm::rotate(transform, glm::radians(rot.y), Vec3(0, 1, 0));
        transform = glm::rotate(transform, glm::radians(rot.z), Vec3(0, 0, 1));

        transforms.push_back(transform);
    }

    return transforms;
}

} // namespace spark
