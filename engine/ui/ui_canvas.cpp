#include "ui_canvas.h"
#include "core/log.h"
#include <imgui.h>

namespace spark {

// =============================================
// UICanvas
// =============================================

UICanvas::UICanvas(const std::string& name) : m_name(name) {}
UICanvas::~UICanvas() = default;

std::shared_ptr<UIWidget> UICanvas::addWidget(UIWidgetType type, const std::string& id) {
    auto widget = std::make_shared<UIWidget>();
    widget->id = id.empty() ? "widget_" + std::to_string(m_widgets.size()) : id;
    widget->type = type;

    // 设置默认属性
    switch (type) {
        case UIWidgetType::Panel:
            widget->name = "Panel";
            widget->size = Vec2(200, 150);
            widget->backgroundColor = Vec4(0.1f, 0.1f, 0.15f, 0.9f);
            break;
        case UIWidgetType::Button:
            widget->name = "Button";
            widget->size = Vec2(120, 40);
            widget->color = Vec4(0.2f, 0.5f, 0.8f, 1.0f);
            widget->text = "Button";
            break;
        case UIWidgetType::Text:
            widget->name = "Text";
            widget->size = Vec2(100, 30);
            widget->color = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
            widget->text = "Text";
            break;
        case UIWidgetType::Image:
            widget->name = "Image";
            widget->size = Vec2(100, 100);
            widget->color = Vec4(1.0f);
            break;
        case UIWidgetType::Input:
            widget->name = "Input";
            widget->size = Vec2(200, 30);
            widget->backgroundColor = Vec4(0.1f, 0.1f, 0.1f, 1.0f);
            break;
        default:
            widget->name = "Widget";
            widget->size = Vec2(100, 50);
            break;
    }

    m_widgets.push_back(widget);
    SPARK_CORE_INFO("UI widget added: {0}", widget->id);
    return widget;
}

void UICanvas::removeWidget(const std::string& id) {
    m_widgets.erase(
        std::remove_if(m_widgets.begin(), m_widgets.end(),
            [&](const std::shared_ptr<UIWidget>& widget) {
                return widget->id == id;
            }),
        m_widgets.end()
    );
}

std::shared_ptr<UIWidget> UICanvas::getWidget(const std::string& id) const {
    for (const auto& widget : m_widgets) {
        if (widget->id == id) {
            return widget;
        }
    }
    return nullptr;
}

void UICanvas::render() {
    if (!m_visible) return;

    // TODO: 实现 UI 渲染
    // 需要创建 UI 渲染管线
}

// =============================================
// UIManager
// =============================================

UIManager& UIManager::get() {
    static UIManager instance;
    return instance;
}

std::shared_ptr<UICanvas> UIManager::createCanvas(const std::string& name) {
    auto canvas = std::make_shared<UICanvas>(name);
    m_canvases[name] = canvas;
    SPARK_CORE_INFO("UI canvas created: {0}", name);
    return canvas;
}

std::shared_ptr<UICanvas> UIManager::getCanvas(const std::string& name) const {
    auto it = m_canvases.find(name);
    if (it != m_canvases.end()) {
        return it->second;
    }
    return nullptr;
}

void UIManager::removeCanvas(const std::string& name) {
    m_canvases.erase(name);
}

void UIManager::renderAll() {
    for (auto& [name, canvas] : m_canvases) {
        canvas->render();
    }
}

} // namespace spark
