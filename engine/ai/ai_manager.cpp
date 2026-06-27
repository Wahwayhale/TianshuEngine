#include "ai_manager.h"
#include "core/log.h"
#include <sstream>
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#endif

namespace spark {

AIManager& AIManager::get() {
    static AIManager instance;
    return instance;
}

bool AIManager::initialize() {
    if (m_initialized) return true;

    m_initialized = true;
    SPARK_CORE_INFO("AI Manager initialized.");
    return true;
}

void AIManager::shutdown() {
    if (!m_initialized) return;
    m_initialized = false;
    SPARK_CORE_INFO("AI Manager shutdown.");
}

void AIManager::setConfig(const AIConfig& config) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_config = config;

    if (config.enabled && !config.apiKey.empty()) {
        SPARK_CORE_INFO("AI configured: {0} ({1})", config.modelName, config.apiEndpoint);
    }
}

// JSON 转义辅助函数
std::string escapeJson(const std::string& s) {
    std::string result;
    for (char c : s) {
        switch (c) {
            case '"':  result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                    result += buf;
                } else {
                    result += c;
                }
        }
    }
    return result;
}

AIResponse AIManager::generateCode(const std::string& description) {
    std::vector<AIMessage> messages = {
        {"system", R"(你是一个游戏开发助手。根据用户的描述，生成可直接使用的 Lua 脚本代码。

要求：
1. 代码必须是完整的、可直接运行的
2. 使用 Spark Engine 的 API
3. 包含 OnInit、OnUpdate、OnDestroy 函数
4. 添加必要的注释
5. 只输出代码，不要其他解释

可用的 API：
- entity.position - 获取/设置位置 (Vec3)
- entity.rotation - 获取/设置旋转 (Vec3)
- entity.scale - 获取/设置缩放 (Vec3)
- Input.isKeyPressed(key) - 检查按键
- Input.isMouseButtonPressed(button) - 检查鼠标按键
- Input.getMousePosition() - 获取鼠标位置
- Time.deltaTime - 帧时间
- Time.totalTime - 总时间
- Log.info(msg) - 输出日志
- math.sin/cos/tan/abs/sqrt - 数学函数
- Vec3(x, y, z) - 创建向量)"},
        {"user", description}
    };

    return chat(messages);
}

AIResponse AIManager::generateScript(const std::string& description) {
    return generateCode(description);
}

AIResponse AIManager::generateAssetDescription(const std::string& type, const std::string& description) {
    std::vector<AIMessage> messages = {
        {"system", R"(你是一个游戏资产描述助手。根据用户的需求，生成详细的资产描述。

输出格式：
- 名称
- 类型
- 风格
- 颜色
- 尺寸
- 其他特征

用于 AI 图像/模型生成)"},
        {"user", "类型: " + type + "\n需求: " + description}
    };

    return chat(messages);
}

AIResponse AIManager::generateSceneDescription(const std::string& description) {
    std::vector<AIMessage> messages = {
        {"system", R"(你是一个游戏场景设计助手。根据用户的描述，生成场景布局。

输出格式：
- 场景名称
- 场景类型
- 包含的物体列表（名称、位置、大小）
- 光照设置
- 氛围描述)"},
        {"user", description}
    };

    return chat(messages);
}

AIResponse AIManager::generateNPCBehavior(const std::string& description) {
    std::vector<AIMessage> messages = {
        {"system", R"(你是一个游戏 AI 助手。根据用户的描述，生成 NPC 行为脚本。

要求：
1. 生成 Lua 脚本
2. 包含状态机逻辑
3. 使用 Spark Engine API
4. 可直接使用)"},
        {"user", description}
    };

    return chat(messages);
}

AIResponse AIManager::generateDialogue(const std::string& character, const std::string& situation) {
    std::vector<AIMessage> messages = {
        {"system", R"(你是一个游戏对话生成助手。根据角色和情境，生成对话内容。

输出格式：
- 角色名称
- 对话内容
- 情感/语气
- 选项（如果有多选)"},
        {"user", "角色: " + character + "\n情境: " + situation}
    };

    return chat(messages);
}

AIResponse AIManager::chat(const std::string& message) {
    std::vector<AIMessage> messages = {
        {"user", message}
    };
    return chat(messages);
}

