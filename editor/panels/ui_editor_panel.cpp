#include "ui_editor_panel.h"
#include "ui/widgets.h"
#include <imgui.h>

namespace spark {

UIEditorPanel::UIEditorPanel() = default;
UIEditorPanel::~UIEditorPanel() = default;

void UIEditorPanel::render() {
    ImGui::Begin("UI Editor");

    renderToolbar();
    ImGui::Separator();

    // 左侧：控件树
    ImGui::BeginChild("WidgetTree", ImVec2(200, 0), true);
    renderWidgetTree();
    ImGui::EndChild();

    ImGui::SameLine();

    // 中间：画布
    ImGui::BeginChild("Canvas", ImVec2(0, 0), true);
    renderCanvas();
    ImGui::EndChild();

    ImGui::SameLine();

    // 右侧：属性
    ImGui::BeginChild("Properties", ImVec2(250, 0), true);
    renderProperties();
    ImGui::EndChild();

    ImGui::End();
}

void UIEditorPanel::renderToolbar() {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));

    // 添加控件按钮
    if (ImGui::Button("Panel")) {
        addWidget(UIWidgetType::Panel);
    }
    ImGui::SameLine();

    if (ImGui::Button("Button")) {
        addWidget(UIWidgetType::Button);
    }
    ImGui::SameLine();

    if (ImGui::Button("Text")) {
        addWidget(UIWidgetType::Text);
    }
    ImGui::SameLine();

    if (ImGui::Button("Image")) {
        addWidget(UIWidgetType::Image);
    }
    ImGui::SameLine();

    if (ImGui::Button("Input")) {
        addWidget(UIWidgetType::Input);
    }

    ImGui::SameLine();
    ui::drawToolbarSeparator();

    // 网格设置
    ImGui::Checkbox("Grid", &m_showGrid);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60);
    ImGui::SliderFloat("##GridSize", &m_gridSize, 5.0f, 50.0f, "%.0f");
    ImGui::SameLine();
    ImGui::Checkbox("Snap", &m_snapToGrid);

    ImGui::PopStyleVar(2);
}

void UIEditorPanel::renderWidgetTree() {
    ImGui::Text("Widget Tree");
    ImGui::Separator();

    // 控件树
    for (const auto& widget : m_widgets) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        if (m_selectedWidgetId == widget.id) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        // 图标
        const char* icon = "";
        switch (widget.type) {
            case UIWidgetType::Panel: icon = ""; break;
            case UIWidgetType::Button: icon = ""; break;
            case UIWidgetType::Text: icon = "T"; break;
            case UIWidgetType::Image: icon = "I"; break;
            default: icon = "W"; break;
        }

        char label[256];
        snprintf(label, sizeof(label), "%s %s", icon, widget.name.c_str());

        bool opened = ImGui::TreeNodeEx(widget.id.c_str(), flags, "%s", label);

        if (ImGui::IsItemClicked()) {
            selectWidget(widget.id);
        }

        if (opened) {
            ImGui::TreePop();
        }
    }

    // 右键菜单
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("Delete Selected")) {
            if (!m_selectedWidgetId.empty()) {
                removeWidget(m_selectedWidgetId);
            }
        }
        ImGui::EndPopup();
    }
}

void UIEditorPanel::renderCanvas() {
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    // 画布背景
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(
        canvasPos,
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(40, 40, 45, 255)
    );

    // 绘制网格
    if (m_showGrid) {
        ImU32 gridColor = IM_COL32(60, 60, 65, 255);

        for (float x = 0; x < canvasSize.x; x += m_gridSize) {
            drawList->AddLine(
                ImVec2(canvasPos.x + x, canvasPos.y),
                ImVec2(canvasPos.x + x, canvasPos.y + canvasSize.y),
                gridColor
            );
        }

        for (float y = 0; y < canvasSize.y; y += m_gridSize) {
            drawList->AddLine(
                ImVec2(canvasPos.x, canvasPos.y + y),
                ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + y),
                gridColor
            );
        }
    }

    // 绘制控件
    for (const auto& widget : m_widgets) {
        if (!widget.visible) continue;

        ImVec2 widgetPos = ImVec2(canvasPos.x + widget.position.x, canvasPos.y + widget.position.y);
        ImVec2 widgetSize = ImVec2(widget.size.x, widget.size.y);

        // 控件背景
        ImU32 bgColor = IM_COL32(
            (int)(widget.color.x * 255),
            (int)(widget.color.y * 255),
            (int)(widget.color.z * 255),
            (int)(widget.color.w * 255)
        );

        drawList->AddRectFilled(widgetPos, ImVec2(widgetPos.x + widgetSize.x, widgetPos.y + widgetSize.y), bgColor, 4.0f);

        // 控件边框
        ImU32 borderColor = (m_selectedWidgetId == widget.id) ?
            IM_COL32(0, 120, 215, 255) : IM_COL32(100, 100, 100, 255);

        drawList->AddRect(widgetPos, ImVec2(widgetPos.x + widgetSize.x, widgetPos.y + widgetSize.y), borderColor, 4.0f);

        // 控件文本
        if (!widget.text.empty()) {
            ImVec2 textSize = ImGui::CalcTextSize(widget.text.c_str());
            ImVec2 textPos = ImVec2(
                widgetPos.x + (widgetSize.x - textSize.x) * 0.5f,
                widgetPos.y + (widgetSize.y - textSize.y) * 0.5f
            );

            drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), widget.text.c_str());
        }
    }

    ImGui::Dummy(canvasSize);
}

