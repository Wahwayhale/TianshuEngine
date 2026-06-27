#pragma once

#include "math/math_types.h"

namespace spark {

class ViewportPanel {
public:
    ViewportPanel();
    ~ViewportPanel();

    void render();

    Vec2 getSize() const { return m_size; }
    Vec2 getPosition() const { return m_position; }

private:
    void renderToolbar();
    void renderOverlay();
    void renderGizmo();

    Vec2 m_size = Vec2(800.0f, 600.0f);
    Vec2 m_position = Vec2(0.0f);

    // 工具状态
    int m_currentTool = 0;  // 0=Move, 1=Rotate, 2=Scale
    bool m_showGrid = true;
    bool m_showGizmo = true;
    bool m_wireframe = false;
};

} // namespace spark
