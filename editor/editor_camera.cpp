#include "editor_camera.h"
#include "core/input.h"
#include <cmath>
#include <algorithm>

namespace spark {

EditorCamera::EditorCamera() = default;
EditorCamera::~EditorCamera() = default;

void EditorCamera::update(float deltaTime) {
    switch (m_mode) {
        case EditorCameraMode::Orbit:
            updateOrbitCamera();
            break;
        case EditorCameraMode::Fly:
            updateFlyCamera();
            break;
    }
}

void EditorCamera::onMouseMove(float x, float y) {
    float xOffset = x - m_lastMouseX;
    float yOffset = m_lastMouseY - y;
    m_lastMouseX = x;
    m_lastMouseY = y;

    if (m_rightMousePressed) {
        m_yaw += xOffset * m_rotateSpeed;
        m_pitch += yOffset * m_rotateSpeed;
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    }

    if (m_middleMousePressed) {
        // 平移
        Vec3 right = glm::normalize(glm::cross(m_target - m_position, m_up));
        Vec3 up = glm::normalize(glm::cross(right, m_target - m_position));
        m_target += right * (-xOffset * 0.01f * m_distance);
        m_target += up * (yOffset * 0.01f * m_distance);
    }
}

void EditorCamera::onMouseScroll(float y) {
    m_distance -= y * m_zoomSpeed;
    m_distance = std::max(m_distance, 1.0f);
}

void EditorCamera::onMouseButton(int button, bool pressed) {
    if (button == 0) m_leftMousePressed = pressed;
    if (button == 1) m_rightMousePressed = pressed;
    if (button == 2) m_middleMousePressed = pressed;
}

void EditorCamera::onKeyDown(int key) {
    // WASD 移动（飞行模式）
    if (m_mode == EditorCameraMode::Fly) {
        Vec3 forward = glm::normalize(m_target - m_position);
        Vec3 right = glm::normalize(glm::cross(forward, m_up));

        if (key == 87) m_position += forward * m_moveSpeed;  // W
        if (key == 83) m_position -= forward * m_moveSpeed;  // S
        if (key == 65) m_position -= right * m_moveSpeed;    // A
        if (key == 68) m_position += right * m_moveSpeed;    // D
    }
}

Mat4 EditorCamera::getViewMatrix() const {
    return glm::lookAt(m_position, m_target, m_up);
}

Mat4 EditorCamera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 1000.0f);
}

void EditorCamera::updateOrbitCamera() {
    // 轨道相机：围绕目标旋转
    float radYaw = glm::radians(m_yaw);
    float radPitch = glm::radians(m_pitch);

    m_position.x = m_target.x + m_distance * cos(radPitch) * cos(radYaw);
    m_position.y = m_target.y + m_distance * sin(radPitch);
    m_position.z = m_target.z + m_distance * cos(radPitch) * sin(radYaw);
}

void EditorCamera::updateFlyCamera() {
    // 飞行相机：自由移动
    // 位置已在 onKeyDown 中更新
}

} // namespace spark
