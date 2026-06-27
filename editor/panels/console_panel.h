#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace spark {

class ConsolePanel {
public:
    ConsolePanel();
    ~ConsolePanel();

    void render();

    // 日志方法
    void addLog(const char* fmt, ...);
    void addInfo(const char* fmt, ...);
    void addWarning(const char* fmt, ...);
    void addError(const char* fmt, ...);
    void clear();

private:
    enum class LogLevel {
        Info,
        Warning,
        Error
    };

    struct LogEntry {
        LogLevel level;
        std::string message;
    };

    void renderToolbar();
    void renderLogEntries();
    void renderInput();

    void executeCommand(const std::string& command);

    std::vector<LogEntry> m_logs;
    char m_inputBuffer[256] = "";
    bool m_autoScroll = true;
    bool m_showInfo = true;
    bool m_showWarning = true;
    bool m_showError = true;
    int m_maxLogs = 1000;
};

} // namespace spark
