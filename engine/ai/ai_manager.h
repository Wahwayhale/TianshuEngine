#pragma once

#include "ai_config.h"
#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <mutex>

namespace spark {

// AI 响应
struct AIResponse {
    bool success = false;
    std::string content;
    std::string error;
    int tokensUsed = 0;
};

// AI 消息
struct AIMessage {
    std::string role;    // "system", "user", "assistant"
    std::string content;
};

// AI 管理器
class AIManager {
public:
    static AIManager& get();

    // 初始化/关闭
    bool initialize();
    void shutdown();

    // 配置
    void setConfig(const AIConfig& config);
    const AIConfig& getConfig() const { return m_config; }
    bool isConfigured() const { return m_config.enabled && !m_config.apiKey.empty(); }

    // 代码生成
    AIResponse generateCode(const std::string& description);
    AIResponse generateScript(const std::string& description);

    // 资产描述生成
    AIResponse generateAssetDescription(const std::string& type, const std::string& description);

    // 场景描述生成
    AIResponse generateSceneDescription(const std::string& description);

    // NPC 行为生成
    AIResponse generateNPCBehavior(const std::string& description);

    // 对话生成
    AIResponse generateDialogue(const std::string& character, const std::string& situation);

    // 通用对话
    AIResponse chat(const std::string& message);
    AIResponse chat(const std::vector<AIMessage>& messages);

    // 测试连接
    AIResponse testConnection();

    // 获取可用模型列表（从 API 自动获取）
    std::vector<std::string> fetchAvailableModels();

    // 获取已缓存的模型列表
    std::vector<std::string> getAvailableModels() const { return m_availableModels; }

    // 是否正在获取模型
    bool isFetchingModels() const { return m_fetchingModels; }

private:
    AIManager() = default;
    ~AIManager() = default;

    // HTTP 请求
    std::string sendRequest(const std::string& endpoint, const std::string& body, bool isGet = false);

    // 构建请求体
    std::string buildRequestBody(const std::vector<AIMessage>& messages);

    // 解析响应
    AIResponse parseResponse(const std::string& response);

    AIConfig m_config;
    std::mutex m_mutex;
    bool m_initialized = false;

    // 模型列表缓存
    std::vector<std::string> m_availableModels;
    bool m_fetchingModels = false;
};

} // namespace spark
