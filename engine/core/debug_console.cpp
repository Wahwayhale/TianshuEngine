#include "debug_console.h"
#include "profiler.h"
#include <algorithm>
#include <iostream>

namespace spark {

DebugConsole& DebugConsole::get() {
    static DebugConsole instance;
    return instance;
}

DebugConsole::DebugConsole() {
    // Register built-in commands
    registerCommand("help", [this](const auto& args) { cmdHelp(args); }, "Show available commands");
    registerCommand("clear", [this](const auto& args) { cmdClear(args); }, "Clear console");
    registerCommand("fps", [this](const auto& args) { cmdFPS(args); }, "Show FPS statistics");
    registerCommand("commands", [this](const auto& args) { cmdListCommands(args); }, "List all commands");
}

void DebugConsole::log(const std::string& message, LogLevel level) {
    std::lock_guard<std::mutex> lock(m_mutex);

    ConsoleMessage msg;
    msg.text = message;
    msg.level = level;
    msg.timestamp = 0.0f;  // Would use actual time

    m_messages.push_back(msg);

    // Also output to stdout
    switch (level) {
        case LogLevel::Info:    std::cout << "[INFO] " << message << std::endl; break;
        case LogLevel::Warning: std::cout << "[WARN] " << message << std::endl; break;
        case LogLevel::Error:   std::cerr << "[ERROR] " << message << std::endl; break;
        case LogLevel::Debug:   std::cout << "[DEBUG] " << message << std::endl; break;
    }
}

void DebugConsole::logInfo(const std::string& message) {
    log(message, LogLevel::Info);
}

void DebugConsole::logWarning(const std::string& message) {
    log(message, LogLevel::Warning);
}

void DebugConsole::logError(const std::string& message) {
    log(message, LogLevel::Error);
}

void DebugConsole::logDebug(const std::string& message) {
    log(message, LogLevel::Debug);
}

bool DebugConsole::executeCommand(const std::string& command) {
    if (command.empty()) return false;

    addToHistory(command);

    // Parse command and arguments
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }

    // Find and execute command
    auto it = m_commands.find(cmd);
    if (it != m_commands.end()) {
        it->second(args);
        return true;
    } else {
        logError("Unknown command: " + cmd + ". Type 'help' for available commands.");
        return false;
    }
}

void DebugConsole::registerCommand(const std::string& name, CommandHandler handler, const std::string& description) {
    m_commands[name] = handler;
    m_commandDescriptions[name] = description;
}

void DebugConsole::clearMessages() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_messages.clear();
}

std::vector<std::string> DebugConsole::autoComplete(const std::string& partial) {
    std::vector<std::string> results;

    for (const auto& pair : m_commands) {
        if (pair.first.substr(0, partial.size()) == partial) {
            results.push_back(pair.first);
        }
    }

    std::sort(results.begin(), results.end());
    return results;
}

void DebugConsole::addToHistory(const std::string& command) {
    m_history.push_back(command);
    if (m_history.size() > 100) {
        m_history.erase(m_history.begin());
    }
}

void DebugConsole::cmdHelp(const std::vector<std::string>& args) {
    logInfo("Available commands:");
    for (const auto& pair : m_commandDescriptions) {
        logInfo("  " + pair.first + " - " + pair.second);
    }
}

void DebugConsole::cmdClear(const std::vector<std::string>& args) {
    clearMessages();
}

void DebugConsole::cmdFPS(const std::vector<std::string>& args) {
    auto& stats = PerformanceMonitor::get().getStats();
    logInfo("FPS: " + std::to_string(PerformanceMonitor::get().getAverageFPS()));
    logInfo("Frame Time: " + std::to_string(stats.frameTime) + " ms");
    logInfo("Draw Calls: " + std::to_string(stats.drawCalls));
    logInfo("Triangles: " + std::to_string(stats.triangles));
    logInfo("Vertices: " + std::to_string(stats.vertices));
}

void DebugConsole::cmdListCommands(const std::vector<std::string>& args) {
    logInfo("Registered commands:");
    for (const auto& pair : m_commands) {
        logInfo("  " + pair.first);
    }
}

} // namespace spark
