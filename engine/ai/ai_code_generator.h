#pragma once

#include "ai_manager.h"
#include <string>
#include <vector>
#include <functional>

namespace spark {

// 代码生成选项
struct CodeGenOptions {
    std::string language = "lua";           // lua, cpp, glsl
    std::string style = "game";             // game, utility, ai
    bool addComments = true;
    bool addErrorHandling = true;
    int complexity = 1;                     // 1-5
};

// 代码模板
struct CodeTemplate {
    std::string name;
    std::string description;
    std::string category;
    std::string code;
};

// AI 代码生成器
class AICodeGenerator {
public:
    static AICodeGenerator& get();

    // 初始化
    bool initialize();
    void shutdown();

    // 代码生成
    AIResponse generateCode(const std::string& description, const CodeGenOptions& options = {});
    AIResponse generateScript(const std::string& description);

    // 代码补全
    AIResponse completeCode(const std::string& code, const std::string& context = "");

    // 代码解释
    AIResponse explainCode(const std::string& code);

    // 代码重构
    AIResponse refactorCode(const std::string& code, const std::string& instructions);

    // 错误修复
    AIResponse fixError(const std::string& code, const std::string& error);

    // 代码优化
    AIResponse optimizeCode(const std::string& code);

    // 添加注释
    AIResponse addComments(const std::string& code);

    // 代码翻译
    AIResponse translateCode(const std::string& code, const std::string& fromLang, const std::string& toLang);

    // 模板管理
    std::vector<CodeTemplate> getTemplates() const;
    std::vector<CodeTemplate> getTemplatesByCategory(const std::string& category) const;
    CodeTemplate getTemplate(const std::string& name) const;
    void addTemplate(const CodeTemplate& templ);
    void loadTemplates();
    void saveTemplates();

    // 历史记录
    struct CodeHistory {
        std::string description;
        std::string generatedCode;
        std::string timestamp;
    };

    void addToHistory(const std::string& description, const std::string& code);
    std::vector<CodeHistory> getHistory() const;
    void clearHistory();

private:
    AICodeGenerator() = default;
    ~AICodeGenerator() = default;

    std::vector<CodeTemplate> m_templates;
    std::vector<CodeHistory> m_history;
    bool m_initialized = false;
};

} // namespace spark
