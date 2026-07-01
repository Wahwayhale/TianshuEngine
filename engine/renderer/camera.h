#pragma once

#include "math/math_types.h"
#include "core/input.h"

namespace spark {

class Camera {
public:
    Camera(Vec3 position = Vec3(0.0f, 0.0f, 3.0f), Vec3 up = Vec3(0.0f, 1.0f, 0.0f));

    Mat4 getViewMatrix() const;
    Mat4 getProjectionMatrix(float aspectRatio) const;

    void update(float deltaTime);

    // Setters
    void setPosition(Vec3 pos) { m_position = pos; }
    void setTarget(Vec3 target) { m_front = glm::normalize(target - m_position); updateVectors(); }
    void setFov(float fov) { m_fov = fov; }
    void setNearFar(float nearPlane, float farPlane) { m_nearPlane = nearPlane; m_farPlane = farPlane; }

    // Getters
    Vec3 getPosition() const { return m_position; }
    Vec3 getFront() const { return m_front; }
    Vec3 getUp() const { return m_up; }
    float getFov() const { return m_fov; }

    // FPS Camera controls
    void processKeyboard(float deltaTime);
    void processMouse(float xOffset, float yOffset);
    void processScroll(float yOffset);

private:
    void updateVectors();

    Vec3 m_position;
    Vec3 m_front;
    Vec3 m_up;
    Vec3 m_right;
    Vec3 m_worldUp;

    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    float m_fov = 45.0f;
    float m_speed = 5.0f;
    float m_sensitivity = 0.1f;
    float m_nearPlane = 0.1f;
    float m_farPlane = 1000.0f;
};

} // namespace spark
