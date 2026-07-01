#pragma once

#include <vector>
#include <cstdint>
#include <functional>

namespace spark {

class Entity;

// 编辑器选择系统
class EditorSelection {
public:
    static EditorSelection& get();

    // 选择操作
    void selectEntity(uint64_t entityId);
    void deselectEntity();
    void deselectAll();

    // 多选
    void addToSelection(uint64_t entityId);
    void removeFromSelection(uint64_t entityId);
    void toggleSelection(uint64_t entityId);

    // 查询
    bool hasSelection() const;
    bool isSelected(uint64_t entityId) const;
    uint64_t getPrimarySelection() const;
    const std::vector<uint64_t>& getSelection() const { return m_selectedIds; }
    size_t getSelectionCount() const { return m_selectedIds.size(); }

    // 回调
    using SelectionChangedCallback = std::function<void(const std::vector<uint64_t>&)>;
    void setSelectionChangedCallback(SelectionChangedCallback callback) { m_callback = callback; }

private:
    EditorSelection() = default;

    std::vector<uint64_t> m_selectedIds;
    SelectionChangedCallback m_callback;
};

} // namespace spark
