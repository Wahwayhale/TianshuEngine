#include "ui_widgets.h"
#include <algorithm>
#include <cmath>

namespace spark {

// ==================== UIWidget 基础类 ====================

UIWidget::UIWidget(const std::string& id)
    : m_id(id) {
}

void UIWidget::update(float deltaTime) {
    for (auto& child : m_children) {
        child->update(deltaTime);
    }
}

void UIWidget::addChild(std::shared_ptr<UIWidget> child) {
    child->m_parent = this;
    m_children.push_back(child);
}

void UIWidget::removeChild(const std::string& id) {
    auto it = std::remove_if(m_children.begin(), m_children.end(),
        [&id](const auto& child) { return child->m_id == id; });
    m_children.erase(it, m_children.end());
}

UIWidget* UIWidget::getChild(const std::string& id) {
    for (auto& child : m_children) {
        if (child->m_id == id) return child.get();
        auto* found = child->getChild(id);
        if (found) return found;
    }
    return nullptr;
}

// ==================== UILabel 文本标签 ====================

UILabel::UILabel(const std::string& text, const std::string& id)
    : UIWidget(id), m_text(text) {
    m_size = Vec2(100.0f, 20.0f);
}

void UILabel::render() {
    if (!m_visible) return;
    // ImGui 渲染将在集成时实现
}

// ==================== UIButton 按钮 ====================

UIButton::UIButton(const std::string& text, const std::string& id)
    : UIWidget(id), m_text(text) {
    m_size = Vec2(120.0f, 32.0f);
}

void UIButton::render() {
    if (!m_visible) return;

    auto& theme = UITheme::get();
    auto& colors = theme.getColors();

    // 根据样式选择颜色
    Color bgColor;
    Color textColor = colors.text;

    switch (m_style) {
        case ButtonStyle::Primary:
            bgColor = m_pressed ? colors.primaryActive :
                      m_hovered ? colors.primaryHover : colors.primary;
            break;
        case ButtonStyle::Secondary:
            bgColor = m_pressed ? colors.backgroundActive :
                      m_hovered ? colors.backgroundHover : colors.backgroundLight;
            break;
        case ButtonStyle::Success:
            bgColor = m_pressed ? colors.success.darken(0.1f) :
                      m_hovered ? colors.success.lighten(0.1f) : colors.success;
            break;
        case ButtonStyle::Warning:
            bgColor = m_pressed ? colors.warning.darken(0.1f) :
                      m_hovered ? colors.warning.lighten(0.1f) : colors.warning;
            textColor = colors.textInverse;
            break;
        case ButtonStyle::Danger:
            bgColor = m_pressed ? colors.error.darken(0.1f) :
                      m_hovered ? colors.error.lighten(0.1f) : colors.error;
            break;
        case ButtonStyle::Ghost:
            bgColor = m_pressed ? colors.backgroundActive :
                      m_hovered ? colors.backgroundHover : Color(0, 0, 0, 0);
            break;
    }

    // 渲染按钮背景（带圆角）
    // 绘制文本
    // 处理点击事件
}

// ==================== UIInput 输入框 ====================

UIInput::UIInput(const std::string& placeholder, const std::string& id)
    : UIWidget(id), m_placeholder(placeholder) {
    m_size = Vec2(200.0f, 32.0f);
}

void UIInput::render() {
    if (!m_visible) return;

    auto& theme = UITheme::get();
    auto& colors = theme.getColors();

    // 输入框背景
    Color bgColor = m_focused ? colors.backgroundLight : colors.background;
    Color borderColor = m_focused ? colors.borderFocus : colors.border;

    // 渲染背景和边框
    // 渲染文本或占位符
    // 处理光标
    // 处理输入事件
}

// ==================== UICheckbox 复选框 ====================

UICheckbox::UICheckbox(const std::string& text, bool checked, const std::string& id)
    : UIWidget(id), m_text(text), m_checked(checked) {
    m_size = Vec2(200.0f, 24.0f);
}

void UICheckbox::render() {
    if (!m_visible) return;

    auto& theme = UITheme::get();
    auto& colors = theme.getColors();

    // 复选框样式
    Color boxColor = m_checked ? colors.primary : colors.backgroundLight;
    Color borderColor = m_hovered ? colors.borderFocus : colors.border;

    // 渲染复选框
    // 渲染勾选标记
    // 渲染文本
    // 处理点击事件
}

// ==================== UISlider 滑块 ====================

UISlider::UISlider(float min, float max, float value, const std::string& id)
    : UIWidget(id), m_min(min), m_max(max), m_value(value) {
    m_size = Vec2(200.0f, 24.0f);
}

void UISlider::render() {
    if (!m_visible) return;

    auto& theme = UITheme::get();
    auto& colors = theme.getColors();

    float normalizedValue = (m_value - m_min) / (m_max - m_min);

    // 渲染轨道背景
    // 渲染填充部分
    // 渲染滑块手柄
    // 渲染数值文本
    // 处理拖拽事件
}

// ==================== UIProgressBar 进度条 ====================

UIProgressBar::UIProgressBar(float value, const std::string& id)
    : UIWidget(id), m_value(value) {
    m_size = Vec2(200.0f, 20.0f);
}

void UIProgressBar::render() {
    if (!m_visible) return;

    auto& theme = UITheme::get();
    auto& colors = theme.getColors();

    // 渲染背景
    // 渲染进度填充
    // 渲染百分比文本
}

// ==================== UIComboBox 下拉框 ====================

UIComboBox::UIComboBox(const std::string& id)
    : UIWidget(id) {
    m_size = Vec2(200.0f, 32.0f);
}

void UIComboBox::render() {
    if (!m_visible) return;

    auto& theme = UITheme::get();
    auto& colors = theme.getColors();

    // 渲染主框体
    // 渲染选中文本
    // 渲染下拉箭头
    // 渲染下拉列表（如果展开）
    // 处理选择事件
}

void UIComboBox::addItem(const std::string& item) {
    m_items.push_back(item);
    if (m_selectedIndex < 0) m_selectedIndex = 0;
}

void UIComboBox::removeItem(int index) {
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        m_items.erase(m_items.begin() + index);
        if (m_selectedIndex >= static_cast<int>(m_items.size())) {
            m_selectedIndex = static_cast<int>(m_items.size()) - 1;
        }
    }
}

