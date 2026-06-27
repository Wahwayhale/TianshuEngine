#include "scene.h"

namespace spark {

Scene::Scene() = default;
Scene::~Scene() = default;

Entity& Scene::createEntity(const std::string& name) {
    auto entity = std::make_unique<Entity>(m_nextEntityID);
    Entity& ref = *entity;

    // 自动添加 TagComponent
    ref.addComponent<TagComponent>(name);

    m_entities[m_nextEntityID] = std::move(entity);
    m_nextEntityID++;
    return ref;
}

void Scene::destroyEntity(EntityID id) {
    m_entities.erase(id);
}

Entity& Scene::getEntity(EntityID id) {
    auto it = m_entities.find(id);
    if (it == m_entities.end()) {
        throw std::runtime_error("Entity not found!");
    }
    return *it->second;
}

void Scene::update(float deltaTime) {
    m_systemManager.update(*this, deltaTime);
}

} // namespace spark
