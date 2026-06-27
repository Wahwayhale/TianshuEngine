#pragma once

#include "component.h"
#include "math/math_types.h"
#include <string>
#include <memory>

namespace spark {

class Mesh;
class Material;

// Tag component for entity name
struct TagComponent : public Component {
    std::string name;

    TagComponent(const std::string& n = "Entity") : name(n) {}
};

// Camera component
struct CameraComponent : public Component {
    enum class ProjectionType { Perspective, Orthographic };

    ProjectionType projectionType = ProjectionType::Perspective;
    float fov = 45.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    float orthoSize = 10.0f;

    Mat4 getProjectionMatrix() const {
        if (projectionType == ProjectionType::Perspective) {
            return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
        } else {
            float halfWidth = orthoSize * aspectRatio;
            float halfHeight = orthoSize;
            return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
        }
    }
};

// Light component
struct LightComponent : public Component {
    enum class LightType { Directional, Point, Spot };

    LightType lightType = LightType::Directional;
    Vec3 color = Vec3(1.0f);
    float intensity = 1.0f;

    float range = 10.0f;
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    float innerCutoff = 12.5f;
    float outerCutoff = 17.5f;
};

// Mesh renderer component
struct MeshRendererComponent : public Component {
    std::shared_ptr<Mesh> mesh;
    Vec4 albedo = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;
    float emission = 0.0f;
    bool visible = true;
    bool castShadow = true;
};

// Rigid body component - 物理刚体
struct RigidBodyComponent : public Component {
    // 线性运动
    Vec3 velocity = Vec3(0.0f);
    Vec3 acceleration = Vec3(0.0f);
    Vec3 force = Vec3(0.0f);

    // 角运动
    Vec3 angularVelocity = Vec3(0.0f);
    Vec3 torque = Vec3(0.0f);

    // 物理属性
    float mass = 1.0f;
    float inverseMass = 1.0f;  // 1/mass，用于计算
    float restitution = 0.3f;  // 弹性系数 (0-1)
    float friction = 0.5f;     // 摩擦系数 (0-1)
    float linearDamping = 0.01f;   // 线性阻尼
    float angularDamping = 0.05f;  // 角阻尼

    // 状态
    bool useGravity = true;
    bool isStatic = false;
    bool isKinematic = false;  // 运动学物体（不受力影响但可碰撞）

    // 设置质量（自动计算逆质量）
    void setMass(float m) {
        mass = m;
        if (m > 0.0f) {
            inverseMass = 1.0f / m;
        } else {
            inverseMass = 0.0f;
            isStatic = true;
        }
    }

    // 施加力
    void applyForce(const Vec3& f) {
        if (!isStatic && !isKinematic) {
            force += f;
        }
    }

    // 施加冲量（瞬时力）
    void applyImpulse(const Vec3& impulse) {
        if (!isStatic && !isKinematic) {
            velocity += impulse * inverseMass;
        }
    }

    // 施加扭矩
    void applyTorque(const Vec3& t) {
        if (!isStatic && !isKinematic) {
            torque += t;
        }
    }
};

// Collider component - 碰撞体
struct ColliderComponent : public Component {
    enum class ColliderType { Box, Sphere, Capsule, Plane };

    ColliderType colliderType = ColliderType::Box;
    Vec3 size = Vec3(0.5f);        // Box: 半尺寸
    float radius = 0.5f;           // Sphere/Capsule: 半径
    float height = 1.0f;           // Capsule: 高度
    bool isTrigger = false;        // 触发器（不产生物理响应）
    Vec3 offset = Vec3(0.0f);      // 相对于实体的偏移
};

// Script component
struct ScriptComponent : public Component {
    std::string scriptPath;
};

} // namespace spark
