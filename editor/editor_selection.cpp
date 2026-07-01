#include "editor_selection.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

EditorSelection& EditorSelection::get() {
    static EditorSelection instance;
    return instance;
}

void EditorSelection::selectEntity(uint64_t entityId) {
    m_selectedIds.clear();
    m_selectedIds.push_back(entityId);

    if (m_callback) {
        m_callback(m_selectedIds);
    }

    SPARK_CORE_INFO("Selected entity: {0}", entityId);
}

void EditorSelection::deselectEntity() {
    m_selectedIds.clear();

    if (m_callback) {
        m_callback(m_selectedIds);
    }
}

void EditorSelection::deselectAll() {
    m_selectedIds.clear();

    if (m_callback) {
        m_callback(m_selectedIds);
    }
}

void EditorSelection::addToSelection(uint64_t entityId) {
    if (!isSelected(entityId)) {
        m_selectedIds.push_back(entityId);

        if (m_callback) {
            m_callback(m_selectedIds);
        }
    }
}

void EditorSelection::removeFromSelection(uint64_t entityId) {
    auto it = std::find(m_selectedIds.begin(), m_selectedIds.end(), entityId);
    if (it != m_selectedIds.end()) {
        m_selectedIds.erase(it);

        if (m_callback) {
            m_callback(m_selectedIds);
        }
    }
}

void EditorSelection::toggleSelection(uint64_t entityId) {
    if (isSelected(entityId)) {
        removeFromSelection(entityId);
    } else {
        addToSelection(entityId);
    }
}

bool EditorSelection::hasSelection() const {
    return !m_selectedIds.empty();
}

bool EditorSelection::isSelected(uint64_t entityId) const {
    return std::find(m_selectedIds.begin(), m_selectedIds.end(), entityId) != m_selectedIds.end();
}

uint64_t EditorSelection::getPrimarySelection() const {
    if (!m_selectedIds.empty()) {
        return m_selectedIds[0];
    }
    return 0;
}

} // namespace spark
