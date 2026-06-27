#include "debug_draw.h"

namespace spark {

DebugDraw::DebugDraw() = default;
DebugDraw::~DebugDraw() = default;

void DebugDraw::drawLine(Vec3 start, Vec3 end, Vec3 color) {
    m_lines.push_back(start);
    m_lines.push_back(end);
    m_colors.push_back(color);
    m_colors.push_back(color);
}

void DebugDraw::drawBox(Vec3 position, Vec3 size, Vec3 color) {
    Vec3 min = position - size;
    Vec3 max = position + size;

    // Bottom face
    drawLine(Vec3(min.x, min.y, min.z), Vec3(max.x, min.y, min.z), color);
    drawLine(Vec3(max.x, min.y, min.z), Vec3(max.x, min.y, max.z), color);
    drawLine(Vec3(max.x, min.y, max.z), Vec3(min.x, min.y, max.z), color);
    drawLine(Vec3(min.x, min.y, max.z), Vec3(min.x, min.y, min.z), color);

    // Top face
    drawLine(Vec3(min.x, max.y, min.z), Vec3(max.x, max.y, min.z), color);
    drawLine(Vec3(max.x, max.y, min.z), Vec3(max.x, max.y, max.z), color);
    drawLine(Vec3(max.x, max.y, max.z), Vec3(min.x, max.y, max.z), color);
    drawLine(Vec3(min.x, max.y, max.z), Vec3(min.x, max.y, min.z), color);

    // Vertical edges
    drawLine(Vec3(min.x, min.y, min.z), Vec3(min.x, max.y, min.z), color);
    drawLine(Vec3(max.x, min.y, min.z), Vec3(max.x, max.y, min.z), color);
    drawLine(Vec3(max.x, min.y, max.z), Vec3(max.x, max.y, max.z), color);
    drawLine(Vec3(min.x, min.y, max.z), Vec3(min.x, max.y, max.z), color);
}

void DebugDraw::drawSphere(Vec3 position, float radius, Vec3 color) {
    const int segments = 16;
    const float angleStep = 2.0f * 3.14159f / segments;

    // XY circle
    for (int i = 0; i < segments; i++) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;
        Vec3 p1 = position + Vec3(cos(angle1) * radius, sin(angle1) * radius, 0.0f);
        Vec3 p2 = position + Vec3(cos(angle2) * radius, sin(angle2) * radius, 0.0f);
        drawLine(p1, p2, color);
    }

    // XZ circle
    for (int i = 0; i < segments; i++) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;
        Vec3 p1 = position + Vec3(cos(angle1) * radius, 0.0f, sin(angle1) * radius);
        Vec3 p2 = position + Vec3(cos(angle2) * radius, 0.0f, sin(angle2) * radius);
        drawLine(p1, p2, color);
    }

    // YZ circle
    for (int i = 0; i < segments; i++) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;
        Vec3 p1 = position + Vec3(0.0f, cos(angle1) * radius, sin(angle1) * radius);
        Vec3 p2 = position + Vec3(0.0f, cos(angle2) * radius, sin(angle2) * radius);
        drawLine(p1, p2, color);
    }
}

void DebugDraw::drawCapsule(Vec3 position, float radius, float height, Vec3 color) {
    float halfHeight = height / 2.0f;

    // Top hemisphere
    Vec3 topCenter = position + Vec3(0.0f, halfHeight, 0.0f);
    drawSphere(topCenter, radius, color);

    // Bottom hemisphere
    Vec3 bottomCenter = position - Vec3(0.0f, halfHeight, 0.0f);
    drawSphere(bottomCenter, radius, color);

    // Cylinder sides
    const int segments = 8;
    const float angleStep = 2.0f * 3.14159f / segments;

    for (int i = 0; i < segments; i++) {
        float angle = i * angleStep;
        Vec3 offset = Vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);
        drawLine(topCenter + offset, bottomCenter + offset, color);
    }
}

void DebugDraw::clear() {
    m_lines.clear();
    m_colors.clear();
}

} // namespace spark
