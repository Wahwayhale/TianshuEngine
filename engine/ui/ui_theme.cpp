#include "ui_theme.h"

namespace spark {

UITheme& UITheme::get() {
    static UITheme instance;
    return instance;
}

UITheme::UITheme() {
    // 默认使用深色主题
    setDarkTheme();
}

void UITheme::setDarkTheme() {
    // 背景色
    m_colors.background = Color::fromRGB(30, 30, 30);
    m_colors.backgroundLight = Color::fromRGB(45, 45, 45);
    m_colors.backgroundDark = Color::fromRGB(20, 20, 20);
    m_colors.backgroundHover = Color::fromRGB(55, 55, 55);
    m_colors.backgroundActive = Color::fromRGB(65, 65, 65);

    // 面板色
    m_colors.panel = Color::fromRGB(35, 35, 35);
    m_colors.panelLight = Color::fromRGB(50, 50, 50);
    m_colors.panelBorder = Color::fromRGB(60, 60, 60);

    // 主题色 - 蓝色
    m_colors.primary = Color::fromRGB(0, 120, 215);
    m_colors.primaryHover = Color::fromRGB(20, 140, 235);
    m_colors.primaryActive = Color::fromRGB(0, 100, 195);

    // 强调色 - 橙色
    m_colors.accent = Color::fromRGB(255, 150, 0);
    m_colors.accentHover = Color::fromRGB(255, 170, 20);

    // 状态色
    m_colors.success = Color::fromRGB(76, 175, 80);
    m_colors.warning = Color::fromRGB(255, 152, 0);
    m_colors.error = Color::fromRGB(244, 67, 54);

    // 文本色
    m_colors.text = Color::fromRGB(240, 240, 240);
    m_colors.textSecondary = Color::fromRGB(180, 180, 180);
    m_colors.textDisabled = Color::fromRGB(120, 120, 120);
    m_colors.textInverse = Color::fromRGB(30, 30, 30);

    // 边框色
    m_colors.border = Color::fromRGB(70, 70, 70);
    m_colors.borderLight = Color::fromRGB(90, 90, 90);
    m_colors.borderFocus = Color::fromRGB(0, 120, 215);

    // 滚动条
    m_colors.scrollbar = Color::fromRGB(60, 60, 60);
    m_colors.scrollbarHover = Color::fromRGB(80, 80, 80);
    m_colors.scrollbarThumb = Color::fromRGB(100, 100, 100);

    // 分割线
    m_colors.separator = Color::fromRGB(50, 50, 50);

    // 阴影
    m_colors.shadow = Color(0, 0, 0, 0.3f);
}

void UITheme::setLightTheme() {
    // 背景色
    m_colors.background = Color::fromRGB(245, 245, 245);
    m_colors.backgroundLight = Color::fromRGB(255, 255, 255);
    m_colors.backgroundDark = Color::fromRGB(230, 230, 230);
    m_colors.backgroundHover = Color::fromRGB(240, 240, 240);
    m_colors.backgroundActive = Color::fromRGB(235, 235, 235);

    // 面板色
    m_colors.panel = Color::fromRGB(255, 255, 255);
    m_colors.panelLight = Color::fromRGB(250, 250, 250);
    m_colors.panelBorder = Color::fromRGB(220, 220, 220);

    // 主题色
    m_colors.primary = Color::fromRGB(0, 120, 215);
    m_colors.primaryHover = Color::fromRGB(20, 140, 235);
    m_colors.primaryActive = Color::fromRGB(0, 100, 195);

    // 强调色
    m_colors.accent = Color::fromRGB(255, 150, 0);
    m_colors.accentHover = Color::fromRGB(255, 170, 20);

    // 状态色
    m_colors.success = Color::fromRGB(76, 175, 80);
    m_colors.warning = Color::fromRGB(255, 152, 0);
    m_colors.error = Color::fromRGB(244, 67, 54);

    // 文本色
    m_colors.text = Color::fromRGB(30, 30, 30);
    m_colors.textSecondary = Color::fromRGB(100, 100, 100);
    m_colors.textDisabled = Color::fromRGB(180, 180, 180);
    m_colors.textInverse = Color::fromRGB(255, 255, 255);

    // 边框色
    m_colors.border = Color::fromRGB(200, 200, 200);
    m_colors.borderLight = Color::fromRGB(220, 220, 220);
    m_colors.borderFocus = Color::fromRGB(0, 120, 215);

    // 滚动条
    m_colors.scrollbar = Color::fromRGB(220, 220, 220);
    m_colors.scrollbarHover = Color::fromRGB(200, 200, 200);
    m_colors.scrollbarThumb = Color::fromRGB(180, 180, 180);

    // 分割线
    m_colors.separator = Color::fromRGB(230, 230, 230);

    // 阴影
    m_colors.shadow = Color(0, 0, 0, 0.1f);
}

void UITheme::setBlueTheme() {
    setDarkTheme();

    // 蓝色主题
    m_colors.background = Color::fromRGB(25, 35, 50);
    m_colors.backgroundLight = Color::fromRGB(35, 50, 70);
    m_colors.backgroundDark = Color::fromRGB(15, 25, 40);

    m_colors.panel = Color::fromRGB(30, 45, 65);
    m_colors.panelLight = Color::fromRGB(40, 60, 85);
    m_colors.panelBorder = Color::fromRGB(50, 75, 100);

    m_colors.primary = Color::fromRGB(50, 150, 255);
    m_colors.primaryHover = Color::fromRGB(70, 170, 255);
    m_colors.primaryActive = Color::fromRGB(30, 130, 235);
}

void UITheme::setGreenTheme() {
    setDarkTheme();

    // 绿色主题
    m_colors.background = Color::fromRGB(25, 40, 30);
    m_colors.backgroundLight = Color::fromRGB(35, 55, 40);
    m_colors.backgroundDark = Color::fromRGB(15, 30, 20);

    m_colors.panel = Color::fromRGB(30, 50, 35);
    m_colors.panelLight = Color::fromRGB(40, 65, 45);
    m_colors.panelBorder = Color::fromRGB(50, 80, 55);

    m_colors.primary = Color::fromRGB(76, 175, 80);
    m_colors.primaryHover = Color::fromRGB(96, 195, 100);
    m_colors.primaryActive = Color::fromRGB(56, 155, 60);
}

} // namespace spark
