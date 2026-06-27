#pragma once

#include "math/math_types.h"
#include <vector>
#include <cstdint>

namespace spark {

// 调试绘制颜色
struct DebugColor {
    float r, g, b, a;

    DebugColor() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    DebugColor(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

    static DebugColor Red() { return DebugColor(1.0f, 0.0f, 0.0f); }
    static DebugColor Green() { return DebugColor(0.0f, 1.0f, 0.0f); }
    static DebugColor Blue() { return DebugColor(0.0f, 0.0f, 1.0f); }
    static DebugColor Yellow() { return DebugColor(1.0f, 1.0f, 0.0f); }
    static DebugColor White() { return DebugColor(1.0f, 1.0f, 1.0f); }
    static DebugColor Gray() { return DebugColor(0.5f, 0.5f, 0.5f); }
};

// 调试绘制图元
struct DebugLine {
    Vec3 start;
    Vec3 end;
    DebugColor color;
    float thickness = 1.0f;
};

struct DebugPoint {
    Vec3 position;
    DebugColor color;
    float size = 5.0f;
};

struct DebugBox {
    Vec3 center;
    Vec3 halfExtents;
    Quat rotation;
    DebugColor color;
    bool wireframe = true;
};

struct DebugSphere {
    Vec3 center;
    float radius;
    DebugColor color;
    bool wireframe = true;
    int segments = 16;
};

// 调试绘制系统
class DebugDraw {
public:
    static DebugDraw& get();

    bool initialize();
    void shutdown();

    // 绘制命令
    void drawLine(const Vec3& start, const Vec3& end, const DebugColor& color = DebugColor::White(), float thickness = 1.0f);
    void drawPoint(const Vec3& position, const DebugColor& color = DebugColor::White(), float size = 5.0f);
    void drawBox(const Vec3& center, const Vec3& halfExtents, const Quat& rotation = Quat(1, 0, 0, 0), const DebugColor& color = DebugColor::White(), bool wireframe = true);
    void drawSphere(const Vec3& center, float radius, const DebugColor& color = DebugColor::White(), bool wireframe = true, int segments = 16);

    // 碰撞体绘制
    void drawAABB(const Vec3& min, const Vec3& max, const DebugColor& color = DebugColor::Yellow());
    void drawOBB(const Vec3& center, const Vec3& halfExtents, const Quat& rotation, const DebugColor& color = DebugColor::Yellow());

    // 物理调试图元
    void drawRay(const Vec3& origin, const Vec3& direction, float length, const DebugColor& color = DebugColor::Red());
    void drawContactPoint(const Vec3& point, const Vec3& normal, float size, const DebugColor& color = DebugColor::Green());

    // 渲染
    void render(VkCommandBuffer commandBuffer);

    // 清除
    void clear();

    // 设置
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    // 统计
    int getLineCount() const { return static_cast<int>(m_lines.size()); }
    int getPointCount() const { return static_cast<int>(m_points.size()); }

private:
    DebugDraw() = default;

    bool m_enabled = true;

    std::vector<DebugLine> m_lines;
    std::vector<DebugPoint> m_points;
    std::vector<DebugBox> m_boxes;
    std::vector<DebugSphere> m_spheres;
};

} // namespace spark
