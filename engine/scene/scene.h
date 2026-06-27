#pragma once

#include "ecs/entity.h"
#include "ecs/system.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>

namespace spark {

class Scene {
public:
    Scene();
    ~Scene();

    Entity& createEntity(const std::string& name = "Entity");
    void destroyEntity(EntityID id);

    Entity& getEntity(EntityID id);
    const std::unordered_map<EntityID, std::unique_ptr<Entity>>& getEntities() const { return m_entities; }

    // 遍历具有特定组件的实体
    template<typename... Components>
    void view(std::function<void(Entity&)> callback) {
        for (const auto& [id, entity] : m_entities) {
            if ((entity->hasComponent<Components>() && ...)) {
                callback(*entity);
            }
        }
    }

    // 遍历具有特定组件的实体（返回实体列表）
    template<typename... Components>
    std::vector<Entity*> getEntitiesWith() {
        std::vector<Entity*> result;
        for (const auto& [id, entity] : m_entities) {
            if ((entity->hasComponent<Components>() && ...)) {
                result.push_back(entity.get());
            }
        }
        return result;
    }

    // 获取实体数量
    size_t getEntityCount() const { return m_entities.size(); }

    // 系统管理
    template<typename T, typename... Args>
    T& addSystem(Args&&... args) {
        return m_systemManager.addSystem<T>(std::forward<Args>(args)...);
    }

    void update(float deltaTime);

private:
    std::unordered_map<EntityID, std::unique_ptr<Entity>> m_entities;
    EntityID m_nextEntityID = 1;
    SystemManager m_systemManager;
};

} // namespace spark
