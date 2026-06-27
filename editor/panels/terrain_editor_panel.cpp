#include "terrain_editor_panel.h"
#include "renderer/terrain_system.h"
#include "ui/widgets.h"
#include <imgui.h>

namespace spark {

TerrainEditorPanel::TerrainEditorPanel() = default;
TerrainEditorPanel::~TerrainEditorPanel() = default;

void TerrainEditorPanel::render() {
    ImGui::Begin("Terrain Editor");

    renderToolbar();
    ImGui::Separator();

    switch (m_editMode) {
        case TerrainEditMode::Sculpt:
            renderSculptTools();
            break;
        case TerrainEditMode::Paint:
            renderPaintTools();
            break;
        case TerrainEditMode::Foliage:
            renderFoliageTools();
            break;
        case TerrainEditMode::Settings:
            renderSettings();
            break;
    }

    ImGui::End();
}

void TerrainEditorPanel::renderToolbar() {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));

    // 编辑模式切换
    const char* modes[] = { "Sculpt", "Smooth", "Paint", "Foliage", "Settings" };
    int currentMode = static_cast<int>(m_editMode);
    if (ImGui::Combo("Mode", &currentMode, modes, 5)) {
        m_editMode = static_cast<TerrainEditMode>(currentMode);
    }

    ImGui::PopStyleVar(2);
}

void TerrainEditorPanel::renderSculptTools() {
    if (ImGui::CollapsingHeader("Sculpt Tools", ImGuiTreeNodeFlags_DefaultOpen)) {
        // 笔刷大小
        ImGui::Text("Brush Size");
        ImGui::SliderFloat("##BrushSize", &m_brushSize, 0.1f, 50.0f, "%.1f");

        // 笔刷强度
        ImGui::Text("Brush Strength");
        ImGui::SliderFloat("##BrushStrength", &m_brushStrength, 0.0f, 1.0f, "%.2f");

        ImGui::Spacing();

        // 雕刻操作
        if (ImGui::Button("Raise Terrain", ImVec2(120, 0))) {
            // TODO: 抬高地形
        }

        ImGui::SameLine();

        if (ImGui::Button("Lower Terrain", ImVec2(120, 0))) {
            // TODO: 降低地形
        }

        if (ImGui::Button("Smooth Terrain", ImVec2(120, 0))) {
            // TODO: 平滑地形
        }

        ImGui::SameLine();

        if (ImGui::Button("Flatten Terrain", ImVec2(120, 0))) {
            // TODO: 平整地形
        }

        ImGui::Spacing();

        // 噪声设置
        ImGui::Text("Noise Scale");
        float noiseScale = 0.1f;
        ImGui::SliderFloat("##NoiseScale", &noiseScale, 0.01f, 1.0f, "%.2f");

        ImGui::Text("Noise Strength");
        float noiseStrength = 0.5f;
        ImGui::SliderFloat("##NoiseStrength", &noiseStrength, 0.0f, 1.0f, "%.2f");

        if (ImGui::Button("Apply Noise", ImVec2(120, 0))) {
            // TODO: 应用噪声
        }
    }
}

