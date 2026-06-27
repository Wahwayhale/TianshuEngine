#pragma once

#include "math/math_types.h"
#include <string>
#include <unordered_map>

namespace spark {

// 颜色定义
struct Color {
    float r, g, b, a;

    Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}

    static Color fromHex(uint32_t hex) {
        return Color(
            ((hex >> 24) & 0xFF) / 255.0f,
            ((hex >> 16) & 0xFF) / 255.0f,
            ((hex >> 8) & 0xFF) / 255.0f,
            (hex & 0xFF) / 255.0f
        );
    }

    static Color fromRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }

    Color withAlpha(float alpha) const { return Color(r, g, b, alpha); }
    Color lighten(float amount) const {
        return Color(
            std::min(1.0f, r + amount),
            std::min(1.0f, g + amount),
            std::min(1.0f, b + amount),
            a
        );
    }
    Color darken(float amount) const {
        return Color(
            std::max(0.0f, r - amount),
            std::max(0.0f, g - amount),
            std::max(0.0f, b - amount),
            a
        );
    }
};

// UI 主题
class UITheme {
public:
    static UITheme& get();

    // 颜色方案
    struct Colors {
        // 背景色
        Color background = Color::fromRGB(30, 30, 30);
        Color backgroundLight = Color::fromRGB(45, 45, 45);
        Color backgroundDark = Color::fromRGB(20, 20, 20);
        Color backgroundHover = Color::fromRGB(55, 55, 55);
        Color backgroundActive = Color::fromRGB(65, 65, 65);

        // 面板色
        Color panel = Color::fromRGB(35, 35, 35);
        Color panelLight = Color::fromRGB(50, 50, 50);
        Color panelBorder = Color::fromRGB(60, 60, 60);

        // 主题色
        Color primary = Color::fromRGB(0, 120, 215);
        Color primaryHover = Color::fromRGB(20, 140, 235);
        Color primaryActive = Color::fromRGB(0, 100, 195);

        // 强调色
        Color accent = Color::fromRGB(255, 150, 0);
        Color accentHover = Color::fromRGB(255, 170, 20);

        // 成功/警告/错误
        Color success = Color::fromRGB(76, 175, 80);
        Color warning = Color::fromRGB(255, 152, 0);
        Color error = Color::fromRGB(244, 67, 54);

        // 文本色
        Color text = Color::fromRGB(240, 240, 240);
        Color textSecondary = Color::fromRGB(180, 180, 180);
        Color textDisabled = Color::fromRGB(120, 120, 120);
        Color textInverse = Color::fromRGB(30, 30, 30);

        // 边框色
        Color border = Color::fromRGB(70, 70, 70);
        Color borderLight = Color::fromRGB(90, 90, 90);
        Color borderFocus = Color::fromRGB(0, 120, 215);

        // 滚动条
        Color scrollbar = Color::fromRGB(60, 60, 60);
        Color scrollbarHover = Color::fromRGB(80, 80, 80);
        Color scrollbarThumb = Color::fromRGB(100, 100, 100);

        // 分割线
        Color separator = Color::fromRGB(50, 50, 50);

        // 阴影
        Color shadow = Color(0, 0, 0, 0.3f);
    };

    // 尺寸定义
    struct Sizes {
        // 字体大小
        float fontSmall = 12.0f;
        float fontNormal = 14.0f;
        float fontLarge = 16.0f;
        float fontTitle = 20.0f;
        float fontHeading = 24.0f;

        // 间距
        float paddingSmall = 4.0f;
        float paddingNormal = 8.0f;
        float paddingLarge = 16.0f;
        float paddingXLarge = 24.0f;

        // 组件尺寸
        float buttonHeight = 32.0f;
        float inputHeight = 32.0f;
        float checkboxSize = 20.0f;
        float radioButtonSize = 20.0f;
        float sliderHeight = 20.0f;
        float scrollbarWidth = 12.0f;
        float titleBarHeight = 32.0f;
        float menuBarHeight = 28.0f;
        float toolbarHeight = 40.0f;
        float statusBarHeight = 24.0f;

        // 圆角
        float borderRadiusSmall = 4.0f;
        float borderRadiusNormal = 6.0f;
        float borderRadiusLarge = 8.0f;
        float borderRadiusRound = 999.0f;

        // 图标大小
        float iconSmall = 16.0f;
        float iconNormal = 20.0f;
        float iconLarge = 24.0f;
    };

    // 动画定义
    struct Animations {
        float fadeInTime = 0.2f;
        float fadeOutTime = 0.15f;
        float hoverTime = 0.15f;
        float clickTime = 0.1f;
        float slideTime = 0.3f;
    };

    const Colors& getColors() const { return m_colors; }
    const Sizes& getSizes() const { return m_sizes; }
    const Animations& getAnimations() const { return m_animations; }

    Colors& getColors() { return m_colors; }
    Sizes& getSizes() { return m_sizes; }
    Animations& getAnimations() { return m_animations; }

    // 预设主题
    void setDarkTheme();
    void setLightTheme();
    void setBlueTheme();
    void setGreenTheme();

private:
    UITheme();

    Colors m_colors;
    Sizes m_sizes;
    Animations m_animations;
};

} // namespace spark
