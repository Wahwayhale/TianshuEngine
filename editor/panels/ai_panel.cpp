#include "ai_panel.h"
#include "ai/ai_manager.h"
#include "ui/widgets.h"
#include <imgui.h>
#include <ctime>

namespace spark {

AIAssistantPanel::AIAssistantPanel() {
    // 加载保存的配置
    // TODO: 从文件加载配置

    // 默认使用 DeepSeek（便宜且效果好）
    m_selectedProvider = 3;  // DeepSeek
    strncpy(m_modelNameBuffer, "deepseek-chat", sizeof(m_modelNameBuffer));
    strncpy(m_apiEndpointBuffer, "https://api.deepseek.com/v1", sizeof(m_apiEndpointBuffer));
}

AIAssistantPanel::~AIAssistantPanel() = default;

void AIAssistantPanel::render() {
    ImGui::Begin("AI Assistant");

    // 顶部工具栏
    if (ImGui::Button(ui::icons::Settings)) {
        m_showConfig = !m_showConfig;
    }
    ImGui::SameLine();
    ImGui::Text("AI Assistant");
    ImGui::SameLine();

    // 状态指示
    if (AIManager::get().isConfigured()) {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "● Connected");
    } else {
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "● Not Configured");
    }

    ImGui::Separator();

    // 配置面板
    if (m_showConfig) {
        renderConfig();
        ImGui::Separator();
    }

    // 快捷操作
    renderQuickActions();
    ImGui::Separator();

    // 聊天区域
    renderChat();

    ImGui::End();
}

void AIAssistantPanel::renderConfig() {
    ImGui::Text("AI Configuration");
    ImGui::Spacing();

    // 提供商选择
    const char* providers[] = {
        "通义千问 (Qwen)",
        "智谱 ChatGLM",
        "百川 Baichuan",
        "深度求索 DeepSeek",
        "月之暗面 Kimi",
        "讯飞星火",
        "百度文心",
        "自定义 (OpenAI 兼容)",
        "本地模型 (Ollama)"
    };

    if (ImGui::Combo("Provider", &m_selectedProvider, providers, IM_ARRAYSIZE(providers))) {
        // 更新默认配置
        switch (m_selectedProvider) {
            case 0:  // Qwen
                strncpy(m_apiEndpointBuffer, "https://dashscope.aliyuncs.com/compatible-mode/v1", sizeof(m_apiEndpointBuffer));
                strncpy(m_modelNameBuffer, "qwen-turbo", sizeof(m_modelNameBuffer));
                break;
            case 1:  // ChatGLM
                strncpy(m_apiEndpointBuffer, "https://open.bigmodel.cn/api/paas/v4", sizeof(m_apiEndpointBuffer));
                strncpy(m_modelNameBuffer, "glm-4-flash", sizeof(m_modelNameBuffer));
                break;
            case 2:  // Baichuan
                strncpy(m_apiEndpointBuffer, "https://api.baichuan-ai.com/v1", sizeof(m_apiEndpointBuffer));
                strncpy(m_modelNameBuffer, "Baichuan4", sizeof(m_modelNameBuffer));
                break;
            case 3:  // DeepSeek
                strncpy(m_apiEndpointBuffer, "https://api.deepseek.com/v1", sizeof(m_apiEndpointBuffer));
                strncpy(m_modelNameBuffer, "deepseek-chat", sizeof(m_modelNameBuffer));
                break;
            case 4:  // Kimi
                strncpy(m_apiEndpointBuffer, "https://api.moonshot.cn/v1", sizeof(m_apiEndpointBuffer));
                strncpy(m_modelNameBuffer, "moonshot-v1-8k", sizeof(m_modelNameBuffer));
                break;
            case 5:  // Spark
                strncpy(m_apiEndpointBuffer, "https://spark-api-open.xf-yun.com/v1", sizeof(m_apiEndpointBuffer));
                strncpy(m_modelNameBuffer, "general", sizeof(m_modelNameBuffer));
                break;
            case 6:  // ERNIE
                strncpy(m_apiEndpointBuffer, "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop", sizeof(m_apiEndpointBuffer));
                strncpy(m_modelNameBuffer, "ernie-speed-128k", sizeof(m_modelNameBuffer));
                break;
            case 7:  // Custom
                break;
            case 8:  // Local
                strncpy(m_apiEndpointBuffer, "http://localhost:11434/v1", sizeof(m_apiEndpointBuffer));
                strncpy(m_modelNameBuffer, "qwen2:7b", sizeof(m_modelNameBuffer));
                strncpy(m_apiKeyBuffer, "ollama", sizeof(m_apiKeyBuffer));
                break;
        }
    }

    // API Key
    ImGui::InputText("API Key", m_apiKeyBuffer, sizeof(m_apiKeyBuffer), ImGuiInputTextFlags_Password);

    // API Endpoint
    ImGui::InputText("API Endpoint", m_apiEndpointBuffer, sizeof(m_apiEndpointBuffer));

    // Model Name - 支持手动输入或从列表选择
    if (!m_availableModels.empty()) {
        // 从获取的模型列表中选择
        std::vector<const char*> modelNames;
        for (const auto& model : m_availableModels) {
            modelNames.push_back(model.c_str());
        }

        if (ImGui::Combo("Model", &m_selectedModel, modelNames.data(), static_cast<int>(modelNames.size()))) {
            strncpy(m_modelNameBuffer, m_availableModels[m_selectedModel].c_str(), sizeof(m_modelNameBuffer));
        }

        ImGui::SameLine();
        if (ImGui::SmallButton("Refresh")) {
            m_fetchingModels = true;
            m_availableModels = AIManager::get().fetchAvailableModels();
            m_fetchingModels = false;
        }

        if (m_fetchingModels) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Fetching...");
        }
    } else {
        // 手动输入模型名称
        ImGui::InputText("Model", m_modelNameBuffer, sizeof(m_modelNameBuffer));
    }

    // 生成参数
    ImGui::SliderFloat("Temperature", &m_config.temperature, 0.0f, 2.0f, "%.2f");
    ImGui::SliderInt("Max Tokens", &m_config.maxTokens, 256, 4096);

    ImGui::Spacing();

    // 保存和测试按钮
    if (ImGui::Button("Save & Connect")) {
        m_config.provider = static_cast<AIProvider>(m_selectedProvider);
        m_config.apiKey = m_apiKeyBuffer;
        m_config.apiEndpoint = m_apiEndpointBuffer;
        m_config.modelName = m_modelNameBuffer;
        m_config.enabled = true;

        AIManager::get().setConfig(m_config);

        // 测试连接
        auto response = AIManager::get().testConnection();
        if (response.success) {
            m_lastError.clear();

            // 自动获取模型列表
            m_fetchingModels = true;
            m_availableModels = AIManager::get().fetchAvailableModels();
            m_fetchingModels = false;

            // 查找当前模型在列表中的位置
            for (size_t i = 0; i < m_availableModels.size(); i++) {
                if (m_availableModels[i] == m_modelNameBuffer) {
                    m_selectedModel = static_cast<int>(i);
                    break;
                }
            }
        } else {
            m_lastError = response.error;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Test Connection")) {
        auto response = AIManager::get().testConnection();
        if (response.success) {
            m_lastError.clear();
            ImGui::OpenPopup("Test Success");
        } else {
            m_lastError = response.error;
            ImGui::OpenPopup("Test Failed");
        }
    }

    // 测试结果弹窗
    if (ImGui::BeginPopup("Test Success")) {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "✓ Connection successful!");
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Test Failed")) {
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "✗ Connection failed");
        ImGui::Text("%s", m_lastError.c_str());
        ImGui::EndPopup();
    }

    // 错误信息
    if (!m_lastError.empty()) {
        ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Error: %s", m_lastError.c_str());
    }
}

