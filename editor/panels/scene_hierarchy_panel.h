#pragma once

#include "ecs/entity.h"
#include <functional>
#include <string>

namespace spark {

class Scene;

class SceneHierarchyPanel {
public:
    SceneHierarchyPanel();
    ~SceneHierarchyPanel();

    void render(Scene& scene);

    using EntitySelectedCallback = std::function<void(Entity*)>;
    void setEntitySelectedCallback(EntitySelectedCallback callback) {
        m_entitySelectedCallback = callback;
    }

    EntityID getSelectedEntityID() const { return m_selectedEntityID; }

private:
    void renderToolbar();
    void renderSearchBar();
    void drawEntityNode(Entity& entity);

    EntitySelectedCallback m_entitySelectedCallback;
    EntityID m_selectedEntityID = 0;

    // 搜索
    char m_searchBuffer[256] = "";
    bool m_showSearch = false;
};

} // namespace spark
