#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace spark {

class AssetBrowserPanel {
public:
    AssetBrowserPanel();
    ~AssetBrowserPanel();

    void render();

    void setRootPath(const std::string& path) { m_rootPath = path; refresh(); }

private:
    enum class ViewMode {
        Grid,
        List
    };

    void renderToolbar();
    void renderBreadcrumb();
    void renderGridView();
    void renderListView();
    void refresh();

    // 文件图标
    const char* getFileIcon(const std::filesystem::path& path) const;
    ImVec4 getFileColor(const std::filesystem::path& path) const;

    std::string m_rootPath = "assets";
    std::vector<std::filesystem::directory_entry> m_entries;
    std::filesystem::path m_currentPath;

    ViewMode m_viewMode = ViewMode::Grid;
    float m_thumbnailSize = 64.0f;
    char m_searchBuffer[256] = "";
    bool m_showSearch = false;
};

} // namespace spark
