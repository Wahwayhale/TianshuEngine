#pragma once

#include "ui_theme.h"
#include "math/math_types.h"
#include <string>
#include <functional>
#include <vector>
#include <memory>

namespace spark {

// 基础组件
class UIWidget {
public:
    UIWidget(const std::string& id = "");
    virtual ~UIWidget() = default;

    virtual void render() = 0;
    virtual void update(float deltaTime);

    // 属性
    void setPosition(Vec2 pos) { m_position = pos; }
    void setSize(Vec2 size) { m_size = size; }
    void setVisible(bool visible) { m_visible = visible; }
    void setEnabled(bool enabled) { m_enabled = enabled; }
    void setTooltip(const std::string& tooltip) { m_tooltip = tooltip; }

    Vec2 getPosition() const { return m_position; }
    Vec2 getSize() const { return m_size; }
    bool isVisible() const { return m_visible; }
    bool isEnabled() const { return m_enabled; }
    bool isHovered() const { return m_hovered; }
    bool isFocused() const { return m_focused; }

    void addChild(std::shared_ptr<UIWidget> child);
    void removeChild(const std::string& id);
    UIWidget* getChild(const std::string& id);

protected:
    std::string m_id;
    Vec2 m_position = Vec2(0.0f);
    Vec2 m_size = Vec2(100.0f, 30.0f);
    bool m_visible = true;
    bool m_enabled = true;
    bool m_hovered = false;
    bool m_focused = false;
    std::string m_tooltip;

    std::vector<std::shared_ptr<UIWidget>> m_children;
    UIWidget* m_parent = nullptr;
};

// 文本标签
class UILabel : public UIWidget {
public:
    UILabel(const std::string& text = "", const std::string& id = "");

    void render() override;

    void setText(const std::string& text) { m_text = text; }
    void setFontSize(float size) { m_fontSize = size; }
    void setColor(Color color) { m_color = color; }
    void setAlignment(int align) { m_alignment = align; }  // 0=左, 1=中, 2=右

    const std::string& getText() const { return m_text; }

private:
    std::string m_text;
    float m_fontSize = 14.0f;
    Color m_color = UITheme::get().getColors().text;
    int m_alignment = 0;
};

// 按钮
class UIButton : public UIWidget {
public:
    UIButton(const std::string& text = "", const std::string& id = "");

    void render() override;

    void setText(const std::string& text) { m_text = text; }
    void setOnClick(std::function<void()> callback) { m_onClick = callback; }
    void setStyle(ButtonStyle style) { m_style = style; }
    void setIcon(const std::string& icon) { m_icon = icon; }

    enum class ButtonStyle {
        Primary,
        Secondary,
        Success,
        Warning,
        Danger,
        Ghost
    };

private:
    std::string m_text;
    std::string m_icon;
    std::function<void()> m_onClick;
    ButtonStyle m_style = ButtonStyle::Primary;
    bool m_pressed = false;
};

// 输入框
class UIInput : public UIWidget {
public:
    UIInput(const std::string& placeholder = "", const std::string& id = "");

    void render() override;

    void setValue(const std::string& value) { m_value = value; }
    void setPlaceholder(const std::string& placeholder) { m_placeholder = placeholder; }
    void setOnChange(std::function<void(const std::string&)> callback) { m_onChange = callback; }
    void setOnSubmit(std::function<void(const std::string&)> callback) { m_onSubmit = callback; }
    void setPassword(bool password) { m_password = password; }
    void setReadOnly(bool readOnly) { m_readOnly = readOnly; }
    void setMaxLength(int maxLen) { m_maxLength = maxLen; }

    const std::string& getValue() const { return m_value; }

private:
    std::string m_value;
    std::string m_placeholder;
    std::function<void(const std::string&)> m_onChange;
    std::function<void(const std::string&)> m_onSubmit;
    bool m_password = false;
    bool m_readOnly = false;
    int m_maxLength = 0;
    int m_cursorPos = 0;
};

// 复选框
class UICheckbox : public UIWidget {
public:
    UICheckbox(const std::string& text = "", bool checked = false, const std::string& id = "");

    void render() override;

    void setText(const std::string& text) { m_text = text; }
    void setChecked(bool checked) { m_checked = checked; }
    void setOnChange(std::function<void(bool)> callback) { m_onChange = callback; }

