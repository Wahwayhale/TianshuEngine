#include "material_editor_panel.h"
#include "renderer/material.h"
#include "ui/widgets.h"
#include <imgui.h>

namespace spark {

MaterialEditorPanel::MaterialEditorPanel() = default;
MaterialEditorPanel::~MaterialEditorPanel() = default;

void MaterialEditorPanel::render() {
    ImGui::Begin("Material Editor");

    if (m_material) {
        // 材质名称
        char nameBuffer[256];
        strncpy(nameBuffer, m_material->getName().c_str(), sizeof(nameBuffer));
        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
            m_material->setName(nameBuffer);
        }

        ImGui::Separator();

        // 材质属性
        renderMaterialProperties();

        ImGui::Separator();

        // 纹理槽
        renderTextureSlots();

        ImGui::Separator();

        // 预览
        if (m_showPreview) {
            renderPreview();
        }

        ImGui::Separator();

        // 预设
        renderPresets();
    } else {
        ui::drawEmptyState(ui::icons::Shader, "No material selected", "Select a material to edit");
    }

    ImGui::End();
}

void MaterialEditorPanel::renderMaterialProperties() {
    if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto& props = m_material->getProperties();

        // 基础颜色
        ImGui::Text("Base Color");
        float albedo[4] = {props.albedo.x, props.albedo.y, props.albedo.z, props.albedo.w};
        if (ImGui::ColorEdit4("##Albedo", albedo)) {
            props.albedo = Vec4(albedo[0], albedo[1], albedo[2], albedo[3]);
        }

        // 金属度
        ImGui::Text("Metallic");
        ImGui::SliderFloat("##Metallic", &props.metallic, 0.0f, 1.0f, "%.2f");

        // 粗糙度
        ImGui::Text("Roughness");
        ImGui::SliderFloat("##Roughness", &props.roughness, 0.0f, 1.0f, "%.2f");

        // 环境光遮蔽
        ImGui::Text("Ambient Occlusion");
        ImGui::SliderFloat("##AO", &props.ao, 0.0f, 1.0f, "%.2f");

        // 自发光
        ImGui::Text("Emission");
        float emission[3] = {props.emission.x, props.emission.y, props.emission.z};
        if (ImGui::ColorEdit3("##Emission", emission)) {
            props.emission = Vec3(emission[0], emission[1], emission[2]);
        }

        ImGui::Text("Emission Intensity");
        ImGui::SliderFloat("##EmissionIntensity", &props.emissionIntensity, 0.0f, 10.0f, "%.1f");

        // 法线强度
        ImGui::Text("Normal Strength");
        ImGui::SliderFloat("##NormalStrength", &props.normalStrength, 0.0f, 2.0f, "%.2f");

        // 视差映射
        ImGui::Text("Height Scale");
        ImGui::SliderFloat("##HeightScale", &props.heightScale, 0.0f, 0.1f, "%.3f");

        // 透明度
        ImGui::Text("Alpha Cutoff");
        ImGui::SliderFloat("##AlphaCutoff", &props.alphaCutoff, 0.0f, 1.0f, "%.2f");

        // 双面渲染
        ImGui::Checkbox("Double Sided", &props.doubleSided);

        // UV 变换
        ImGui::Text("UV Offset");
        ImGui::SliderFloat2("##UVOffset", &props.uvOffset.x, -1.0f, 1.0f, "%.2f");

        ImGui::Text("UV Scale");
        ImGui::SliderFloat2("##UVScale", &props.uvScale.x, 0.1f, 10.0f, "%.2f");
    }
}

void MaterialEditorPanel::renderTextureSlots() {
    if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Albedo 纹理
        ImGui::Text("Albedo Map");
        if (m_material->hasTexture(MaterialTextureSlot::Albedo)) {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "✓ Loaded");
            ImGui::SameLine();
            if (ImGui::SmallButton("Remove##Albedo")) {
                m_material->removeTexture(MaterialTextureSlot::Albedo);
            }
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No texture");
            ImGui::SameLine();
            if (ImGui::SmallButton("Load##Albedo")) {
                // TODO: 打开文件对话框
            }
        }

        // 法线贴图
        ImGui::Text("Normal Map");
        if (m_material->hasTexture(MaterialTextureSlot::Normal)) {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "✓ Loaded");
            ImGui::SameLine();
            if (ImGui::SmallButton("Remove##Normal")) {
                m_material->removeTexture(MaterialTextureSlot::Normal);
            }
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No texture");
            ImGui::SameLine();
            if (ImGui::SmallButton("Load##Normal")) {
                // TODO: 打开文件对话框
            }
        }

        // 金属度粗糙度贴图
        ImGui::Text("Metallic/Roughness Map");
        if (m_material->hasTexture(MaterialTextureSlot::MetallicRoughness)) {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "✓ Loaded");
            ImGui::SameLine();
            if (ImGui::SmallButton("Remove##MetallicRoughness")) {
                m_material->removeTexture(MaterialTextureSlot::MetallicRoughness);
            }
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No texture");
            ImGui::SameLine();
            if (ImGui::SmallButton("Load##MetallicRoughness")) {
                // TODO: 打开文件对话框
            }
        }

        // AO 贴图
        ImGui::Text("AO Map");
        if (m_material->hasTexture(MaterialTextureSlot::AO)) {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "✓ Loaded");
            ImGui::SameLine();
            if (ImGui::SmallButton("Remove##AO")) {
                m_material->removeTexture(MaterialTextureSlot::AO);
            }
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No texture");
            ImGui::SameLine();
            if (ImGui::SmallButton("Load##AO")) {
                // TODO: 打开文件对话框
            }
        }

        // 自发光贴图
        ImGui::Text("Emission Map");
        if (m_material->hasTexture(MaterialTextureSlot::Emission)) {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "✓ Loaded");
            ImGui::SameLine();
            if (ImGui::SmallButton("Remove##Emission")) {
                m_material->removeTexture(MaterialTextureSlot::Emission);
            }
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No texture");
            ImGui::SameLine();
            if (ImGui::SmallButton("Load##Emission")) {
                // TODO: 打开文件对话框
            }
        }

        // 高度贴图
        ImGui::Text("Height Map");
        if (m_material->hasTexture(MaterialTextureSlot::Height)) {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "✓ Loaded");
            ImGui::SameLine();
            if (ImGui::SmallButton("Remove##Height")) {
                m_material->removeTexture(MaterialTextureSlot::Height);
            }
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No texture");
            ImGui::SameLine();
            if (ImGui::SmallButton("Load##Height")) {
                // TODO: 打开文件对话框
            }
        }
    }
}

