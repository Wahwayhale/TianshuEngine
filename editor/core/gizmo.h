#pragma once

#include "math/math_types.h"
#include <cstdint>

namespace spark {

class Camera;
class Entity;

// Gizmo 模式
enum class GizmoMode {
    Translate,
    Rotate,
    Scale
};

// Gizmo 空间
enum class GizmoSpace {
    World,
    Local
};

// Gizmo 系统
class GizmoSystem {
public:
    GizmoSystem();
    ~GizmoSystem();

    // 初始化
    bool initialize();
    void shutdown();

    // 渲染 Gizmo
    void render(VkCommandBuffer commandBuffer, Entity& entity, Camera& camera,
                float viewportWidth, float viewportHeight);

    // 模式切换
    void setMode(GizmoMode mode) { m_mode = mode; }
    GizmoMode getMode() const { return m_mode; }
    void cycleMode();

    // 空间切换
    void setSpace(GizmoSpace space) { m_space = space; }
    GizmoSpace getSpace() const { return m_space; }
    void toggleSpace();

    // 交互
    bool isHovered() const { return m_hovered; }
    bool isActive() const { return m_active; }

    // 鼠标交互
    void onMouseDown(float x, float y);
    void onMouseMove(float x, float y);
    void onMouseUp();

private:
    // 绘制 Gizmo 轴
    void drawTranslateGizmo(VkCommandBuffer commandBuffer, const Mat4& modelMatrix, const Mat4& viewMatrix, const Mat4& projMatrix);
    void drawRotateGizmo(VkCommandBuffer commandBuffer, const Mat4& modelMatrix, const Mat4& viewMatrix, const Mat4& projMatrix);
    void drawScaleGizmo(VkCommandBuffer commandBuffer, const Mat4& modelMatrix, const Mat4& viewMatrix, const Mat4& projMatrix);

    GizmoMode m_mode = GizmoMode::Translate;
    GizmoSpace m_space = GizmoSpace::World;

    bool m_hovered = false;
    bool m_active = false;
    int m_hoveredAxis = -1;  // 0=X, 1=Y, 2=Z, -1=none
    int m_activeAxis = -1;

    Vec3 m_startPosition;
    Vec3 m_startRotation;
    Vec3 m_startScale;
    Vec2 m_startMousePos;

    // 缓存
    Mat4 m_modelMatrix;
    Mat4 m_viewMatrix;
    Mat4 m_projMatrix;
};

} // namespace spark
