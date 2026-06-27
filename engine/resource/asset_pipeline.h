#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <filesystem>

namespace spark {

// 资产类型
enum class AssetType {
    Unknown,
    Texture,
    Model,
    Audio,
    Shader,
    Material,
    Animation,
    Script,
    Scene,
    Font
};

// 资产信息
struct AssetInfo {
    std::string path;
    std::string name;
    AssetType type = AssetType::Unknown;
    size_t size = 0;
    std::string extension;
    bool isLoaded = false;
};

// 资产导入选项
struct AssetImportOptions {
    bool compress = true;
    bool generateMipmaps = true;
    int maxTextureSize = 4096;
    bool optimizeMesh = true;
    float meshSimplification = 0.0f;  // 0.0 = no simplification
};

// 资产处理器接口
class AssetProcessor {
public:
    virtual ~AssetProcessor() = default;

    virtual bool canProcess(const std::string& extension) const = 0;
    virtual bool process(const std::string& inputPath, const std::string& outputPath, const AssetImportOptions& options) = 0;
    virtual AssetType getAssetType() const = 0;
};

// 纹理处理器
class TextureProcessor : public AssetProcessor {
public:
    bool canProcess(const std::string& extension) const override;
    bool process(const std::string& inputPath, const std::string& outputPath, const AssetImportOptions& options) override;
    AssetType getAssetType() const override { return AssetType::Texture; }
};

// 模型处理器
class ModelProcessor : public AssetProcessor {
public:
    bool canProcess(const std::string& extension) const override;
    bool process(const std::string& inputPath, const std::string& outputPath, const AssetImportOptions& options) override;
    AssetType getAssetType() const override { return AssetType::Model; }
};

// 音频处理器
class AudioProcessor : public AssetProcessor {
public:
    bool canProcess(const std::string& extension) const override;
    bool process(const std::string& inputPath, const std::string& outputPath, const AssetImportOptions& options) override;
    AssetType getAssetType() const override { return AssetType::Audio; }
};

// 资产管线
class AssetPipeline {
public:
    static AssetPipeline& get();

    // 初始化
    bool initialize();
    void shutdown();

    // 资产处理
    bool importAsset(const std::string& inputPath, const std::string& outputPath, const AssetImportOptions& options = {});
    bool reimportAsset(const std::string& assetPath);

    // 资产扫描
    void scanDirectory(const std::string& directory);
    void scanAssetDirectory();

    // 资产查询
    AssetInfo getAssetInfo(const std::string& path) const;
    std::vector<AssetInfo> getAssetsByType(AssetType type) const;
    std::vector<AssetInfo> searchAssets(const std::string& query) const;

    // 资产处理器
    void registerProcessor(std::shared_ptr<AssetProcessor> processor);

    // 资产库
    const std::vector<AssetInfo>& getAssets() const { return m_assets; }

    // 资产目录
    void setAssetDirectory(const std::string& directory) { m_assetDirectory = directory; }
    const std::string& getAssetDirectory() const { return m_assetDirectory; }

private:
    AssetPipeline() = default;

    AssetType detectAssetType(const std::string& extension) const;
    std::string getAssetName(const std::string& path) const;

    std::string m_assetDirectory = "assets";
    std::vector<AssetInfo> m_assets;
    std::vector<std::shared_ptr<AssetProcessor>> m_processors;
};

} // namespace spark