void UIEditorPanel::renderProperties() {
    ImGui::Text("Properties");
    ImGui::Separator();

    if (m_selectedWidgetId.empty()) {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No widget selected");
        return;
    }

    // 查找选中的控件
    UIWidget* selectedWidget = nullptr;
    for (auto& widget : m_widgets) {
        if (widget.id == m_selectedWidgetId) {
            selectedWidget = &widget;
            break;
        }
    }

    if (!selectedWidget) return;

    // 名称
    char nameBuffer[256];
    strncpy(nameBuffer, selectedWidget->name.c_str(), sizeof(nameBuffer));
    if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
        selectedWidget->name = nameBuffer;
    }

    // 类型
    const char* types[] = { "Panel", "Button", "Text", "Image", "Input", "Checkbox", "Slider", "ProgressBar", "Dropdown", "List", "Container" };
    int currentType = static_cast<int>(selectedWidget->type);
    ImGui::Text("Type: %s", types[currentType]);

    ImGui::Spacing();

    // 位置
    ImGui::Text("Position");
    ImGui::DragFloat2("##Position", &selectedWidget->position.x, 1.0f);

    // 大小
    ImGui::Text("Size");
    ImGui::DragFloat2("##Size", &selectedWidget->size.x, 1.0f);

    ImGui::Spacing();

    // 颜色
    ImGui::Text("Color");
    ImGui::ColorEdit4("##Color", &selectedWidget->color.x);

    // 文本
    if (selectedWidget->type == UIWidgetType::Text || selectedWidget->type == UIWidgetType::Button) {
        ImGui::Text("Text");
        char textBuffer[256];
        strncpy(textBuffer, selectedWidget->text.c_str(), sizeof(textBuffer));
        if (ImGui::InputText("##Text", textBuffer, sizeof(textBuffer))) {
            selectedWidget->text = textBuffer;
        }
    }

    // 字体大小
    if (selectedWidget->type == UIWidgetType::Text) {
        ImGui::Text("Font Size");
        ImGui::SliderFloat("##FontSize", &selectedWidget->fontSize, 8.0f, 72.0f, "%.0f");
    }

    ImGui::Spacing();

    // 可见性
    ImGui::Checkbox("Visible", &selectedWidget->visible);

    // 交互性
    ImGui::Checkbox("Interactive", &selectedWidget->interactive);

    // Z 轴
    ImGui::Text("Z-Index");
    ImGui::SliderInt("##ZIndex", &selectedWidget->zIndex, 0, 100);
}

void UIEditorPanel::addWidget(UIWidgetType type) {
    UIWidget widget;
    widget.id = "widget_" + std::to_string(m_widgets.size());
    widget.type = type;

    switch (type) {
        case UIWidgetType::Panel:
            widget.name = "Panel";
            widget.size = Vec2(200, 150);
            widget.color = Vec4(0.2f, 0.2f, 0.25f, 0.9f);
            break;
        case UIWidgetType::Button:
            widget.name = "Button";
            widget.size = Vec2(120, 40);
            widget.color = Vec4(0.2f, 0.5f, 0.8f, 1.0f);
            widget.text = "Button";
            break;
        case UIWidgetType::Text:
            widget.name = "Text";
            widget.size = Vec2(100, 30);
            widget.color = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
            widget.text = "Text";
            break;
        case UIWidgetType::Image:
            widget.name = "Image";
            widget.size = Vec2(100, 100);
            widget.color = Vec4(0.3f, 0.3f, 0.35f, 1.0f);
            break;
        case UIWidgetType::Input:
            widget.name = "Input";
            widget.size = Vec2(200, 30);
            widget.color = Vec4(0.15f, 0.15f, 0.18f, 1.0f);
            break;
        default:
            widget.name = "Widget";
            widget.size = Vec2(100, 50);
            widget.color = Vec4(0.2f, 0.2f, 0.25f, 0.9f);
            break;
    }

    widget.position = Vec2(100, 100);
    m_widgets.push_back(widget);
    m_selectedWidgetId = widget.id;
}

void UIEditorPanel::removeWidget(const std::string& id) {
    m_widgets.erase(
        std::remove_if(m_widgets.begin(), m_widgets.end(),
            [&](const UIWidget& w) { return w.id == id; }),
        m_widgets.end()
    );

    if (m_selectedWidgetId == id) {
        m_selectedWidgetId.clear();
    }
}

void UIEditorPanel::selectWidget(const std::string& id) {
    m_selectedWidgetId = id;
}

} // namespace spark
