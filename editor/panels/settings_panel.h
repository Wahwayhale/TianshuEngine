#pragma once

namespace spark {

class UISystem;

class SettingsPanel {
public:
    SettingsPanel();
    ~SettingsPanel();

    void render();
    void setUISystem(UISystem* uiSystem) { m_uiSystem = uiSystem; }

private:
    void renderGeneral();
    void renderGraphics();
    void renderEditor();
    void renderShortcuts();

    UISystem* m_uiSystem = nullptr;

    // 设置状态
    bool m_vsync = true;
    int m_msaaSamples = 1;
    float m_fontSize = 16.0f;
    bool m_darkTheme = true;
    float m_mouseSensitivity = 0.1f;
    bool m_showGrid = true;
    bool m_showGizmos = true;
};

} // namespace spark
