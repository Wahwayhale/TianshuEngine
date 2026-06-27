#pragma once

#include <imgui.h>
#include <string>

namespace spark {
namespace ui {

// =============================================
// 主题配置结构体
// =============================================

struct ThemeColors {
    // 背景色
    ImVec4 background;        // 最深背景
    ImVec4 panelBg;           // 面板背景
    ImVec4 panelBgDark;       // 深面板背景
    ImVec4 panelBgHover;      // 面板悬停
    ImVec4 childBg;           // 子窗口背景
    ImVec4 popupBg;           // 弹出窗口背景

    // 边框色
    ImVec4 border;            // 边框
    ImVec4 borderLight;       // 亮边框

    // 文本色
    ImVec4 text;              // 主文本
    ImVec4 textDim;           // 暗文本
    ImVec4 textBright;        // 亮文本
    ImVec4 textDisabled;      // 禁用文本

    // 强调色
    ImVec4 accent;            // 主强调色
    ImVec4 accentHover;       // 强调色悬停
    ImVec4 accentActive;      // 强调色激活

    // 状态色
    ImVec4 success;           // 成功/确认
    ImVec4 warning;           // 警告
    ImVec4 error;             // 错误
    ImVec4 info;              // 信息

    // 交互色
    ImVec4 button;            // 按钮
    ImVec4 buttonHover;       // 按钮悬停
    ImVec4 buttonActive;      // 按钮激活

    // 标题栏
    ImVec4 titleBg;           // 标题栏背景
    ImVec4 titleBgActive;     // 活动标题栏
    ImVec4 titleBgCollapsed;  // 折叠标题栏

    // 标签页
    ImVec4 tab;               // 标签页
    ImVec4 tabHover;          // 标签页悬停
    ImVec4 tabActive;         // 活动标签页
    ImVec4 tabUnfocused;      // 非聚焦标签页

    // 滚动条
    ImVec4 scrollbarBg;       // 滚动条背景
    ImVec4 scrollbarGrab;     // 滚动条抓手
    ImVec4 scrollbarGrabHover;// 滚动条抓手悬停
    ImVec4 scrollbarGrabActive;// 滚动条抓手激活

    // 选择
    ImVec4 selection;         // 选中项
    ImVec4 selectionActive;   // 活动选中项

    // 分割线
    ImVec4 separator;         // 分割线
    ImVec4 separatorHover;    // 分割线悬停

    // Resize Grip
    ImVec4 resizeGrip;
    ImVec4 resizeGripHover;
    ImVec4 resizeGripActive;
};

struct ThemeStyle {
    float windowRounding;
    float frameRounding;
    float popupRounding;
    float scrollbarRounding;
    float grabRounding;
    float tabRounding;
    float childRounding;

    ImVec2 windowPadding;
    ImVec2 framePadding;
    ImVec2 itemSpacing;
    ImVec2 itemInnerSpacing;
    ImVec2 scrollbarSize;
    ImVec2 grabMinSize;

    float indentSpacing;
    float columnsMinSpacing;
    float scrollbarSize_val;
    float frameBorderSize;
    float windowBorderSize;
    float childBorderSize;
    float popupBorderSize;
    float tabBorderSize;

    ImVec2 displayWindowPadding;
    ImVec2 displaySafeAreaPadding;
};

struct Theme {
    std::string name;
    ThemeColors colors;
    ThemeStyle style;
};

// =============================================
// 预设主题
// =============================================

namespace themes {

// 精美暗色主题 - 参考 Unreal Engine 风格
inline Theme createDarkTheme() {
    Theme theme;
    theme.name = "Spark Dark";

    // 调色板
    const ImVec4 bgDarkest   = ImVec4(0.06f, 0.06f, 0.08f, 1.00f);
    const ImVec4 bgDark      = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
    const ImVec4 bgMedium    = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
    const ImVec4 bgLight     = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
    const ImVec4 bgLighter   = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
    const ImVec4 bgLightest  = ImVec4(0.22f, 0.22f, 0.25f, 1.00f);

    const ImVec4 accent      = ImVec4(0.20f, 0.55f, 0.90f, 1.00f);  // 蓝色强调
    const ImVec4 accentLight = ImVec4(0.30f, 0.65f, 1.00f, 1.00f);
    const ImVec4 accentDark  = ImVec4(0.15f, 0.45f, 0.80f, 1.00f);

    const ImVec4 textMain    = ImVec4(0.90f, 0.90f, 0.92f, 1.00f);
    const ImVec4 textDim     = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);
    const ImVec4 textBright  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

