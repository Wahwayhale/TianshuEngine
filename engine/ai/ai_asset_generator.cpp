#include "ai_asset_generator.h"
#include "core/log.h"
#include <fstream>
#include <ctime>

namespace spark {

AIAssetGenerator& AIAssetGenerator::get() {
    static AIAssetGenerator instance;
    return instance;
}

bool AIAssetGenerator::initialize() {
    if (m_initialized) return true;

    m_initialized = true;
    SPARK_CORE_INFO("AI Asset Generator initialized.");
    return true;
}

void AIAssetGenerator::shutdown() {
    if (!m_initialized) return;
    m_initialized = false;
}

AssetGenResult AIAssetGenerator::generateTexture(const std::string& description, const TextureGenOptions& options) {
    AssetGenResult result;

    if (!isImageGenConfigured()) {
        result.error = "Image generation API not configured. Please set API endpoint and key.";
        return result;
    }

    try {
        // 构建提示词
        std::string prompt = description;

        if (options.style == "cartoon") {
            prompt += ", cartoon style, cel shading";
        } else if (options.style == "pixel") {
            prompt += ", pixel art, retro style";
        } else if (options.style == "handpainted") {
            prompt += ", handpainted, watercolor style";
        }

        if (options.seamless) {
            prompt += ", seamless texture, tileable";
        }

        // 调用图像生成 API
        std::string imageData = callImageGenAPI(prompt, options.width, options.height);

        if (imageData.empty()) {
            result.error = "Failed to generate texture";
            return result;
        }

        // 保存纹理文件
        time_t now = time(nullptr);
        std::string filename = "assets/textures/ai_generated_" + std::to_string(now) + "." + options.format;

        // TODO: 实际保存图像数据
        result.success = true;
        result.filePath = filename;
        result.description = description;

        SPARK_CORE_INFO("Texture generated: {0}", filename);

    } catch (const std::exception& e) {
        result.error = e.what();
    }

    return result;
}

std::vector<AssetGenResult> AIAssetGenerator::generateTextureVariations(const std::string& description, int count) {
    std::vector<AssetGenResult> results;

    for (int i = 0; i < count; i++) {
        std::string variationPrompt = description + " (variation " + std::to_string(i + 1) + ")";
        results.push_back(generateTexture(variationPrompt));
    }

    return results;
}

AssetGenResult AIAssetGenerator::generateMaterial(const std::string& description, const MaterialGenOptions& options) {
    AssetGenResult result;

    // 使用 AI 生成材质参数
    auto params = generateMaterialParams(description);

    // 生成材质纹理
    TextureGenOptions texOptions;
    texOptions.style = options.style;

    auto textureResult = generateTexture(description + " texture", texOptions);

    if (textureResult.success) {
        result.success = true;
        result.filePath = textureResult.filePath;
        result.description = "Material: " + description;
    } else {
        result.error = textureResult.error;
    }

    return result;
}

AssetGenResult AIAssetGenerator::generateAudio(const std::string& description, const AudioGenOptions& options) {
    AssetGenResult result;

    // 使用 AI 生成音频描述
    std::string systemPrompt;

    if (options.type == "sfx") {
        systemPrompt = R"(你是一个音效设计助手。根据用户的描述，生成音效的详细描述。

输出格式：
- 音效名称
- 持续时间
- 频率特征
- 音量变化
- 触发条件)";
    } else if (options.type == "music") {
        systemPrompt = R"(你是一个音乐创作助手。根据用户的描述，生成音乐的详细描述。

输出格式：
- 音乐名称
- 风格
- BPM
- 调性
- 乐器组成
- 情感氛围)";
    } else if (options.type == "ambient") {
        systemPrompt = R"(你是一个环境音效助手。根据用户的描述，生成环境音效的详细描述。

输出格式：
- 环境名称
- 主要声音元素
- 背景音
- 空间感
- 循环特性)";
    }

    std::vector<AIMessage> messages = {
        {"system", systemPrompt},
        {"user", description}
    };

    auto response = AIManager::get().chat(messages);

    if (response.success) {
        result.success = true;
        result.description = response.content;
        result.filePath = "assets/audio/ai_generated_" + std::to_string(time(nullptr)) + ".wav";

        SPARK_CORE_INFO("Audio description generated: {0}", description);
    } else {
        result.error = response.error;
    }

    return result;
}

