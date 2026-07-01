#pragma once

#include "math/math_types.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace spark {

class Texture;

// UI 控件类型
enum class UIWidgetType {
    Panel,
    Button,
    Text,
    Image,
    Input,
    Checkbox,
    Slider,
    ProgressBar,
    Dropdown
};

// UI 控件
struct UIWidget {
    std::string id;
    std::string name;
    UIWidgetType type;

    Vec2 position;
    Vec2 size;
    Vec4 color = Vec4(1.0f);
    Vec4 backgroundColor = Vec4(0.1f, 0.1f, 0.1f, 0.9f);

    std::string text;
    float fontSize = 14.0f;
    Vec4 textColor = Vec4(1.0f);

    std::shared_ptr<Texture> texture;

    bool visible = true;
    bool enabled = true;
    bool interactive = true;

    // 回调
    std::function<void()> onClick;
    std::function<void(const std::string&)> onTextChanged;
    std::function<void(float)> onValueChanged;
};

// UI 画布
class UICanvas {
public:
    UICanvas(const std::string& name = "Canvas");
    ~UICanvas();

    // 控件管理
    std::shared_ptr<UIWidget> addWidget(UIWidgetType type, const std::string& id = "");
    void removeWidget(const std::string& id);
    std::shared_ptr<UIWidget> getWidget(const std::string& id) const;

    // 渲染
    void render();

    // 属性
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    Vec2 getSize() const { return m_size; }
    void setSize(const Vec2& size) { m_size = size; }

    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

    // 获取所有控件
    const std::vector<std::shared_ptr<UIWidget>>& getWidgets() const { return m_widgets; }

private:
    std::string m_name;
    Vec2 m_size = Vec2(1920.0f, 1080.0f);
    bool m_visible = true;

    std::vector<std::shared_ptr<UIWidget>> m_widgets;
};

// UI 管理器
class UIManager {
public:
    static UIManager& get();

    // 画布管理
    std::shared_ptr<UICanvas> createCanvas(const std::string& name);
    std::shared_ptr<UICanvas> getCanvas(const std::string& name) const;
    void removeCanvas(const std::string& name);

    // 渲染所有画布
    void renderAll();

private:
    UIManager() = default;

    std::unordered_map<std::string, std::shared_ptr<UICanvas>> m_canvases;
};

} // namespace spark
