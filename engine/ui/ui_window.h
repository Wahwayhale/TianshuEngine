#pragma once

#include "ui_widgets.h"
#include "ui_theme.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace spark {

// UI 窗口
class UIWindow : public UIWidget {
public:
    UIWindow(const std::string& title = "窗口", const std::string& id = "");
    ~UIWindow() override = default;

    void render() override;
    void update(float deltaTime) override;

    // 窗口属性
    void setTitle(const std::string& title) { m_title = title; }
    void setPosition(Vec2 pos) { m_position = pos; }
    void setSize(Vec2 size) { m_size = size; }
    void setMinSize(Vec2 size) { m_minSize = size; }
    void setMaxSize(Vec2 size) { m_maxSize = size; }

    // 窗口行为
    void setDraggable(bool draggable) { m_draggable = draggable; }
    void setResizable(bool resizable) { m_resizable = resizable; }
    void setMovable(bool movable) { m_movable = movable; }
    void setCloseable(bool closeable) { m_closeable = closeable; }
    void setCollapsible(bool collapsible) { m_collapsible = collapsible; }
    void setAlwaysOnTop(bool onTop) { m_alwaysOnTop = onTop; }

    // 窗口状态
    void setCollapsed(bool collapsed) { m_collapsed = collapsed; }
    void setFocused(bool focused) { m_focused = focused; }
    void close();

    bool isCollapsed() const { return m_collapsed; }
    bool isFocused() const { return m_focused; }
    bool isOpen() const { return m_open; }

    // 回调
    void setOnClose(std::function<void()> callback) { m_onClose = callback; }
    void setOnCollapse(std::function<void(bool)> callback) { m_onCollapse = callback; }
    void setOnFocus(std::function<void(bool)> callback) { m_onFocus = callback; }

    // 内容区域
    void addWidget(std::shared_ptr<UIWidget> widget);
    void removeWidget(const std::string& id);
    void clearWidgets();

    // 布局
    void setLayoutVertical(bool vertical) { m_verticalLayout = vertical; }
    void setLayoutSpacing(float spacing) { m_layoutSpacing = spacing; }
    void setPadding(float padding) { m_padding = padding; }

private:
    void renderTitleBar();
    void renderContent();
    void renderResizeHandle();

    // 标题栏交互
    void handleTitleBarClick(Vec2 mousePos);
    void handleTitleBarDrag(Vec2 mouseDelta);

    // 调整大小交互
    void handleResize(Vec2 mouseDelta);

    std::string m_title;
    Vec2 m_minSize = Vec2(200.0f, 100.0f);
    Vec2 m_maxSize = Vec2(2000.0f, 2000.0f);

    // 窗口行为
    bool m_draggable = true;
    bool m_resizable = true;
    bool m_movable = true;
    bool m_closeable = true;
    bool m_collapsible = true;
    bool m_alwaysOnTop = false;

    // 窗口状态
    bool m_collapsed = false;
    bool m_open = true;
    bool m_dragging = false;
    bool m_resizing = false;
    Vec2 m_dragOffset = Vec2(0.0f);
    Vec2 m_resizeStart = Vec2(0.0f);
    Vec2 m_resizeStartSize = Vec2(0.0f);

    // 布局
    bool m_verticalLayout = true;
    float m_layoutSpacing = 4.0f;
    float m_padding = 8.0f;

    // 内容组件
    std::vector<std::shared_ptr<UIWidget>> m_contentWidgets;

    // 回调
    std::function<void()> m_onClose;
    std::function<void(bool)> m_onCollapse;
    std::function<void(bool)> m_onFocus;
};

// 对话框
class UIDialog : public UIWindow {
public:
    UIDialog(const std::string& title = "对话框", const std::string& id = "");
    ~UIDialog() override = default;

    void render() override;

    // 对话框类型
    enum class DialogType {
        Info,
        Warning,
        Error,
        Confirm,
        Input
    };

    void setType(DialogType type) { m_type = type; }
    void setMessage(const std::string& message) { m_message = message; }

    // 按钮
    void setButtons(const std::vector<std::string>& buttons);
    void setOnButton(std::function<void(int)> callback) { m_onButton = callback; }

private:
    void renderButtons();

    DialogType m_type = DialogType::Info;
    std::string m_message;
    std::vector<std::string> m_buttons = {"确定", "取消"};
    std::function<void(int)> m_onButton;
};

// 消息框（静态方法）
class UIMessageBox {
public:
    static void showInfo(const std::string& title, const std::string& message);
    static void showWarning(const std::string& title, const std::string& message);
    static void showError(const std::string& title, const std::string& message);
    static void showConfirm(const std::string& title, const std::string& message,
                           std::function<void(bool)> callback);
    static void showInput(const std::string& title, const std::string& message,
                         std::function<void(const std::string&)> callback);
};

} // namespace spark
