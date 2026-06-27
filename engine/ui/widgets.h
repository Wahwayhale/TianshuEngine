#pragma once

#include <imgui.h>
#include <string>
#include <functional>
#include "math/math_types.h"

namespace spark {
namespace ui {

// =============================================
// 图标定义 (使用 Unicode 字符模拟图标)
// =============================================

namespace icons {
    // 这些是常用的 Unicode 符号，可作为图标使用
    // 实际项目中可集成 FontAwesome 或其他图标字体

    // 文件/文件夹
    constexpr const char* Folder      = "\xF0\x9F\x93\x81";  // 📁
    constexpr const char* File        = "\xF0\x9F\x93\x84";  // 📄
    constexpr const char* Image       = "\xF0\x9F\x96\xBC";  // 🖼
    constexpr const char* Model       = "\xF0\x9F\x8E\xB2";  // 🎲
    constexpr const char* Script      = "\xF0\x9E\xA7\x99";  // ⚙
    constexpr const char* Audio       = "\xF0\x9F\x8E\xB5";  // 🎵
    constexpr const char* Shader      = "\xF0\x9F\x94\xAE";  // 🔮

    // 编辑器操作
    constexpr const char* Play        = "\xE2\x96\xB6";      // ▶
    constexpr const char* Pause       = "\xE2\x8F\xB8";      // ⏸
    constexpr const char* Stop        = "\xE2\x8F\xB9";      // ⏹
    constexpr const char* Forward     = "\xE2\x8F\xA9";      // ⏩
    constexpr const char* Rewind      = "\xE2\x8F\xAA";      // ⏪

    // 变换工具
    constexpr const char* Move        = "\xE2\x86\x94";      // ↔
    constexpr const char* Rotate      = "\xF0\x9F\x94\x83";  // 🔄
    constexpr const char* Scale       = "\xF0\x9F\x94\x8D";  // 🔍

    // 组件图标
    constexpr const char* Transform   = "\xF0\x9F\x93\x8C";  // 📌
    constexpr const char* Camera      = "\xF0\x9F\x93\xB7";  // 📷
    constexpr const char* Light       = "\xF0\x9F\x92\xA1";  // 💡
    constexpr const char* Mesh        = "\xF0\x9F\x94\xB7";  // 🔷
    constexpr const char* Physics     = "\xE2\x9A\x97";      // ⚗
    constexpr const char* Collider    = "\xF0\x9F\x9B\xA1";  // 🛡
    constexpr const char* ScriptComp  = "\xF0\x9E\xA7\x99";  // ⚙
    constexpr const char* AudioComp   = "\xF0\x9F\x8E\xB5";  // 🎵

    // 状态图标
    constexpr const char* Success     = "\xE2\x9C\x85";      // ✅
    constexpr const char* Warning     = "\xE2\x9A\xA0";      // ⚠
    constexpr const char* Error       = "\xE2\x9D\x8C";      // ❌
    constexpr const char* Info        = "\xE2\x84\xB9";      // ℹ

