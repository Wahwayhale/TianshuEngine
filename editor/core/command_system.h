#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "math/math_types.h"
#include "ecs/entity.h"

namespace spark {

class Entity;
class Scene;

// 命令基类（Command 模式）
class Command {
public:
    virtual ~Command() = default;

    // 执行命令
    virtual void execute() = 0;

    // 撤销命令
    virtual void undo() = 0;

    // 命令描述（用于调试）
    virtual std::string getDescription() const = 0;
};

// 命令管理器（撤销/重做）
class CommandManager {
public:
    static CommandManager& get();

    // 执行命令（自动添加到历史）
    void executeCommand(std::unique_ptr<Command> command);

    // 撤销
    void undo();

    // 重做
    void redo();

    // 清空历史
    void clear();

    // 状态查询
    bool canUndo() const;
    bool canRedo() const;

    size_t getUndoCount() const;
    size_t getRedoCount() const;

    // 获取历史描述
    std::vector<std::string> getUndoHistory() const;
    std::vector<std::string> getRedoHistory() const;

private:
    CommandManager() = default;

    std::vector<std::unique_ptr<Command>> m_undoStack;
    std::vector<std::unique_ptr<Command>> m_redoStack;
    size_t m_maxHistorySize = 100;
};

// =============================================
// 具体命令实现
// =============================================

// 修改实体位置命令
class SetPositionCommand : public Command {
public:
    SetPositionCommand(Entity* entity, const Vec3& oldPos, const Vec3& newPos);

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    Entity* m_entity;
    Vec3 m_oldPosition;
    Vec3 m_newPosition;
};

// 修改实体旋转命令
class SetRotationCommand : public Command {
public:
    SetRotationCommand(Entity* entity, const Vec3& oldRot, const Vec3& newRot);

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    Entity* m_entity;
    Vec3 m_oldRotation;
    Vec3 m_newRotation;
};

// 修改实体缩放命令
class SetScaleCommand : public Command {
public:
    SetScaleCommand(Entity* entity, const Vec3& oldScale, const Vec3& newScale);

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    Entity* m_entity;
    Vec3 m_oldScale;
    Vec3 m_newScale;
};

// 创建实体命令
class CreateEntityCommand : public Command {
public:
    CreateEntityCommand(Scene* scene, const std::string& name);

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    Scene* m_scene;
    std::string m_name;
    EntityID m_createdEntityID = 0;
};

// 删除实体命令
class DeleteEntityCommand : public Command {
public:
    DeleteEntityCommand(Scene* scene, EntityID entityID);

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    Scene* m_scene;
    EntityID m_entityID;
    std::string m_entityName;
    // TODO: 保存实体完整状态以便恢复
};

// 添加组件命令
class AddComponentCommand : public Command {
public:
    AddComponentCommand(Entity* entity, const std::string& componentType);

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    Entity* m_entity;
    std::string m_componentType;
};

// 删除组件命令
class RemoveComponentCommand : public Command {
public:
    RemoveComponentCommand(Entity* entity, const std::string& componentType);

    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    Entity* m_entity;
    std::string m_componentType;
};

} // namespace spark