    bool isChecked() const { return m_checked; }

private:
    std::string m_text;
    bool m_checked = false;
    std::function<void(bool)> m_onChange;
};

// 单选按钮
class UIRadioButton : public UIWidget {
public:
    UIRadioButton(const std::string& text = "", const std::string& group = "", const std::string& id = "");

    void render() override;

    void setText(const std::string& text) { m_text = text; }
    void setSelected(bool selected) { m_selected = selected; }
    void setOnChange(std::function<void(bool)> callback) { m_onChange = callback; }

    bool isSelected() const { return m_selected; }

private:
    std::string m_text;
    std::string m_group;
    bool m_selected = false;
    std::function<void(bool)> m_onChange;
};

// 滑块
class UISlider : public UIWidget {
public:
    UISlider(float min = 0.0f, float max = 1.0f, float value = 0.0f, const std::string& id = "");

    void render() override;

    void setRange(float min, float max) { m_min = min; m_max = max; }
    void setValue(float value) { m_value = std::clamp(value, m_min, m_max); }
    void setStep(float step) { m_step = step; }
    void setShowValue(bool show) { m_showValue = show; }
    void setOnChange(std::function<void(float)> callback) { m_onChange = callback; }

    float getValue() const { return m_value; }

private:
    float m_min = 0.0f;
    float m_max = 1.0f;
    float m_value = 0.0f;
    float m_step = 0.01f;
    bool m_showValue = true;
    std::function<void(float)> m_onChange;
};

// 进度条
class UIProgressBar : public UIWidget {
public:
    UIProgressBar(float value = 0.0f, const std::string& id = "");

    void render() override;

    void setValue(float value) { m_value = std::clamp(value, 0.0f, 1.0f); }
    void setShowPercentage(bool show) { m_showPercentage = show; }
    void setColor(Color color) { m_color = color; }

    float getValue() const { return m_value; }

private:
    float m_value = 0.0f;
    bool m_showPercentage = true;
    Color m_color = UITheme::get().getColors().primary;
};

// 下拉框
class UIComboBox : public UIWidget {
public:
    UIComboBox(const std::string& id = "");

    void render() override;

    void addItem(const std::string& item);
    void removeItem(int index);
    void setSelectedIndex(int index);
    void setOnChange(std::function<void(int)> callback) { m_onChange = callback; }

    int getSelectedIndex() const { return m_selectedIndex; }
    std::string getSelectedItem() const;

private:
    std::vector<std::string> m_items;
    int m_selectedIndex = -1;
    bool m_open = false;
    std::function<void(int)> m_onChange;
};

// 列表
class UIList : public UIWidget {
public:
    UIList(const std::string& id = "");

    void render() override;

    void addItem(const std::string& item);
    void removeItem(int index);
    void clearItems();
    void setSelectedIndex(int index);
    void setOnChange(std::function<void(int)> callback) { m_onChange = callback; }

    int getSelectedIndex() const { return m_selectedIndex; }
    std::string getSelectedItem() const;

private:
    std::vector<std::string> m_items;
    int m_selectedIndex = -1;
    float m_scrollY = 0.0f;
    std::function<void(int)> m_onChange;
};

// 面板
class UIPanel : public UIWidget {
public:
    UIPanel(const std::string& title = "", const std::string& id = "");

    void render() override;

    void setTitle(const std::string& title) { m_title = title; }
    void setCollapsible(bool collapsible) { m_collapsible = collapsible; }
    void setCollapsed(bool collapsed) { m_collapsed = collapsed; }
    void setPadding(float padding) { m_padding = padding; }

private:
    std::string m_title;
    bool m_collapsible = true;
    bool m_collapsed = false;
    float m_padding = 8.0f;
};

// 分组
class UIGroup : public UIWidget {
public:
    UIGroup(const std::string& id = "");

    void render() override;

    void setHorizontal(bool horizontal) { m_horizontal = horizontal; }
    void setSpacing(float spacing) { m_spacing = spacing; }

private:
    bool m_horizontal = false;
    float m_spacing = 4.0f;
}

// 分割线
class UISeparator : public UIWidget {
public:
    UISeparator(const std::string& id = "");

    void render() override;
};

// 工具提示
class UITooltip : public UIWidget {
public:
    UITooltip(const std::string& text = "", const std::string& id = "");

    void render() override;

    void setText(const std::string& text) { m_text = text; }
    void show(Vec2 position);
    void hide();

private:
    std::string m_text;
    bool m_visible = false;
};

} // namespace spark
