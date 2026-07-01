#pragma once

#include "math/math_types.h"

namespace spark {

// 编辑器相机模式
enum class EditorCameraMode {
    Orbit,      // 轨道模式
    Fly         // 飞行模式
};

// 编辑器相机
class EditorCamera {
public:
    EditorCamera();
    ~EditorCamera();

    // 更新
    void update(float deltaTime);

    // 鼠标输入
    void onMouseMove(float x, float y);
    void onMouseScroll(float y);
    void onMouseButton(int button, bool pressed);

    // 键盘输入
    void onKeyDown(int key);

    // 属性
    const Vec3& getPosition() const { return m_position; }
    void setPosition(const Vec3& position) { m_position = position; }

    const Vec3& getTarget() const { return m_target; }
    void setTarget(const Vec3& target) { m_target = target; }

    float getDistance() const { return m_distance; }
    void setDistance(float distance) { m_distance = distance; }

    float getFov() const { return m_fov; }
    void setFov(float fov) { m_fov = fov; }

    // 矩阵
    Mat4 getViewMatrix() const;
    Mat4 getProjectionMatrix(float aspectRatio) const;

    // 模式
    EditorCameraMode getMode() const { return m_mode; }
    void setMode(EditorCameraMode mode) { m_mode = mode; }

private:
    void updateOrbitCamera();
    void updateFlyCamera();

    EditorCameraMode m_mode = EditorCameraMode::Orbit;

    Vec3 m_position = Vec3(0.0f, 5.0f, 10.0f);
    Vec3 m_target = Vec3(0.0f);
    Vec3 m_up = Vec3(0.0f, 1.0f, 0.0f);

    float m_distance = 10.0f;
    float m_yaw = -90.0f;
    float m_pitch = -30.0f;
    float m_fov = 45.0f;

    float m_moveSpeed = 5.0f;
    float m_rotateSpeed = 0.1f;
    float m_zoomSpeed = 1.0f;

    bool m_leftMousePressed = false;
    bool m_rightMousePressed = false;
    bool m_middleMousePressed = false;

    float m_lastMouseX = 0.0f;
    float m_lastMouseY = 0.0f;
};

} // namespace spark
