#include "widgets.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cmath>

namespace spark {
namespace ui {

// =============================================
// Vec3 控件
// =============================================

bool drawVec3Control(const char* label, Vec3& values, float resetValue, float columnWidth) {
    bool changed = false;

    ImGui::PushID(label);

    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label);
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));

    float lineHeight = ImGui::GetFrameHeight();
    ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

    // X
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.80f, 0.20f, 0.20f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,  ImVec4(0.90f, 0.30f, 0.30f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,   ImVec4(0.70f, 0.15f, 0.15f, 1.00f));
    if (ImGui::Button("X", buttonSize)) {
        values.x = resetValue;
        changed = true;
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    changed |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    // Y
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.20f, 0.70f, 0.20f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,  ImVec4(0.30f, 0.80f, 0.30f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,   ImVec4(0.15f, 0.60f, 0.15f, 1.00f));
    if (ImGui::Button("Y", buttonSize)) {
        values.y = resetValue;
        changed = true;
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    changed |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    // Z
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.20f, 0.30f, 0.80f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,  ImVec4(0.30f, 0.40f, 0.90f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,   ImVec4(0.15f, 0.25f, 0.70f, 1.00f));
    if (ImGui::Button("Z", buttonSize)) {
        values.z = resetValue;
        changed = true;
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    changed |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();
    ImGui::Columns(1);

    ImGui::PopID();

    return changed;
}

// =============================================
// 颜色编辑器
// =============================================

bool drawColorEdit3(const char* label, float* color, ImGuiColorEditFlags flags) {
    return ImGui::ColorEdit3(label, color, flags | ImGuiColorEditFlags_PickerHueWheel);
}

bool drawColorEdit4(const char* label, float* color, ImGuiColorEditFlags flags) {
    return ImGui::ColorEdit4(label, color, flags | ImGuiColorEditFlags_PickerHueWheel);
}

// =============================================
// 搜索框
// =============================================

bool drawSearchBar(char* buffer, size_t bufferSize, const char* hint, float width) {
    bool changed = false;

    if (width > 0) {
        ImGui::SetNextItemWidth(width);
    }

    // 搜索图标
    ImGui::Text("%s", icons::Search);
    ImGui::SameLine();

    // 输入框
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 6));

    changed = ImGui::InputTextWithHint("##Search", hint, buffer, bufferSize);

    ImGui::PopStyleVar(2);

    return changed;
}

// =============================================
// 分隔线
// =============================================

void drawSeparator(const char* label) {
    if (label) {
        ImVec2 textSize = ImGui::CalcTextSize(label);
        float availWidth = ImGui::GetContentRegionAvail().x;
        float separatorWidth = (availWidth - textSize.x - 16.0f) * 0.5f;

        if (separatorWidth > 0) {
            ImGui::SameLine();
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
            ImGui::SameLine();
            ImGui::TextDisabled("%s", label);
            ImGui::SameLine();
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
        } else {
            ImGui::TextDisabled("%s", label);
        }
    } else {
        ImGui::Separator();
    }
}

// =============================================
// 图标按钮
// =============================================

bool drawIconButton(const char* icon, const char* tooltip, bool selected) {
    bool clicked = false;

    if (selected) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    }

    clicked = ImGui::Button(icon, ImVec2(28, 28));

    if (selected) {
        ImGui::PopStyleColor();
    }

    if (tooltip && ImGui::IsItemHovered()) {
        drawTooltip(tooltip);
    }

    return clicked;
}

bool drawIconButton(const char* icon, const ImVec4& tint, const char* tooltip, bool selected) {
    bool clicked = false;

    ImGui::PushStyleColor(ImGuiCol_Text, tint);
    clicked = drawIconButton(icon, tooltip, selected);
    ImGui::PopStyleColor();

    return clicked;
}

// =============================================
// 工具栏按钮
// =============================================

bool drawToolButton(const char* icon, const char* label, bool active, const ImVec2& size) {
    bool clicked = false;

    if (active) {
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.20f, 0.55f, 0.90f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,  ImVec4(0.30f, 0.65f, 1.00f, 0.90f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,   ImVec4(0.15f, 0.45f, 0.80f, 1.00f));
    }

    char fullLabel[128];
    snprintf(fullLabel, sizeof(fullLabel), "%s %s", icon, label);
    clicked = ImGui::Button(fullLabel, size);

    if (active) {
        ImGui::PopStyleColor(3);
    }

    return clicked;
}

