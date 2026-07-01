#include "content_browser.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

ContentBrowser::ContentBrowser() = default;
ContentBrowser::~ContentBrowser() = default;

void ContentBrowser::setRootPath(const std::string& path) {
    m_rootPath = path;
    m_currentPath = path;
    m_history.clear();
    m_historyIndex = -1;
    refreshEntries();
}

void ContentBrowser::navigateTo(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        SPARK_CORE_WARN("Path does not exist: {0}", path);
        return;
    }

    // 保存历史
    if (m_historyIndex < static_cast<int>(m_history.size()) - 1) {
        m_history.erase(m_history.begin() + m_historyIndex + 1, m_history.end());
    }
    m_history.push_back(m_currentPath);
    m_historyIndex = static_cast<int>(m_history.size()) - 1;

    m_currentPath = path;
    refreshEntries();
}

void ContentBrowser::goUp() {
    std::filesystem::path current(m_currentPath);
    if (current.has_parent_path()) {
        navigateTo(current.parent_path().string());
    }
}

void ContentBrowser::goBack() {
    if (m_historyIndex > 0) {
        m_historyIndex--;
        m_currentPath = m_history[m_historyIndex];
        refreshEntries();
    }
}

void ContentBrowser::goForward() {
    if (m_historyIndex < static_cast<int>(m_history.size()) - 1) {
        m_historyIndex++;
        m_currentPath = m_history[m_historyIndex];
        refreshEntries();
    }
}

std::vector<AssetInfo> ContentBrowser::search(const std::string& query) const {
    std::vector<AssetInfo> results;

    for (const auto& entry : m_entries) {
        if (entry.name.find(query) != std::string::npos) {
            results.push_back(entry);
        }
    }

    return results;
}

bool ContentBrowser::createDirectory(const std::string& name) {
    try {
        std::filesystem::create_directory(m_currentPath + "/" + name);
        refreshEntries();
        return true;
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to create directory: {0}", e.what());
        return false;
    }
}

bool ContentBrowser::deleteAsset(const std::string& path) {
    try {
        std::filesystem::remove_all(path);
        refreshEntries();
        return true;
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to delete asset: {0}", e.what());
        return false;
    }
}

bool ContentBrowser::renameAsset(const std::string& oldPath, const std::string& newPath) {
    try {
        std::filesystem::rename(oldPath, newPath);
        refreshEntries();
        return true;
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to rename asset: {0}", e.what());
        return false;
    }
}

bool ContentBrowser::copyAsset(const std::string& src, const std::string& dst) {
    try {
        std::filesystem::copy(src, dst, std::filesystem::copy_options::recursive);
        refreshEntries();
        return true;
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to copy asset: {0}", e.what());
        return false;
    }
}

bool ContentBrowser::moveAsset(const std::string& src, const std::string& dst) {
    try {
        std::filesystem::rename(src, dst);
        refreshEntries();
        return true;
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to move asset: {0}", e.what());
        return false;
    }
}

AssetType ContentBrowser::detectAssetType(const std::string& extension) {
    std::string ext = extension;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga") {
        return AssetType::Texture;
    }
    if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb") {
        return AssetType::Model;
    }
    if (ext == ".wav" || ext == ".mp3" || ext == ".ogg") {
        return AssetType::Audio;
    }
    if (ext == ".lua" || ext == ".py" || ext == ".js") {
        return AssetType::Script;
    }
    if (ext == ".scene" || ext == ".json") {
        return AssetType::Scene;
    }
    if (ext == ".glsl" || ext == ".vert" || ext == ".frag" || ext == ".spv") {
        return AssetType::Shader;
    }
    if (ext == ".mat") {
        return AssetType::Material;
    }
    if (ext == ".ttf" || ext == ".otf") {
        return AssetType::Font;
    }

    return AssetType::Unknown;
}

void ContentBrowser::refreshEntries() {
    m_entries.clear();

    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_currentPath)) {
            AssetInfo info;
            info.name = entry.path().filename().string();
            info.path = entry.path().string();
            info.extension = entry.path().extension().string();
            info.isDirectory = entry.is_directory();
            info.size = entry.is_directory() ? 0 : entry.file_size();
            info.type = detectAssetType(info.extension);

            m_entries.push_back(info);
        }

        // 排序：目录在前，文件在后
        std::sort(m_entries.begin(), m_entries.end(), [](const AssetInfo& a, const AssetInfo& b) {
            if (a.isDirectory != b.isDirectory) {
                return a.isDirectory;
            }
            return a.name < b.name;
        });

    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to refresh entries: {0}", e.what());
    }
}

} // namespace spark
