#pragma once

#include <string>
#include <vector>
#include <memory>
#include "math/math_types.h"

namespace spark {

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
    Dropdown,
    List,
    Container
};

// UI 控件
struct UIWidget {
    std::string id;
    std::string name;
    UIWidgetType type;
    Vec2 position;
    Vec2 size;
    Vec4 color;
    std::string text;
    float fontSize = 14.0f;
    bool visible = true;
    bool interactive = true;

    // 层级
    int zIndex = 0;
    std::string parentId;
    std::vector<std::string> childrenIds;
};

// UI 编辑器面板
class UIEditorPanel {
public:
    UIEditorPanel();
    ~UIEditorPanel();

    void render();

private:
    void renderToolbar();
    void renderWidgetTree();
    void renderCanvas();
    void renderProperties();
    void renderWidgetPalette();

    // 控件管理
    void addWidget(UIWidgetType type);
    void removeWidget(const std::string& id);
    void selectWidget(const std::string& id);

    std::vector<UIWidget> m_widgets;
    std::string m_selectedWidgetId;

    // 画布设置
    float m_canvasWidth = 1920.0f;
    float m_canvasHeight = 1080.0f;
    float m_zoom = 1.0f;
    Vec2 m_scroll = Vec2(0.0f);

    // 网格设置
    bool m_showGrid = true;
    float m_gridSize = 10.0f;
    bool m_snapToGrid = true;
};

} // namespace spark