void TerrainEditorPanel::renderPaintTools() {
    if (ImGui::CollapsingHeader("Paint Tools", ImGuiTreeNodeFlags_DefaultOpen)) {
        // 纹理层选择
        ImGui::Text("Texture Layer");

        // 层列表
        const char* layers[] = { "Layer 0", "Layer 1", "Layer 2", "Layer 3" };
        int currentLayer = m_selectedLayer;
        if (ImGui::Combo("##Layer", &currentLayer, layers, 4)) {
            m_selectedLayer = currentLayer;
        }

        ImGui::Spacing();

        // 绘制操作
        if (ImGui::Button("Paint", ImVec2(120, 0))) {
            // TODO: 绘制纹理
        }

        ImGui::SameLine();

        if (ImGui::Button("Erase", ImVec2(120, 0))) {
            // TODO: 擦除纹理
        }

        ImGui::Spacing();

        // 笔刷设置
        ImGui::Text("Brush Size");
        ImGui::SliderFloat("##PaintBrushSize", &m_brushSize, 0.1f, 50.0f, "%.1f");

        ImGui::Text("Brush Strength");
        ImGui::SliderFloat("##PaintBrushStrength", &m_brushStrength, 0.0f, 1.0f, "%.2f");

        ImGui::Spacing();

        // 纹理层设置
        ImGui::Text("Layer Settings");

        ImGui::Text("Min Height");
        float minHeight = 0.0f;
        ImGui::SliderFloat("##MinHeight", &minHeight, 0.0f, 1.0f, "%.2f");

        ImGui::Text("Max Height");
        float maxHeight = 1.0f;
        ImGui::SliderFloat("##MaxHeight", &maxHeight, 0.0f, 1.0f, "%.2f");

        ImGui::Text("Blend Range");
        float blendRange = 0.1f;
        ImGui::SliderFloat("##BlendRange", &blendRange, 0.0f, 0.5f, "%.2f");
    }
}

void TerrainEditorPanel::renderFoliageTools() {
    if (ImGui::CollapsingHeader("Foliage Tools", ImGuiTreeNodeFlags_DefaultOpen)) {
        // 植被类型选择
        ImGui::Text("Vegetation Type");

        const char* types[] = { "Tree", "Bush", "Grass", "Rock" };
        int currentType = m_selectedFoliage;
        if (ImGui::Combo("##Type", &currentType, types, 4)) {
            m_selectedFoliage = currentType;
        }

        ImGui::Spacing();

        // 植被操作
        if (ImGui::Button("Place Foliage", ImVec2(120, 0))) {
            // TODO: 放置植被
        }

        ImGui::SameLine();

        if (ImGui::Button("Remove Foliage", ImVec2(120, 0))) {
            // TODO: 移除植被
        }

        ImGui::Spacing();

        // 植被设置
        ImGui::Text("Density");
        float density = 0.1f;
        ImGui::SliderFloat("##Density", &density, 0.01f, 1.0f, "%.2f");

        ImGui::Text("Min Scale");
        float minScale = 0.8f;
        ImGui::SliderFloat("##MinScale", &minScale, 0.1f, 2.0f, "%.1f");

        ImGui::Text("Max Scale");
        float maxScale = 1.2f;
        ImGui::SliderFloat("##MaxScale", &maxScale, 0.1f, 2.0f, "%.1f");

        ImGui::Spacing();

        if (ImGui::Button("Generate Foliage", ImVec2(150, 0))) {
            // TODO: 自动生成植被
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear Foliage", ImVec2(150, 0))) {
            // TODO: 清除所有植被
        }
    }
}

void TerrainEditorPanel::renderSettings() {
    if (ImGui::CollapsingHeader("Terrain Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        // 地形大小
        ImGui::Text("Terrain Size");
        ImGui::SliderInt("##Size", &m_terrainSize, 64, 1024);

        // 地形缩放
        ImGui::Text("Terrain Scale");
        ImGui::SliderFloat("##Scale", &m_terrainScale, 10.0f, 500.0f, "%.0f");

        // 高度缩放
        ImGui::Text("Height Scale");
        ImGui::SliderFloat("##HeightScale", &m_heightScale, 1.0f, 200.0f, "%.0f");

        ImGui::Spacing();

        // 生成设置
        ImGui::Text("Seed");
        ImGui::SliderInt("##Seed", &m_seed, 0, 9999);

        if (ImGui::Button("Generate Terrain", ImVec2(150, 0))) {
            // TODO: 生成地形
        }

        ImGui::SameLine();

        if (ImGui::Button("Load Heightmap", ImVec2(150, 0))) {
            // TODO: 加载高度图
        }

        ImGui::Spacing();

        // 导入/导出
        if (ImGui::Button("Import Heightmap", ImVec2(150, 0))) {
            // TODO: 导入高度图
        }

        ImGui::SameLine();

        if (ImGui::Button("Export Heightmap", ImVec2(150, 0))) {
            // TODO: 导出高度图
        }
    }
}

} // namespace spark