    // 背景色
    theme.colors.background      = bgDarkest;
    theme.colors.panelBg         = bgDark;
    theme.colors.panelBgDark     = bgDarkest;
    theme.colors.panelBgHover    = bgMedium;
    theme.colors.childBg         = bgDark;
    theme.colors.popupBg         = ImVec4(0.10f, 0.10f, 0.12f, 0.98f);

    // 边框色
    theme.colors.border          = ImVec4(0.20f, 0.20f, 0.23f, 0.60f);
    theme.colors.borderLight     = ImVec4(0.30f, 0.30f, 0.35f, 0.80f);

    // 文本色
    theme.colors.text            = textMain;
    theme.colors.textDim         = textDim;
    theme.colors.textBright      = textBright;
    theme.colors.textDisabled    = ImVec4(0.40f, 0.40f, 0.45f, 0.60f);

    // 强调色
    theme.colors.accent          = accent;
    theme.colors.accentHover     = accentLight;
    theme.colors.accentActive    = accentDark;

    // 状态色
    theme.colors.success         = ImVec4(0.20f, 0.80f, 0.40f, 1.00f);
    theme.colors.warning         = ImVec4(0.95f, 0.70f, 0.20f, 1.00f);
    theme.colors.error           = ImVec4(0.90f, 0.25f, 0.25f, 1.00f);
    theme.colors.info            = ImVec4(0.40f, 0.70f, 0.95f, 1.00f);

    // 交互色
    theme.colors.button          = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
    theme.colors.buttonHover     = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);
    theme.colors.buttonActive    = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);

    // 标题栏
    theme.colors.titleBg         = bgDarkest;
    theme.colors.titleBgActive   = bgDark;
    theme.colors.titleBgCollapsed = bgDarkest;

    // 标签页
    theme.colors.tab             = bgMedium;
    theme.colors.tabHover        = bgLighter;
    theme.colors.tabActive       = bgLight;
    theme.colors.tabUnfocused    = bgDark;

    // 滚动条
    theme.colors.scrollbarBg     = bgDarkest;
    theme.colors.scrollbarGrab   = bgLighter;
    theme.colors.scrollbarGrabHover = bgLightest;
    theme.colors.scrollbarGrabActive = accent;

    // 选择
    theme.colors.selection       = ImVec4(0.20f, 0.45f, 0.75f, 0.40f);
    theme.colors.selectionActive = ImVec4(0.25f, 0.55f, 0.85f, 0.60f);

    // 分割线
    theme.colors.separator       = ImVec4(0.20f, 0.20f, 0.23f, 0.60f);
    theme.colors.separatorHover  = accent;

    // Resize Grip
    theme.colors.resizeGrip      = ImVec4(0.20f, 0.20f, 0.23f, 0.20f);
    theme.colors.resizeGripHover = ImVec4(0.20f, 0.55f, 0.90f, 0.60f);
    theme.colors.resizeGripActive = accent;

    // 样式
    theme.style.windowRounding     = 6.0f;
    theme.style.frameRounding      = 4.0f;
    theme.style.popupRounding      = 6.0f;
    theme.style.scrollbarRounding  = 6.0f;
    theme.style.grabRounding       = 4.0f;
    theme.style.tabRounding        = 4.0f;
    theme.style.childRounding      = 4.0f;