void AIAssistantPanel::renderQuickActions() {
    ImGui::Text("Quick Actions");
    ImGui::Spacing();

    // 代码生成按钮
    if (ImGui::Button("Generate Movement Script")) {
        sendMessage("生成一个让角色按 WASD 移动的脚本");
    }
    ImGui::SameLine();

    if (ImGui::Button("Generate Rotation Script")) {
        sendMessage("生成一个让物体旋转的脚本");
    }

    if (ImGui::Button("Generate NPC Patrol")) {
        sendMessage("生成一个 NPC 巡逻脚本，在几个点之间来回移动");
    }
    ImGui::SameLine();

    if (ImGui::Button("Generate Jump Script")) {
        sendMessage("生成一个角色跳跃脚本，支持二段跳");
    }

    ImGui::Spacing();
}

void AIAssistantPanel::renderChat() {
    ImGui::Text("Chat");
    ImGui::Spacing();

    // 聊天历史区域
    ImGui::BeginChild("ChatHistory", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2), true);

    for (const auto& msg : m_chatHistory) {
        if (msg.role == "user") {
            ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "You:");
        } else {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "AI:");
        }
        ImGui::SameLine();
        ImGui::TextWrapped("%s", msg.content.c_str());
        ImGui::Spacing();
    }

    // 自动滚动到底部
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();

    // 输入区域
    ImGui::PushItemWidth(-70);
    bool enterPressed = ImGui::InputText("##ChatInput", m_inputBuffer, sizeof(m_inputBuffer),
                                          ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::PopItemWidth();

    ImGui::SameLine();

    if (ImGui::Button("Send") || enterPressed) {
        if (strlen(m_inputBuffer) > 0) {
            sendMessage(m_inputBuffer);
            memset(m_inputBuffer, 0, sizeof(m_inputBuffer));
        }
    }

    // 生成状态
    if (m_isGenerating) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Generating...");
    }
}

void AIAssistantPanel::sendMessage(const std::string& message) {
    if (!AIManager::get().isConfigured()) {
        m_lastError = "Please configure AI first (click Settings icon)";
        return;
    }

    // 添加用户消息
    addMessage("user", message);

    // 生成 AI 响应
    m_isGenerating = true;

    auto response = AIManager::get().generateCode(message);

    m_isGenerating = false;

    if (response.success) {
        addMessage("assistant", response.content);
        m_lastError.clear();
    } else {
        addMessage("assistant", "Error: " + response.error);
        m_lastError = response.error;
    }
}

void AIAssistantPanel::addMessage(const std::string& role, const std::string& content) {
    ChatMessage msg;
    msg.role = role;
    msg.content = content;

    // 获取时间戳
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);
    msg.timestamp = timestamp;

    m_chatHistory.push_back(msg);

    // 限制历史长度
    if (m_chatHistory.size() > 100) {
        m_chatHistory.erase(m_chatHistory.begin());
    }
}

} // namespace spark
