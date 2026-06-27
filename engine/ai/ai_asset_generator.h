#pragma once

#include "ai_manager.h"
#include <string>
#include <vector>
#include <functional>

namespace spark {

// 资产类型
enum class AssetType {
    Texture,
    Material,
    Audio,
    Model,
    Animation,
    Particle
};

// 纹理生成选项
struct TextureGenOptions {
    int width = 1024;
    int height = 1024;
    std::string style = "realistic";    // realistic, cartoon, pixel, handpainted
    std::string format = "png";
    bool seamless = false;
    bool generateNormalMap = false;
};

// 材质生成选项
struct MaterialGenOptions {
    std::string style = "pbr";
    bool metallic = false;
    float roughness = 0.5f;
};

// 音频生成选项
struct AudioGenOptions {
    std::string type = "sfx";           // sfx, music, ambient
    int duration = 5;                   // 秒
    std::string format = "wav";
};

// 资产生成结果
struct AssetGenResult {
    bool success = false;
    std::string filePath;
    std::string error;
    std::string description;
};

// AI 资产生成器
class AIAssetGenerator {
public:
    static AIAssetGenerator& get();

    // 初始化
    bool initialize();
    void shutdown();

    // 纹理生成
    AssetGenResult generateTexture(const std::string& description, const TextureGenOptions& options = {});
    std::vector<AssetGenResult> generateTextureVariations(const std::string& description, int count = 4);

    // 材质生成
    AssetGenResult generateMaterial(const std::string& description, const MaterialGenOptions& options = {});

    // 音频生成
    AssetGenResult generateAudio(const std::string& description, const AudioGenOptions& options = {});

    // 3D 模型生成（需要外部服务）
    AssetGenResult generateModel(const std::string& description);

    // 纹理描述生成
    std::string generateTextureDescription(const std::string& style, const std::string& subject);

    // 材质参数生成
    struct MaterialParams {
        float metallic = 0.0f;
        float roughness = 0.5f;
        float ao = 1.0f;
        float emission = 0.0f;
        Vec4 albedo = Vec4(1.0f);
    };

    MaterialParams generateMaterialParams(const std::string& description);

    // 图像生成 API 配置
    struct ImageGenConfig {
        std::string apiEndpoint;
        std::string apiKey;
        std::string model;
    };

    void setImageGenConfig(const ImageGenConfig& config) { m_imageConfig = config; }
    bool isImageGenConfigured() const { return !m_imageConfig.apiKey.empty(); }

private:
    AIAssetGenerator() = default;
    ~AIAssetGenerator() = default;

    // 调用图像生成 API
    std::string callImageGenAPI(const std::string& prompt, int width, int height);

    ImageGenConfig m_imageConfig;
    bool m_initialized = false;
};

} // namespace spark
