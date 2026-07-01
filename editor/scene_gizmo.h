#pragma once

#include "math/math_types.h"
#include <vulkan/vulkan.h>

namespace spark {

class Entity;
class Camera;

// Gizmo 模式
enum class SceneGizmoMode {
    Translate,
    Rotate,
    Scale
};

// 场景 Gizmo
class SceneGizmo {
public:
    SceneGizmo();
    ~SceneGizmo();

    // 渲染 Gizmo
    void render(VkCommandBuffer commandBuffer, Entity& entity, Camera& camera,
                float viewportWidth, float viewportHeight);

    // 模式
    void setMode(SceneGizmoMode mode) { m_mode = mode; }
    SceneGizmoMode getMode() const { return m_mode; }
    void cycleMode();

    // 交互
    bool isHovered() const { return m_hovered; }
    bool isActive() const { return m_active; }

    // 鼠标交互
    void onMouseDown(float x, float y);
    void onMouseMove(float x, float y);
    void onMouseUp();

private:
    SceneGizmoMode m_mode = SceneGizmoMode::Translate;
    bool m_hovered = false;
    bool m_active = false;
    int m_hoveredAxis = -1;
};

} // namespace spark
