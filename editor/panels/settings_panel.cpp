#include "settings_panel.h"
#include "ui/ui_system.h"
#include "ui/widgets.h"
#include <imgui.h>

namespace spark {

SettingsPanel::SettingsPanel() = default;
SettingsPanel::~SettingsPanel() = default;

void SettingsPanel::render() {
    ImGui::Begin("Settings");

    // 标签页
    if (ImGui::BeginTabBar("SettingsTabs")) {
        if (ImGui::BeginTabItem("General")) {
            renderGeneral();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Graphics")) {
            renderGraphics();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Editor")) {
            renderEditor();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Shortcuts")) {
            renderShortcuts();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void SettingsPanel::renderGeneral() {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

    // 主题设置
    ui::drawSeparator("Theme");

    bool isDark = m_darkTheme;
    if (ImGui::Checkbox("Dark Theme", &m_darkTheme)) {
        if (m_uiSystem) {
            if (m_darkTheme) {
                m_uiSystem->applyDarkTheme();
            } else {
                m_uiSystem->applyLightTheme();
            }
        }
    }

    ImGui::Spacing();

    // 字体设置
    ui::drawSeparator("Font");

    ImGui::Text("Font Size");
    ImGui::SameLine();
    if (ImGui::SliderFloat("##FontSize", &m_fontSize, 10.0f, 24.0f, "%.0f px")) {
        // TODO: 应用字体大小
    }

    ImGui::Spacing();

    // 鼠标设置
    ui::drawSeparator("Input");

    ImGui::Text("Mouse Sensitivity");
    ImGui::SameLine();
    ImGui::SliderFloat("##MouseSensitivity", &m_mouseSensitivity, 0.01f, 1.0f, "%.2f");

    ImGui::PopStyleVar(2);
}

void SettingsPanel::renderGraphics() {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

    // 显示设置
    ui::drawSeparator("Display");

    ImGui::Checkbox("VSync", &m_vsync);

    ImGui::Spacing();

    // 抗锯齿
    ui::drawSeparator("Anti-Aliasing");

    const char* msaaOptions[] = {"None (1x)", "2x", "4x", "8x"};
    int msaaIndex = 0;
    if (m_msaaSamples == 2) msaaIndex = 1;
    else if (m_msaaSamples == 4) msaaIndex = 2;
    else if (m_msaaSamples == 8) msaaIndex = 3;

    ImGui::Text("MSAA Samples");
    ImGui::SameLine();
    if (ImGui::Combo("##MSAA", &msaaIndex, msaaOptions, 4)) {
        switch (msaaIndex) {
            case 0: m_msaaSamples = 1; break;
            case 1: m_msaaSamples = 2; break;
            case 2: m_msaaSamples = 4; break;
            case 3: m_msaaSamples = 8; break;
        }
    }

    ImGui::Spacing();

    // 渲染设置
    ui::drawSeparator("Rendering");

    ImGui::Checkbox("Show Grid", &m_showGrid);
    ImGui::Checkbox("Show Gizmos", &m_showGizmos);

    ImGui::PopStyleVar(2);
}

void SettingsPanel::renderEditor() {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

    // 编辑器行为
    ui::drawSeparator("Behavior");

    static bool autoSave = true;
    ImGui::Checkbox("Auto Save", &autoSave);

    static float autoSaveInterval = 5.0f;
    if (autoSave) {
        ImGui::Text("Auto Save Interval");
        ImGui::SameLine();
        ImGui::SliderFloat("##AutoSaveInterval", &autoSaveInterval, 1.0f, 30.0f, "%.0f min");
    }

    ImGui::Spacing();

    // 界面设置
    ui::drawSeparator("Interface");

    static bool showToolbar = true;
    ImGui::Checkbox("Show Toolbar", &showToolbar);

    static bool showStatusBar = true;
    ImGui::Checkbox("Show Status Bar", &showStatusBar);

    static bool showConsole = true;
    ImGui::Checkbox("Show Console", &showConsole);

    ImGui::PopStyleVar(2);
}

void SettingsPanel::renderShortcuts() {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

    // 场景操作
    ui::drawSeparator("Scene");

    ui::drawProperty("Play/Pause", "Ctrl+P");
    ui::drawProperty("Stop", "Ctrl+.");
    ui::drawProperty("Save Scene", "Ctrl+S");
    ui::drawProperty("Load Scene", "Ctrl+O");

    ImGui::Spacing();

    // 编辑操作
    ui::drawSeparator("Edit");

    ui::drawProperty("Undo", "Ctrl+Z");
    ui::drawProperty("Redo", "Ctrl+Y");
    ui::drawProperty("Copy", "Ctrl+C");
    ui::drawProperty("Paste", "Ctrl+V");
    ui::drawProperty("Delete", "Delete");

    ImGui::Spacing();

    // 工具
    ui::drawSeparator("Tools");

    ui::drawProperty("Move Tool", "W");
    ui::drawProperty("Rotate Tool", "E");
    ui::drawProperty("Scale Tool", "R");
    ui::drawProperty("Toggle Grid", "G");

    ImGui::Spacing();

    // 视图
    ui::drawSeparator("View");

    ui::drawProperty("Focus Selected", "F");
    ui::drawProperty("Fullscreen", "F11");
    ui::drawProperty("Toggle Console", "`");

    ImGui::PopStyleVar(2);
}

} // namespace spark