// =============================================
// 带图标的树节点
// =============================================

bool drawIconTreeNode(const char* icon, const char* label, ImGuiTreeNodeFlags flags) {
    ImGui::Text("%s", icon);
    ImGui::SameLine();
    return ImGui::TreeNodeEx(label, flags);
}

// =============================================
// 带图标的可选列表
// =============================================

bool drawIconSelectable(const char* icon, const char* label, bool selected, ImGuiSelectableFlags flags) {
    char fullLabel[256];
    snprintf(fullLabel, sizeof(fullLabel), "%s  %s", icon, label);
    return ImGui::Selectable(fullLabel, selected, flags);
}

// =============================================
// 悬浮提示
// =============================================

void drawTooltip(const char* text) {
    ImGui::BeginTooltip();
    ImGui::Text("%s", text);
    ImGui::EndTooltip();
}

void drawTooltip(const char* title, const char* body) {
    ImGui::BeginTooltip();
    ImGui::TextColored(ImVec4(1.0f, 0.9f, 0.6f, 1.0f), "%s", title);
    ImGui::Separator();
    ImGui::Text("%s", body);
    ImGui::EndTooltip();
}

// =============================================
// 进度条
// =============================================

void drawProgressBar(float fraction, const ImVec2& size, const char* overlay) {
    // 动画效果
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.20f, 0.55f, 0.90f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.18f, 1.00f));
    ImGui::ProgressBar(fraction, size, overlay);
    ImGui::PopStyleColor(2);
}

// =============================================
// 面板标题栏
// =============================================

bool drawPanelHeader(const char* icon, const char* title, bool* pOpen) {
    bool headerClicked = false;

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.12f, 0.12f, 0.15f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    char fullTitle[256];
    snprintf(fullTitle, sizeof(fullTitle), "%s  %s", icon, title);

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    headerClicked = ImGui::CollapsingHeader(fullTitle, pOpen ? ImGuiTreeNodeFlags_AllowOverlap : 0);

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    return headerClicked;
}

// =============================================
// 组件标题栏
// =============================================

bool drawComponentHeader(const char* icon, const char* title, bool& open, std::function<void()> onDelete) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    // 背景色
    ImVec4 headerColor = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
    ImGui::PushStyleColor(ImGuiCol_Header, headerColor);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.15f, 0.15f, 0.18f, 1.00f));

    char fullTitle[256];
    snprintf(fullTitle, sizeof(fullTitle), "%s  %s", icon, title);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (open) {
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }

    bool isOpen = ImGui::TreeNodeEx(fullTitle, flags);

    // 右键菜单
    if (ImGui::BeginPopupContextItem()) {
        if (onDelete && ImGui::MenuItem("Remove Component")) {
            onDelete();
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();

    return isOpen;
}

// =============================================
// 信息面板
// =============================================

void drawInfoPanel(const char* icon, const char* label, const char* value, const ImVec4& color) {
    ImGui::TextColored(color, "%s", icon);
    ImGui::SameLine();
    ImGui::Text("%s:", label);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.9f), "%s", value);
}

// =============================================
// 状态指示器
// =============================================

void drawStatusDot(const ImVec4& color, const char* tooltip) {
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float radius = 5.0f;

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddCircleFilled(ImVec2(pos.x + radius, pos.y + radius), radius, colorToU32(color));

    ImGui::Dummy(ImVec2(radius * 2 + 4, radius * 2));

    if (tooltip && ImGui::IsItemHovered()) {
        drawTooltip(tooltip);
    }
}

// =============================================
// 快捷键标签
// =============================================

void drawKeyLabel(const char* key) {
    ImVec2 textSize = ImGui::CalcTextSize(key);
    float padding = 4.0f;

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // 背景
    drawList->AddRectFilled(
        pos,
        ImVec2(pos.x + textSize.x + padding * 2, pos.y + textSize.y + padding * 2),
        colorToU32(ImVec4(0.20f, 0.20f, 0.25f, 1.00f)),
        3.0f
    );

    // 文本
    drawList->AddText(
        ImVec2(pos.x + padding, pos.y + padding),
        colorToU32(ImVec4(0.70f, 0.70f, 0.75f, 1.00f)),
        key
    );

    ImGui::Dummy(ImVec2(textSize.x + padding * 2, textSize.y + padding * 2));
}

