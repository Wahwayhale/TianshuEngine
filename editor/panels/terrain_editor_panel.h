#pragma once

#include <string>
#include <vector>
#include <memory>
#include "math/math_types.h"

namespace spark {

class TerrainSystem;

// 地形编辑模式
enum class TerrainEditMode {
    Sculpt,      // 雕刻
    Smooth,      // 平滑
    Paint,       // 绘制纹理
    Foliage,     // 植被放置
    Settings     // 设置
};

// 地形编辑器面板
class TerrainEditorPanel {
public:
    TerrainEditorPanel();
    ~TerrainEditorPanel();

    void render();

    void setTerrainSystem(TerrainSystem* terrain) { m_terrainSystem = terrain; }

private:
    void renderToolbar();
    void renderSculptTools();
    void renderPaintTools();
    void renderFoliageTools();
    void renderSettings();

    TerrainSystem* m_terrainSystem = nullptr;

    // 编辑状态
    TerrainEditMode m_editMode = TerrainEditMode::Sculpt;
    float m_brushSize = 5.0f;
    float m_brushStrength = 0.5f;
    int m_selectedLayer = 0;
    int m_selectedFoliage = 0;

    // 地形设置
    int m_terrainSize = 256;
    float m_terrainScale = 100.0f;
    float m_heightScale = 50.0f;
    int m_seed = 42;
};

} // namespace spark
