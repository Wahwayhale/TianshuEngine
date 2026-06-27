#include "viewport_panel.h"
#include "ui/widgets.h"
#include <imgui.h>

namespace spark {

ViewportPanel::ViewportPanel() = default;
ViewportPanel::~ViewportPanel() = default;

void ViewportPanel::render() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar);

    // 获取窗口信息
    m_size = Vec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
    m_position = Vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);

    // 渲染 3D 场景的区域
    // 这里应该是渲染到纹理的输出
    // 暂时用占位符
    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();

    // 背景
    drawList->AddRectFilled(
        pos,
        ImVec2(pos.x + avail.x, pos.y + avail.y),
        colorToU32(ImVec4(0.08f, 0.08f, 0.10f, 1.00f))
    );

    // 网格线（如果启用）
    if (m_showGrid) {
        ImU32 gridColor = colorToU32(ImVec4(0.15f, 0.15f, 0.18f, 0.50f));
        float gridSize = 50.0f;

        for (float x = 0; x < avail.x; x += gridSize) {
            drawList->AddLine(
                ImVec2(pos.x + x, pos.y),
                ImVec2(pos.x + x, pos.y + avail.y),
                gridColor
            );
        }

        for (float y = 0; y < avail.y; y += gridSize) {
            drawList->AddLine(
                ImVec2(pos.x, pos.y + y),
                ImVec2(pos.x + avail.x, pos.y + y),
                gridColor
            );
        }
    }

    // 占位文本
    const char* placeholder = "Viewport - Scene Rendering Area";
    ImVec2 textSize = ImGui::CalcTextSize(placeholder);
    drawList->AddText(
        ImVec2(pos.x + (avail.x - textSize.x) * 0.5f, pos.y + (avail.y - textSize.y) * 0.5f),
        colorToU32(ImVec4(0.30f, 0.30f, 0.35f, 1.00f)),
        placeholder
    );

    // 占用空间
    ImGui::Dummy(avail);

    // Overlay 工具栏
    renderOverlay();

    ImGui::End();
    ImGui::PopStyleVar();
}

void ViewportPanel::renderToolbar() {
    // 这个方法可以在需要时调用
}

void ViewportPanel::renderOverlay() {
    // 在视口上方绘制 overlay 工具栏
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return;

    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();

    // 工具栏背景
    float toolbarHeight = 32.0f;
    ImVec2 toolbarPos = ImVec2(windowPos.x + 8, windowPos.y + 8);
    ImVec2 toolbarSize = ImVec2(200, toolbarHeight);

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // 背景
    drawList->AddRectFilled(
        toolbarPos,
        ImVec2(toolbarPos.x + toolbarSize.x, toolbarPos.y + toolbarSize.y),
        colorToU32(ImVec4(0.08f, 0.08f, 0.10f, 0.90f)),
        6.0f
    );

    // 边框
    drawList->AddRect(
        toolbarPos,
        ImVec2(toolbarPos.x + toolbarSize.x, toolbarPos.y + toolbarSize.y),
        colorToU32(ImVec4(0.20f, 0.20f, 0.23f, 0.60f)),
        6.0f
    );

    // 工具按钮
    ImGui::SetCursorScreenPos(ImVec2(toolbarPos.x + 4, toolbarPos.y + 2));

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 0));

    // 移动工具
    bool isMove = (m_currentTool == 0);
    if (isMove) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.55f, 0.90f, 0.80f));
    }
    if (ImGui::SmallButton(ui::icons::Move)) {
        m_currentTool = 0;
    }
    if (isMove) {
        ImGui::PopStyleColor();
    }
    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Move Tool (W)");
    }

    ImGui::SameLine();

    // 旋转工具
    bool isRotate = (m_currentTool == 1);
    if (isRotate) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.55f, 0.90f, 0.80f));
    }
    if (ImGui::SmallButton(ui::icons::Rotate)) {
        m_currentTool = 1;
    }
    if (isRotate) {
        ImGui::PopStyleColor();
    }
    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Rotate Tool (E)");
    }

    ImGui::SameLine();

    // 缩放工具
    bool isScale = (m_currentTool == 2);
    if (isScale) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.55f, 0.90f, 0.80f));
    }
    if (ImGui::SmallButton(ui::icons::Scale)) {
        m_currentTool = 2;
    }
    if (isScale) {
        ImGui::PopStyleColor();
    }
    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Scale Tool (R)");
    }

    ImGui::PopStyleVar(2);

    // 右上角信息
    ImVec2 infoPos = ImVec2(windowPos.x + windowSize.x - 150, windowPos.y + 8);
    ImGui::SetCursorScreenPos(infoPos);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.60f, 0.60f, 0.65f, 1.00f));
    ImGui::Text("%.0fx%.0f", m_size.x, m_size.y);
    ImGui::PopStyleColor();
}

void ViewportPanel::renderGizmo() {
    // Gizmo 渲染（需要 ImGuizmo 集成）
    // 这里是占位实现
}

} // namespace spark
