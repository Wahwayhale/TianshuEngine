#pragma once

#include "math/math_types.h"

namespace spark {

class AudioListener {
public:
    AudioListener();
    ~AudioListener();

    void setPosition(Vec3 position);
    void setVelocity(Vec3 velocity);
    void setOrientation(Vec3 forward, Vec3 up);

    Vec3 getPosition() const { return m_position; }
    Vec3 getVelocity() const { return m_velocity; }

private:
    Vec3 m_position = Vec3(0.0f);
    Vec3 m_velocity = Vec3(0.0f);
    Vec3 m_forward = Vec3(0.0f, 0.0f, -1.0f);
    Vec3 m_up = Vec3(0.0f, 1.0f, 0.0f);
};

} // namespace spark