// =============================================
// 属性行
// =============================================

void drawProperty(const char* label, const char* value) {
    ImGui::TextDisabled("%s:", label);
    ImGui::SameLine();
    ImGui::Text("%s", value);
}

void drawProperty(const char* label, float value, const char* format) {
    ImGui::TextDisabled("%s:", label);
    ImGui::SameLine();
    ImGui::Text(format, value);
}

void drawProperty(const char* label, int value) {
    ImGui::TextDisabled("%s:", label);
    ImGui::SameLine();
    ImGui::Text("%d", value);
}

void drawProperty(const char* label, bool value) {
    ImGui::TextDisabled("%s:", label);
    ImGui::SameLine();
    ImGui::TextColored(
        value ? ImVec4(0.20f, 0.80f, 0.40f, 1.00f) : ImVec4(0.90f, 0.25f, 0.25f, 1.00f),
        "%s", value ? "True" : "False"
    );
}

// =============================================
// 菜单项带图标
// =============================================

bool drawIconMenuItem(const char* icon, const char* label, const char* shortcut, bool selected, bool enabled) {
    char fullLabel[256];
    snprintf(fullLabel, sizeof(fullLabel), "%s  %s", icon, label);
    return ImGui::MenuItem(fullLabel, shortcut, selected, enabled);
}

// =============================================
// 工具栏分隔线
// =============================================

void drawToolbarSeparator() {
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();
}

// =============================================
// 空状态提示
// =============================================

void drawEmptyState(const char* icon, const char* message, const char* submessage) {
    ImVec2 avail = ImGui::GetContentRegionAvail();
    ImVec2 textSize = ImGui::CalcTextSize(message);
    float iconSize = 48.0f;

    float yOffset = (avail.y - textSize.y - (submessage ? 20 : 0) - iconSize) * 0.5f;
    if (yOffset > 0) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yOffset);
    }

    // 图标
    ImVec2 iconTextSize = ImGui::CalcTextSize(icon);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail.x - iconSize) * 0.5f);
    ImGui::PushFont(nullptr);  // 使用大字体（如果有）
    ImGui::TextColored(ImVec4(0.30f, 0.30f, 0.35f, 1.00f), "%s", icon);
    ImGui::PopFont();

    // 消息
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail.x - textSize.x) * 0.5f);
    ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.55f, 1.00f), "%s", message);

    // 子消息
    if (submessage) {
        ImVec2 subTextSize = ImGui::CalcTextSize(submessage);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail.x - subTextSize.x) * 0.5f);
        ImGui::TextColored(ImVec4(0.35f, 0.35f, 0.40f, 1.00f), "%s", submessage);
    }
}

// =============================================
// 统计卡片
// =============================================

void drawStatCard(const char* label, const char* value, const ImVec4& color) {
    ImVec2 avail = ImGui::GetContentRegionAvail();
    float cardWidth = 120.0f;

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // 背景
    drawList->AddRectFilled(
        pos,
        ImVec2(pos.x + cardWidth, pos.y + 50),
        colorToU32(ImVec4(0.10f, 0.10f, 0.12f, 1.00f)),
        6.0f
    );

    // 顶部颜色条
    drawList->AddRectFilled(
        pos,
        ImVec2(pos.x + cardWidth, pos.y + 3),
        colorToU32(color),
        6.0f, ImDrawFlags_RoundCornersTop
    );

    // 数值
    drawList->AddText(
        ImVec2(pos.x + 10, pos.y + 10),
        colorToU32(ImVec4(1.0f, 1.0f, 1.0f, 0.95f)),
        value
    );

    // 标签
    drawList->AddText(
        ImVec2(pos.x + 10, pos.y + 30),
        colorToU32(ImVec4(0.50f, 0.50f, 0.55f, 1.00f)),
        label
    );

    ImGui::Dummy(ImVec2(cardWidth, 50));

    // 同行排列
    if (avail.x > cardWidth + 10) {
        ImGui::SameLine(0, 10);
    }
}

} // namespace ui
} // namespace spark
