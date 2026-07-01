#include "prefab.h"
#include "scene.h"
#include "ecs/components.h"
#include "core/log.h"
#include <fstream>

namespace spark {

// =============================================
// Prefab
// =============================================

Prefab::Prefab(const std::string& name) : m_name(name) {}
Prefab::~Prefab() = default;

bool Prefab::saveToFile(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        SPARK_CORE_ERROR("Failed to save prefab: {0}", filepath);
        return false;
    }

    file << serialize();
    file.close();

    SPARK_CORE_INFO("Prefab saved: {0}", filepath);
    return true;
}

bool Prefab::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        SPARK_CORE_ERROR("Failed to load prefab: {0}", filepath);
        return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    m_serializedData = ss.str();

    file.close();

    SPARK_CORE_INFO("Prefab loaded: {0}", filepath);
    return true;
}

Entity* Prefab::instantiate(Scene& scene, const Vec3& position, const Vec3& rotation, const Vec3& scale) {
    // 创建实体
    auto& entity = scene.createEntity(m_name);

    // 添加变换组件
    auto& transform = entity.addComponent<TransformComponent>();
    transform.position = position;
    transform.rotation = rotation;
    transform.scale = scale;

    // TODO: 从序列化数据恢复其他组件

    SPARK_CORE_INFO("Prefab instantiated: {0}", m_name);
    return &entity;
}

std::string Prefab::serialize() const {
    // TODO: 序列化预制体数据
    return "{}";
}

bool Prefab::deserialize(const std::string& json) {
    // TODO: 反序列化预制体数据
    m_serializedData = json;
    return true;
}

// =============================================
// PrefabManager
// =============================================

PrefabManager& PrefabManager::get() {
    static PrefabManager instance;
    return instance;
}

std::shared_ptr<Prefab> PrefabManager::createPrefab(const std::string& name, Entity& entity) {
    auto prefab = std::make_shared<Prefab>(name);

    // TODO: 从实体序列化数据

    m_prefabs[name] = prefab;
    SPARK_CORE_INFO("Prefab created: {0}", name);
    return prefab;
}

std::shared_ptr<Prefab> PrefabManager::loadPrefab(const std::string& filepath) {
    auto prefab = std::make_shared<Prefab>("");
    if (prefab->loadFromFile(filepath)) {
        m_prefabs[prefab->getName()] = prefab;
        return prefab;
    }
    return nullptr;
}

std::shared_ptr<Prefab> PrefabManager::getPrefab(const std::string& name) const {
    auto it = m_prefabs.find(name);
    if (it != m_prefabs.end()) {
        return it->second;
    }
    return nullptr;
}

void PrefabManager::removePrefab(const std::string& name) {
    m_prefabs.erase(name);
}

void PrefabManager::saveAll() {
    for (const auto& [name, prefab] : m_prefabs) {
        prefab->saveToFile("assets/prefabs/" + name + ".prefab");
    }
}

} // namespace spark
