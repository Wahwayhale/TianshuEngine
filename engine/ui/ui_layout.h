#pragma once

#include "ui_widgets.h"
#include <vector>
#include <memory>

namespace spark {

// 布局方向
enum class LayoutDirection {
    Horizontal,
    Vertical
};

// 对齐方式
enum class Alignment {
    Start,      // 左/上
    Center,     // 居中
    End,        // 右/下
    Stretch     // 拉伸
};

// 布局管理器
class UILayout : public UIWidget {
public:
    UILayout(LayoutDirection direction = LayoutDirection::Vertical, const std::string& id = "");
    ~UILayout() override = default;

    void render() override;
    void update(float deltaTime) override;

    // 布局配置
    void setDirection(LayoutDirection direction) { m_direction = direction; }
    void setSpacing(float spacing) { m_spacing = spacing; }
    void setPadding(float padding) { m_padding = padding; }
    void setAlignment(Alignment align) { m_alignment = align; }

    // 子组件管理
    void addChild(std::shared_ptr<UIWidget> child);
    void removeChild(const std::string& id);
    void clearChildren();

    // 布局计算
    void calculateLayout();

private:
    void layoutHorizontal();
    void layoutVertical();

    LayoutDirection m_direction = LayoutDirection::Vertical;
    float m_spacing = 4.0f;
    float m_padding = 8.0f;
    Alignment m_alignment = Alignment::Start;
};

// 网格布局
class UIGridLayout : public UIWidget {
public:
    UIGridLayout(int columns = 2, const std::string& id = "");
    ~UIGridLayout() override = default;

    void render() override;
    void update(float deltaTime) override;

    // 网格配置
    void setColumns(int columns) { m_columns = columns; }
    void setSpacing(Vec2 spacing) { m_spacing = spacing; }
    void setPadding(float padding) { m_padding = padding; }

    // 子组件管理
    void addChild(std::shared_ptr<UIWidget> child);
    void removeChild(const std::string& id);
    void clearChildren();

    // 布局计算
    void calculateLayout();

private:
    int m_columns = 2;
    Vec2 m_spacing = Vec2(8.0f, 8.0f);
    float m_padding = 8.0f;
    std::vector<std::shared_ptr<UIWidget>> m_gridChildren;
};

// 弹性布局（类似 CSS Flexbox）
class UIFlexLayout : public UIWidget {
public:
    UIFlexLayout(const std::string& id = "");
    ~UIFlexLayout() override = default;

    void render() override;
    void update(float deltaTime) override;

    // 弹性配置
    void setDirection(LayoutDirection direction) { m_direction = direction; }
    void setWrap(bool wrap) { m_wrap = wrap; }
    void setJustifyContent(Alignment justify) { m_justifyContent = justify; }
    void setAlignItems(Alignment align) { m_alignItems = align; }
    void setGap(float gap) { m_gap = gap; }

    // 子组件管理
    void addChild(std::shared_ptr<UIWidget> child, int flex = 0);
    void removeChild(const std::string& id);
    void clearChildren();

    // 布局计算
    void calculateLayout();

private:
    LayoutDirection m_direction = LayoutDirection::Horizontal;
    bool m_wrap = false;
    Alignment m_justifyContent = Alignment::Start;
    Alignment m_alignItems = Alignment::Start;
    float m_gap = 8.0f;

    struct FlexItem {
        std::shared_ptr<UIWidget> widget;
        int flex = 0;
    };
    std::vector<FlexItem> m_flexChildren;
};

// 栈布局（层叠）
class UIStackLayout : public UIWidget {
public:
    UIStackLayout(const std::string& id = "");
    ~UIStackLayout() override = default;

    void render() override;
    void update(float deltaTime) override;

    // 栈配置
    void setAlignment(Alignment align) { m_alignment = align; }
    void setPadding(float padding) { m_padding = padding; }

    // 子组件管理
    void addChild(std::shared_ptr<UIWidget> child);
    void removeChild(const std::string& id);
    void clearChildren();

private:
    Alignment m_alignment = Alignment::Center;
    float m_padding = 8.0f;
    std::vector<std::shared_ptr<UIWidget>> m_stackChildren;
};

// 滚动容器
class UIScrollView : public UIWidget {
public:
    UIScrollView(const std::string& id = "");
    ~UIScrollView() override = default;

    void render() override;
    void update(float deltaTime) override;

    // 滚动配置
    void setScrollX(bool scroll) { m_scrollX = scroll; }
    void setScrollY(bool scroll) { m_scrollY = scroll; }
    void setScrollbarWidth(float width) { m_scrollbarWidth = width; }
    void setShowScrollbar(bool show) { m_showScrollbar = show; }

    // 滚动位置
    void setScrollPosition(Vec2 pos);
    Vec2 getScrollPosition() const { return m_scrollPosition; }
    void scrollToTop();
    void scrollToBottom();

    // 内容管理
    void setContent(std::shared_ptr<UIWidget> content);
    std::shared_ptr<UIWidget> getContent() const { return m_content; }

private:
    bool m_scrollX = false;
    bool m_scrollY = true;
    float m_scrollbarWidth = 8.0f;
    bool m_showScrollbar = true;

    Vec2 m_scrollPosition = Vec2(0.0f);
    Vec2 m_contentSize = Vec2(0.0f);
    std::shared_ptr<UIWidget> m_content;
};

// 标签页
class UITabView : public UIWidget {
public:
    UITabView(const std::string& id = "");
    ~UITabView() override = default;

    void render() override;
    void update(float deltaTime) override;

    // 标签管理
    int addTab(const std::string& title);
    void removeTab(int index);
    void setActiveTab(int index);
    int getActiveTab() const { return m_activeTab; }

    // 标签内容
    void setTabContent(int index, std::shared_ptr<UIWidget> content);
    std::shared_ptr<UIWidget> getTabContent(int index) const;

    // 标签配置
    void setTabHeight(float height) { m_tabHeight = height; }
    void setCloseable(bool closeable) { m_closeable = closeable; }

private:
    void renderTabBar();
    void renderTabContent();

    struct Tab {
        std::string title;
        std::shared_ptr<UIWidget> content;
        bool closeable = true;
    };

    std::vector<Tab> m_tabs;
    int m_activeTab = 0;
    float m_tabHeight = 32.0f;
    bool m_closeable = true;
};

// 分割视图
class UISplitView : public UIWidget {
public:
    enum class SplitDirection {
        Horizontal,
        Vertical
    };

    UISplitView(SplitDirection direction = SplitDirection::Horizontal, const std::string& id = "");
    ~UISplitView() override = default;

    void render() override;
    void update(float deltaTime) override;

    // 分割配置
    void setDirection(SplitDirection direction) { m_direction = direction; }
    void setSplitPosition(float position) { m_splitPosition = position; }
    void setMinSize(float minSize) { m_minSize = minSize; }
    void setDraggable(bool draggable) { m_draggable = draggable; }

    // 内容管理
    void setFirst(std::shared_ptr<UIWidget> widget);
    void setSecond(std::shared_ptr<UIWidget> widget);

private:
    void renderSplitter();

    SplitDirection m_direction = SplitDirection::Horizontal;
    float m_splitPosition = 0.5f;  // 0.0 - 1.0
    float m_minSize = 100.0f;
    bool m_draggable = true;
    bool m_dragging = false;

    std::shared_ptr<UIWidget> m_first;
    std::shared_ptr<UIWidget> m_second;
};

} // namespace spark
