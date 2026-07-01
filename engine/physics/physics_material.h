#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace spark {

// 物理材质
struct PhysicsMaterial {
    std::string name;
    float staticFriction = 0.5f;      // 静摩擦系数
    float dynamicFriction = 0.5f;     // 动摩擦系数
    float restitution = 0.3f;         // 弹性系数
    float density = 1.0f;             // 密度 (kg/m³)
    float frictionCombine = 0.0f;     // 摩擦混合模式
    float restitutionCombine = 0.0f;  // 弹性混合模式
};

// 混合模式
enum class CombineMode {
    Average,
    Minimum,
    Maximum,
    Multiply
};

// 物理材质管理器
class PhysicsMaterialManager {
public:
    static PhysicsMaterialManager& get();

    // 创建材质
    std::shared_ptr<PhysicsMaterial> createMaterial(const std::string& name,
                                                     float staticFriction = 0.5f,
                                                     float dynamicFriction = 0.5f,
                                                     float restitution = 0.3f,
                                                     float density = 1.0f);

    // 获取材质
    std::shared_ptr<PhysicsMaterial> getMaterial(const std::string& name) const;

    // 预设材质
    void createDefaultMaterials();

    // 获取所有材质
    const std::unordered_map<std::string, std::shared_ptr<PhysicsMaterial>>& getMaterials() const { return m_materials; }

private:
    PhysicsMaterialManager() = default;

    std::unordered_map<std::string, std::shared_ptr<PhysicsMaterial>> m_materials;
};

} // namespace spark