AIResponse AIManager::chat(const std::vector<AIMessage>& messages) {
    std::lock_guard<std::mutex> lock(m_mutex);

    AIResponse response;

    if (!m_config.enabled || m_config.apiKey.empty()) {
        response.success = false;
        response.error = "AI not configured. Please set API key and endpoint.";
        return response;
    }

    try {
        std::string body = buildRequestBody(messages);
        std::string endpoint = m_config.apiEndpoint;

        // 确保 endpoint 以 / 结尾
        if (!endpoint.empty() && endpoint.back() != '/') {
            endpoint += "/";
        }
        endpoint += "chat/completions";

        SPARK_CORE_INFO("AI Request: {0}", endpoint);

        std::string result = sendRequest(endpoint, body, false);

        SPARK_CORE_INFO("AI Response received, length: {0}", result.size());

        return parseResponse(result);

    } catch (const std::exception& e) {
        response.success = false;
        response.error = std::string("AI request failed: ") + e.what();
        SPARK_CORE_ERROR("AI Error: {0}", response.error);
        return response;
    }
}

AIResponse AIManager::testConnection() {
    std::vector<AIMessage> messages = {
        {"user", "Hello, please respond with 'OK' to test the connection."}
    };

    auto response = chat(messages);
    if (response.success) {
        SPARK_CORE_INFO("AI connection test successful.");
    } else {
        SPARK_CORE_ERROR("AI connection test failed: {0}", response.error);
    }

    return response;
}

std::vector<std::string> AIManager::fetchAvailableModels() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_config.enabled || m_config.apiKey.empty()) {
        return {};
    }

    m_fetchingModels = true;

    try {
        std::string endpoint = m_config.apiEndpoint;
        if (!endpoint.empty() && endpoint.back() != '/') {
            endpoint += "/";
        }
        endpoint += "models";

        std::string result = sendRequest(endpoint, "", true);

        m_availableModels.clear();

        // 解析模型列表
        size_t pos = 0;
        while ((pos = result.find("\"id\"", pos)) != std::string::npos) {
            size_t valueStart = result.find("\"", pos + 4);
            if (valueStart == std::string::npos) break;
            valueStart++;

            size_t valueEnd = result.find("\"", valueStart);
            if (valueEnd == std::string::npos) break;

            std::string modelId = result.substr(valueStart, valueEnd - valueStart);
            if (!modelId.empty()) {
                m_availableModels.push_back(modelId);
            }

            pos = valueEnd + 1;
        }

        if (m_availableModels.empty()) {
            m_availableModels = {
                "qwen-turbo",
                "qwen-plus",
                "qwen-max",
                "glm-4-flash",
                "glm-4",
                "Baichuan4",
                "deepseek-chat",
                "deepseek-coder",
                "moonshot-v1-8k",
                "moonshot-v1-32k",
                "ernie-speed-128k"
            };
        }

        m_fetchingModels = false;
        SPARK_CORE_INFO("Fetched {0} models from API.", m_availableModels.size());
        return m_availableModels;

    } catch (const std::exception& e) {
        m_fetchingModels = false;
        SPARK_CORE_ERROR("Failed to fetch models: {0}", e.what());
        return m_availableModels;
    }
}

