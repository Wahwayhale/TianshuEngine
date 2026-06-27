#include "inspector_panel.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include "ui/widgets.h"
#include <imgui.h>

namespace spark {

InspectorPanel::InspectorPanel() = default;
InspectorPanel::~InspectorPanel() = default;

void InspectorPanel::render() {
    ImGui::Begin("Inspector");

    if (m_selectedEntity) {
        renderHeader();
        ImGui::Spacing();

        // 组件列表
        renderTransform();
        renderMeshRenderer();
        renderLight();
        renderCamera();
        renderRigidBody();
        renderCollider();
        renderScript();

        ImGui::Spacing();
        renderAddComponent();
    } else {
        ui::drawEmptyState(ui::icons::Settings, "No entity selected", "Select an entity from the hierarchy");
    }

    ImGui::End();
}

void InspectorPanel::renderHeader() {
    // 实体名称
    if (m_selectedEntity->hasComponent<TagComponent>()) {
        auto& tag = m_selectedEntity->getComponent<TagComponent>();

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strncpy(buffer, tag.name.c_str(), sizeof(buffer));
        if (ImGui::InputText("##EntityName", buffer, sizeof(buffer))) {
            tag.name = std::string(buffer);
        }

        ImGui::PopStyleVar(2);
    }

    // 实体 ID
    ImGui::SameLine();
    ImGui::TextDisabled("ID: %u", m_selectedEntity->getID());
}

void InspectorPanel::renderTransform() {
    if (!m_selectedEntity->hasComponent<TransformComponent>()) return;

    auto& transform = m_selectedEntity->getComponent<TransformComponent>();

    ImGui::PushID("Transform");
    bool open = ui::drawComponentHeader(ui::icons::Transform, "Transform", /*open=*/true);

    if (open) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

        ui::drawVec3Control("Position", transform.position);
        ui::drawVec3Control("Rotation", transform.rotation);
        ui::drawVec3Control("Scale", transform.scale, 1.0f);

        ImGui::PopStyleVar();
        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::Spacing();
}

void InspectorPanel::renderMeshRenderer() {
    if (!m_selectedEntity->hasComponent<MeshRendererComponent>()) return;

    auto& meshRenderer = m_selectedEntity->getComponent<MeshRendererComponent>();

    ImGui::PushID("MeshRenderer");
    bool open = ui::drawComponentHeader(ui::icons::Mesh, "Mesh Renderer", /*open=*/true);

    if (open) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

        ImGui::Checkbox("Visible", &meshRenderer.visible);

        ImGui::PopStyleVar();
        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::Spacing();
}

void InspectorPanel::renderLight() {
    if (!m_selectedEntity->hasComponent<LightComponent>()) return;

    auto& light = m_selectedEntity->getComponent<LightComponent>();

    ImGui::PushID("Light");
    bool open = ui::drawComponentHeader(ui::icons::Light, "Light", /*open=*/true);

    if (open) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

        const char* lightTypes[] = { "Directional", "Point", "Spot" };
        int currentType = static_cast<int>(light.lightType);

        ImGui::Text("Type");
        ImGui::SameLine();
        if (ImGui::Combo("##Type", &currentType, lightTypes, 3)) {
            light.lightType = static_cast<LightComponent::LightType>(currentType);
        }

        ui::drawColorEdit3("Color", &light.color.x);

        ImGui::Text("Intensity");
        ImGui::SameLine();
        ImGui::SliderFloat("##Intensity", &light.intensity, 0.0f, 100.0f, "%.1f");

        if (light.lightType == LightComponent::LightType::Point ||
            light.lightType == LightComponent::LightType::Spot) {
            ImGui::Text("Range");
            ImGui::SameLine();
            ImGui::DragFloat("##Range", &light.range, 0.1f, 0.0f, 1000.0f, "%.1f");
        }

        if (light.lightType == LightComponent::LightType::Spot) {
            ImGui::Text("Inner Cutoff");
            ImGui::SameLine();
            ImGui::SliderFloat("##InnerCutoff", &light.innerCutoff, 0.0f, light.outerCutoff, "%.1f°");

            ImGui::Text("Outer Cutoff");
            ImGui::SameLine();
            ImGui::SliderFloat("##OuterCutoff", &light.outerCutoff, light.innerCutoff, 90.0f, "%.1f°");
        }

        ImGui::PopStyleVar();
        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::Spacing();
}

void InspectorPanel::renderCamera() {
    if (!m_selectedEntity->hasComponent<CameraComponent>()) return;

    auto& camera = m_selectedEntity->getComponent<CameraComponent>();

    ImGui::PushID("Camera");
    bool open = ui::drawComponentHeader(ui::icons::Camera, "Camera", /*open=*/true);

    if (open) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

        const char* projTypes[] = { "Perspective", "Orthographic" };
        int currentType = static_cast<int>(camera.projectionType);

        ImGui::Text("Projection");
        ImGui::SameLine();
        if (ImGui::Combo("##Projection", &currentType, projTypes, 2)) {
            camera.projectionType = static_cast<CameraComponent::ProjectionType>(currentType);
        }

        if (camera.projectionType == CameraComponent::ProjectionType::Perspective) {
            ImGui::Text("FOV");
            ImGui::SameLine();
            ImGui::SliderFloat("##FOV", &camera.fov, 1.0f, 179.0f, "%.0f°");
        } else {
            ImGui::Text("Size");
            ImGui::SameLine();
            ImGui::DragFloat("##Size", &camera.orthoSize, 0.1f, 0.1f, 100.0f, "%.1f");
        }

        ImGui::Text("Near Plane");
        ImGui::SameLine();
        ImGui::DragFloat("##Near", &camera.nearPlane, 0.01f, 0.01f, 100.0f, "%.2f");

        ImGui::Text("Far Plane");
        ImGui::SameLine();
        ImGui::DragFloat("##Far", &camera.farPlane, 1.0f, 1.0f, 10000.0f, "%.0f");

        ImGui::PopStyleVar();
        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::Spacing();
}

void InspectorPanel::renderRigidBody() {
    if (!m_selectedEntity->hasComponent<RigidBodyComponent>()) return;

    auto& rb = m_selectedEntity->getComponent<RigidBodyComponent>();

    ImGui::PushID("RigidBody");
    bool open = ui::drawComponentHeader(ui::icons::Physics, "Rigid Body", /*open=*/true);

    if (open) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

        ui::drawVec3Control("Velocity", rb.velocity);

        ImGui::Text("Mass");
        ImGui::SameLine();
        ImGui::DragFloat("##Mass", &rb.mass, 0.1f, 0.1f, 1000.0f, "%.2f");

        ImGui::Checkbox("Use Gravity", &rb.useGravity);
        ImGui::Checkbox("Is Static", &rb.isStatic);

        ImGui::PopStyleVar();
        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::Spacing();
}

void InspectorPanel::renderCollider() {
    if (!m_selectedEntity->hasComponent<ColliderComponent>()) return;

    auto& collider = m_selectedEntity->getComponent<ColliderComponent>();

    ImGui::PushID("Collider");
    bool open = ui::drawComponentHeader(ui::icons::Collider, "Collider", /*open=*/true);

    if (open) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

        const char* colliderTypes[] = { "Box", "Sphere", "Capsule", "Mesh" };
        int currentType = static_cast<int>(collider.colliderType);

        ImGui::Text("Type");
        ImGui::SameLine();
        if (ImGui::Combo("##Type", &currentType, colliderTypes, 4)) {
            collider.colliderType = static_cast<ColliderComponent::ColliderType>(currentType);
        }

        switch (collider.colliderType) {
            case ColliderComponent::ColliderType::Box:
                ui::drawVec3Control("Size", collider.size);
                break;
            case ColliderComponent::ColliderType::Sphere:
                ImGui::Text("Radius");
                ImGui::SameLine();
                ImGui::DragFloat("##Radius", &collider.radius, 0.1f, 0.0f, 100.0f, "%.2f");
                break;
            case ColliderComponent::ColliderType::Capsule:
                ImGui::Text("Radius");
                ImGui::SameLine();
                ImGui::DragFloat("##Radius", &collider.radius, 0.1f, 0.0f, 100.0f, "%.2f");
                ImGui::Text("Height");
                ImGui::SameLine();
                ImGui::DragFloat("##Height", &collider.height, 0.1f, 0.0f, 100.0f, "%.2f");
                break;
            default:
                break;
        }

        ImGui::Checkbox("Is Trigger", &collider.isTrigger);

        ImGui::PopStyleVar();
        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::Spacing();
}

void InspectorPanel::renderScript() {
    if (!m_selectedEntity->hasComponent<ScriptComponent>()) return;

    auto& script = m_selectedEntity->getComponent<ScriptComponent>();

    ImGui::PushID("Script");
    bool open = ui::drawComponentHeader(ui::icons::ScriptComp, "Script", /*open=*/true);

    if (open) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strncpy(buffer, script.scriptPath.c_str(), sizeof(buffer));

        ImGui::Text("Script Path");
        if (ImGui::InputText("##ScriptPath", buffer, sizeof(buffer))) {
            script.scriptPath = std::string(buffer);
        }

        ImGui::PopStyleVar();
        ImGui::TreePop();
    }

