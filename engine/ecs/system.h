#pragma once

#include "entity.h"
#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace spark {

class Scene;

// Base system class
class System {
public:
    virtual ~System() = default;
    virtual void update(Scene& scene, float deltaTime) = 0;
    virtual void render(Scene& scene) {}
};

// System manager
class SystemManager {
public:
    template<typename T, typename... Args>
    T& addSystem(Args&&... args) {
        static_assert(std::is_base_of<System, T>::value, "T must inherit from System");
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *system;
        m_systems.push_back(std::move(system));
        return ref;
    }

    void update(Scene& scene, float deltaTime) {
        for (auto& system : m_systems) {
            system->update(scene, deltaTime);
        }
    }

    void render(Scene& scene) {
        for (auto& system : m_systems) {
            system->render(scene);
        }
    }

private:
    std::vector<std::unique_ptr<System>> m_systems;
};

} // namespace spark
