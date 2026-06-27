#include "command_system.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include "scene/scene.h"
#include "core/log.h"

namespace spark {

// =============================================
// CommandManager
// =============================================

CommandManager& CommandManager::get() {
    static CommandManager instance;
    return instance;
}

void CommandManager::executeCommand(std::unique_ptr<Command> command) {
    // 执行命令
    command->execute();

    // 添加到撤销栈
    m_undoStack.push_back(std::move(command));

    // 清空重做栈
    m_redoStack.clear();

    // 限制历史大小
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

void CommandManager::clear() {
    m_undoStack.clear();
    m_redoStack.clear();
}

bool CommandManager::canUndo() const {
    return !m_undoStack.empty();
}

bool CommandManager::canRedo() const {
    return !m_redoStack.empty();
}

size_t CommandManager::getUndoCount() const {
    return m_undoStack.size();
}

size_t CommandManager::getRedoCount() const {
    return m_redoStack.size();
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

// =============================================
// SetPositionCommand
// =============================================

SetPositionCommand::SetPositionCommand(Entity* entity, const Vec3& oldPos, const Vec3& newPos)
    : m_entity(entity), m_oldPosition(oldPos), m_newPosition(newPos) {}

void SetPositionCommand::execute() {
    if (m_entity && m_entity->hasComponent<TransformComponent>()) {
        m_entity->getComponent<TransformComponent>().position = m_newPosition;
    }
}

void SetPositionCommand::undo() {
    if (m_entity && m_entity->hasComponent<TransformComponent>()) {
        m_entity->getComponent<TransformComponent>().position = m_oldPosition;
    }
}

std::string SetPositionCommand::getDescription() const {
    return "Set Position";
}

// =============================================
// SetRotationCommand
// =============================================

SetRotationCommand::SetRotationCommand(Entity* entity, const Vec3& oldRot, const Vec3& newRot)
    : m_entity(entity), m_oldRotation(oldRot), m_newRotation(newRot) {}

void SetRotationCommand::execute() {
    if (m_entity && m_entity->hasComponent<TransformComponent>()) {
        m_entity->getComponent<TransformComponent>().rotation = m_newRotation;
    }
}

void SetRotationCommand::undo() {
    if (m_entity && m_entity->hasComponent<TransformComponent>()) {
        m_entity->getComponent<TransformComponent>().rotation = m_oldRotation;
    }
}

std::string SetRotationCommand::getDescription() const {
    return "Set Rotation";
}

// =============================================
// SetScaleCommand
// =============================================

SetScaleCommand::SetScaleCommand(Entity* entity, const Vec3& oldScale, const Vec3& newScale)
    : m_entity(entity), m_oldScale(oldScale), m_newScale(newScale) {}

void SetScaleCommand::execute() {
    if (m_entity && m_entity->hasComponent<TransformComponent>()) {
        m_entity->getComponent<TransformComponent>().scale = m_newScale;
    }
}

void SetScaleCommand::undo() {
    if (m_entity && m_entity->hasComponent<TransformComponent>()) {
        m_entity->getComponent<TransformComponent>().scale = m_oldScale;
    }
}

std::string SetScaleCommand::getDescription() const {
    return "Set Scale";
}

// =============================================
// CreateEntityCommand
// =============================================

CreateEntityCommand::CreateEntityCommand(Scene* scene, const std::string& name)
    : m_scene(scene), m_name(name) {}

void CreateEntityCommand::execute() {
    if (m_scene) {
        auto& entity = m_scene->createEntity(m_name);
        m_createdEntityID = entity.getID();
    }
}

void CreateEntityCommand::undo() {
    if (m_scene && m_createdEntityID != 0) {
        m_scene->destroyEntity(m_createdEntityID);
    }
}

std::string CreateEntityCommand::getDescription() const {
    return "Create Entity: " + m_name;
}

// =============================================
// DeleteEntityCommand
// =============================================

DeleteEntityCommand::DeleteEntityCommand(Scene* scene, EntityID entityID)
    : m_scene(scene), m_entityID(entityID) {}

void DeleteEntityCommand::execute() {
    if (m_scene) {
        try {
            auto& entity = m_scene->getEntity(m_entityID);
            if (entity.hasComponent<TagComponent>()) {
                m_entityName = entity.getComponent<TagComponent>().name;
            }
            m_scene->destroyEntity(m_entityID);
        } catch (...) {
            SPARK_CORE_ERROR("Failed to delete entity {0}", m_entityID);
        }
    }
}

void DeleteEntityCommand::undo() {
    // TODO: 恢复已删除的实体（需要保存完整状态）
    SPARK_CORE_WARN("Undo delete entity not fully implemented");
}

std::string DeleteEntityCommand::getDescription() const {
    return "Delete Entity: " + m_entityName;
}

// =============================================
// AddComponentCommand
// =============================================

AddComponentCommand::AddComponentCommand(Entity* entity, const std::string& componentType)
    : m_entity(entity), m_componentType(componentType) {}

void AddComponentCommand::execute() {
    if (!m_entity) return;

    // 根据类型添加组件
    if (m_componentType == "Transform" && !m_entity->hasComponent<TransformComponent>()) {
        m_entity->addComponent<TransformComponent>();
    } else if (m_componentType == "Camera" && !m_entity->hasComponent<CameraComponent>()) {
        m_entity->addComponent<CameraComponent>();
    } else if (m_componentType == "Light" && !m_entity->hasComponent<LightComponent>()) {
        m_entity->addComponent<LightComponent>();
    } else if (m_componentType == "MeshRenderer" && !m_entity->hasComponent<MeshRendererComponent>()) {
        m_entity->addComponent<MeshRendererComponent>();
    } else if (m_componentType == "RigidBody" && !m_entity->hasComponent<RigidBodyComponent>()) {
        m_entity->addComponent<RigidBodyComponent>();
    } else if (m_componentType == "Collider" && !m_entity->hasComponent<ColliderComponent>()) {
        m_entity->addComponent<ColliderComponent>();
    }
}

void AddComponentCommand::undo() {
    if (!m_entity) return;

    // 移除添加的组件
    if (m_componentType == "Transform") {
        m_entity->removeComponent<TransformComponent>();
    } else if (m_componentType == "Camera") {
        m_entity->removeComponent<CameraComponent>();
    } else if (m_componentType == "Light") {
        m_entity->removeComponent<LightComponent>();
    } else if (m_componentType == "MeshRenderer") {
        m_entity->removeComponent<MeshRendererComponent>();
    } else if (m_componentType == "RigidBody") {
        m_entity->removeComponent<RigidBodyComponent>();
    } else if (m_componentType == "Collider") {
        m_entity->removeComponent<ColliderComponent>();
    }
}

std::string AddComponentCommand::getDescription() const {
    return "Add Component: " + m_componentType;
}

// =============================================
// RemoveComponentCommand
// =============================================

RemoveComponentCommand::RemoveComponentCommand(Entity* entity, const std::string& componentType)
    : m_entity(entity), m_componentType(componentType) {}

void RemoveComponentCommand::execute() {
    if (!m_entity) return;

    if (m_componentType == "Transform") {
        m_entity->removeComponent<TransformComponent>();
    } else if (m_componentType == "Camera") {
        m_entity->removeComponent<CameraComponent>();
    } else if (m_componentType == "Light") {
        m_entity->removeComponent<LightComponent>();
    } else if (m_componentType == "MeshRenderer") {
        m_entity->removeComponent<MeshRendererComponent>();
    } else if (m_componentType == "RigidBody") {
        m_entity->removeComponent<RigidBodyComponent>();
    } else if (m_componentType == "Collider") {
        m_entity->removeComponent<ColliderComponent>();
    }
}

void RemoveComponentCommand::undo() {
    if (!m_entity) return;

    // TODO: 恢复已删除的组件（需要保存组件数据）
    SPARK_CORE_WARN("Undo remove component not fully implemented");
}

std::string RemoveComponentCommand::getDescription() const {
    return "Remove Component: " + m_componentType;
}

} // namespace spark
