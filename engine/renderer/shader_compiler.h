#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace spark {

// 着色器类型
enum class ShaderType {
    Vertex,
    Fragment,
    Geometry,
    Compute,
    TessControl,
    TessEvaluation
};

// 着色器编译结果
struct ShaderCompileResult {
    bool success = false;
    std::vector<uint32_t> spirvCode;
    std::string errorMessage;
    std::string warningMessage;
};

// 着色器编译器
class ShaderCompiler {
public:
    static ShaderCompiler& get();

    // 初始化
    bool initialize();
    void shutdown();

    // 编译着色器
    ShaderCompileResult compile(const std::string& source, ShaderType type, const std::string& entryPoint = "main");
    ShaderCompileResult compileFile(const std::string& filepath, ShaderType type, const std::string& entryPoint = "main");

    // 缓存
    bool hasCachedShader(const std::string& key) const;
    std::vector<uint32_t> getCachedShader(const std::string& key) const;
    void cacheShader(const std::string& key, const std::vector<uint32_t>& code);

    // 热重载
    void watchFile(const std::string& filepath);
    void checkForChanges();

private:
    ShaderCompiler() = default;

    std::unordered_map<std::string, std::vector<uint32_t>> m_cache;
    std::vector<std::string> m_watchedFiles;
    bool m_initialized = false;
};

} // namespace spark