void MaterialEditorPanel::renderPreview() {
    if (ImGui::CollapsingHeader("Preview", ImGuiTreeNodeFlags_DefaultOpen)) {
        // 预览区域
        ImVec2 previewSize = ImVec2(200, 200);
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();

        // 绘制预览背景
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            cursorPos,
            ImVec2(cursorPos.x + previewSize.x, cursorPos.y + previewSize.y),
            IM_COL32(30, 30, 35, 255)
        );

        // 绘制材质预览球体（简化）
        auto& props = m_material->getProperties();
        ImU32 color = IM_COL32(
            (int)(props.albedo.x * 255),
            (int)(props.albedo.y * 255),
            (int)(props.albedo.z * 255),
            (int)(props.albedo.w * 255)
        );

        // 简单的圆形预览
        float radius = previewSize.x * 0.4f;
        ImVec2 center = ImVec2(cursorPos.x + previewSize.x * 0.5f, cursorPos.y + previewSize.y * 0.5f);
        drawList->AddCircleFilled(center, radius, color);
        drawList->AddCircle(center, radius, IM_COL32(100, 100, 100, 255), 32, 1.0f);

        ImGui::Dummy(previewSize);

        // 旋转控制
        ImGui::SliderFloat("Rotation", &m_previewRotation, 0.0f, 360.0f, "%.0f°");
    }
}

void MaterialEditorPanel::renderPresets() {
    if (ImGui::CollapsingHeader("Presets")) {
        auto& props = m_material->getProperties();

        // 预设按钮
        if (ImGui::Button("Default", ImVec2(80, 0))) {
            props.albedo = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
            props.metallic = 0.0f;
            props.roughness = 0.5f;
        }
        ImGui::SameLine();

        if (ImGui::Button("Metal", ImVec2(80, 0))) {
            props.albedo = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
            props.metallic = 1.0f;
            props.roughness = 0.2f;
        }
        ImGui::SameLine();

        if (ImGui::Button("Plastic", ImVec2(80, 0))) {
            props.albedo = Vec4(0.8f, 0.2f, 0.2f, 1.0f);
            props.metallic = 0.0f;
            props.roughness = 0.5f;
        }
        ImGui::SameLine();

        if (ImGui::Button("Glass", ImVec2(80, 0))) {
            props.albedo = Vec4(0.9f, 0.9f, 0.95f, 0.3f);
            props.metallic = 0.0f;
            props.roughness = 0.05f;
        }
        ImGui::SameLine();

        if (ImGui::Button("Emissive", ImVec2(80, 0))) {
            props.albedo = Vec4(1.0f, 0.9f, 0.7f, 1.0f);
            props.emission = Vec3(1.0f, 0.9f, 0.7f);
            props.emissionIntensity = 2.0f;
        }

        // 更多预设
        if (ImGui::Button("Gold", ImVec2(80, 0))) {
            props.albedo = Vec4(1.0f, 0.84f, 0.0f, 1.0f);
            props.metallic = 1.0f;
            props.roughness = 0.1f;
        }
        ImGui::SameLine();

        if (ImGui::Button("Silver", ImVec2(80, 0))) {
            props.albedo = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
            props.metallic = 1.0f;
            props.roughness = 0.2f;
        }
        ImGui::SameLine();

        if (ImGui::Button("Copper", ImVec2(80, 0))) {
            props.albedo = Vec4(0.95f, 0.64f, 0.54f, 1.0f);
            props.metallic = 1.0f;
            props.roughness = 0.3f;
        }
        ImGui::SameLine();

        if (ImGui::Button("Chrome", ImVec2(80, 0))) {
            props.albedo = Vec4(0.9f, 0.9f, 0.9f, 1.0f);
            props.metallic = 1.0f;
            props.roughness = 0.05f;
        }
    }
}

} // namespace spark
