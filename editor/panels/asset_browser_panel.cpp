#include "asset_browser_panel.h"
#include "ui/widgets.h"
#include <imgui.h>
#include <filesystem>
#include <algorithm>

namespace spark {

AssetBrowserPanel::AssetBrowserPanel() {
    m_currentPath = std::filesystem::path(m_rootPath);
    refresh();
}

AssetBrowserPanel::~AssetBrowserPanel() = default;

void AssetBrowserPanel::render() {
    ImGui::Begin("Asset Browser");

    // 工具栏
    renderToolbar();

    // 面包屑导航
    renderBreadcrumb();

    ImGui::Separator();
    ImGui::Spacing();

    // 内容区域
    if (m_entries.empty()) {
        ui::drawEmptyState(ui::icons::Assets, "No assets found", "Import assets to get started");
    } else {
        if (m_viewMode == ViewMode::Grid) {
            renderGridView();
        } else {
            renderListView();
        }
    }

    ImGui::End();
}

void AssetBrowserPanel::renderToolbar() {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));

    // 返回按钮
    bool canGoBack = m_currentPath.has_parent_path() && m_currentPath != m_rootPath;
    if (!canGoBack) {
        ImGui::BeginDisabled();
    }

    if (ImGui::SmallButton(ui::icons::ArrowRight)) {
        m_currentPath = m_currentPath.parent_path();
        refresh();
    }

    if (!canGoBack) {
        ImGui::EndDisabled();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Go Back");
    }

    ImGui::SameLine();

    // 刷新按钮
    if (ImGui::SmallButton(ui::icons::Forward)) {
        refresh();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Refresh");
    }

    ImGui::SameLine();
    ui::drawToolbarSeparator();

    // 视图切换
    if (m_viewMode == ViewMode::Grid) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.55f, 0.90f, 0.80f));
    }

    if (ImGui::SmallButton(ui::icons::Assets)) {
        m_viewMode = ViewMode::Grid;
    }

    if (m_viewMode == ViewMode::Grid) {
        ImGui::PopStyleColor();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Grid View");
    }

    ImGui::SameLine();

    if (m_viewMode == ViewMode::List) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.55f, 0.90f, 0.80f));
    }

    if (ImGui::SmallButton(ui::icons::File)) {
        m_viewMode = ViewMode::List;
    }

    if (m_viewMode == ViewMode::List) {
        ImGui::PopStyleColor();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("List View");
    }

    ImGui::SameLine();
    ui::drawToolbarSeparator();

    // 搜索
    if (m_showSearch) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 4));

        ImGui::SetNextItemWidth(150);
        ImGui::InputTextWithHint("##AssetSearch", "Search...", m_searchBuffer, sizeof(m_searchBuffer));

        ImGui::PopStyleVar(2);
    }

    bool searchActive = m_showSearch;
    if (searchActive) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.55f, 0.90f, 0.80f));
    }

    if (ImGui::SmallButton(ui::icons::Search)) {
        m_showSearch = !m_showSearch;
        if (!m_showSearch) {
            memset(m_searchBuffer, 0, sizeof(m_searchBuffer));
        }
    }

    if (searchActive) {
        ImGui::PopStyleColor();
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Toggle Search");
    }

    // 缩略图大小（仅网格模式）
    if (m_viewMode == ViewMode::Grid) {
        ImGui::SameLine();
        ui::drawToolbarSeparator();

        ImGui::Text("Size:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        ImGui::SliderFloat("##ThumbSize", &m_thumbnailSize, 32.0f, 128.0f, "%.0f");
    }

    ImGui::PopStyleVar(2);
}

