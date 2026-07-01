#pragma once

#include "math/math_types.h"
#include "ecs/entity.h"
#include <string>
#include <vector>
#include <functional>

namespace spark {

class Entity;
class Scene;

// 编辑器模式
enum class EditorMode {
    Edit,    // 编辑模式
    Play,    // 播放模式
    Pause    // 暂停模式
};

// 编辑器状态管理
class EditorState {
public:
    static EditorState& get();

    // 模式切换
    void play();
    void pause();
    void stop();
    void togglePause();

    EditorMode getMode() const { return m_mode; }
    bool isPlaying() const { return m_mode == EditorMode::Play; }
    bool isPaused() const { return m_mode == EditorMode::Pause; }
    bool isEditing() const { return m_mode == EditorMode::Edit; }

    // 选中的实体
    void selectEntity(EntityID id);
    void deselectEntity();
    EntityID getSelectedEntityID() const { return m_selectedEntityID; }
    bool hasSelection() const { return m_selectedEntityID != 0; }

    // Gizmo 模式
    enum class GizmoMode { Translate, Rotate, Scale };

    void setGizmoMode(GizmoMode mode) { m_gizmoMode = mode; }
    GizmoMode getGizmoMode() const { return m_gizmoMode; }
    void cycleGizmoMode();

    // Gizmo 空间
    enum class GizmoSpace { World, Local };

    void setGizmoSpace(GizmoSpace space) { m_gizmoSpace = space; }
    GizmoSpace getGizmoSpace() const { return m_gizmoSpace; }
    void toggleGizmoSpace();

    // 场景快照（Play 前保存）
    void saveSceneSnapshot(Scene& scene);
    void restoreSceneSnapshot(Scene& scene);

    // 回调
    using ModeChangeCallback = std::function<void(EditorMode)>;
    void setModeChangeCallback(ModeChangeCallback callback) { m_modeChangeCallback = callback; }

    using SelectionChangeCallback = std::function<void(EntityID)>;
    void setSelectionChangeCallback(SelectionChangeCallback callback) { m_selectionChangeCallback = callback; }

private:
    EditorState() = default;

    EditorMode m_mode = EditorMode::Edit;
    EntityID m_selectedEntityID = 0;
    GizmoMode m_gizmoMode = GizmoMode::Translate;
    GizmoSpace m_gizmoSpace = GizmoSpace::World;

    ModeChangeCallback m_modeChangeCallback;
    SelectionChangeCallback m_selectionChangeCallback;

    // 场景快照数据
    std::string m_sceneSnapshot;
};

} // namespace spark
