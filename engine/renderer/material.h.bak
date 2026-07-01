#pragma once

#include "math/math_types.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace spark {

class Texture;

// PBR 材质属性
struct MaterialProperties {
    // 基础颜色
    Vec4 albedo = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;

    // 自发光
    Vec3 emission = Vec3(0.0f);
    float emissionIntensity = 0.0f;

    // 法线强度
    float normalStrength = 1.0f;

    // 视差映射
    float heightScale = 0.05f;

    // 透明度
    float alphaCutoff = 0.5f;
    bool doubleSided = false;

    // UV 变换
    Vec2 uvOffset = Vec2(0.0f);
    Vec2 uvScale = Vec2(1.0f);
};

// 材质纹理槽
enum class MaterialTextureSlot {
    Albedo,
    Normal,
    MetallicRoughness,
    AO,
    Emission,
    Height,
    Count
};

// PBR 材质类
class Material {
public:
    Material(const std::string& name = "Default");
    ~Material();

    // 属性
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    // 材质属性
    MaterialProperties& getProperties() { return m_properties; }
    const MaterialProperties& getProperties() const { return m_properties; }
    void setProperties(const MaterialProperties& props) { m_properties = props; }

    // 纹理管理
    void setTexture(MaterialTextureSlot slot, std::shared_ptr<Texture> texture);
    std::shared_ptr<Texture> getTexture(MaterialTextureSlot slot) const;
    bool hasTexture(MaterialTextureSlot slot) const;
    void removeTexture(MaterialTextureSlot slot);

    // 便捷方法
    void setAlbedoTexture(std::shared_ptr<Texture> tex) { setTexture(MaterialTextureSlot::Albedo, tex); }
    void setNormalTexture(std::shared_ptr<Texture> tex) { setTexture(MaterialTextureSlot::Normal, tex); }
    void setMetallicRoughnessTexture(std::shared_ptr<Texture> tex) { setTexture(MaterialTextureSlot::MetallicRoughness, tex); }
    void setAOTexture(std::shared_ptr<Texture> tex) { setTexture(MaterialTextureSlot::AO, tex); }
    void setEmissionTexture(std::shared_ptr<Texture> tex) { setTexture(MaterialTextureSlot::Emission, tex); }
    void setHeightTexture(std::shared_ptr<Texture> tex) { setTexture(MaterialTextureSlot::Height, tex); }

    // 预设材质
    static std::shared_ptr<Material> createDefault();
    static std::shared_ptr<Material> createMetal(const Vec3& color, float roughness = 0.2f);
    static std::shared_ptr<Material> createPlastic(const Vec3& color, float roughness = 0.5f);
    static std::shared_ptr<Material> createGlass(float opacity = 0.3f);
    static std::shared_ptr<Material> createEmissive(const Vec3& color, float intensity = 1.0f);

private:
    std::string m_name;
    MaterialProperties m_properties;
    std::shared_ptr<Texture> m_textures[static_cast<int>(MaterialTextureSlot::Count)];
};

// 材质管理器
class MaterialManager {
public:
    static MaterialManager& get();

    std::shared_ptr<Material> createMaterial(const std::string& name);
    std::shared_ptr<Material> getMaterial(const std::string& name) const;
    void removeMaterial(const std::string& name);
    bool hasMaterial(const std::string& name) const;

    void createDefaultMaterials();

    const std::unordered_map<std::string, std::shared_ptr<Material>>& getMaterials() const { return m_materials; }

private:
    MaterialManager() = default;

    std::unordered_map<std::string, std::shared_ptr<Material>> m_materials;
};

} // namespace spark
