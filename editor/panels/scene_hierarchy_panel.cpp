#include "scene_hierarchy_panel.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include "ui/widgets.h"
#include <imgui.h>
#include <algorithm>

namespace spark {

SceneHierarchyPanel::SceneHierarchyPanel() = default;
SceneHierarchyPanel::~SceneHierarchyPanel() = default;

void SceneHierarchyPanel::render(Scene& scene) {
    ImGui::Begin("Scene Hierarchy");

    // 工具栏
    renderToolbar();

    // 搜索栏
    if (m_showSearch) {
        renderSearchBar();
        ImGui::Spacing();
    }

    ImGui::Separator();
    ImGui::Spacing();

    // 实体列表
    const auto& entities = scene.getEntities();
    bool anyMatched = false;

    for (const auto& [id, entity] : entities) {
        // 搜索过滤
        if (m_searchBuffer[0] != '\0') {
            std::string name = "Entity";
            if (entity->hasComponent<TagComponent>()) {
                name = entity->getComponent<TagComponent>().name;
            }

            // 简单的子字符串搜索
            std::string lowerName = name;
            std::string lowerSearch = m_searchBuffer;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);

            if (lowerName.find(lowerSearch) == std::string::npos) {
                continue;
            }
        }

        drawEntityNode(*entity);
        anyMatched = true;
    }

    // 空状态
    if (!anyMatched) {
        if (m_searchBuffer[0] != '\0') {
            ui::drawEmptyState(ui::icons::Search, "No matching entities", "Try a different search term");
        } else {
            ui::drawEmptyState(ui::icons::Hierarchy, "Scene is empty", "Right-click to create an entity");
        }
    }

    // 右键上下文菜单
    if (ImGui::BeginPopupContextWindow("SceneContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

        if (ui::drawIconMenuItem(ui::icons::Plus, "Create Empty Entity")) {
            scene.createEntity("Empty");
        }

        ImGui::Separator();

        if (ui::drawIconMenuItem(ui::icons::Hierarchy, "Expand All")) {
            // TODO: 展开所有节点
        }

        if (ui::drawIconMenuItem(ui::icons::Hierarchy, "Collapse All")) {
            // TODO: 折叠所有节点
        }

        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }

    ImGui::End();
}

void SceneHierarchyPanel::renderToolbar() {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));

    // 搜索按钮
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
        ui::drawTooltip("Toggle Search (Ctrl+F)");
    }

    ImGui::SameLine();

    // 添加按钮
    if (ImGui::SmallButton(ui::icons::Plus)) {
        ImGui::OpenPopup("QuickAddEntity");
    }

    if (ImGui::IsItemHovered()) {
        ui::drawTooltip("Add Entity");
    }

    // 快速添加菜单
    if (ImGui::BeginPopup("QuickAddEntity")) {
        if (ImGui::MenuItem("Empty Entity")) {
            // TODO: 创建空实体
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Cube")) {
            // TODO: 创建立方体
        }
        if (ImGui::MenuItem("Sphere")) {
            // TODO: 创建球体
        }
        if (ImGui::MenuItem("Plane")) {
            // TODO: 创建平面
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Directional Light")) {
            // TODO: 创建方向光
        }
        if (ImGui::MenuItem("Point Light")) {
            // TODO: 创建点光源
        }
        if (ImGui::MenuItem("Camera")) {
            // TODO: 创建相机
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar(2);
}

void SceneHierarchyPanel::renderSearchBar() {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 6));

    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##Search", "Search entities...", m_searchBuffer, sizeof(m_searchBuffer));

    ImGui::PopStyleVar(2);
}

void SceneHierarchyPanel::drawEntityNode(Entity& entity) {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (m_selectedEntityID == entity.getID()) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // 获取实体名称
    std::string name = "Entity";
    if (entity.hasComponent<TagComponent>()) {
        name = entity.getComponent<TagComponent>().name;
    }

    // 确定图标
    const char* icon = ui::icons::Hierarchy;
    if (entity.hasComponent<CameraComponent>()) {
        icon = ui::icons::Camera;
    } else if (entity.hasComponent<LightComponent>()) {
        icon = ui::icons::Light;
    } else if (entity.hasComponent<MeshRendererComponent>()) {
        icon = ui::icons::Mesh;
    } else if (entity.hasComponent<ScriptComponent>()) {
        icon = ui::icons::ScriptComp;
    }

    // 绘制节点
    char nodeLabel[256];
    snprintf(nodeLabel, sizeof(nodeLabel), "%s  %s", icon, name.c_str());

    bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(entity.getID()), flags, "%s", nodeLabel);

    // 选中处理
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        m_selectedEntityID = entity.getID();
        if (m_entitySelectedCallback) {
            m_entitySelectedCallback(&entity);
        }
    }

    // 右键上下文菜单
    if (ImGui::BeginPopupContextItem()) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

        if (ui::drawIconMenuItem(ui::icons::Plus, "Add Child")) {
            // TODO: 添加子实体
        }

        ImGui::Separator();

        if (ui::drawIconMenuItem(ui::icons::File, "Duplicate")) {
            // TODO: 复制实体
        }

        if (ui::drawIconMenuItem(ui::icons::Close, "Delete")) {
            // TODO: 删除实体
        }

        ImGui::Separator();

        if (ui::drawIconMenuItem(ui::icons::ArrowRight, "Rename")) {
            // TODO: 重命名
        }

        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }

    // 拖拽源
    if (ImGui::BeginDragDropSource()) {
        EntityID id = entity.getID();
        ImGui::SetDragDropPayload("ENTITY", &id, sizeof(EntityID));
        ImGui::Text("%s %s", icon, name.c_str());
        ImGui::EndDragDropSource();
    }

    // 拖拽目标
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY")) {
            EntityID draggedID = *(const EntityID*)payload->Data;
            // TODO: 处理父子关系
        }
        ImGui::EndDragDropTarget();
    }

    // 展开后显示子节点和组件信息
    if (opened) {
        // 组件图标列表
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.60f, 0.60f, 0.65f, 1.00f));

        if (entity.hasComponent<TransformComponent>()) {
            ImGui::BulletText("%s Transform", ui::icons::Transform);
        }
        if (entity.hasComponent<CameraComponent>()) {
            ImGui::BulletText("%s Camera", ui::icons::Camera);
        }
        if (entity.hasComponent<LightComponent>()) {
            ImGui::BulletText("%s Light", ui::icons::Light);
        }
        if (entity.hasComponent<MeshRendererComponent>()) {
            ImGui::BulletText("%s Mesh Renderer", ui::icons::Mesh);
        }
        if (entity.hasComponent<RigidBodyComponent>()) {
            ImGui::BulletText("%s Rigid Body", ui::icons::Physics);
        }
        if (entity.hasComponent<ColliderComponent>()) {
            ImGui::BulletText("%s Collider", ui::icons::Collider);
        }
        if (entity.hasComponent<ScriptComponent>()) {
            ImGui::BulletText("%s Script", ui::icons::ScriptComp);
        }

        ImGui::PopStyleColor();
        ImGui::TreePop();
    }
}

} // namespace spark
