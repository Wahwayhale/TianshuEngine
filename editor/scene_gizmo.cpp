#include "scene_gizmo.h"
#include "renderer/camera.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include "core/log.h"
#include <cmath>

namespace spark {

SceneGizmo::SceneGizmo() = default;
SceneGizmo::~SceneGizmo() = default;

void SceneGizmo::render(VkCommandBuffer commandBuffer, Entity& entity, Camera& camera,
                         float viewportWidth, float viewportHeight) {
    // TODO: 渲染 Gizmo
    // 需要绘制 3D 箭头/圆环/方块
}

void SceneGizmo::cycleMode() {
    switch (m_mode) {
        case SceneGizmoMode::Translate:
            m_mode = SceneGizmoMode::Rotate;
            break;
        case SceneGizmoMode::Rotate:
            m_mode = SceneGizmoMode::Scale;
            break;
        case SceneGizmoMode::Scale:
            m_mode = SceneGizmoMode::Translate;
            break;
    }
}

void SceneGizmo::onMouseDown(float x, float y) {
    // TODO: 检测点击 Gizmo 轴
    m_active = true;
}

void SceneGizmo::onMouseMove(float x, float y) {
    if (!m_active) return;

    // TODO: 拖拽 Gizmo
}

void SceneGizmo::onMouseUp() {
    m_active = false;
    m_hoveredAxis = -1;
}

} // namespace spark