std::string AIManager::sendRequest(const std::string& endpoint, const std::string& body, bool isGet) {
#ifdef _WIN32
    // 解析 URL
    std::string host, path;
    int port = 443;
    bool isHttps = true;

    size_t protocolEnd = endpoint.find("://");
    if (protocolEnd != std::string::npos) {
        std::string protocol = endpoint.substr(0, protocolEnd);
        isHttps = (protocol == "https");

        size_t hostStart = protocolEnd + 3;
        size_t pathStart = endpoint.find("/", hostStart);

        if (pathStart != std::string::npos) {
            host = endpoint.substr(hostStart, pathStart - hostStart);
            path = endpoint.substr(pathStart);
        } else {
            host = endpoint.substr(hostStart);
            path = "/";
        }

        size_t portStart = host.find(":");
        if (portStart != std::string::npos) {
            port = std::stoi(host.substr(portStart + 1));
            host = host.substr(0, portStart);
        }
    }

    // 转换为宽字符
    std::wstring whost(host.begin(), host.end());
    std::wstring wpath(path.begin(), path.end());

    // 打开 HTTP 会话
    HINTERNET hSession = WinHttpOpen(L"SparkEngine/1.0",
                                      WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                      WINHTTP_NO_PROXY_NAME,
                                      WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        throw std::runtime_error("Failed to open HTTP session");
    }

    // 设置超时
    WinHttpSetTimeouts(hSession, 30000, 30000, 30000, 30000);

    // 连接到服务器
    HINTERNET hConnect = WinHttpConnect(hSession, whost.c_str(),
                                         isHttps ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        throw std::runtime_error("Failed to connect to server: " + host);
    }

    // 创建请求
    LPCWSTR method = isGet ? L"GET" : L"POST";
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, method, wpath.c_str(),
                                             NULL, WINHTTP_NO_REFERER,
                                             WINHTTP_DEFAULT_ACCEPT_TYPES,
                                             isHttps ? WINHTTP_FLAG_SECURE : 0);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        throw std::runtime_error("Failed to create request");
    }

    // 启用 TLS 1.2/1.3
    if (isHttps) {
        DWORD secureProtocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2 | WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_3;
        WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURE_PROTOCOLS, &secureProtocols, sizeof(secureProtocols));
    }

    // 设置请求头
    std::string authHeader = "Authorization: Bearer " + m_config.apiKey;
    std::wstring wAuthHeader(authHeader.begin(), authHeader.end());
    std::wstring wContentType = L"Content-Type: application/json";

    WinHttpAddRequestHeaders(hRequest, wAuthHeader.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD);
    WinHttpAddRequestHeaders(hRequest, wContentType.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD);

    // 发送请求
    BOOL result;
    if (isGet) {
        result = WinHttpSendRequest(hRequest,
                                     WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                     WINHTTP_NO_REQUEST_DATA, 0,
                                     0, 0);
    } else {
        result = WinHttpSendRequest(hRequest,
                                     WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                     (LPVOID)body.c_str(), static_cast<DWORD>(body.size()),
                                     static_cast<DWORD>(body.size()), 0);
    }

    if (!result) {
        DWORD error = GetLastError();
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        throw std::runtime_error("Failed to send request, error code: " + std::to_string(error));
    }

    // 接收响应
    result = WinHttpReceiveResponse(hRequest, NULL);
    if (!result) {
        DWORD error = GetLastError();
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        throw std::runtime_error("Failed to receive response, error code: " + std::to_string(error));
    }

    // 检查 HTTP 状态码
    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    WinHttpQueryHeaders(hRequest,
                         WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                         WINHTTP_HEADER_NAME_BY_INDEX,
                         &statusCode, &statusCodeSize, WINHTTP_NO_HEADER_INDEX);

    if (statusCode != 200) {
        // 读取错误响应
        std::string errorResponse;
        DWORD bytesRead = 0;
        DWORD bytesAvailable = 0;

        do {
            WinHttpQueryDataAvailable(hRequest, &bytesAvailable);
            if (bytesAvailable > 0) {
                char* buffer = new char[bytesAvailable + 1];
                WinHttpReadData(hRequest, buffer, bytesAvailable, &bytesRead);
                buffer[bytesRead] = '\0';
                errorResponse += buffer;
                delete[] buffer;
            }
        } while (bytesAvailable > 0);

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        throw std::runtime_error("HTTP error " + std::to_string(statusCode) + ": " + errorResponse);
    }

    // 读取响应数据
    std::string response;
    DWORD bytesRead = 0;
    DWORD bytesAvailable = 0;

    do {
        WinHttpQueryDataAvailable(hRequest, &bytesAvailable);
        if (bytesAvailable > 0) {
            char* buffer = new char[bytesAvailable + 1];
            WinHttpReadData(hRequest, buffer, bytesAvailable, &bytesRead);
            buffer[bytesRead] = '\0';
            response += buffer;
            delete[] buffer;
        }
    } while (bytesAvailable > 0);

    // 清理
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return response;
#else
    // Linux/macOS 使用 curl
    std::string command;
    if (isGet) {
        command = "curl -s -X GET '" + endpoint + "'";
    } else {
        command = "curl -s -X POST '" + endpoint + "'";
    }
    command += " -H 'Content-Type: application/json'";
    command += " -H 'Authorization: Bearer " + m_config.apiKey + "'";
    command += " --connect-timeout 30";
    command += " --max-time 60";

    if (!isGet && !body.empty()) {
        // 使用临时文件传递 body 避免 shell 转义问题
        std::string tmpFile = "/tmp/spark_ai_request.json";
        std::ofstream ofs(tmpFile);
        ofs << body;
        ofs.close();
        command += " -d @" + tmpFile;
    }

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("Failed to execute curl");
    }

    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }
    int exitCode = pclose(pipe);

    if (exitCode != 0) {
        throw std::runtime_error("curl failed with exit code: " + std::to_string(exitCode));
    }

    return result;
