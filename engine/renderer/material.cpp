#include "material.h"
#include "texture.h"
#include "core/log.h"

namespace spark {

// =============================================
// Material
// =============================================

Material::Material(const std::string& name) : m_name(name) {
    // 初始化纹理数组
    for (int i = 0; i < static_cast<int>(MaterialTextureSlot::Count); i++) {
        m_textures[i] = nullptr;
    }
}

Material::~Material() = default;

void Material::setTexture(MaterialTextureSlot slot, std::shared_ptr<Texture> texture) {
    int index = static_cast<int>(slot);
    if (index >= 0 && index < static_cast<int>(MaterialTextureSlot::Count)) {
        m_textures[index] = texture;
    }
}

std::shared_ptr<Texture> Material::getTexture(MaterialTextureSlot slot) const {
    int index = static_cast<int>(slot);
    if (index >= 0 && index < static_cast<int>(MaterialTextureSlot::Count)) {
        return m_textures[index];
    }
    return nullptr;
}

bool Material::hasTexture(MaterialTextureSlot slot) const {
    return getTexture(slot) != nullptr;
}

void Material::removeTexture(MaterialTextureSlot slot) {
    setTexture(slot, nullptr);
}

std::shared_ptr<Material> Material::createDefault() {
    auto material = std::make_shared<Material>("Default");
    material->m_properties.albedo = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
    material->m_properties.metallic = 0.0f;
    material->m_properties.roughness = 0.5f;
    return material;
}

std::shared_ptr<Material> Material::createMetal(const Vec3& color, float roughness) {
    auto material = std::make_shared<Material>("Metal");
    material->m_properties.albedo = Vec4(color.x, color.y, color.z, 1.0f);
    material->m_properties.metallic = 1.0f;
    material->m_properties.roughness = roughness;
    return material;
}

std::shared_ptr<Material> Material::createPlastic(const Vec3& color, float roughness) {
    auto material = std::make_shared<Material>("Plastic");
    material->m_properties.albedo = Vec4(color.x, color.y, color.z, 1.0f);
    material->m_properties.metallic = 0.0f;
    material->m_properties.roughness = roughness;
    return material;
}

std::shared_ptr<Material> Material::createGlass(float opacity) {
    auto material = std::make_shared<Material>("Glass");
    material->m_properties.albedo = Vec4(0.9f, 0.9f, 0.95f, opacity);
    material->m_properties.metallic = 0.0f;
    material->m_properties.roughness = 0.05f;
    return material;
}

std::shared_ptr<Material> Material::createEmissive(const Vec3& color, float intensity) {
    auto material = std::make_shared<Material>("Emissive");
    material->m_properties.albedo = Vec4(color.x, color.y, color.z, 1.0f);
    material->m_properties.emission = color;
    material->m_properties.emissionIntensity = intensity;
    return material;
}

// =============================================
// MaterialManager
// =============================================

MaterialManager& MaterialManager::get() {
    static MaterialManager instance;
    return instance;
}

std::shared_ptr<Material> MaterialManager::createMaterial(const std::string& name) {
    if (m_materials.find(name) != m_materials.end()) {
        SPARK_CORE_WARN("Material '{0}' already exists.", name);
        return m_materials[name];
    }

    auto material = std::make_shared<Material>(name);
    m_materials[name] = material;
    SPARK_CORE_INFO("Created material: {0}", name);
    return material;
}

std::shared_ptr<Material> MaterialManager::getMaterial(const std::string& name) const {
    auto it = m_materials.find(name);
    if (it != m_materials.end()) {
        return it->second;
    }
    return nullptr;
}

void MaterialManager::removeMaterial(const std::string& name) {
    m_materials.erase(name);
}

bool MaterialManager::hasMaterial(const std::string& name) const {
    return m_materials.find(name) != m_materials.end();
}

void MaterialManager::createDefaultMaterials() {
    // 创建默认材质
    auto defaultMat = Material::createDefault();
    m_materials["Default"] = defaultMat;

    // 创建常用材质预设
    auto redPlastic = Material::createPlastic(Vec3(0.8f, 0.1f, 0.1f));
    m_materials["Red Plastic"] = redPlastic;

    auto bluePlastic = Material::createPlastic(Vec3(0.1f, 0.2f, 0.8f));
    m_materials["Blue Plastic"] = bluePlastic;

    auto greenPlastic = Material::createPlastic(Vec3(0.1f, 0.6f, 0.2f));
    m_materials["Green Plastic"] = greenPlastic;

    auto gold = Material::createMetal(Vec3(1.0f, 0.84f, 0.0f), 0.1f);
    m_materials["Gold"] = gold;

    auto silver = Material::createMetal(Vec3(0.8f, 0.8f, 0.8f), 0.2f);
    m_materials["Silver"] = silver;

    auto copper = Material::createMetal(Vec3(0.95f, 0.64f, 0.54f), 0.3f);
    m_materials["Copper"] = copper;

    auto chrome = Material::createMetal(Vec3(0.9f, 0.9f, 0.9f), 0.05f);
    m_materials["Chrome"] = chrome;

    auto glass = Material::createGlass(0.3f);
    m_materials["Glass"] = glass;

    auto emissive = Material::createEmissive(Vec3(1.0f, 0.9f, 0.7f), 2.0f);
    m_materials["Emissive"] = emissive;

    SPARK_CORE_INFO("Created {0} default materials.", m_materials.size());
}

} // namespace spark
