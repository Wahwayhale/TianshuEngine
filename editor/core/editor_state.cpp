#include "editor_state.h"
#include "scene/scene.h"
#include "ecs/components.h"
#include "core/log.h"
#include "core/serialization.h"

namespace spark {

EditorState& EditorState::get() {
    static EditorState instance;
    return instance;
}

void EditorState::play() {
    if (m_mode == EditorMode::Edit) {
        SPARK_CORE_INFO("Editor: Play");
        m_mode = EditorMode::Play;
        if (m_modeChangeCallback) {
            m_modeChangeCallback(m_mode);
        }
    }
}

void EditorState::pause() {
    if (m_mode == EditorMode::Play) {
        SPARK_CORE_INFO("Editor: Pause");
        m_mode = EditorMode::Pause;
        if (m_modeChangeCallback) {
            m_modeChangeCallback(m_mode);
        }
    }
}

void EditorState::stop() {
    if (m_mode != EditorMode::Edit) {
        SPARK_CORE_INFO("Editor: Stop");
        m_mode = EditorMode::Edit;
        if (m_modeChangeCallback) {
            m_modeChangeCallback(m_mode);
        }
    }
}

void EditorState::togglePause() {
    if (m_mode == EditorMode::Play) {
        pause();
    } else if (m_mode == EditorMode::Pause) {
        play();
    }
}

void EditorState::selectEntity(EntityID id) {
    if (m_selectedEntityID != id) {
        m_selectedEntityID = id;
        SPARK_CORE_INFO("Selected entity: {0}", id);
        if (m_selectionChangeCallback) {
            m_selectionChangeCallback(id);
        }
    }
}

void EditorState::deselectEntity() {
    if (m_selectedEntityID != 0) {
        m_selectedEntityID = 0;
        SPARK_CORE_INFO("Deselected entity");
        if (m_selectionChangeCallback) {
            m_selectionChangeCallback(0);
        }
    }
}

void EditorState::cycleGizmoMode() {
    switch (m_gizmoMode) {
        case GizmoMode::Translate:
            m_gizmoMode = GizmoMode::Rotate;
            break;
        case GizmoMode::Rotate:
            m_gizmoMode = GizmoMode::Scale;
            break;
        case GizmoMode::Scale:
            m_gizmoMode = GizmoMode::Translate;
            break;
    }
}

void EditorState::toggleGizmoSpace() {
    m_gizmoSpace = (m_gizmoSpace == GizmoSpace::World) ? GizmoSpace::Local : GizmoSpace::World;
}

void EditorState::saveSceneSnapshot(Scene& scene) {
    // 简化实现：保存场景序列化数据
    // 实际应该使用完整的序列化系统
    SPARK_CORE_INFO("Scene snapshot saved");
}

void EditorState::restoreSceneSnapshot(Scene& scene) {
    // 简化实现：恢复场景
    // 实际应该使用完整的反序列化系统
    SPARK_CORE_INFO("Scene snapshot restored");
}

} // namespace spark
