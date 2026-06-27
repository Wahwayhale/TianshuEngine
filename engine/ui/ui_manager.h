#pragma once

#include "ui_widgets.h"
#include "ui_theme.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace spark {

// UI 管理器
class UIManager {
public:
    static UIManager& get();

    // 初始化
    void initialize();
    void shutdown();

    // 更新和渲染
    void update(float deltaTime);
    void render();

    // 组件管理
    void addWidget(std::shared_ptr<UIWidget> widget);
    void removeWidget(const std::string& id);
    UIWidget* getWidget(const std::string& id);

    // 快捷创建方法
    std::shared_ptr<UILabel> createLabel(const std::string& text, const std::string& id = "");
    std::shared_ptr<UIButton> createButton(const std::string& text, const std::string& id = "");
    std::shared_ptr<UIInput> createInput(const std::string& placeholder = "", const std::string& id = "");
    std::shared_ptr<UICheckbox> createCheckbox(const std::string& text, bool checked = false, const std::string& id = "");
    std::shared_ptr<UISlider> createSlider(float min, float max, float value, const std::string& id = "");
    std::shared_ptr<UIProgressBar> createProgressBar(float value = 0.0f, const std::string& id = "");
    std::shared_ptr<UIComboBox> createComboBox(const std::string& id = "");
    std::shared_ptr<UIList> createList(const std::string& id = "");
    std::shared_ptr<UIPanel> createPanel(const std::string& title, const std::string& id = "");
    std::shared_ptr<UIGroup> createGroup(const std::string& id = "");
    std::shared_ptr<UISeparator> createSeparator(const std::string& id = "");

    // 布局管理
    void setLayoutHorizontal(bool horizontal) { m_horizontalLayout = horizontal; }
    void setLayoutSpacing(float spacing) { m_layoutSpacing = spacing; }
    void setLayoutPadding(float padding) { m_layoutPadding = padding; }

    // 焦点管理
    void setFocus(UIWidget* widget);
    void clearFocus();
    UIWidget* getFocused() const { return m_focusedWidget; }

    // 鼠标状态
    Vec2 getMousePosition() const { return m_mousePosition; }
    bool isMousePressed(int button) const;
    bool isMouseReleased(int button) const;

    // 主题
    UITheme& getTheme() { return UITheme::get(); }

private:
    UIManager();

    std::vector<std::shared_ptr<UIWidget>> m_widgets;
    std::unordered_map<std::string, size_t> m_widgetMap;

    UIWidget* m_focusedWidget = nullptr;
    UIWidget* m_hoveredWidget = nullptr;

    Vec2 m_mousePosition = Vec2(0.0f);
    bool m_mouseButtons[3] = {false, false, false};
    bool m_prevMouseButtons[3] = {false, false, false};

    bool m_horizontalLayout = false;
    float m_layoutSpacing = 4.0f;
    float m_layoutPadding = 8.0f;

    bool m_initialized = false;
};

// 便捷宏
#define UI_LABEL(text) UIManager::get().createLabel(text)
#define UI_BUTTON(text) UIManager::get().createButton(text)
#define UI_INPUT(placeholder) UIManager::get().createInput(placeholder)
#define UI_CHECKBOX(text, checked) UIManager::get().createCheckbox(text, checked)
#define UI_SLIDER(min, max, value) UIManager::get().createSlider(min, max, value)
#define UI_PROGRESSBAR(value) UIManager::get().createProgressBar(value)
#define UI_COMBOBOX() UIManager::get().createComboBox()
#define UI_LIST() UIManager::get().createList()
#define UI_PANEL(title) UIManager::get().createPanel(title)
#define UI_GROUP() UIManager::get().createGroup()
#define UI_SEPARATOR() UIManager::get().createSeparator()

} // namespace spark
