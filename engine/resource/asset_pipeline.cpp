#include "asset_pipeline.h"
#include "core/log.h"
#include <algorithm>
#include <fstream>

namespace spark {

// =============================================
// TextureProcessor
// =============================================

bool TextureProcessor::canProcess(const std::string& extension) const {
    static const std::vector<std::string> extensions = {
        ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".tiff", ".webp"
    };
    std::string lowerExt = extension;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
    return std::find(extensions.begin(), extensions.end(), lowerExt) != extensions.end();
}

bool TextureProcessor::process(const std::string& inputPath, const std::string& outputPath, const AssetImportOptions& options) {
    // 纹理处理：压缩、生成 mipmap 等
    // 简化实现：直接复制文件
    try {
        std::filesystem::copy_file(inputPath, outputPath, std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to process texture: {0}", e.what());
        return false;
    }
}

// =============================================
// ModelProcessor
// =============================================

bool ModelProcessor::canProcess(const std::string& extension) const {
    static const std::vector<std::string> extensions = {
        ".gltf", ".glb", ".obj", ".fbx"
    };
    std::string lowerExt = extension;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
    return std::find(extensions.begin(), extensions.end(), lowerExt) != extensions.end();
}

bool ModelProcessor::process(const std::string& inputPath, const std::string& outputPath, const AssetImportOptions& options) {
    // 模型处理：优化网格、生成 LOD 等
    // 简化实现：直接复制文件
    try {
        std::filesystem::copy_file(inputPath, outputPath, std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to process model: {0}", e.what());
        return false;
    }
}

// =============================================
// AudioProcessor
// =============================================

bool AudioProcessor::canProcess(const std::string& extension) const {
    static const std::vector<std::string> extensions = {
        ".wav", ".mp3", ".ogg", ".flac"
    };
    std::string lowerExt = extension;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
    return std::find(extensions.begin(), extensions.end(), lowerExt) != extensions.end();
}

bool AudioProcessor::process(const std::string& inputPath, const std::string& outputPath, const AssetImportOptions& options) {
    // 音频处理：转换格式、压缩等
    // 简化实现：直接复制文件
    try {
        std::filesystem::copy_file(inputPath, outputPath, std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to process audio: {0}", e.what());
        return false;
    }
}

// =============================================
// AssetPipeline
// =============================================

AssetPipeline& AssetPipeline::get() {
    static AssetPipeline instance;
    return instance;
}

bool AssetPipeline::initialize() {
    // 注册默认处理器
    registerProcessor(std::make_shared<TextureProcessor>());
    registerProcessor(std::make_shared<ModelProcessor>());
    registerProcessor(std::make_shared<AudioProcessor>());

    SPARK_CORE_INFO("Asset pipeline initialized with {0} processors.", m_processors.size());
    return true;
}

void AssetPipeline::shutdown() {
    m_processors.clear();
    m_assets.clear();
    SPARK_CORE_INFO("Asset pipeline shutdown.");
}

bool AssetPipeline::importAsset(const std::string& inputPath, const std::string& outputPath, const AssetImportOptions& options) {
    // 检查输入文件是否存在
    if (!std::filesystem::exists(inputPath)) {
        SPARK_CORE_ERROR("Input file does not exist: {0}", inputPath);
        return false;
    }

    // 获取文件扩展名
    std::string extension = std::filesystem::path(inputPath).extension().string();

    // 查找合适的处理器
    for (auto& processor : m_processors) {
        if (processor->canProcess(extension)) {
            SPARK_CORE_INFO("Importing asset: {0}", inputPath);

            if (processor->process(inputPath, outputPath, options)) {
                // 添加到资产库
                AssetInfo info;
                info.path = outputPath;
                info.name = getAssetName(outputPath);
                info.type = processor->getAssetType();
                info.size = std::filesystem::file_size(outputPath);
                info.extension = std::filesystem::path(outputPath).extension().string();
                info.isLoaded = true;

                m_assets.push_back(info);

                SPARK_CORE_INFO("Asset imported successfully: {0}", outputPath);
                return true;
            } else {
                SPARK_CORE_ERROR("Failed to import asset: {0}", inputPath);
                return false;
            }
        }
    }

    SPARK_CORE_WARN("No processor found for extension: {0}", extension);
    return false;
}

bool AssetPipeline::reimportAsset(const std::string& assetPath) {
    // 查找资产
    for (auto& asset : m_assets) {
        if (asset.path == assetPath) {
            // 重新导入
            SPARK_CORE_INFO("Reimporting asset: {0}", assetPath);
            // TODO: 实现重新导入逻辑
            return true;
        }
    }

    SPARK_CORE_WARN("Asset not found: {0}", assetPath);
    return false;
}

void AssetPipeline::scanDirectory(const std::string& directory) {
    if (!std::filesystem::exists(directory)) {
        SPARK_CORE_WARN("Directory does not exist: {0}", directory);
        return;
    }

    SPARK_CORE_INFO("Scanning directory: {0}", directory);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            AssetInfo info;
            info.path = entry.path().string();
            info.name = entry.path().stem().string();
            info.extension = entry.path().extension().string();
            info.size = entry.file_size();
            info.type = detectAssetType(info.extension);
            info.isLoaded = false;

            if (info.type != AssetType::Unknown) {
                m_assets.push_back(info);
            }
        }
    }

    SPARK_CORE_INFO("Found {0} assets.", m_assets.size());
}

void AssetPipeline::scanAssetDirectory() {
    scanDirectory(m_assetDirectory);
}

AssetInfo AssetPipeline::getAssetInfo(const std::string& path) const {
    for (const auto& asset : m_assets) {
        if (asset.path == path) {
            return asset;
        }
    }
    return AssetInfo{};
}

std::vector<AssetInfo> AssetPipeline::getAssetsByType(AssetType type) const {
    std::vector<AssetInfo> result;
    for (const auto& asset : m_assets) {
        if (asset.type == type) {
            result.push_back(asset);
        }
    }
    return result;
}

std::vector<AssetInfo> AssetPipeline::searchAssets(const std::string& query) const {
    std::vector<AssetInfo> result;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    for (const auto& asset : m_assets) {
        std::string lowerName = asset.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (lowerName.find(lowerQuery) != std::string::npos) {
            result.push_back(asset);
        }
    }
    return result;
}

void AssetPipeline::registerProcessor(std::shared_ptr<AssetProcessor> processor) {
    m_processors.push_back(processor);
}

AssetType AssetPipeline::detectAssetType(const std::string& extension) const {
    std::string lowerExt = extension;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);

    if (lowerExt == ".png" || lowerExt == ".jpg" || lowerExt == ".jpeg" ||
        lowerExt == ".bmp" || lowerExt == ".tga" || lowerExt == ".tiff") {
        return AssetType::Texture;
    }

    if (lowerExt == ".gltf" || lowerExt == ".glb" || lowerExt == ".obj" || lowerExt == ".fbx") {
        return AssetType::Model;
    }

    if (lowerExt == ".wav" || lowerExt == ".mp3" || lowerExt == ".ogg") {
        return AssetType::Audio;
    }

    if (lowerExt == ".glsl" || lowerExt == ".vert" || lowerExt == ".frag" || lowerExt == ".spv") {
        return AssetType::Shader;
    }

    if (lowerExt == ".lua") {
        return AssetType::Script;
    }

    if (lowerExt == ".json") {
        return AssetType::Scene;
    }

    if (lowerExt == ".ttf" || lowerExt == ".otf") {
        return AssetType::Font;
    }

    return AssetType::Unknown;
}

std::string AssetPipeline::getAssetName(const std::string& path) const {
    return std::filesystem::path(path).stem().string();
}

} // namespace spark
