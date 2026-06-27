#pragma once

#include "math/math_types.h"
#include <vector>

namespace spark {

class DebugDraw {
public:
    DebugDraw();
    ~DebugDraw();

    void drawLine(Vec3 start, Vec3 end, Vec3 color = Vec3(1.0f));
    void drawBox(Vec3 position, Vec3 size, Vec3 color = Vec3(1.0f));
    void drawSphere(Vec3 position, float radius, Vec3 color = Vec3(1.0f));
    void drawCapsule(Vec3 position, float radius, float height, Vec3 color = Vec3(1.0f));

    void clear();
    const std::vector<Vec3>& getLines() const { return m_lines; }
    const std::vector<Vec3>& getColors() const { return m_colors; }

private:
    std::vector<Vec3> m_lines;
    std::vector<Vec3> m_colors;
};

} // namespace spark
