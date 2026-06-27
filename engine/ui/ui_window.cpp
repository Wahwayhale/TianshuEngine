#include "ui_window.h"
#include "ui_manager.h"
#include "core/input.h"
#include <algorithm>

namespace spark {

// ==================== UIWindow ====================

UIWindow::UIWindow(const std::string& title, const std::string& id)
    : UIWidget(id), m_title(title) {
    m_size = Vec2(400.0f, 300.0f);
}

void UIWindow::render() {
    if (!m_visible || !m_open) return;

    auto& theme = UITheme::get();
    auto& colors = theme.getColors();
    auto& sizes = theme.getSizes();

    // 渲染阴影
    // 渲染窗口背景
    // 渲染边框

    // 渲染标题栏
    renderTitleBar();

    // 渲染内容（如果未折叠）
    if (!m_collapsed) {
        renderContent();
    }

    // 渲染调整大小手柄
    if (m_resizable) {
        renderResizeHandle();
    }
}

void UIWindow::update(float deltaTime) {
    if (!m_visible || !m_open) return;

    auto& input = UIManager::get();
    Vec2 mousePos = input.getMousePosition();

    // 处理拖拽
    if (m_dragging) {
        if (input.isMouseReleased(0)) {
            m_dragging = false;
        } else {
            Vec2 delta = mousePos - m_dragOffset;
            m_position += delta;
            m_dragOffset = mousePos;
        }
    }

    // 处理调整大小
    if (m_resizing) {
        if (input.isMouseReleased(0)) {
            m_resizing = false;
        } else {
            Vec2 delta = mousePos - m_resizeStart;
            Vec2 newSize = m_resizeStartSize + delta;
            newSize = glm::max(newSize, m_minSize);
            newSize = glm::min(newSize, m_maxSize);
            m_size = newSize;
        }
    }

    // 更新子组件
    for (auto& widget : m_contentWidgets) {
        widget->update(deltaTime);
    }
}

void UIWindow::close() {
    m_open = false;
    if (m_onClose) {
        m_onClose();
    }
}

void UIWindow::addWidget(std::shared_ptr<UIWidget> widget) {
    if (widget) {
        m_contentWidgets.push_back(widget);
    }
}

void UIWindow::removeWidget(const std::string& id) {
    auto it = std::remove_if(m_contentWidgets.begin(), m_contentWidgets.end(),
        [&id](const auto& widget) { return widget->m_id == id; });
    m_contentWidgets.erase(it, m_contentWidgets.end());
}

void UIWindow::clearWidgets() {
    m_contentWidgets.clear();
}

void UIWindow::renderTitleBar() {
    auto& theme = UITheme::get();
    auto& colors = theme.getColors();
    auto& sizes = theme.getSizes();

    // 标题栏背景
    Color titleBg = m_focused ? colors.primary : colors.panelLight;

    // 渲染标题栏
    // 渲染标题文本
    // 渲染按钮（折叠、关闭）

    // 处理标题栏点击
    // ...
}

void UIWindow::renderContent() {
    // 渲染内容区域背景
    // 渲染子组件

    Vec2 contentPos = m_position + Vec2(m_padding, m_padding + 32.0f); // 32 = 标题栏高度
    Vec2 contentSize = m_size - Vec2(m_padding * 2, m_padding * 2 + 32.0f);

    for (auto& widget : m_contentWidgets) {
        if (!widget->isVisible()) continue;

        // 设置组件位置
        widget->setPosition(contentPos);
        widget->render();

        // 更新下一个组件位置
        if (m_verticalLayout) {
            contentPos.y += widget->getSize().y + m_layoutSpacing;
        } else {
            contentPos.x += widget->getSize().x + m_layoutSpacing;
        }
    }
}

void UIWindow::renderResizeHandle() {
    auto& colors = UITheme::get().getColors();

    // 渲染右下角的调整大小手柄
    Vec2 handlePos = m_position + m_size - Vec2(16.0f, 16.0f);
    Vec2 handleSize = Vec2(12.0f, 12.0f);

    // 渲染三条斜线
}

void UIWindow::handleTitleBarClick(Vec2 mousePos) {
    // 检查是否点击了标题栏区域
    Vec2 titleBarPos = m_position;
    Vec2 titleBarSize = Vec2(m_size.x, 32.0f);

    if (mousePos.x >= titleBarPos.x && mousePos.x <= titleBarPos.x + titleBarSize.x &&
        mousePos.y >= titleBarPos.y && mousePos.y <= titleBarPos.y + titleBarSize.y) {

        // 检查是否点击了关闭按钮
        // 检查是否点击了折叠按钮
        // 否则开始拖拽
        if (m_draggable) {
            m_dragging = true;
            m_dragOffset = mousePos;
        }
    }
}

void UIWindow::handleTitleBarDrag(Vec2 mouseDelta) {
    if (m_dragging) {
        m_position += mouseDelta;
    }
}

void UIWindow::handleResize(Vec2 mouseDelta) {
    if (m_resizing) {
        Vec2 newSize = m_size + mouseDelta;
        newSize = glm::max(newSize, m_minSize);
        newSize = glm::min(newSize, m_maxSize);
        m_size = newSize;
    }
}

// ==================== UIDialog ====================

UIDialog::UIDialog(const std::string& title, const std::string& id)
    : UIWindow(title, id) {
    m_size = Vec2(350.0f, 200.0f);
    m_resizable = false;
    m_collapsible = false;
    m_closeable = true;
}

void UIDialog::render() {
    if (!m_visible || !m_open) return;

    // 渲染半透明背景遮罩
    // 渲染对话框窗口
    UIWindow::render();

    // 渲染消息
    // 渲染按钮
    renderButtons();
}

void UIDialog::setButtons(const std::vector<std::string>& buttons) {
    m_buttons = buttons;
}

void UIDialog::renderButtons() {
    // 根据对话框类型渲染不同的图标和按钮布局
    Vec2 buttonPos = m_position + Vec2(m_size.x - 100.0f, m_size.y - 50.0f);

    for (size_t i = 0; i < m_buttons.size(); i++) {
        // 渲染按钮
        buttonPos.x -= 80.0f;
    }
}

// ==================== UIMessageBox ====================

void UIMessageBox::showInfo(const std::string& title, const std::string& message) {
    auto dialog = std::make_shared<UIDialog>(title);
    dialog->setType(UIDialog::DialogType::Info);
    dialog->setMessage(message);
    dialog->setButtons({"确定"});
    UIManager::get().addWidget(dialog);
}

void UIMessageBox::showWarning(const std::string& title, const std::string& message) {
    auto dialog = std::make_shared<UIDialog>(title);
    dialog->setType(UIDialog::DialogType::Warning);
    dialog->setMessage(message);
    dialog->setButtons({"确定"});
    UIManager::get().addWidget(dialog);
}

void UIMessageBox::showError(const std::string& title, const std::string& message) {
    auto dialog = std::make_shared<UIDialog>(title);
    dialog->setType(UIDialog::DialogType::Error);
    dialog->setMessage(message);
    dialog->setButtons({"确定"});
    UIManager::get().addWidget(dialog);
}

void UIMessageBox::showConfirm(const std::string& title, const std::string& message,
                               std::function<void(bool)> callback) {
    auto dialog = std::make_shared<UIDialog>(title);
    dialog->setType(UIDialog::DialogType::Confirm);
    dialog->setMessage(message);
    dialog->setButtons({"确定", "取消"});
    dialog->setOnButton([callback](int button) {
        if (callback) callback(button == 0);
    });
    UIManager::get().addWidget(dialog);
}

void UIMessageBox::showInput(const std::string& title, const std::string& message,
                             std::function<void(const std::string&)> callback) {
    auto dialog = std::make_shared<UIDialog>(title);
    dialog->setType(UIDialog::DialogType::Input);
    dialog->setMessage(message);
    dialog->setButtons({"确定", "取消"});
    dialog->setOnButton([callback](int button) {
        if (button == 0 && callback) {
            // 获取输入值
            callback("");
        }
    });
    UIManager::get().addWidget(dialog);
}

} // namespace spark
