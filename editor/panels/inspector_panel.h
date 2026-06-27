#pragma once

#include <functional>
#include <string>

namespace spark {

class Entity;

class InspectorPanel {
public:
    InspectorPanel();
    ~InspectorPanel();

    void render();
    void setEntity(Entity* entity) { m_selectedEntity = entity; }
    Entity* getEntity() const { return m_selectedEntity; }

private:
    void renderHeader();
    void renderTransform();
    void renderMeshRenderer();
    void renderLight();
    void renderCamera();
    void renderRigidBody();
    void renderCollider();
    void renderScript();
    void renderAddComponent();

    Entity* m_selectedEntity = nullptr;
};

} // namespace spark
