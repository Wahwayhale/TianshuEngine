#include "gizmo.h"
#include "renderer/camera.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include "core/log.h"
#include <cmath>

namespace spark {

GizmoSystem::GizmoSystem() = default;
GizmoSystem::~GizmoSystem() = default;

bool GizmoSystem::initialize() {
    SPARK_CORE_INFO("Gizmo system initialized.");
    return true;
}

void GizmoSystem::shutdown() {
    SPARK_CORE_INFO("Gizmo system shutdown.");
}

void GizmoSystem::render(VkCommandBuffer commandBuffer, Entity& entity, Camera& camera,
                          float viewportWidth, float viewportHeight) {
    if (!entity.hasComponent<TransformComponent>()) return;

    auto& transform = entity.getComponent<TransformComponent>();

    // 计算矩阵
    m_modelMatrix = transform.getTransformMatrix();
    m_viewMatrix = camera.getViewMatrix();
    m_projMatrix = camera.getProjectionMatrix(viewportWidth / viewportHeight);

    // 根据模式绘制 Gizmo
    switch (m_mode) {
        case GizmoMode::Translate:
            drawTranslateGizmo(commandBuffer, m_modelMatrix, m_viewMatrix, m_projMatrix);
            break;
        case GizmoMode::Rotate:
            drawRotateGizmo(commandBuffer, m_modelMatrix, m_viewMatrix, m_projMatrix);
            break;
        case GizmoMode::Scale:
            drawScaleGizmo(commandBuffer, m_modelMatrix, m_viewMatrix, m_projMatrix);
            break;
    }
}

void GizmoSystem::cycleMode() {
    switch (m_mode) {
        case GizmoMode::Translate:
            m_mode = GizmoMode::Rotate;
            break;
        case GizmoMode::Rotate:
            m_mode = GizmoMode::Scale;
            break;
        case GizmoMode::Scale:
            m_mode = GizmoMode::Translate;
            break;
    }
}

void GizmoSystem::toggleSpace() {
    m_space = (m_space == GizmoSpace::World) ? GizmoSpace::Local : GizmoSpace::World;
}

void GizmoSystem::onMouseDown(float x, float y) {
    // 检测是否点击在 Gizmo 轴上
    // 简化实现：实际需要射线-Gizmo 相交检测
    m_active = true;
    m_startMousePos = Vec2(x, y);
}

void GizmoSystem::onMouseMove(float x, float y) {
    if (!m_active) return;

    // 计算鼠标移动距离
    Vec2 delta = Vec2(x, y) - m_startMousePos;

    // 根据模式和轴应用变换
    // 简化实现
}

void GizmoSystem::onMouseUp() {
    m_active = false;
    m_activeAxis = -1;
}

void GizmoSystem::drawTranslateGizmo(VkCommandBuffer commandBuffer,
                                       const Mat4& modelMatrix, const Mat4& viewMatrix, const Mat4& projMatrix) {
    // 绘制三个轴（X=红, Y=绿, Z=蓝）
    // 简化实现：实际需要绘制 3D 线段

    Vec3 position = Vec3(modelMatrix[3]);

    // X 轴（红色）
    // drawLine(position, position + Vec3(1, 0, 0), Vec3(1, 0, 0));

    // Y 轴（绿色）
    // drawLine(position, position + Vec3(0, 1, 0), Vec3(0, 1, 0));

    // Z 轴（蓝色）
    // drawLine(position, position + Vec3(0, 0, 1), Vec3(0, 0, 1));
}

void GizmoSystem::drawRotateGizmo(VkCommandBuffer commandBuffer,
                                    const Mat4& modelMatrix, const Mat4& viewMatrix, const Mat4& projMatrix) {
    // 绘制三个旋转圆环
    // 简化实现
}

void GizmoSystem::drawScaleGizmo(VkCommandBuffer commandBuffer,
                                   const Mat4& modelMatrix, const Mat4& viewMatrix, const Mat4& projMatrix) {
    // 绘制三个缩放方块
    // 简化实现
}

} // namespace spark
