#include "toolbar_panel.h"
#include "ui/widgets.h"
#include <imgui.h>

namespace spark {

ToolbarPanel::ToolbarPanel() = default;
ToolbarPanel::~ToolbarPanel() = default;

void ToolbarPanel::render() {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

    ImGui::Begin("##Toolbar", nullptr, flags);

    // 场景控制
    renderSceneControls();

    ImGui::SameLine();
    ui::drawToolbarSeparator();

    // 变换工具
    renderTransformTools();

    ImGui::SameLine();
    ui::drawToolbarSeparator();

    // 视图选项
    renderViewOptions();

    ImGui::SameLine();
    ui::drawToolbarSeparator();

    // 快捷操作
    renderQuickActions();

    ImGui::End();
    ImGui::PopStyleVar(2);
}

void ToolbarPanel::renderSceneControls() {
    // 播放按钮
    if (m_isPlaying) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.80f, 0.40f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.90f, 0.50f, 0.90f));
    }

    if (ImGui::Button(ui::icons::Play, ImVec2(28, 28))) {
        m_isPlaying = !m_isPlaying;
        m_isPaused = false;
        if (m_playCallback) m_playCallback();
    }

    if (m_isPlaying) {
        ImGui::PopStyleColor(2);
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip(m_isPlaying ? "Stop" : "Play (Ctrl+P)");
    }

    ImGui::SameLine();

    // 暂停按钮
    bool canPause = m_isPlaying && !m_isPaused;
    if (!canPause) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button(ui::icons::Pause, ImVec2(28, 28))) {
        m_isPaused = !m_isPaused;
        if (m_pauseCallback) m_pauseCallback();
    }

    if (!canPause) {
        ImGui::EndDisabled();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Pause");
    }

    ImGui::SameLine();

    // 停止按钮
    bool canStop = m_isPlaying;
    if (!canStop) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button(ui::icons::Stop, ImVec2(28, 28))) {
        m_isPlaying = false;
        m_isPaused = false;
        if (m_stopCallback) m_stopCallback();
    }

    if (!canStop) {
        ImGui::EndDisabled();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Stop");
    }

    ImGui::SameLine();

    // 步进按钮
    bool canStep = m_isPaused;
    if (!canStep) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button(ui::icons::Forward, ImVec2(28, 28))) {
        // TODO: 步进一帧
    }

    if (!canStep) {
        ImGui::EndDisabled();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Step Frame");
    }
}

void ToolbarPanel::renderTransformTools() {
    // 移动工具
    bool isMove = (m_currentTool == 0);
    if (isMove) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.55f, 0.90f, 0.80f));
    }

    if (ImGui::Button(ui::icons::Move, ImVec2(28, 28))) {
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

    if (ImGui::Button(ui::icons::Rotate, ImVec2(28, 28))) {
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

    if (ImGui::Button(ui::icons::Scale, ImVec2(28, 28))) {
        m_currentTool = 2;
    }

    if (isScale) {
        ImGui::PopStyleColor();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Scale Tool (R)");
    }
}

void ToolbarPanel::renderViewOptions() {
    // 网格显示
    static bool showGrid = true;
    if (showGrid) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.55f, 0.90f, 0.60f));
    }

    if (ImGui::Button("G", ImVec2(28, 28))) {
        showGrid = !showGrid;
    }

    if (showGrid) {
        ImGui::PopStyleColor();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Toggle Grid");
    }

    ImGui::SameLine();

    // 线框模式
    static bool wireframe = false;
    if (wireframe) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.55f, 0.90f, 0.60f));
    }

    if (ImGui::Button("W", ImVec2(28, 28))) {
        wireframe = !wireframe;
    }

    if (wireframe) {
        ImGui::PopStyleColor();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Toggle Wireframe");
    }
}

void ToolbarPanel::renderQuickActions() {
    // 保存
    if (ImGui::Button(ui::icons::File, ImVec2(28, 28))) {
        // TODO: 保存场景
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Save Scene (Ctrl+S)");
    }

    ImGui::SameLine();

    // 撤销
    if (ImGui::Button(ui::icons::Rewind, ImVec2(28, 28))) {
        // TODO: 撤销
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Undo (Ctrl+Z)");
    }

    ImGui::SameLine();

    // 重做
    if (ImGui::Button(ui::icons::Forward, ImVec2(28, 28))) {
        // TODO: 重做
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Redo (Ctrl+Y)");
    }
}

} // namespace spark
