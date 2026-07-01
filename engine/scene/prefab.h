#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace spark {

class Entity;
class Scene;

// 预制体
class Prefab {
public:
    Prefab(const std::string& name);
    ~Prefab();

    // 保存预制体
    bool saveToFile(const std::string& filepath);
    bool loadFromFile(const std::string& filepath);

    // 实例化
    Entity* instantiate(Scene& scene, const Vec3& position = Vec3(0.0f),
                        const Vec3& rotation = Vec3(0.0f),
                        const Vec3& scale = Vec3(1.0f));

    // 序列化
    std::string serialize() const;
    bool deserialize(const std::string& json);

    // 属性
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

private:
    std::string m_name;
    std::string m_serializedData;
};

// 预制体管理器
class PrefabManager {
public:
    static PrefabManager& get();

    // 创建预制体
    std::shared_ptr<Prefab> createPrefab(const std::string& name, Entity& entity);

    // 加载预制体
    std::shared_ptr<Prefab> loadPrefab(const std::string& filepath);

    // 获取预制体
    std::shared_ptr<Prefab> getPrefab(const std::string& name) const;

    // 删除预制体
    void removePrefab(const std::string& name);

    // 保存所有预制体
    void saveAll();

private:
    PrefabManager() = default;

    std::unordered_map<std::string, std::shared_ptr<Prefab>> m_prefabs;
};

} // namespace spark