void AssetBrowserPanel::renderBreadcrumb() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 0));

    // 根目录
    if (ImGui::SmallButton(ui::icons::Assets)) {
        m_currentPath = m_rootPath;
        refresh();
    }

    // 路径各部分
    std::filesystem::path relativePath;
    try {
        relativePath = std::filesystem::relative(m_currentPath, m_rootPath);
    } catch (...) {
        relativePath = m_currentPath;
    }

    for (const auto& part : relativePath) {
        if (part == "." || part == "..") continue;

        ImGui::SameLine();
        ImGui::TextDisabled("/");
        ImGui::SameLine();

        std::string partStr = part.string();
        if (ImGui::SmallButton(partStr.c_str())) {
            // 构建到这部分的完整路径
            m_currentPath = m_rootPath;
            for (const auto& p : relativePath) {
                m_currentPath /= p;
                if (p == part) break;
            }
            refresh();
        }
    }

    ImGui::PopStyleVar();
}

void AssetBrowserPanel::renderGridView() {
    float availWidth = ImGui::GetContentRegionAvail().x;
    int columns = std::max(1, (int)(availWidth / (m_thumbnailSize + 16)));

    ImGui::Columns(columns, nullptr, false);

    for (const auto& entry : m_entries) {
        // 搜索过滤
        if (m_searchBuffer[0] != '\0') {
            std::string filename = entry.path().filename().string();
            std::string lowerFilename = filename;
            std::string lowerSearch = m_searchBuffer;
            std::transform(lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), ::tolower);
            std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);

            if (lowerFilename.find(lowerSearch) == std::string::npos) {
                continue;
            }
        }

        ImGui::PushID(entry.path().string().c_str());

        const char* icon = getFileIcon(entry.path());
        ImVec4 color = getFileColor(entry.path());

        // 绘制缩略图
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // 背景
        drawList->AddRectFilled(
            pos,
            ImVec2(pos.x + m_thumbnailSize, pos.y + m_thumbnailSize),
            colorToU32(ImVec4(0.12f, 0.12f, 0.15f, 1.00f)),
            6.0f
        );

        // 图标
        ImVec2 iconSize = ImGui::CalcTextSize(icon);
        drawList->AddText(
            ImVec2(pos.x + (m_thumbnailSize - iconSize.x) * 0.5f, pos.y + (m_thumbnailSize - iconSize.y) * 0.5f),
            colorToU32(color),
            icon
        );

        // 交互区域
        ImGui::Dummy(ImVec2(m_thumbnailSize, m_thumbnailSize));

        bool isSelected = false;
        if (ImGui::IsItemClicked()) {
            if (entry.is_directory()) {
                if (ImGui::IsMouseDoubleClicked(0)) {
                    m_currentPath = entry.path();
                    refresh();
                }
            }
        }

        // 文件名
        std::string filename = entry.path().filename().string();
        float textWidth = ImGui::CalcTextSize(filename.c_str()).x;
        float textX = ImGui::GetCursorPosX() + (m_thumbnailSize - textWidth) * 0.5f;
        if (textX > ImGui::GetCursorPosX()) {
            ImGui::SetCursorPosX(textX);
        }

        // 截断过长的文件名
        if (textWidth > m_thumbnailSize) {
            filename = filename.substr(0, 8) + "...";
        }

        ImGui::TextWrapped("%s", filename.c_str());

        ImGui::NextColumn();
        ImGui::PopID();
    }

    ImGui::Columns(1);
}

