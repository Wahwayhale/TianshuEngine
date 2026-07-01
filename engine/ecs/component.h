#pragma once

#include <cstdint>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <vector>
#include "math/math_types.h"

namespace spark {

using EntityID = uint64_t;
using ComponentTypeID = std::type_index;

// Component base - all components should inherit from this
struct Component {
    virtual ~Component() = default;
};

// Transform component
struct TransformComponent : public Component {
    Vec3 position = Vec3(0.0f);
    Vec3 rotation = Vec3(0.0f);  // Euler angles (degrees)
    Vec3 scale = Vec3(1.0f);

    TransformComponent() = default;
    TransformComponent(const Vec3& pos) : position(pos) {}
    TransformComponent(const Vec3& pos, const Vec3& rot, const Vec3& scl)
        : position(pos), rotation(rot), scale(scl) {}

    Mat4 getTransformMatrix() const {
        Mat4 transform = Mat4(1.0f);
        transform = glm::translate(transform, position);
        transform = glm::rotate(transform, glm::radians(rotation.x), Vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(rotation.y), Vec3(0.0f, 1.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(rotation.z), Vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, scale);
        return transform;
    }
};

} // namespace spark
