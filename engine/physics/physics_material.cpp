#include "physics_material.h"
#include "core/log.h"

namespace spark {

PhysicsMaterialManager& PhysicsMaterialManager::get() {
    static PhysicsMaterialManager instance;
    return instance;
}

std::shared_ptr<PhysicsMaterial> PhysicsMaterialManager::createMaterial(
    const std::string& name,
    float staticFriction,
    float dynamicFriction,
    float restitution,
    float density) {

    auto material = std::make_shared<PhysicsMaterial>();
    material->name = name;
    material->staticFriction = staticFriction;
    material->dynamicFriction = dynamicFriction;
    material->restitution = restitution;
    material->density = density;

    m_materials[name] = material;
    SPARK_CORE_INFO("Physics material created: {0}", name);
    return material;
}

std::shared_ptr<PhysicsMaterial> PhysicsMaterialManager::getMaterial(const std::string& name) const {
    auto it = m_materials.find(name);
    if (it != m_materials.end()) {
        return it->second;
    }
    return nullptr;
}

void PhysicsMaterialManager::createDefaultMaterials() {
    // 默认材质
    createMaterial("Default", 0.5f, 0.5f, 0.3f, 1.0f);

    // 金属
    createMaterial("Metal", 0.4f, 0.3f, 0.2f, 7.8f);

    // 木头
    createMaterial("Wood", 0.6f, 0.5f, 0.3f, 0.6f);

    // 石头
    createMaterial("Stone", 0.7f, 0.6f, 0.2f, 2.5f);

    // 橡胶
    createMaterial("Rubber", 0.9f, 0.8f, 0.8f, 1.2f);

    // 冰
    createMaterial("Ice", 0.02f, 0.01f, 0.1f, 0.9f);

    // 玻璃
    createMaterial("Glass", 0.4f, 0.3f, 0.5f, 2.5f);

    // 布料
    createMaterial("Cloth", 0.8f, 0.7f, 0.1f, 0.3f);

    // 沙子
    createMaterial("Sand", 0.9f, 0.8f, 0.1f, 1.6f);

    // 水
    createMaterial("Water", 0.0f, 0.0f, 0.0f, 1.0f);

    SPARK_CORE_INFO("Default physics materials created.");
}

} // namespace spark