void UIComboBox::setSelectedIndex(int index) {
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        m_selectedIndex = index;
        if (m_onChange) m_onChange(m_selectedIndex);
    }
}

std::string UIComboBox::getSelectedItem() const {
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_items.size())) {
        return m_items[m_selectedIndex];
    }
    return "";
}

// ==================== UIList 列表 ====================

UIList::UIList(const std::string& id)
    : UIWidget(id) {
    m_size = Vec2(200.0f, 150.0f);
}

void UIList::render() {
    if (!m_visible) return;

    auto& theme = UITheme::get();
    auto& colors = theme.getColors();

    // 渲染列表背景
    // 渲染列表项
    // 处理滚动
    // 处理选择事件
}

void UIList::addItem(const std::string& item) {
    m_items.push_back(item);
}

void UIList::removeItem(int index) {
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        m_items.erase(m_items.begin() + index);
        if (m_selectedIndex >= static_cast<int>(m_items.size())) {
            m_selectedIndex = static_cast<int>(m_items.size()) - 1;
        }
    }
}

void UIList::clearItems() {
    m_items.clear();
    m_selectedIndex = -1;
    m_scrollY = 0.0f;
}

void UIList::setSelectedIndex(int index) {
    if (index >= 0 && index < static_cast<int>(m_items.size())) {
        m_selectedIndex = index;
        if (m_onChange) m_onChange(m_selectedIndex);
    }
}

std::string UIList::getSelectedItem() const {
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_items.size())) {
        return m_items[m_selectedIndex];
    }
    return "";
}

// ==================== UIPanel 面板 ====================

UIPanel::UIPanel(const std::string& title, const std::string& id)
    : UIWidget(id), m_title(title) {
    m_size = Vec2(300.0f, 200.0f);
}

void UIPanel::render() {
    if (!m_visible) return;

    auto& theme = UITheme::get();
    auto& colors = theme.getColors();
    auto& sizes = theme.getSizes();

    // 渲染面板背景
    // 渲染标题栏
    // 渲染折叠按钮（如果可折叠）
    // 渲染子组件
}

// ==================== UIGroup 分组 ====================

UIGroup::UIGroup(const std::string& id)
    : UIWidget(id) {
}

void UIGroup::render() {
    if (!m_visible) return;

    // 按照水平或垂直方向排列子组件
    Vec2 currentPos = m_position;

    for (auto& child : m_children) {
        if (!child->isVisible()) continue;

        child->setPosition(currentPos);
        child->render();

        if (m_horizontal) {
            currentPos.x += child->getSize().x + m_spacing;
        } else {
            currentPos.y += child->getSize().y + m_spacing;
        }
    }
}

// ==================== UISeparator 分割线 ====================

UISeparator::UISeparator(const std::string& id)
    : UIWidget(id) {
    m_size = Vec2(200.0f, 2.0f);
}

void UISeparator::render() {
    if (!m_visible) return;

    auto& colors = UITheme::get().getColors();
    // 渲染水平分割线
}

// ==================== UITooltip 工具提示 ====================

UITooltip::UITooltip(const std::string& text, const std::string& id)
    : UIWidget(id), m_text(text) {
    m_size = Vec2(150.0f, 24.0f);
}

void UITooltip::render() {
    if (!m_visible || m_text.empty()) return;

    auto& colors = UITheme::get().getColors();

    // 渲染背景（带阴影）
    // 渲染文本
}

void UITooltip::show(Vec2 position) {
    m_position = position;
    m_visible = true;
}

void UITooltip::hide() {
    m_visible = false;
}

} // namespace spark
