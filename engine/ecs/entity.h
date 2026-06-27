#pragma once

#include "component.h"
#include <unordered_map>
#include <memory>
#include <typeindex>

namespace spark {

class Entity {
public:
    Entity(EntityID id) : m_id(id) {}

    EntityID getID() const { return m_id; }

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *component;
        m_components[std::type_index(typeid(T))] = std::move(component);
        return ref;
    }

    template<typename T>
    T& getComponent() {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        auto it = m_components.find(std::type_index(typeid(T)));
        if (it == m_components.end()) {
            throw std::runtime_error("Component not found!");
        }
        return *static_cast<T*>(it->second.get());
    }

    template<typename T>
    bool hasComponent() const {
        return m_components.find(std::type_index(typeid(T))) != m_components.end();
    }

    template<typename T>
    void removeComponent() {
        m_components.erase(std::type_index(typeid(T)));
    }

private:
    EntityID m_id;
    std::unordered_map<std::type_index, std::unique_ptr<Component>> m_components;
};

} // namespace spark