    theme.style.windowPadding      = ImVec2(10.0f, 10.0f);
    theme.style.framePadding       = ImVec2(8.0f, 4.0f);
    theme.style.itemSpacing        = ImVec2(8.0f, 6.0f);
    theme.style.itemInnerSpacing   = ImVec2(6.0f, 4.0f);
    theme.style.scrollbarSize      = ImVec2(12.0f, 12.0f);
    theme.style.grabMinSize        = 8.0f;

    theme.style.indentSpacing      = 20.0f;
    theme.style.columnsMinSpacing  = 6.0f;
    theme.style.scrollbarSize_val  = 12.0f;
    theme.style.frameBorderSize    = 0.0f;
    theme.style.windowBorderSize   = 1.0f;
    theme.style.childBorderSize    = 1.0f;
    theme.style.popupBorderSize    = 1.0f;
    theme.style.tabBorderSize      = 0.0f;

    theme.style.displayWindowPadding   = ImVec2(10.0f, 10.0f);
    theme.style.displaySafeAreaPadding = ImVec2(4.0f, 4.0f);

    return theme;
}

// 亮色主题
inline Theme createLightTheme() {
    Theme theme;
    theme.name = "Spark Light";

    const ImVec4 bgBase      = ImVec4(0.94f, 0.94f, 0.95f, 1.00f);
    const ImVec4 bgPanel     = ImVec4(0.96f, 0.96f, 0.97f, 1.00f);
    const ImVec4 bgDark      = ImVec4(0.88f, 0.88f, 0.90f, 1.00f);
    const ImVec4 accent      = ImVec4(0.15f, 0.45f, 0.80f, 1.00f);

    theme.colors.background      = bgBase;
    theme.colors.panelBg         = bgPanel;
    theme.colors.panelBgDark     = bgDark;
    theme.colors.panelBgHover    = ImVec4(0.90f, 0.90f, 0.92f, 1.00f);
    theme.colors.childBg         = bgPanel;
    theme.colors.popupBg         = ImVec4(0.97f, 0.97f, 0.98f, 0.98f);

    theme.colors.border          = ImVec4(0.80f, 0.80f, 0.82f, 0.60f);
    theme.colors.borderLight     = ImVec4(0.70f, 0.70f, 0.73f, 0.80f);

    theme.colors.text            = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
    theme.colors.textDim         = ImVec4(0.45f, 0.45f, 0.50f, 1.00f);
    theme.colors.textBright      = ImVec4(0.05f, 0.05f, 0.08f, 1.00f);
    theme.colors.textDisabled    = ImVec4(0.60f, 0.60f, 0.65f, 0.60f);

    theme.colors.accent          = accent;
    theme.colors.accentHover     = ImVec4(0.20f, 0.55f, 0.90f, 1.00f);
    theme.colors.accentActive    = ImVec4(0.10f, 0.35f, 0.70f, 1.00f);

    theme.colors.success         = ImVec4(0.15f, 0.65f, 0.30f, 1.00f);
    theme.colors.warning         = ImVec4(0.85f, 0.60f, 0.10f, 1.00f);
    theme.colors.error           = ImVec4(0.80f, 0.20f, 0.20f, 1.00f);
    theme.colors.info            = ImVec4(0.30f, 0.55f, 0.85f, 1.00f);

    theme.colors.button          = ImVec4(0.90f, 0.90f, 0.92f, 1.00f);
    theme.colors.buttonHover     = ImVec4(0.85f, 0.85f, 0.88f, 1.00f);
    theme.colors.buttonActive    = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);

    theme.colors.titleBg         = bgDark;
    theme.colors.titleBgActive   = bgPanel;
    theme.colors.titleBgCollapsed = bgDark;

    theme.colors.tab             = bgDark;
    theme.colors.tabHover        = bgPanel;
    theme.colors.tabActive       = bgBase;
    theme.colors.tabUnfocused    = bgDark;

    theme.colors.scrollbarBg     = bgBase;
    theme.colors.scrollbarGrab   = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    theme.colors.scrollbarGrabHover = ImVec4(0.75f, 0.75f, 0.78f, 1.00f);
    theme.colors.scrollbarGrabActive = accent;

    theme.colors.selection       = ImVec4(0.20f, 0.45f, 0.75f, 0.30f);
    theme.colors.selectionActive = ImVec4(0.25f, 0.55f, 0.85f, 0.50f);

    theme.colors.separator       = ImVec4(0.80f, 0.80f, 0.82f, 0.60f);
    theme.colors.separatorHover  = accent;

    theme.colors.resizeGrip      = ImVec4(0.80f, 0.80f, 0.82f, 0.20f);
    theme.colors.resizeGripHover = accent;
    theme.colors.resizeGripActive = accent;

    // 样式与暗色主题相同
    theme.style.windowRounding     = 6.0f;
    theme.style.frameRounding      = 4.0f;
    theme.style.popupRounding      = 6.0f;
    theme.style.scrollbarRounding  = 6.0f;
    theme.style.grabRounding       = 4.0f;
    theme.style.tabRounding        = 4.0f;
    theme.style.childRounding      = 4.0f;

    theme.style.windowPadding      = ImVec2(10.0f, 10.0f);
    theme.style.framePadding       = ImVec2(8.0f, 4.0f);
    theme.style.itemSpacing        = ImVec2(8.0f, 6.0f);
    theme.style.itemInnerSpacing   = ImVec2(6.0f, 4.0f);
    theme.style.scrollbarSize      = ImVec2(12.0f, 12.0f);
    theme.style.grabMinSize        = 8.0f;

    theme.style.indentSpacing      = 20.0f;
    theme.style.columnsMinSpacing  = 6.0f;
    theme.style.scrollbarSize_val  = 12.0f;
    theme.style.frameBorderSize    = 0.0f;
    theme.style.windowBorderSize   = 1.0f;
    theme.style.childBorderSize    = 1.0f;
    theme.style.popupBorderSize    = 1.0f;
    theme.style.tabBorderSize      = 0.0f;

    theme.style.displayWindowPadding   = ImVec2(10.0f, 10.0f);
    theme.style.displaySafeAreaPadding = ImVec2(4.0f, 4.0f);

    return theme;
}

} // namespace themes

