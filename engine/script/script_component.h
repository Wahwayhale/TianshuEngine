#pragma once

#include "ecs/component.h"
#include <string>
#include <functional>

namespace spark {

class Entity;

struct ScriptComponent : public Component {
    std::string scriptPath;

    // Callbacks
    std::function<void(Entity&, float)> onUpdate;
    std::function<void(Entity&)> onInit;
    std::function<void(Entity&)> onDestroy;
    std::function<void(Entity&, Entity&)> onCollision;

    ScriptComponent(const std::string& path = "") : scriptPath(path) {}
};

} // namespace spark
