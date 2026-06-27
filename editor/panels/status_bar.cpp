#include "status_bar.h"
#include "ui/widgets.h"
#include <imgui.h>
#include <cstdio>

namespace spark {

StatusBar::StatusBar() = default;
StatusBar::~StatusBar() = default;

void StatusBar::render() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDecoration;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.08f, 1.00f));

    ImGui::Begin("##StatusBar", nullptr, flags);

    float availWidth = ImGui::GetContentRegionAvail().x;

    // 左侧信息
    renderLeft();

    // 中间信息
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - 300) * 0.5f);
    renderCenter();

    // 右侧信息
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - 200);
    renderRight();

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void StatusBar::renderLeft() {
    // FPS
    ImVec4 fpsColor;
    if (m_fps >= 60.0f) {
        fpsColor = ImVec4(0.20f, 0.80f, 0.40f, 1.00f);  // 绿色
    } else if (m_fps >= 30.0f) {
        fpsColor = ImVec4(0.95f, 0.70f, 0.20f, 1.00f);  // 黄色
    } else {
        fpsColor = ImVec4(0.90f, 0.25f, 0.25f, 1.00f);  // 红色
    }

    ImGui::TextColored(fpsColor, "%.1f FPS", m_fps);

    ImGui::SameLine();
    ui::drawToolbarSeparator();

    // 实体数量
    ImGui::TextColored(ImVec4(0.60f, 0.60f, 0.65f, 1.00f), "%d Entities", m_entityCount);

    ImGui::SameLine();
    ui::drawToolbarSeparator();

    // 内存使用
    float memMB = m_memoryUsage / (1024.0f * 1024.0f);
    ImGui::TextColored(ImVec4(0.60f, 0.60f, 0.65f, 1.00f), "%.1f MB", memMB);
}

void StatusBar::renderCenter() {
    // 选中的实体
    if (!m_selectedEntityName.empty()) {
        ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.00f), "Selected: %s", m_selectedEntityName.c_str());
    } else {
        ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.55f, 1.00f), "No selection");
    }
}

void StatusBar::renderRight() {
    // 引擎信息
    ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.55f, 1.00f), "Spark Engine v0.10.0");
}

void StatusBar::updateStats(float fps, int entityCount, size_t memoryUsage) {
    m_fps = fps;
    m_entityCount = entityCount;
    m_memoryUsage = memoryUsage;
}

void StatusBar::setSelectedEntity(const std::string& name) {
    m_selectedEntityName = name;
}

} // namespace spark
