#pragma once

#include <string>
#include <vector>
#include "ai/ai_config.h"

namespace spark {

class AIAssistantPanel {
public:
    AIAssistantPanel();
    ~AIAssistantPanel();

    void render();

private:
    void renderConfig();
    void renderChat();
    void renderQuickActions();
    void renderHistory();

    void sendMessage(const std::string& message);
    void addMessage(const std::string& role, const std::string& content);

    // 聊天历史
    struct ChatMessage {
        std::string role;
        std::string content;
        std::string timestamp;
    };

    std::vector<ChatMessage> m_chatHistory;
    char m_inputBuffer[4096] = "";
    bool m_showConfig = false;

    // 配置
    AIConfig m_config;
    int m_selectedProvider = 0;
    char m_apiKeyBuffer[512] = "";
    char m_apiEndpointBuffer[512] = "";
    char m_modelNameBuffer[128] = "";

    // 模型列表
    std::vector<std::string> m_availableModels;
    int m_selectedModel = 0;
    bool m_fetchingModels = false;

    // 状态
    bool m_isGenerating = false;
    std::string m_lastError;
};

} // namespace spark
