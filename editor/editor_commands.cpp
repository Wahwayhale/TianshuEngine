#include "editor_commands.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include "core/log.h"

namespace spark {

// =============================================
// CreateEntityCommand
// =============================================

CreateEntityCommand::CreateEntityCommand(Scene* scene, const std::string& name)
    : m_scene(scene), m_name(name) {}

void CreateEntityCommand::execute() {
    if (m_scene) {
        auto& entity = m_scene->createEntity(m_name);
        m_createdEntityId = entity.getID();
    }
}

void CreateEntityCommand::undo() {
    if (m_scene && m_createdEntityId != 0) {
        m_scene->destroyEntity(m_createdEntityId);
    }
}

std::string CreateEntityCommand::getDescription() const {
    return "Create Entity: " + m_name;
}

// =============================================
// DeleteEntityCommand
// =============================================

DeleteEntityCommand::DeleteEntityCommand(Scene* scene, uint64_t entityId)
    : m_scene(scene), m_entityId(entityId) {}

void DeleteEntityCommand::execute() {
    if (m_scene) {
        try {
            auto& entity = m_scene->getEntity(m_entityId);
            if (entity.hasComponent<TagComponent>()) {
                m_entityName = entity.getComponent<TagComponent>().name;
            }
            m_scene->destroyEntity(m_entityId);
        } catch (...) {
            SPARK_CORE_ERROR("Failed to delete entity {0}", m_entityId);
        }
    }
}

void DeleteEntityCommand::undo() {
    // TODO: 恢复已删除的实体
    SPARK_CORE_WARN("Undo delete entity not implemented");
}

std::string DeleteEntityCommand::getDescription() const {
    return "Delete Entity: " + m_entityName;
}

// =============================================
// SetTransformCommand
// =============================================

SetTransformCommand::SetTransformCommand(Entity* entity,
                                          const Vec3& oldPos, const Vec3& newPos,
                                          const Vec3& oldRot, const Vec3& newRot,
                                          const Vec3& oldScale, const Vec3& newScale)
    : m_entity(entity),
      m_oldPosition(oldPos), m_newPosition(newPos),
      m_oldRotation(oldRot), m_newRotation(newRot),
      m_oldScale(oldScale), m_newScale(newScale) {}

void SetTransformCommand::execute() {
    if (m_entity && m_entity->hasComponent<TransformComponent>()) {
        auto& t = m_entity->getComponent<TransformComponent>();
        t.position = m_newPosition;
        t.rotation = m_newRotation;
        t.scale = m_newScale;
    }
}

void SetTransformCommand::undo() {
    if (m_entity && m_entity->hasComponent<TransformComponent>()) {
        auto& t = m_entity->getComponent<TransformComponent>();
        t.position = m_oldPosition;
        t.rotation = m_oldRotation;
        t.scale = m_oldScale;
    }
}

std::string SetTransformCommand::getDescription() const {
    return "Set Transform";
}

// =============================================
// SetMaterialCommand
// =============================================

SetMaterialCommand::SetMaterialCommand(Entity* entity,
                                        const Vec4& oldAlbedo, const Vec4& newAlbedo,
                                        float oldMetallic, float newMetallic,
                                        float oldRoughness, float newRoughness)
    : m_entity(entity),
      m_oldAlbedo(oldAlbedo), m_newAlbedo(newAlbedo),
      m_oldMetallic(oldMetallic), m_newMetallic(newMetallic),
      m_oldRoughness(oldRoughness), m_newRoughness(newRoughness) {}

void SetMaterialCommand::execute() {
    if (m_entity && m_entity->hasComponent<MeshRendererComponent>()) {
        auto& r = m_entity->getComponent<MeshRendererComponent>();
        r.albedo = m_newAlbedo;
        r.metallic = m_newMetallic;
        r.roughness = m_newRoughness;
    }
}

void SetMaterialCommand::undo() {
    if (m_entity && m_entity->hasComponent<MeshRendererComponent>()) {
        auto& r = m_entity->getComponent<MeshRendererComponent>();
        r.albedo = m_oldAlbedo;
        r.metallic = m_oldMetallic;
        r.roughness = m_oldRoughness;
    }
}

std::string SetMaterialCommand::getDescription() const {
    return "Set Material";
}

// =============================================
// CommandManager
// =============================================

CommandManager& CommandManager::get() {
    static CommandManager instance;
    return instance;
}

void CommandManager::executeCommand(std::unique_ptr<EditorCommand> command) {
    command->execute();
    m_undoStack.push_back(std::move(command));
    m_redoStack.clear();

    if (m_undoStack.size() > m_maxHistorySize) {
        m_undoStack.erase(m_undoStack.begin());
    }

    SPARK_CORE_INFO("Command executed: {0}", m_undoStack.back()->getDescription());
}

void CommandManager::undo() {
    if (!canUndo()) return;

    auto& command = m_undoStack.back();
    command->undo();

    SPARK_CORE_INFO("Undo: {0}", command->getDescription());

    m_redoStack.push_back(std::move(command));
    m_undoStack.pop_back();
}

void CommandManager::redo() {
    if (!canRedo()) return;

    auto& command = m_redoStack.back();
    command->execute();

    SPARK_CORE_INFO("Redo: {0}", command->getDescription());

    m_undoStack.push_back(std::move(command));
    m_redoStack.pop_back();
}

bool CommandManager::canUndo() const {
    return !m_undoStack.empty();
}

bool CommandManager::canRedo() const {
    return !m_redoStack.empty();
}

std::vector<std::string> CommandManager::getUndoHistory() const {
    std::vector<std::string> history;
    for (const auto& cmd : m_undoStack) {
        history.push_back(cmd->getDescription());
    }
    return history;
}

std::vector<std::string> CommandManager::getRedoHistory() const {
    std::vector<std::string> history;
    for (const auto& cmd : m_redoStack) {
        history.push_back(cmd->getDescription());
    }
    return history;
}

void CommandManager::clear() {
    m_undoStack.clear();
    m_redoStack.clear();
}

} // namespace spark
