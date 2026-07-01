#include "texture_manager.h"
#include "texture.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

TextureManager& TextureManager::get() {
    static TextureManager instance;
    return instance;
}

bool TextureManager::initialize(Device* device) {
    if (m_device) return true;

    m_device = device;
    createDefaultTextures();

    SPARK_CORE_INFO("Texture manager initialized.");
    return true;
}

void TextureManager::shutdown() {
    m_textures.clear();
    m_defaultWhite.reset();
    m_defaultBlack.reset();
    m_defaultNormal.reset();
    m_defaultMetallic.reset();
    m_device = nullptr;

    SPARK_CORE_INFO("Texture manager shutdown.");
}

std::shared_ptr<Texture> TextureManager::loadTexture(const std::string& path) {
    // 检查缓存
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }

    // 加载纹理
    try {
        auto texture = std::make_shared<Texture>(*m_device, path);
        m_textures[path] = texture;
        SPARK_CORE_INFO("Texture loaded: {0}", path);
        return texture;
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to load texture: {0} - {1}", path, e.what());
        return nullptr;
    }
}

std::shared_ptr<Texture> TextureManager::createTexture(uint32_t width, uint32_t height, const void* data) {
    try {
        auto texture = std::make_shared<Texture>(*m_device, width, height, data);
        return texture;
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to create texture: {0}", e.what());
        return nullptr;
    }
}

bool TextureManager::hasTexture(const std::string& path) const {
    return m_textures.find(path) != m_textures.end();
}

std::shared_ptr<Texture> TextureManager::getTexture(const std::string& path) const {
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }
    return nullptr;
}

void TextureManager::removeTexture(const std::string& path) {
    m_textures.erase(path);
}

bool TextureManager::compressTexture(const std::string& inputPath, const std::string& outputPath, int quality) {
    // TODO: 实现纹理压缩
    SPARK_CORE_WARN("Texture compression not implemented yet.");
    return false;
}

std::shared_ptr<Texture> TextureManager::generateNoiseTexture(uint32_t width, uint32_t height, float scale) {
    // 生成噪声纹理
    std::vector<uint32_t> pixels(width * height);

    // 简化的噪声生成
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            float noise = (sin(x * scale * 0.1f) + cos(y * scale * 0.1f)) * 0.5f + 0.5f;
            uint8_t value = static_cast<uint8_t>(noise * 255);
            pixels[y * width + x] = (255 << 24) | (value << 16) | (value << 8) | value;
        }
    }

    return createTexture(width, height, pixels.data());
}

std::shared_ptr<Texture> TextureManager::generateCheckerboardTexture(uint32_t width, uint32_t height, int checkSize) {
    // 生成棋盘格纹理
    std::vector<uint32_t> pixels(width * height);

    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            bool isWhite = ((x / checkSize) + (y / checkSize)) % 2 == 0;
            uint8_t value = isWhite ? 255 : 0;
            pixels[y * width + x] = (255 << 24) | (value << 16) | (value << 8) | value;
        }
    }

    return createTexture(width, height, pixels.data());
}

void TextureManager::createDefaultTextures() {
    // 创建默认白色纹理
    uint32_t whitePixel = 0xFFFFFFFF;
    m_defaultWhite = createTexture(1, 1, &whitePixel);

    // 创建默认黑色纹理
    uint32_t blackPixel = 0xFF000000;
    m_defaultBlack = createTexture(1, 1, &blackPixel);

    // 创建默认法线纹理 (0.5, 0.5, 1.0)
    uint32_t normalPixel = 0xFFFF8080;
    m_defaultNormal = createTexture(1, 1, &normalPixel);

    // 创建默认金属度纹理 (黑色 = 非金属)
    m_defaultMetallic = m_defaultBlack;

    SPARK_CORE_INFO("Default textures created.");
}

} // namespace spark
