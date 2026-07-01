#pragma once

#include "math/math_types.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <vulkan/vulkan.h>

namespace spark {

class Device;
class Buffer;
class Texture;

// 字形信息
struct GlyphInfo {
    Vec2 uvMin;
    Vec2 uvMax;
    Vec2 size;
    Vec2 offset;
    float advance;
};

// 字体
class Font {
public:
    Font(Device& device, const std::string& filepath, float size);
    ~Font();

    // 获取字形
    const GlyphInfo* getGlyph(uint32_t codepoint) const;

    // 获取字体纹理
    Texture* getTexture() const { return m_texture.get(); }

    // 字体属性
    float getSize() const { return m_size; }
    float getLineHeight() const { return m_lineHeight; }
    float getAscent() const { return m_ascent; }
    float getDescent() const { return m_descent; }

private:
    void loadFont(const std::string& filepath);

    Device& m_device;
    float m_size;
    float m_lineHeight;
    float m_ascent;
    float m_descent;

    std::unordered_map<uint32_t, GlyphInfo> m_glyphs;
    std::unique_ptr<Texture> m_texture;
};

// 字体渲染器
class FontRenderer {
public:
    FontRenderer(Device& device);
    ~FontRenderer();

    // 加载字体
    std::shared_ptr<Font> loadFont(const std::string& filepath, float size);

    // 渲染文本
    void renderText(VkCommandBuffer commandBuffer, const std::string& text,
                    const Vec2& position, const Vec4& color = Vec4(1.0f),
                    float scale = 1.0f, const std::string& fontName = "default");

    // 获取文本尺寸
    Vec2 getTextSize(const std::string& text, float scale = 1.0f, const std::string& fontName = "default");

private:
    Device& m_device;
    std::unordered_map<std::string, std::shared_ptr<Font>> m_fonts;
};

} // namespace spark
