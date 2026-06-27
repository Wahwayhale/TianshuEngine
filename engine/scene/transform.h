#pragma once

#include "math/math_types.h"

namespace spark {

struct Transform {
    Vec3 position = Vec3(0.0f);
    Quat rotation = Quat(1.0f, 0.0f, 0.0f, 0.0f);
    Vec3 scale = Vec3(1.0f);

    Mat4 getLocalMatrix() const {
        Mat4 mat = Mat4(1.0f);
        mat = glm::translate(mat, position);
        mat *= glm::mat4_cast(rotation);
        mat = glm::scale(mat, scale);
        return mat;
    }
};

} // namespace spark