    ImGui::PopID();
    ImGui::Spacing();
}

void InspectorPanel::renderAddComponent() {
    // 添加组件按钮
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.22f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.30f, 1.00f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

    float availWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - 200) * 0.5f);

    if (ImGui::Button("Add Component", ImVec2(200, 0))) {
        ImGui::OpenPopup("AddComponent");
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);

    // 弹出菜单
    if (ImGui::BeginPopup("AddComponent")) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

        if (ui::drawIconMenuItem(ui::icons::Transform, "Transform")) {
            m_selectedEntity->addComponent<TransformComponent>();
        }
        if (ui::drawIconMenuItem(ui::icons::Camera, "Camera")) {
            m_selectedEntity->addComponent<CameraComponent>();
        }
        if (ui::drawIconMenuItem(ui::icons::Light, "Light")) {
            m_selectedEntity->addComponent<LightComponent>();
        }
        if (ui::drawIconMenuItem(ui::icons::Mesh, "Mesh Renderer")) {
            m_selectedEntity->addComponent<MeshRendererComponent>();
        }
        if (ui::drawIconMenuItem(ui::icons::Physics, "Rigid Body")) {
            m_selectedEntity->addComponent<RigidBodyComponent>();
        }
        if (ui::drawIconMenuItem(ui::icons::Collider, "Collider")) {
            m_selectedEntity->addComponent<ColliderComponent>();
        }
        if (ui::drawIconMenuItem(ui::icons::ScriptComp, "Script")) {
            m_selectedEntity->addComponent<ScriptComponent>();
        }

        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }
}

} // namespace spark
