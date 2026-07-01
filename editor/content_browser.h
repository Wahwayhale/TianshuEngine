#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <functional>

namespace spark {

// 资产类型
enum class AssetType {
    Unknown,
    Texture,
    Model,
    Audio,
    Script,
    Scene,
    Shader,
    Material,
    Font
};

// 资产信息
struct AssetInfo {
    std::string name;
    std::string path;
    std::string extension;
    AssetType type;
    size_t size;
    bool isDirectory;
};

// 内容浏览器
class ContentBrowser {
public:
    ContentBrowser();
    ~ContentBrowser();

    // 设置根目录
    void setRootPath(const std::string& path);
    const std::string& getRootPath() const { return m_rootPath; }

    // 浏览
    void navigateTo(const std::string& path);
    void goUp();
    void goBack();
    void goForward();

    // 获取当前目录内容
    const std::vector<AssetInfo>& getEntries() const { return m_entries; }

    // 搜索
    std::vector<AssetInfo> search(const std::string& query) const;

    // 文件操作
    bool createDirectory(const std::string& name);
    bool deleteAsset(const std::string& path);
    bool renameAsset(const std::string& oldPath, const std::string& newPath);
    bool copyAsset(const std::string& src, const std::string& dst);
    bool moveAsset(const std::string& src, const std::string& dst);

    // 资产类型检测
    static AssetType detectAssetType(const std::string& extension);

    // 当前路径
    const std::string& getCurrentPath() const { return m_currentPath; }

private:
    void refreshEntries();

    std::string m_rootPath;
    std::string m_currentPath;
    std::vector<std::string> m_history;
    int m_historyIndex = -1;

    std::vector<AssetInfo> m_entries;
};

} // namespace spark