    // 其他
    constexpr const char* Search      = "\xF0\x9F\x94\x8D";  // 🔍
    constexpr const char* Settings    = "\xE2\x9A\x99";      // ⚙
    constexpr const char* Close       = "\xE2\x9C\x95";      // ✕
    constexpr const char* Plus        = "\xE2\x9E\x95";      // ➕
    constexpr const char* Minus       = "\xE2\x9E\x96";      // ➖
    constexpr const char* ArrowRight  = "\xE2\x9E\xA1";      // ➡
    constexpr const char* ArrowDown   = "\xE2\xAC\x87";      // ⬇
    constexpr const char* Check       = "\xE2\x9C\x93";      // ✓
    constexpr const char* Cross       = "\xE2\x9C\x97";      // ✗
    constexpr const char* Console     = "\xF0\x9F\x96\xA5";  // 🖥
    constexpr const char* Hierarchy   = "\xF0\x9F\x8C\xB3";  // 🌳
    constexpr const char* Assets      = "\xF0\x9F\x93\xA6";  // 📦
    constexpr const char* Viewport    = "\xF0\x9F\x8E\xAC";  // 🎬
    constexpr const char* Settings2   = "\xF0\x9F\x94\xA7";  // 🔧
}

// =============================================
// 颜色辅助
// =============================================

inline ImVec4 colorFromHex(uint32_t hex) {
    return ImVec4(
        ((hex >> 24) & 0xFF) / 255.0f,
        ((hex >> 16) & 0xFF) / 255.0f,
        ((hex >> 8)  & 0xFF) / 255.0f,
        (hex         & 0xFF) / 255.0f
    );
}

inline ImVec4 colorWithAlpha(const ImVec4& color, float alpha) {
    return ImVec4(color.x, color.y, color.z, alpha);
}

inline ImU32 colorToU32(const ImVec4& color) {
    return ImGui::ColorConvertFloat4ToU32(color);
}

// =============================================
// 自定义控件
// =============================================

// Vec3 输入控件（带颜色标签）
bool drawVec3Control(const char* label, Vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

// 带预览的颜色编辑器
bool drawColorEdit3(const char* label, float* color, ImGuiColorEditFlags flags = 0);
bool drawColorEdit4(const char* label, float* color, ImGuiColorEditFlags flags = 0);

// 搜索框
bool drawSearchBar(char* buffer, size_t bufferSize, const char* hint = "Search...", float width = 0.0f);

// 带标签的分隔线
void drawSeparator(const char* label = nullptr);

// 图标按钮
bool drawIconButton(const char* icon, const char* tooltip = nullptr, bool selected = false);
bool drawIconButton(const char* icon, const ImVec4& tint, const char* tooltip = nullptr, bool selected = false);

// 工具栏按钮
bool drawToolButton(const char* icon, const char* label, bool active = false, const ImVec2& size = ImVec2(0, 0));

// 带图标的树节点
bool drawIconTreeNode(const char* icon, const char* label, ImGuiTreeNodeFlags flags = 0);

// 带图标的可选列表
bool drawIconSelectable(const char* icon, const char* label, bool selected = false, ImGuiSelectableFlags flags = 0);

// 悬浮提示
void drawTooltip(const char* text);
void drawTooltip(const char* title, const char* body);

// 进度条（带动画）
void drawProgressBar(float fraction, const ImVec2& size = ImVec2(-1, 0), const char* overlay = nullptr);

// 面板标题栏（带图标和操作按钮）
bool drawPanelHeader(const char* icon, const char* title, bool* pOpen = nullptr);

// 组件标题栏（带折叠和删除按钮）
bool drawComponentHeader(const char* icon, const char* title, bool& open, std::function<void()> onDelete = nullptr);

// 信息面板（带图标和颜色标记）
void drawInfoPanel(const char* icon, const char* label, const char* value, const ImVec4& color = ImVec4(1, 1, 1, 1));

// 状态指示器
void drawStatusDot(const ImVec4& color, const char* tooltip = nullptr);

// 快捷键标签
void drawKeyLabel(const char* key);

// 属性行（标签 + 值）
void drawProperty(const char* label, const char* value);
void drawProperty(const char* label, float value, const char* format = "%.2f");
void drawProperty(const char* label, int value);
void drawProperty(const char* label, bool value);

// 菜单项带图标
bool drawIconMenuItem(const char* icon, const char* label, const char* shortcut = nullptr, bool selected = false, bool enabled = true);

// 工具栏分隔线
void drawToolbarSeparator();

// 空状态提示
void drawEmptyState(const char* icon, const char* message, const char* submessage = nullptr);

// 统计卡片
void drawStatCard(const char* label, const char* value, const ImVec4& color = ImVec4(1, 1, 1, 1));

} // namespace ui
} // namespace spark