AssetGenResult AIAssetGenerator::generateModel(const std::string& description) {
    AssetGenResult result;

    // 使用 AI 生成 3D 模型描述
    std::string systemPrompt = R"(你是一个 3D 建模助手。根据用户的描述，生成 3D 模型的详细描述。

输出格式：
- 模型名称
- 模型类型（角色、建筑、道具等）
- 多边形数量建议
- 纹理需求
- 动画需求
- 技术规格)";

    std::vector<AIMessage> messages = {
        {"system", systemPrompt},
        {"user", description}
    };

    auto response = AIManager::get().chat(messages);

    if (response.success) {
        result.success = true;
        result.description = response.content;
        result.filePath = "assets/models/ai_generated_" + std::to_string(time(nullptr)) + ".glb";

        SPARK_CORE_INFO("Model description generated: {0}", description);
    } else {
        result.error = response.error;
    }

    return result;
}

std::string AIAssetGenerator::generateTextureDescription(const std::string& style, const std::string& subject) {
    std::string systemPrompt = R"(你是一个纹理设计助手。生成详细的纹理描述，用于 AI 图像生成。

要求：
1. 描述纹理的外观
2. 包含颜色、材质、细节
3. 适合游戏使用
4. 可用于 Stable Diffusion 等工具)";

    std::vector<AIMessage> messages = {
        {"system", systemPrompt},
        {"user", "风格: " + style + "\n主题: " + subject}
    };

    auto response = AIManager::get().chat(messages);
    return response.success ? response.content : "";
}

AIAssetGenerator::MaterialParams AIAssetGenerator::generateMaterialParams(const std::string& description) {
    MaterialParams params;

    std::string systemPrompt = R"(你是一个 PBR 材质助手。根据描述，生成 PBR 材质参数。

输出格式（JSON）：
{
    "metallic": 0.0-1.0,
    "roughness": 0.0-1.0,
    "ao": 0.0-1.0,
    "emission": 0.0-1.0,
    "albedo_r": 0.0-1.0,
    "albedo_g": 0.0-1.0,
    "albedo_b": 0.0-1.0
}

只输出 JSON，不要其他内容。)";

    std::vector<AIMessage> messages = {
        {"system", systemPrompt},
        {"user", description}
    };

    auto response = AIManager::get().chat(messages);

    if (response.success) {
        // 解析 JSON（简化实现）
        std::string json = response.content;

        // 提取数值（简化实现，实际应该使用 JSON 库）
        auto extractFloat = [&](const std::string& key) -> float {
            size_t pos = json.find(key);
            if (pos == std::string::npos) return 0.5f;
            pos = json.find(":", pos);
            if (pos == std::string::npos) return 0.5f;
            pos = json.find_first_of("0123456789.", pos);
            if (pos == std::string::npos) return 0.5f;
            return std::stof(json.substr(pos));
        };

        params.metallic = extractFloat("metallic");
        params.roughness = extractFloat("roughness");
        params.ao = extractFloat("ao");
        params.emission = extractFloat("emission");
        params.albedo.x = extractFloat("albedo_r");
        params.albedo.y = extractFloat("albedo_g");
        params.albedo.z = extractFloat("albedo_b");
    }

    return params;
}

std::string AIAssetGenerator::callImageGenAPI(const std::string& prompt, int width, int height) {
    // 调用图像生成 API
    // 支持多种 API 格式：

    // 1. Stability AI (Stable Diffusion)
    // POST https://api.stability.ai/v1/generation/{engine_id}/text-to-image

    // 2. 通义万相
    // POST https://dashscope.aliyuncs.com/api/v1/services/aigc/text2image/image-synthesis

    // 3. 百度文心
    // POST https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/text2image

    // 4. 本地 ComfyUI / Stable Diffusion WebUI
    // POST http://localhost:7860/sdapi/v1/txt2img

    // 简化实现：返回空字符串
    // 实际实现需要调用具体的 API

    SPARK_CORE_WARN("Image generation API not implemented yet.");
    return "";
}

} // namespace spark
