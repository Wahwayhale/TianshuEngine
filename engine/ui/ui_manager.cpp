#include "ui_manager.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

UIManager& UIManager::get() {
    static UIManager instance;
    return instance;
}

UIManager::UIManager() = default;

void UIManager::initialize() {
    if (m_initialized) return;

    SPARK_CORE_INFO("UI 管理器初始化");
    m_initialized = true;
}

void UIManager::shutdown() {
    if (!m_initialized) return;

    m_widgets.clear();
    m_widgetMap.clear();
    m_focusedWidget = nullptr;
    m_hoveredWidget = nullptr;

    SPARK_CORE_INFO("UI 管理器关闭");
    m_initialized = false;
}

void UIManager::update(float deltaTime) {
    if (!m_initialized) return;

    // 更新鼠标状态
    // ...

    // 更新所有组件
    for (auto& widget : m_widgets) {
        widget->update(deltaTime);
    }
}

void UIManager::render() {
    if (!m_initialized) return;

    // 渲染所有组件
    for (auto& widget : m_widgets) {
        widget->render();
    }
}

void UIManager::addWidget(std::shared_ptr<UIWidget> widget) {
    if (!widget) return;

    const std::string& id = widget->m_id;
    if (id.empty()) {
        // 自动生成ID
        static int counter = 0;
        widget->m_id = "__auto_" + std::to_string(counter++);
    }

    m_widgetMap[widget->m_id] = m_widgets.size();
    m_widgets.push_back(widget);
}

void UIManager::removeWidget(const std::string& id) {
    auto it = m_widgetMap.find(id);
    if (it == m_widgetMap.end()) return;

    size_t index = it->second;
    m_widgets.erase(m_widgets.begin() + index);

    // 重建索引
    m_widgetMap.clear();
    for (size_t i = 0; i < m_widgets.size(); i++) {
        m_widgetMap[m_widgets[i]->m_id] = i;
    }
}

UIWidget* UIManager::getWidget(const std::string& id) {
    auto it = m_widgetMap.find(id);
    if (it == m_widgetMap.end()) return nullptr;
    return m_widgets[it->second].get();
}

std::shared_ptr<UILabel> UIManager::createLabel(const std::string& text, const std::string& id) {
    auto widget = std::make_shared<UILabel>(text, id);
    addWidget(widget);
    return widget;
}

std::shared_ptr<UIButton> UIManager::createButton(const std::string& text, const std::string& id) {
    auto widget = std::make_shared<UIButton>(text, id);
    addWidget(widget);
    return widget;
}

std::shared_ptr<UIInput> UIManager::createInput(const std::string& placeholder, const std::string& id) {
    auto widget = std::make_shared<UIInput>(placeholder, id);
    addWidget(widget);
    return widget;
}

std::shared_ptr<UICheckbox> UIManager::createCheckbox(const std::string& text, bool checked, const std::string& id) {
    auto widget = std::make_shared<UICheckbox>(text, checked, id);
    addWidget(widget);
    return widget;
}

std::shared_ptr<UISlider> UIManager::createSlider(float min, float max, float value, const std::string& id) {
    auto widget = std::make_shared<UISlider>(min, max, value, id);
    addWidget(widget);
    return widget;
}

std::shared_ptr<UIProgressBar> UIManager::createProgressBar(float value, const std::string& id) {
    auto widget = std::make_shared<UIProgressBar>(value, id);
    addWidget(widget);
    return widget;
}

std::shared_ptr<UIComboBox> UIManager::createComboBox(const std::string& id) {
    auto widget = std::make_shared<UIComboBox>(id);
    addWidget(widget);
    return widget;
}

std::shared_ptr<UIList> UIManager::createList(const std::string& id) {
    auto widget = std::make_shared<UIList>(id);
    addWidget(widget);
    return widget;
}

std::shared_ptr<UIPanel> UIManager::createPanel(const std::string& title, const std::string& id) {
    auto widget = std::make_shared<UIPanel>(title, id);
    addWidget(widget);
    return widget;
}

std::shared_ptr<UIGroup> UIManager::createGroup(const std::string& id) {
    auto widget = std::make_shared<UIGroup>(id);
    addWidget(widget);
    return widget;
}

std::shared_ptr<UISeparator> UIManager::createSeparator(const std::string& id) {
    auto widget = std::make_shared<UISeparator>(id);
    addWidget(widget);
    return widget;
}

void UIManager::setFocus(UIWidget* widget) {
    if (m_focusedWidget) {
        m_focusedWidget->m_focused = false;
    }
    m_focusedWidget = widget;
    if (m_focusedWidget) {
        m_focusedWidget->m_focused = true;
    }
}

void UIManager::clearFocus() {
    if (m_focusedWidget) {
        m_focusedWidget->m_focused = false;
        m_focusedWidget = nullptr;
    }
}

bool UIManager::isMousePressed(int button) const {
    if (button < 0 || button >= 3) return false;
    return m_mouseButtons[button] && !m_prevMouseButtons[button];
}

bool UIManager::isMouseReleased(int button) const {
    if (button < 0 || button >= 3) return false;
    return !m_mouseButtons[button] && m_prevMouseButtons[button];
}

} // namespace spark
