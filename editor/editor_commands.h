#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace spark {

class Entity;
class Scene;

// 编辑器命令基类（撤销/重做）
class EditorCommand {
public:
    virtual ~EditorCommand() = default;

    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string getDescription() const = 0;
};

// 创建实体命令
class CreateEntityCommand : public EditorCommand {
public:
    CreateEntityCommand(Scene* scene, const std::string& name);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    Scene* m_scene;
    std::string m_name;
    uint64_t m_createdEntityId = 0;
};

// 删除实体命令
class DeleteEntityCommand : public EditorCommand {
public:
    DeleteEntityCommand(Scene* scene, uint64_t entityId);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    Scene* m_scene;
    uint64_t m_entityId;
    std::string m_entityName;
    // TODO: 保存实体完整状态以便恢复
};

// 修改变换命令
class SetTransformCommand : public EditorCommand {
public:
    SetTransformCommand(Entity* entity, const Vec3& oldPos, const Vec3& newPos,
                        const Vec3& oldRot, const Vec3& newRot,
                        const Vec3& oldScale, const Vec3& newScale);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    Entity* m_entity;
    Vec3 m_oldPosition, m_newPosition;
    Vec3 m_oldRotation, m_newRotation;
    Vec3 m_oldScale, m_newScale;
};

// 修改材质命令
class SetMaterialCommand : public EditorCommand {
public:
    SetMaterialCommand(Entity* entity, const Vec4& oldAlbedo, const Vec4& newAlbedo,
                       float oldMetallic, float newMetallic,
                       float oldRoughness, float newRoughness);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;

private:
    Entity* m_entity;
    Vec4 m_oldAlbedo, m_newAlbedo;
    float m_oldMetallic, m_newMetallic;
    float m_oldRoughness, m_newRoughness;
};

// 命令管理器
class CommandManager {
public:
    static CommandManager& get();

    // 执行命令
    void executeCommand(std::unique_ptr<EditorCommand> command);

    // 撤销/重做
    void undo();
    void redo();

    // 状态
    bool canUndo() const;
    bool canRedo() const;

    // 历史
    std::vector<std::string> getUndoHistory() const;
    std::vector<std::string> getRedoHistory() const;

    // 清空
    void clear();

private:
    CommandManager() = default;

    std::vector<std::unique_ptr<EditorCommand>> m_undoStack;
    std::vector<std::unique_ptr<EditorCommand>> m_redoStack;
    size_t m_maxHistorySize = 100;
};

} // namespace spark
