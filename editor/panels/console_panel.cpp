#include "console_panel.h"
#include "ui/widgets.h"
#include <imgui.h>
#include <cstdarg>
#include <cstdio>

namespace spark {

ConsolePanel::ConsolePanel() {
    // 添加欢迎信息
    addInfo("Welcome to Spark Engine Console");
    addInfo("Type 'help' for available commands");
}

ConsolePanel::~ConsolePanel() = default;

void ConsolePanel::render() {
    ImGui::Begin("Console");

    // 工具栏
    renderToolbar();

    ImGui::Separator();

    // 日志区域
    renderLogEntries();

    // 输入框
    renderInput();

    ImGui::End();
}

void ConsolePanel::renderToolbar() {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));

    // 清除按钮
    if (ImGui::SmallButton(ui::icons::Close)) {
        clear();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Clear Console");
    }

    ImGui::SameLine();

    // 过滤按钮
    bool showInfo = m_showInfo;
    if (showInfo) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.55f, 0.90f, 0.60f));
    }

    if (ImGui::SmallButton(ui::icons::Info)) {
        m_showInfo = !m_showInfo;
    }

    if (showInfo) {
        ImGui::PopStyleColor();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Toggle Info");
    }

    ImGui::SameLine();

    bool showWarning = m_showWarning;
    if (showWarning) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.95f, 0.70f, 0.20f, 0.60f));
    }

    if (ImGui::SmallButton(ui::icons::Warning)) {
        m_showWarning = !m_showWarning;
    }

    if (showWarning) {
        ImGui::PopStyleColor();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Toggle Warnings");
    }

    ImGui::SameLine();

    bool showError = m_showError;
    if (showError) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.90f, 0.25f, 0.25f, 0.60f));
    }

    if (ImGui::SmallButton(ui::icons::Error)) {
        m_showError = !m_showError;
    }

    if (showError) {
        ImGui::PopStyleColor();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Toggle Errors");
    }

    ImGui::SameLine();
    ui::drawToolbarSeparator();

    // 自动滚动
    ImGui::Checkbox("Auto-scroll", &m_autoScroll);

    ImGui::SameLine();

    // 日志计数
    ImGui::TextDisabled("%d logs", (int)m_logs.size());

    ImGui::PopStyleVar(2);
}

void ConsolePanel::renderLogEntries() {
    // 日志区域
    ImGui::BeginChild("LogRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto& log : m_logs) {
        // 过滤
        if (log.level == LogLevel::Info && !m_showInfo) continue;
        if (log.level == LogLevel::Warning && !m_showWarning) continue;
        if (log.level == LogLevel::Error && !m_showError) continue;

        // 图标和颜色
        const char* icon;
        ImVec4 color;

        switch (log.level) {
            case LogLevel::Info:
                icon = ui::icons::Info;
                color = ImVec4(0.40f, 0.70f, 0.95f, 1.00f);
                break;
            case LogLevel::Warning:
                icon = ui::icons::Warning;
                color = ImVec4(0.95f, 0.70f, 0.20f, 1.00f);
                break;
            case LogLevel::Error:
                icon = ui::icons::Error;
                color = ImVec4(0.90f, 0.25f, 0.25f, 1.00f);
                break;
        }

        // 绘制日志条目
        ImGui::PushStyleColor(ImGuiCol_Text, color);

        // 图标
        ImGui::Text("%s", icon);
        ImGui::SameLine();

        // 消息
        ImGui::TextWrapped("%s", log.message.c_str());

        ImGui::PopStyleColor();
    }

    // 自动滚动到底部
    if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
}

void ConsolePanel::renderInput() {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    // 输入框
    bool reclaimFocus = false;
    ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue;

    if (ImGui::InputText("##ConsoleInput", m_inputBuffer, sizeof(m_inputBuffer), inputFlags)) {
        if (m_inputBuffer[0] != '\0') {
            executeCommand(m_inputBuffer);
            memset(m_inputBuffer, 0, sizeof(m_inputBuffer));
            reclaimFocus = true;
        }
    }

    // 自动聚焦
    ImGui::SetItemDefaultFocus();
    if (reclaimFocus) {
        ImGui::SetKeyboardFocusHere(-1);
    }

    ImGui::PopStyleVar(2);
}

void ConsolePanel::addLog(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    m_logs.push_back({LogLevel::Info, buffer});

    // 限制日志数量
    if ((int)m_logs.size() > m_maxLogs) {
        m_logs.erase(m_logs.begin());
    }
}

void ConsolePanel::addInfo(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    m_logs.push_back({LogLevel::Info, buffer});

    if ((int)m_logs.size() > m_maxLogs) {
        m_logs.erase(m_logs.begin());
    }
}

void ConsolePanel::addWarning(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    m_logs.push_back({LogLevel::Warning, buffer});

    if ((int)m_logs.size() > m_maxLogs) {
        m_logs.erase(m_logs.begin());
    }
}

void ConsolePanel::addError(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    m_logs.push_back({LogLevel::Error, buffer});

    if ((int)m_logs.size() > m_maxLogs) {
        m_logs.erase(m_logs.begin());
    }
}

void ConsolePanel::clear() {
    m_logs.clear();
    addInfo("Console cleared");
}

void ConsolePanel::executeCommand(const std::string& command) {
    // 回显命令
    addInfo("> %s", command.c_str());

    // 命令处理
    if (command == "help") {
        addInfo("Available commands:");
        addInfo("  help     - Show this help");
        addInfo("  clear    - Clear console");
        addInfo("  version  - Show engine version");
        addInfo("  fps      - Show FPS statistics");
        addInfo("  entities - Show entity count");
    } else if (command == "clear") {
        clear();
    } else if (command == "version") {
        addInfo("Spark Engine v0.10.0");
    } else if (command == "fps") {
        addInfo("FPS: Use the status bar to view real-time FPS");
    } else if (command == "entities") {
        addInfo("Use the Scene Hierarchy panel to view entities");
    } else {
        addWarning("Unknown command: %s", command.c_str());
        addInfo("Type 'help' for available commands");
    }
}

} // namespace spark
