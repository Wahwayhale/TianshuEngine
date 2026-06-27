#pragma once

#include <string>
#include <memory>

namespace spark {

class Material;

class MaterialEditorPanel {
public:
    MaterialEditorPanel();
    ~MaterialEditorPanel();

    void render();

    void setMaterial(std::shared_ptr<Material> material) { m_material = material; }
    std::shared_ptr<Material> getMaterial() const { return m_material; }

private:
    void renderMaterialProperties();
    void renderTextureSlots();
    void renderPreview();
    void renderPresets();

    std::shared_ptr<Material> m_material;

    // 预览
    bool m_showPreview = true;
    float m_previewRotation = 0.0f;
};

} // namespace spark
