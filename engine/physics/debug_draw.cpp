#include "debug_draw.h"
#include "core/log.h"
#include <cmath>

namespace spark {

DebugDraw& DebugDraw::get() {
    static DebugDraw instance;
    return instance;
}

bool DebugDraw::initialize() {
    SPARK_CORE_INFO("Debug draw system initialized.");
    return true;
}

void DebugDraw::shutdown() {
    clear();
    SPARK_CORE_INFO("Debug draw system shutdown.");
}

void DebugDraw::drawLine(const Vec3& start, const Vec3& end, const DebugColor& color, float thickness) {
    if (!m_enabled) return;

    DebugLine line;
    line.start = start;
    line.end = end;
    line.color = color;
    line.thickness = thickness;
    m_lines.push_back(line);
}

void DebugDraw::drawPoint(const Vec3& position, const DebugColor& color, float size) {
    if (!m_enabled) return;

    DebugPoint point;
    point.position = position;
    point.color = color;
    point.size = size;
    m_points.push_back(point);
}

void DebugDraw::drawBox(const Vec3& center, const Vec3& halfExtents, const Quat& rotation, const DebugColor& color, bool wireframe) {
    if (!m_enabled) return;

    DebugBox box;
    box.center = center;
    box.halfExtents = halfExtents;
    box.rotation = rotation;
    box.color = color;
    box.wireframe = wireframe;
    m_boxes.push_back(box);

    // 绘制线框
    if (wireframe) {
        Vec3 corners[8] = {
            center + Vec3(-halfExtents.x, -halfExtents.y, -halfExtents.z),
            center + Vec3( halfExtents.x, -halfExtents.y, -halfExtents.z),
            center + Vec3( halfExtents.x,  halfExtents.y, -halfExtents.z),
            center + Vec3(-halfExtents.x,  halfExtents.y, -halfExtents.z),
            center + Vec3(-halfExtents.x, -halfExtents.y,  halfExtents.z),
            center + Vec3( halfExtents.x, -halfExtents.y,  halfExtents.z),
            center + Vec3( halfExtents.x,  halfExtents.y,  halfExtents.z),
            center + Vec3(-halfExtents.x,  halfExtents.y,  halfExtents.z)
        };

        // 底面
        drawLine(corners[0], corners[1], color);
        drawLine(corners[1], corners[2], color);
        drawLine(corners[2], corners[3], color);
        drawLine(corners[3], corners[0], color);

        // 顶面
        drawLine(corners[4], corners[5], color);
        drawLine(corners[5], corners[6], color);
        drawLine(corners[6], corners[7], color);
        drawLine(corners[7], corners[4], color);

        // 竖线
        drawLine(corners[0], corners[4], color);
        drawLine(corners[1], corners[5], color);
        drawLine(corners[2], corners[6], color);
        drawLine(corners[3], corners[7], color);
    }
}

void DebugDraw::drawSphere(const Vec3& center, float radius, const DebugColor& color, bool wireframe, int segments) {
    if (!m_enabled) return;

    DebugSphere sphere;
    sphere.center = center;
    sphere.radius = radius;
    sphere.color = color;
    sphere.wireframe = wireframe;
    sphere.segments = segments;
    m_spheres.push_back(sphere);

    // 绘制线框
    if (wireframe) {
        float step = 2.0f * 3.14159f / segments;

        // XY 平面圆
        for (int i = 0; i < segments; i++) {
            float angle1 = i * step;
            float angle2 = (i + 1) * step;
            Vec3 p1 = center + Vec3(cos(angle1) * radius, sin(angle1) * radius, 0.0f);
            Vec3 p2 = center + Vec3(cos(angle2) * radius, sin(angle2) * radius, 0.0f);
            drawLine(p1, p2, color);
        }

        // XZ 平面圆
        for (int i = 0; i < segments; i++) {
            float angle1 = i * step;
            float angle2 = (i + 1) * step;
            Vec3 p1 = center + Vec3(cos(angle1) * radius, 0.0f, sin(angle1) * radius);
            Vec3 p2 = center + Vec3(cos(angle2) * radius, 0.0f, sin(angle2) * radius);
            drawLine(p1, p2, color);
        }

        // YZ 平面圆
        for (int i = 0; i < segments; i++) {
            float angle1 = i * step;
            float angle2 = (i + 1) * step;
            Vec3 p1 = center + Vec3(0.0f, cos(angle1) * radius, sin(angle1) * radius);
            Vec3 p2 = center + Vec3(0.0f, cos(angle2) * radius, sin(angle2) * radius);
            drawLine(p1, p2, color);
        }
    }
}

void DebugDraw::drawAABB(const Vec3& min, const Vec3& max, const DebugColor& color) {
    Vec3 center = (min + max) * 0.5f;
    Vec3 halfExtents = (max - min) * 0.5f;
    drawBox(center, halfExtents, Quat(1, 0, 0, 0), color, true);
}

void DebugDraw::drawOBB(const Vec3& center, const Vec3& halfExtents, const Quat& rotation, const DebugColor& color) {
    drawBox(center, halfExtents, rotation, color, true);
}

void DebugDraw::drawRay(const Vec3& origin, const Vec3& direction, float length, const DebugColor& color) {
    Vec3 end = origin + direction * length;
    drawLine(origin, end, color);

    // 绘制箭头
    Vec3 right = glm::cross(direction, Vec3(0, 1, 0));
    if (glm::length(right) < 0.001f) {
        right = glm::cross(direction, Vec3(1, 0, 0));
    }
    right = glm::normalize(right);

    Vec3 up = glm::cross(right, direction);
    float arrowSize = length * 0.1f;

    drawLine(end, end - direction * arrowSize + right * arrowSize * 0.5f, color);
    drawLine(end, end - direction * arrowSize - right * arrowSize * 0.5f, color);
    drawLine(end, end - direction * arrowSize + up * arrowSize * 0.5f, color);
    drawLine(end, end - direction * arrowSize - up * arrowSize * 0.5f, color);
}

void DebugDraw::drawContactPoint(const Vec3& point, const Vec3& normal, float size, const DebugColor& color) {
    drawPoint(point, color, size);
    drawLine(point, point + normal * size * 2.0f, color);
}

void DebugDraw::render(VkCommandBuffer commandBuffer) {
    // 渲染所有调试图元
    // 简化实现：实际需要创建渲染管线和顶点缓冲
}

void DebugDraw::clear() {
    m_lines.clear();
    m_points.clear();
    m_boxes.clear();
    m_spheres.clear();
}

} // namespace spark