void AssetBrowserPanel::renderListView() {
    // 表头
    if (ImGui::BeginTable("AssetList", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableHeadersRow();

        for (const auto& entry : m_entries) {
            // 搜索过滤
            if (m_searchBuffer[0] != '\0') {
                std::string filename = entry.path().filename().string();
                std::string lowerFilename = filename;
                std::string lowerSearch = m_searchBuffer;
                std::transform(lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), ::tolower);
                std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);

                if (lowerFilename.find(lowerSearch) == std::string::npos) {
                    continue;
                }
            }

            ImGui::TableNextRow();

            // 名称列
            ImGui::TableNextColumn();
            const char* icon = getFileIcon(entry.path());
            ImVec4 color = getFileColor(entry.path());

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            char label[256];
            snprintf(label, sizeof(label), "%s  %s", icon, entry.path().filename().string().c_str());

            if (ImGui::Selectable(label, false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick)) {
                if (entry.is_directory() && ImGui::IsMouseDoubleClicked(0)) {
                    m_currentPath = entry.path();
                    refresh();
                }
            }
            ImGui::PopStyleColor();

            // 类型列
            ImGui::TableNextColumn();
            if (entry.is_directory()) {
                ImGui::TextDisabled("Folder");
            } else {
                std::string ext = entry.path().extension().string();
                ImGui::TextDisabled("%s", ext.c_str());
            }

            // 大小列
            ImGui::TableNextColumn();
            if (!entry.is_directory()) {
                try {
                    auto size = entry.file_size();
                    if (size < 1024) {
                        ImGui::TextDisabled("%llu B", size);
                    } else if (size < 1024 * 1024) {
                        ImGui::TextDisabled("%.1f KB", size / 1024.0);
                    } else {
                        ImGui::TextDisabled("%.1f MB", size / (1024.0 * 1024.0));
                    }
                } catch (...) {
                    ImGui::TextDisabled("--");
                }
            }
        }

        ImGui::EndTable();
    }
}

void AssetBrowserPanel::refresh() {
    m_entries.clear();

    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_currentPath)) {
            m_entries.push_back(entry);
        }
    } catch (const std::filesystem::filesystem_error&) {
        // Handle error
    }

    // 排序：文件夹优先，然后按名称
    std::sort(m_entries.begin(), m_entries.end(), [](const auto& a, const auto& b) {
        if (a.is_directory() != b.is_directory()) {
            return a.is_directory();
        }
        return a.path().filename() < b.path().filename();
    });
}

const char* AssetBrowserPanel::getFileIcon(const std::filesystem::path& path) const {
    if (std::filesystem::is_directory(path)) {
        return ui::icons::Folder;
    }

    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // 图片
    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga") {
        return ui::icons::Image;
    }

    // 3D 模型
    if (ext == ".gltf" || ext == ".glb" || ext == ".obj" || ext == ".fbx") {
        return ui::icons::Model;
    }

    // 脚本
    if (ext == ".lua" || ext == ".py" || ext == ".js") {
        return ui::icons::Script;
    }

    // 音频
    if (ext == ".wav" || ext == ".mp3" || ext == ".ogg") {
        return ui::icons::Audio;
    }

    // 着色器
    if (ext == ".glsl" || ext == ".vert" || ext == ".frag" || ext == ".spv") {
        return ui::icons::Shader;
    }

    return ui::icons::File;
}

ImVec4 AssetBrowserPanel::getFileColor(const std::filesystem::path& path) const {
    if (std::filesystem::is_directory(path)) {
        return ImVec4(0.95f, 0.75f, 0.30f, 1.00f);  // 金色
    }

    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // 图片
    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga") {
        return ImVec4(0.40f, 0.80f, 0.60f, 1.00f);  // 绿色
    }

    // 3D 模型
    if (ext == ".gltf" || ext == ".glb" || ext == ".obj" || ext == ".fbx") {
        return ImVec4(0.60f, 0.40f, 0.90f, 1.00f);  // 紫色
    }

    // 脚本
    if (ext == ".lua" || ext == ".py" || ext == ".js") {
        return ImVec4(0.30f, 0.70f, 0.90f, 1.00f);  // 蓝色
    }

    // 音频
    if (ext == ".wav" || ext == ".mp3" || ext == ".ogg") {
        return ImVec4(0.90f, 0.50f, 0.30f, 1.00f);  // 橙色
    }

    // 着色器
    if (ext == ".glsl" || ext == ".vert" || ext == ".frag" || ext == ".spv") {
        return ImVec4(0.90f, 0.30f, 0.50f, 1.00f);  // 粉红色
    }

    return ImVec4(0.70f, 0.70f, 0.75f, 1.00f);  // 默认灰色
}

} // namespace spark
