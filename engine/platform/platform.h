#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace spark {

// 平台类型
enum class PlatformType {
    Windows,
    Linux,
    macOS,
    Android,
    iOS,
    Unknown
};

// 平台抽象层
class Platform {
public:
    static Platform& get();

    // 初始化/关闭
    bool initialize();
    void shutdown();

    // 平台信息
    PlatformType getType() const;
    std::string getTypeName() const;
    std::string getOSVersion() const;
    std::string getCPUInfo() const;
    size_t getTotalMemory() const;
    size_t getAvailableMemory() const;

    // 文件系统
    std::string getExecutablePath() const;
    std::string getWorkingDirectory() const;
    std::string getUserDataPath() const;
    std::string getTempPath() const;

    bool fileExists(const std::string& path) const;
    bool directoryExists(const std::string& path) const;
    bool createDirectory(const std::string& path) const;
    bool deleteFile(const std::string& path) const;

    // 动态库
    void* loadLibrary(const std::string& path);
    void* getSymbol(void* library, const std::string& name);
    void freeLibrary(void* library);

    // 时间
    double getTime() const;  // 秒
    uint64_t getTicks() const;

    // 线程
    uint32_t getThreadCount() const;
    void sleep(uint32_t milliseconds);

    // 消息框
    enum class MessageType {
        Info,
        Warning,
        Error
    };

    void showMessage(const std::string& title, const std::string& message, MessageType type = MessageType::Info);

    // 剪贴板
    std::string getClipboard() const;
    void setClipboard(const std::string& text);

    // 网络
    bool isNetworkAvailable() const;

private:
    Platform() = default;
    ~Platform() = default;

    bool m_initialized = false;
};

} // namespace spark
