#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <sstream>
#include <mutex>

namespace spark {

enum class LogLevel {
    Info,
    Warning,
    Error,
    Debug
};

struct ConsoleMessage {
    std::string text;
    LogLevel level;
    float timestamp;
};

class DebugConsole {
public:
    static DebugConsole& get();

    // Log messages
    void log(const std::string& message, LogLevel level = LogLevel::Info);
    void logInfo(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);
    void logDebug(const std::string& message);

    // Execute command
    bool executeCommand(const std::string& command);

    // Register command handler
    using CommandHandler = std::function<void(const std::vector<std::string>&)>;
    void registerCommand(const std::string& name, CommandHandler handler, const std::string& description = "");

    // Get messages
    const std::vector<ConsoleMessage>& getMessages() const { return m_messages; }
    void clearMessages();

    // Auto-complete
    std::vector<std::string> autoComplete(const std::string& partial);

    // History
    void addToHistory(const std::string& command);
    const std::vector<std::string>& getHistory() const { return m_history; }

private:
    DebugConsole();

    std::vector<ConsoleMessage> m_messages;
    std::unordered_map<std::string, CommandHandler> m_commands;
    std::unordered_map<std::string, std::string> m_commandDescriptions;
    std::vector<std::string> m_history;
    std::mutex m_mutex;

    // Built-in commands
    void cmdHelp(const std::vector<std::string>& args);
    void cmdClear(const std::vector<std::string>& args);
    void cmdFPS(const std::vector<std::string>& args);
    void cmdListCommands(const std::vector<std::string>& args);
};

} // namespace spark
