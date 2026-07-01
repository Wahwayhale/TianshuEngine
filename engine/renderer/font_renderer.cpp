#include "font_renderer.h"
#include "texture.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <fstream>

namespace spark {

// =============================================
// Font
// =============================================

Font::Font(Device& device, const std::string& filepath, float size)
    : m_device(device), m_size(size), m_lineHeight(size * 1.2f), m_ascent(size * 0.8f), m_descent(size * 0.2f) {

    loadFont(filepath);
}

Font::~Font() = default;

const GlyphInfo* Font::getGlyph(uint32_t codepoint) const {
    auto it = m_glyphs.find(codepoint);
    if (it != m_glyphs.end()) {
        return &it->second;
    }
    return nullptr;
}

void Font::loadFont(const std::string& filepath) {
    // TODO: 使用 FreeType 加载字体
    // 这里创建一个简单的占位实现

    // 创建默认字形
    for (uint32_t i = 32; i < 128; i++) {
        GlyphInfo glyph;
        glyph.uvMin = Vec2(0.0f);
        glyph.uvMax = Vec2(1.0f);
        glyph.size = Vec2(m_size * 0.6f, m_size);
        glyph.offset = Vec2(0.0f);
        glyph.advance = m_size * 0.6f;
        m_glyphs[i] = glyph;
    }

    // 创建字体纹理
    uint32_t whitePixel = 0xFFFFFFFF;
    m_texture = std::make_unique<Texture>(m_device, 1, 1, &whitePixel);

    SPARK_CORE_INFO("Font loaded: {0} (size: {1})", filepath, m_size);
}

// =============================================
// FontRenderer
// =============================================

FontRenderer::FontRenderer(Device& device) : m_device(device) {
    SPARK_CORE_INFO("Font renderer initialized.");
}

FontRenderer::~FontRenderer() = default;

std::shared_ptr<Font> FontRenderer::loadFont(const std::string& filepath, float size) {
    std::string key = filepath + "_" + std::to_string(static_cast<int>(size));

    auto it = m_fonts.find(key);
    if (it != m_fonts.end()) {
        return it->second;
    }

    auto font = std::make_shared<Font>(m_device, filepath, size);
    m_fonts[key] = font;
    return font;
}

void FontRenderer::renderText(VkCommandBuffer commandBuffer, const std::string& text,
                               const Vec2& position, const Vec4& color,
                               float scale, const std::string& fontName) {
    // TODO: 实现文本渲染
    // 需要创建字体渲染管线和着色器
}

Vec2 FontRenderer::getTextSize(const std::string& text, float scale, const std::string& fontName) {
    // TODO: 计算文本尺寸
    return Vec2(text.size() * 10.0f * scale, 20.0f * scale);
}

} // namespace spark
