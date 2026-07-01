#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace spark {

class Device;
class Texture;

// 纹理管理器
class TextureManager {
public:
    static TextureManager& get();

    // 初始化
    bool initialize(Device* device);
    void shutdown();

    // 纹理加载
    std::shared_ptr<Texture> loadTexture(const std::string& path);
    std::shared_ptr<Texture> createTexture(uint32_t width, uint32_t height, const void* data);

    // 纹理缓存
    bool hasTexture(const std::string& path) const;
    std::shared_ptr<Texture> getTexture(const std::string& path) const;
    void removeTexture(const std::string& path);

    // 默认纹理
    std::shared_ptr<Texture> getDefaultWhiteTexture() const { return m_defaultWhite; }
    std::shared_ptr<Texture> getDefaultBlackTexture() const { return m_defaultBlack; }
    std::shared_ptr<Texture> getDefaultNormalTexture() const { return m_defaultNormal; }
    std::shared_ptr<Texture> getDefaultMetallicTexture() const { return m_defaultMetallic; }

    // 纹理压缩
    bool compressTexture(const std::string& inputPath, const std::string& outputPath, int quality = 80);

    // 纹理生成
    std::shared_ptr<Texture> generateNoiseTexture(uint32_t width, uint32_t height, float scale = 1.0f);
    std::shared_ptr<Texture> generateCheckerboardTexture(uint32_t width, uint32_t height, int checkSize = 8);

    // 获取所有纹理
    const std::unordered_map<std::string, std::shared_ptr<Texture>>& getTextures() const { return m_textures; }

private:
    TextureManager() = default;

    void createDefaultTextures();

    Device* m_device = nullptr;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;

    // 默认纹理
    std::shared_ptr<Texture> m_defaultWhite;
    std::shared_ptr<Texture> m_defaultBlack;
    std::shared_ptr<Texture> m_defaultNormal;
    std::shared_ptr<Texture> m_defaultMetallic;
};

} // namespace spark
