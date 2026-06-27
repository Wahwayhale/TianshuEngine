#include "camera.h"
#include <algorithm>

namespace spark {

Camera::Camera(Vec3 position, Vec3 up)
    : m_position(position), m_worldUp(up) {
    updateVectors();
}

Mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

Mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::update(float deltaTime) {
    processKeyboard(deltaTime);
}

void Camera::processKeyboard(float deltaTime) {
    float velocity = m_speed * deltaTime;

    if (Input::isKeyPressed(GLFW_KEY_W)) {
        m_position += m_front * velocity;
    }
    if (Input::isKeyPressed(GLFW_KEY_S)) {
        m_position -= m_front * velocity;
    }
    if (Input::isKeyPressed(GLFW_KEY_A)) {
        m_position -= m_right * velocity;
    }
    if (Input::isKeyPressed(GLFW_KEY_D)) {
        m_position += m_right * velocity;
    }
    if (Input::isKeyPressed(GLFW_KEY_SPACE)) {
        m_position += m_worldUp * velocity;
    }
    if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        m_position -= m_worldUp * velocity;
    }
}

void Camera::processMouse(float xOffset, float yOffset) {
    xOffset *= m_sensitivity;
    yOffset *= m_sensitivity;

    m_yaw += xOffset;
    m_pitch += yOffset;

    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    updateVectors();
}

void Camera::processScroll(float yOffset) {
    m_fov -= yOffset;
    m_fov = std::clamp(m_fov, 1.0f, 120.0f);
}

void Camera::updateVectors() {
    Vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

} // namespace spark