// =============================================
// 主题应用函数
// =============================================

inline void applyTheme(const Theme& theme) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark();  // 先重置为基础

    // 应用颜色
    auto& colors = style.Colors;

    colors[ImGuiCol_WindowBg]              = theme.colors.panelBg;
    colors[ImGuiCol_ChildBg]               = theme.colors.childBg;
    colors[ImGuiCol_PopupBg]               = theme.colors.popupBg;
    colors[ImGuiCol_Border]                = theme.colors.border;
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    colors[ImGuiCol_FrameBg]               = theme.colors.panelBgDark;
    colors[ImGuiCol_FrameBgHovered]        = theme.colors.panelBgHover;
    colors[ImGuiCol_FrameBgActive]         = theme.colors.panelBgHover;

    colors[ImGuiCol_TitleBg]               = theme.colors.titleBg;
    colors[ImGuiCol_TitleBgActive]         = theme.colors.titleBgActive;
    colors[ImGuiCol_TitleBgCollapsed]      = theme.colors.titleBgCollapsed;

    colors[ImGuiCol_MenuBarBg]             = theme.colors.panelBgDark;

    colors[ImGuiCol_ScrollbarBg]           = theme.colors.scrollbarBg;
    colors[ImGuiCol_ScrollbarGrab]         = theme.colors.scrollbarGrab;
    colors[ImGuiCol_ScrollbarGrabHovered]  = theme.colors.scrollbarGrabHover;
    colors[ImGuiCol_ScrollbarGrabActive]   = theme.colors.scrollbarGrabActive;

    colors[ImGuiCol_CheckMark]             = theme.colors.accent;
    colors[ImGuiCol_SliderGrab]            = theme.colors.accent;
    colors[ImGuiCol_SliderGrabActive]      = theme.colors.accentHover;

    colors[ImGuiCol_Button]                = theme.colors.button;
    colors[ImGuiCol_ButtonHovered]         = theme.colors.buttonHover;
    colors[ImGuiCol_ButtonActive]          = theme.colors.buttonActive;

    colors[ImGuiCol_Header]                = theme.colors.panelBgHover;
    colors[ImGuiCol_HeaderHovered]         = theme.colors.panelBgHover;
    colors[ImGuiCol_HeaderActive]          = theme.colors.panelBgHover;

    colors[ImGuiCol_Separator]             = theme.colors.separator;
    colors[ImGuiCol_SeparatorHovered]      = theme.colors.separatorHover;
    colors[ImGuiCol_SeparatorActive]       = theme.colors.accent;

    colors[ImGuiCol_ResizeGrip]            = theme.colors.resizeGrip;
    colors[ImGuiCol_ResizeGripHovered]     = theme.colors.resizeGripHover;
    colors[ImGuiCol_ResizeGripActive]      = theme.colors.resizeGripActive;

    colors[ImGuiCol_Tab]                   = theme.colors.tab;
    colors[ImGuiCol_TabHovered]            = theme.colors.tabHover;
    colors[ImGuiCol_TabSelected]           = theme.colors.tabActive;
    colors[ImGuiCol_TabSelectedOverline]   = theme.colors.accent;
    colors[ImGuiCol_TabDimmed]             = theme.colors.tabUnfocused;
    colors[ImGuiCol_TabDimmedSelected]     = theme.colors.tabActive;

    colors[ImGuiCol_DockingPreview]        = ImVec4(theme.colors.accent.x, theme.colors.accent.y, theme.colors.accent.z, 0.40f);
    colors[ImGuiCol_DockingEmptyBg]        = theme.colors.panelBgDark;

    colors[ImGuiCol_Text]                  = theme.colors.text;
    colors[ImGuiCol_TextDisabled]          = theme.colors.textDisabled;

    colors[ImGuiCol_TableHeaderBg]         = theme.colors.panelBgDark;
    colors[ImGuiCol_TableBorderStrong]     = theme.colors.borderLight;
    colors[ImGuiCol_TableBorderLight]      = theme.colors.border;
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);

    colors[ImGuiCol_TextLink]              = theme.colors.accent;

    // 应用样式
    style.WindowRounding     = theme.style.windowRounding;
    style.FrameRounding      = theme.style.frameRounding;
    style.PopupRounding      = theme.style.popupRounding;
    style.ScrollbarRounding  = theme.style.scrollbarRounding;
    style.GrabRounding       = theme.style.grabRounding;
    style.TabRounding        = theme.style.tabRounding;
    style.ChildRounding      = theme.style.childRounding;

    style.WindowPadding      = theme.style.windowPadding;
    style.FramePadding       = theme.style.framePadding;
    style.ItemSpacing        = theme.style.itemSpacing;
    style.ItemInnerSpacing   = theme.style.itemInnerSpacing;
    style.ScrollbarSize      = theme.style.scrollbarSize.x;
    style.GrabMinSize        = theme.style.grabMinSize;

    style.IndentSpacing      = theme.style.indentSpacing;
    style.ColumnsMinSpacing  = theme.style.columnsMinSpacing;
    style.FrameBorderSize    = theme.style.frameBorderSize;
    style.WindowBorderSize   = theme.style.windowBorderSize;
    style.ChildBorderSize    = theme.style.childBorderSize;
    style.PopupBorderSize    = theme.style.popupBorderSize;
    style.TabBorderSize      = theme.style.tabBorderSize;

    style.DisplayWindowPadding   = theme.style.displayWindowPadding;
    style.DisplaySafeAreaPadding = theme.style.displaySafeAreaPadding;
}

} // namespace ui
} // namespace spark
