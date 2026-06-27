#include "animation_editor_panel.h"
#include "renderer/animation.h"
#include "ui/widgets.h"
#include <imgui.h>

namespace spark {

AnimationEditorPanel::AnimationEditorPanel() = default;
AnimationEditorPanel::~AnimationEditorPanel() = default;

void AnimationEditorPanel::render() {
    ImGui::Begin("Animation Editor");

    if (m_animation) {
        renderToolbar();
        ImGui::Separator();
        renderTimeline();
        ImGui::Separator();
        renderKeyframeEditor();
        ImGui::Separator();
        renderPreview();
        ImGui::Separator();
        renderProperties();
    } else {
        ui::drawEmptyState(ui::icons::Assets, "No animation selected", "Select an animation to edit");
    }

    ImGui::End();
}

void AnimationEditorPanel::renderToolbar() {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));

    // 播放控制
    if (m_isPlaying) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.8f));
    }

    if (ImGui::Button(m_isPlaying ? ui::icons::Pause : ui::icons::Play, ImVec2(28, 28))) {
        m_isPlaying = !m_isPlaying;
    }

    if (m_isPlaying) {
        ImGui::PopStyleColor();
    }

    ImGui::SameLine();

    // 停止
    if (ImGui::Button(ui::icons::Stop, ImVec2(28, 28))) {
        m_isPlaying = false;
        m_currentTime = 0.0f;
    }

    ImGui::SameLine();

    // 循环
    if (m_isLooping) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 0.8f));
    }

    if (ImGui::Button("Loop", ImVec2(50, 28))) {
        m_isLooping = !m_isLooping;
    }

    if (m_isLooping) {
        ImGui::PopStyleColor();
    }

    ImGui::SameLine();
    ui::drawToolbarSeparator();

    // 播放速度
    ImGui::Text("Speed:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::SliderFloat("##Speed", &m_playbackSpeed, 0.1f, 3.0f, "%.1fx");

    ImGui::SameLine();
    ui::drawToolbarSeparator();

    // 当前时间
    ImGui::Text("Time:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::DragFloat("##Time", &m_currentTime, 0.01f, 0.0f, m_duration, "%.2f s");

    ImGui::SameLine();
    ImGui::Text("/ %.2f s", m_duration);

    ImGui::PopStyleVar(2);
}

void AnimationEditorPanel::renderTimeline() {
    ImGui::Text("Timeline");

    ImVec2 avail = ImGui::GetContentRegionAvail();
    float timelineHeight = 100.0f;
    float timelineWidth = avail.x;

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // 绘制时间线背景
    drawList->AddRectFilled(
        cursorPos,
        ImVec2(cursorPos.x + timelineWidth, cursorPos.y + timelineHeight),
        IM_COL32(20, 20, 25, 255)
    );

    // 绘制时间刻度
    float pixelsPerSecond = timelineWidth / m_duration * m_zoom;
    for (float t = 0; t <= m_duration; t += 0.1f) {
        float x = cursorPos.x + t * pixelsPerSecond;
        bool isMajor = (fmod(t, 0.5f) < 0.01f);

        drawList->AddLine(
            ImVec2(x, cursorPos.y),
            ImVec2(x, cursorPos.y + (isMajor ? 15 : 8)),
            IM_COL32(100, 100, 100, 255)
        );

        if (isMajor) {
            char timeStr[32];
            snprintf(timeStr, sizeof(timeStr), "%.1f", t);
            drawList->AddText(
                ImVec2(x + 2, cursorPos.y + 18),
                IM_COL32(150, 150, 150, 255),
                timeStr
            );
        }
    }

    // 绘制播放头
    float playheadX = cursorPos.x + m_currentTime * pixelsPerSecond;
    drawList->AddLine(
        ImVec2(playheadX, cursorPos.y),
        ImVec2(playheadX, cursorPos.y + timelineHeight),
        IM_COL32(255, 50, 50, 255),
        2.0f
    );

    // 绘制关键帧
    // 简化实现：实际需要从动画数据中获取关键帧

    ImGui::Dummy(ImVec2(timelineWidth, timelineHeight));

    // 时间线交互
    if (ImGui::IsItemClicked()) {
        ImVec2 mousePos = ImGui::GetMousePos();
        m_currentTime = (mousePos.x - cursorPos.x) / pixelsPerSecond;
        m_currentTime = std::clamp(m_currentTime, 0.0f, m_duration);
    }
}

void AnimationEditorPanel::renderKeyframeEditor() {
    if (ImGui::CollapsingHeader("Keyframes", ImGuiTreeNodeFlags_DefaultOpen)) {
        // 关键帧类型选择
        const char* types[] = { "Position", "Rotation", "Scale" };
        int currentType = static_cast<int>(m_selectedType);
        if (ImGui::Combo("Type", &currentType, types, 3)) {
            m_selectedType = static_cast<KeyframeType>(currentType);
        }

        ImGui::Spacing();

        // 关键帧列表
        ImGui::Text("Keyframes at current time:");

        // 添加关键帧按钮
        if (ImGui::Button("Add Keyframe", ImVec2(120, 0))) {
            // TODO: 添加关键帧
        }

        ImGui::SameLine();

        if (ImGui::Button("Delete Keyframe", ImVec2(120, 0))) {
            // TODO: 删除关键帧
        }

        ImGui::Spacing();

        // 关键帧值编辑
        ImGui::Text("Value:");
        float value[3] = {0.0f, 0.0f, 0.0f};
        if (ImGui::DragFloat3("##Value", value, 0.01f)) {
            // TODO: 更新关键帧值
        }

        ImGui::Text("Interpolation:");
        const char* interpolations[] = { "Linear", "Cubic", "Step" };
        int currentInterp = 0;
        if (ImGui::Combo("##Interpolation", &currentInterp, interpolations, 3)) {
            // TODO: 更新插值类型
        }
    }
}

void AnimationEditorPanel::renderPreview() {
    if (!m_showPreview) return;

    if (ImGui::CollapsingHeader("Preview", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImVec2 previewSize = ImVec2(200, 200);
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // 绘制预览背景
        drawList->AddRectFilled(
            cursorPos,
            ImVec2(cursorPos.x + previewSize.x, cursorPos.y + previewSize.y),
            IM_COL32(30, 30, 35, 255)
        );

        // 绘制简单的骨骼预览
        ImVec2 center = ImVec2(cursorPos.x + previewSize.x * 0.5f, cursorPos.y + previewSize.y * 0.5f);

        // 绘制骨骼线条
        drawList->AddLine(
            ImVec2(center.x, center.y - 40),
            center,
            IM_COL32(200, 200, 200, 255),
            2.0f
        );

        drawList->AddLine(
            center,
            ImVec2(center.x - 30, center.y + 40),
            IM_COL32(200, 200, 200, 255),
            2.0f
        );

        drawList->AddLine(
            center,
            ImVec2(center.x + 30, center.y + 40),
            IM_COL32(200, 200, 200, 255),
            2.0f
        );

        // 绘制关节
        drawList->AddCircleFilled(ImVec2(center.x, center.y - 40), 5, IM_COL32(255, 100, 100, 255));
        drawList->AddCircleFilled(center, 5, IM_COL32(100, 255, 100, 255));
        drawList->AddCircleFilled(ImVec2(center.x - 30, center.y + 40), 5, IM_COL32(100, 100, 255, 255));
        drawList->AddCircleFilled(ImVec2(center.x + 30, center.y + 40), 5, IM_COL32(100, 100, 255, 255));

        ImGui::Dummy(previewSize);

        // 旋转控制
        ImGui::SliderFloat("Preview Rotation", &m_previewRotation, 0.0f, 360.0f, "%.0f°");
    }
}

void AnimationEditorPanel::renderProperties() {
    if (ImGui::CollapsingHeader("Properties")) {
        ImGui::Text("Animation Properties");

        ImGui::Text("Duration:");
        ImGui::SameLine();
        ImGui::Text("%.2f s", m_duration);

        ImGui::Text("FPS:");
        ImGui::SameLine();
        ImGui::Text("30");

        ImGui::Text("Keyframes:");
        ImGui::SameLine();
        ImGui::Text("0");

        ImGui::Spacing();

        // 导入/导出
        if (ImGui::Button("Import Animation", ImVec2(150, 0))) {
            // TODO: 导入动画文件
        }

        ImGui::SameLine();

        if (ImGui::Button("Export Animation", ImVec2(150, 0))) {
            // TODO: 导出动画文件
        }
    }
}

} // namespace spark
