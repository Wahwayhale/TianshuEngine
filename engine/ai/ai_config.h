#pragma once

#include <string>
#include <vector>

namespace spark {

// AI 模型提供商
enum class AIProvider {
    Custom,         // 自定义（兼容 OpenAI API）
    Qwen,           // 通义千问
    ChatGLM,        // 智谱 ChatGLM
    Baichuan,       // 百川
    DeepSeek,       // 深度求索
    Kimi,           // 月之暗面
    Spark,          // 讯飞星火
    ERNIE,          // 百度文心
    MiniMax,        // MiniMax
    Local           // 本地模型
};

// AI 配置
struct AIConfig {
    // 提供商
    AIProvider provider = AIProvider::Custom;

    // API 配置
    std::string apiKey;
    std::string apiEndpoint;
    std::string modelName;

    // 生成参数
    float temperature = 0.7f;
    int maxTokens = 2048;
    float topP = 0.9f;

    // 超时设置
    int timeoutSeconds = 30;

    // 是否启用
    bool enabled = false;
};

// 预设配置
namespace ai_presets {

// 通义千问
inline AIConfig qwen(const std::string& apiKey) {
    AIConfig config;
    config.provider = AIProvider::Qwen;
    config.apiKey = apiKey;
    config.apiEndpoint = "https://dashscope.aliyuncs.com/compatible-mode/v1";
    config.modelName = "qwen-turbo";
    return config;
}

// 智谱 ChatGLM
inline AIConfig chatglm(const std::string& apiKey) {
    AIConfig config;
    config.provider = AIProvider::ChatGLM;
    config.apiKey = apiKey;
    config.apiEndpoint = "https://open.bigmodel.cn/api/paas/v4";
    config.modelName = "glm-4-flash";
    return config;
}

// 百川
inline AIConfig baichuan(const std::string& apiKey) {
    AIConfig config;
    config.provider = AIProvider::Baichuan;
    config.apiKey = apiKey;
    config.apiEndpoint = "https://api.baichuan-ai.com/v1";
    config.modelName = "Baichuan4";
    return config;
}

// 深度求索 DeepSeek
inline AIConfig deepseek(const std::string& apiKey) {
    AIConfig config;
    config.provider = AIProvider::DeepSeek;
    config.apiKey = apiKey;
    config.apiEndpoint = "https://api.deepseek.com/v1";
    config.modelName = "deepseek-chat";
    return config;
}

// 月之暗面 Kimi
inline AIConfig kimi(const std::string& apiKey) {
    AIConfig config;
    config.provider = AIProvider::Kimi;
    config.apiKey = apiKey;
    config.apiEndpoint = "https://api.moonshot.cn/v1";
    config.modelName = "moonshot-v1-8k";
    return config;
}

// 讯飞星火
inline AIConfig spark_ai(const std::string& apiKey) {
    AIConfig config;
    config.provider = AIProvider::Spark;
    config.apiKey = apiKey;
    config.apiEndpoint = "https://spark-api-open.xf-yun.com/v1";
    config.modelName = "general";
    return config;
}

// 百度文心
inline AIConfig ernie(const std::string& apiKey) {
    AIConfig config;
    config.provider = AIProvider::ERNIE;
    config.apiKey = apiKey;
    config.apiEndpoint = "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop";
    config.modelName = "ernie-speed-128k";
    return config;
}

// 自定义（兼容 OpenAI API）
inline AIConfig custom(const std::string& apiKey, const std::string& endpoint, const std::string& model) {
    AIConfig config;
    config.provider = AIProvider::Custom;
    config.apiKey = apiKey;
    config.apiEndpoint = endpoint;
    config.modelName = model;
    return config;
}

// 本地模型（Ollama 等）
inline AIConfig local(const std::string& endpoint = "http://localhost:11434/v1", const std::string& model = "qwen2:7b") {
    AIConfig config;
    config.provider = AIProvider::Local;
    config.apiKey = "ollama";  // 本地模型不需要 key
    config.apiEndpoint = endpoint;
    config.modelName = model;
    return config;
}

} // namespace ai_presets

} // namespace spark
