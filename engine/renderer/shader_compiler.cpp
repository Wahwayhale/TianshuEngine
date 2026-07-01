#include "shader_compiler.h"
#include "core/log.h"
#include <fstream>

namespace spark {

ShaderCompiler& ShaderCompiler::get() {
    static ShaderCompiler instance;
    return instance;
}

bool ShaderCompiler::initialize() {
    if (m_initialized) return true;

    m_initialized = true;
    SPARK_CORE_INFO("Shader compiler initialized.");
    return true;
}

void ShaderCompiler::shutdown() {
    m_cache.clear();
    m_watchedFiles.clear();
    m_initialized = false;
    SPARK_CORE_INFO("Shader compiler shutdown.");
}

ShaderCompileResult ShaderCompiler::compile(const std::string& source, ShaderType type, const std::string& entryPoint) {
    ShaderCompileResult result;

    // 注意：实际实现需要调用 glslc 或 glslangValidator
    // 这里是简化的实现

    result.success = false;
    result.errorMessage = "Shader compilation not implemented yet.";

    return result;
}

ShaderCompileResult ShaderCompiler::compileFile(const std::string& filepath, ShaderType type, const std::string& entryPoint) {
    // 读取文件
    std::ifstream file(filepath);
    if (!file.is_open()) {
        ShaderCompileResult result;
        result.success = false;
        result.errorMessage = "Failed to open shader file: " + filepath;
        return result;
    }

    std::string source((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    return compile(source, type, entryPoint);
}

bool ShaderCompiler::hasCachedShader(const std::string& key) const {
    return m_cache.find(key) != m_cache.end();
}

std::vector<uint32_t> ShaderCompiler::getCachedShader(const std::string& key) const {
    auto it = m_cache.find(key);
    if (it != m_cache.end()) {
        return it->second;
    }
    return {};
}

void ShaderCompiler::cacheShader(const std::string& key, const std::vector<uint32_t>& code) {
    m_cache[key] = code;
}

void ShaderCompiler::watchFile(const std::string& filepath) {
    m_watchedFiles.push_back(filepath);
}

void ShaderCompiler::checkForChanges() {
    // TODO: 检查文件变化并重新编译
}

} // namespace spark