#endif
}

std::string AIManager::buildRequestBody(const std::vector<AIMessage>& messages) {
    std::stringstream ss;
    ss << "{";
    ss << "\"model\":\"" << escapeJson(m_config.modelName) << "\",";
    ss << "\"messages\":[";

    for (size_t i = 0; i < messages.size(); i++) {
        if (i > 0) ss << ",";
        ss << "{";
        ss << "\"role\":\"" << escapeJson(messages[i].role) << "\",";
        ss << "\"content\":\"" << escapeJson(messages[i].content) << "\"";
        ss << "}";
    }

    ss << "],";
    ss << "\"temperature\":" << m_config.temperature << ",";
    ss << "\"max_tokens\":" << m_config.maxTokens << ",";
    ss << "\"top_p\":" << m_config.topP;
    ss << "}";

    return ss.str();
}

AIResponse AIManager::parseResponse(const std::string& response) {
    AIResponse result;

    // 检查是否为空响应
    if (response.empty()) {
        result.success = false;
        result.error = "Empty response from API";
        return result;
    }

    // 检查是否有错误
    size_t errorPos = response.find("\"error\"");
    if (errorPos != std::string::npos) {
        size_t messagePos = response.find("\"message\"", errorPos);
        if (messagePos != std::string::npos) {
            size_t valueStart = response.find("\"", messagePos + 9);
            if (valueStart != std::string::npos) {
                valueStart++;
                size_t valueEnd = response.find("\"", valueStart);
                if (valueEnd != std::string::npos) {
                    result.error = response.substr(valueStart, valueEnd - valueStart);
                    result.success = false;
                    return result;
                }
            }
        }
    }

    // 查找 choices
    size_t choicesPos = response.find("\"choices\"");
    if (choicesPos == std::string::npos) {
        result.success = false;
        result.error = "Invalid response format: no 'choices' field";
        return result;
    }

    // 查找 message 中的 content
    size_t contentPos = response.find("\"content\"", choicesPos);
    if (contentPos == std::string::npos) {
        // 尝试查找 delta 中的 content（流式响应）
        contentPos = response.find("\"content\":", choicesPos);
        if (contentPos == std::string::npos) {
            result.success = false;
            result.error = "No content in response";
            return result;
        }
    }

    // 找到 content 值的开始
    size_t valueStart = response.find(":", contentPos);
    if (valueStart == std::string::npos) {
        result.success = false;
        result.error = "Invalid content format";
        return result;
    }
    valueStart++;

    // 跳过空白
    while (valueStart < response.size() && (response[valueStart] == ' ' || response[valueStart] == '\t')) {
        valueStart++;
    }

    // 检查是否是字符串
    if (valueStart >= response.size() || response[valueStart] != '"') {
        result.success = false;
        result.error = "Content is not a string";
        return result;
    }
    valueStart++;  // 跳过引号

    // 找到结束引号（处理转义）
    std::string content;
    size_t valueEnd = valueStart;
    while (valueEnd < response.size()) {
        if (response[valueEnd] == '\\') {
            // 处理转义字符
            if (valueEnd + 1 < response.size()) {
                char next = response[valueEnd + 1];
                switch (next) {
                    case 'n': content += '\n'; break;
                    case 't': content += '\t'; break;
                    case 'r': content += '\r'; break;
                    case '\\': content += '\\'; break;
                    case '"': content += '"'; break;
                    default: content += next; break;
                }
                valueEnd += 2;
            } else {
                valueEnd++;
            }
        } else if (response[valueEnd] == '"') {
            break;
        } else {
            content += response[valueEnd];
            valueEnd++;
        }
    }

    result.content = content;
    result.success = true;

    // 提取 token 使用量
    size_t tokensPos = response.find("\"total_tokens\"");
    if (tokensPos != std::string::npos) {
        size_t colonPos = response.find(":", tokensPos);
        if (colonPos != std::string::npos) {
            try {
                result.tokensUsed = std::stoi(response.substr(colonPos + 1));
            } catch (...) {}
        }
    }

    return result;
}

} // namespace spark
