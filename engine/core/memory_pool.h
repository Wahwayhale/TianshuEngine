#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include <cstring>

namespace spark {

template<typename T>
class MemoryPool {
public:
    MemoryPool(size_t initialSize = 1024);
    ~MemoryPool();

    // Allocate an object
    T* allocate();

    // Deallocate an object
    void deallocate(T* ptr);

    // Get number of allocated objects
    size_t getAllocatedCount() const { return m_allocatedCount; }

    // Get total capacity
    size_t getCapacity() const { return m_capacity; }

    // Clear all allocations
    void clear();

private:
    struct Block {
        T data;
        bool used;
    };

    std::vector<Block> m_blocks;
    size_t m_capacity;
    size_t m_allocatedCount;
    size_t m_firstFree;
};

template<typename T>
MemoryPool<T>::MemoryPool(size_t initialSize)
    : m_capacity(initialSize), m_allocatedCount(0), m_firstFree(0) {

    m_blocks.resize(m_capacity);
    for (size_t i = 0; i < m_capacity; i++) {
        m_blocks[i].used = false;
    }
}

template<typename T>
MemoryPool<T>::~MemoryPool() {
    clear();
}

template<typename T>
T* MemoryPool<T>::allocate() {
    // Find free block
    for (size_t i = m_firstFree; i < m_capacity; i++) {
        if (!m_blocks[i].used) {
            m_blocks[i].used = true;
            m_allocatedCount++;
            m_firstFree = i + 1;
            return &m_blocks[i].data;
        }
    }

    // No free block found, expand pool
    size_t oldCapacity = m_capacity;
    m_capacity *= 2;
    m_blocks.resize(m_capacity);

    for (size_t i = oldCapacity; i < m_capacity; i++) {
        m_blocks[i].used = false;
    }

    // Allocate from new space
    m_blocks[oldCapacity].used = true;
    m_allocatedCount++;
    m_firstFree = oldCapacity + 1;

    return &m_blocks[oldCapacity].data;
}

template<typename T>
void MemoryPool<T>::deallocate(T* ptr) {
    if (!ptr) return;

    // Find block
    for (size_t i = 0; i < m_capacity; i++) {
        if (&m_blocks[i].data == ptr) {
            m_blocks[i].used = false;
            m_allocatedCount--;
            if (i < m_firstFree) {
                m_firstFree = i;
            }
            return;
        }
    }
}

template<typename T>
void MemoryPool<T>::clear() {
    for (size_t i = 0; i < m_capacity; i++) {
        m_blocks[i].used = false;
    }
    m_allocatedCount = 0;
    m_firstFree = 0;
}

// Object pool for frequently created/destroyed objects
template<typename T>
class ObjectPool {
public:
    ObjectPool(size_t poolSize = 100);
    ~ObjectPool();

    // Get an object from the pool
    T* acquire();

    // Return an object to the pool
    void release(T* obj);

    // Get available count
    size_t getAvailableCount() const { return m_available.size(); }

    // Get total count
    size_t getTotalCount() const { return m_totalCount; }

private:
    std::vector<T*> m_available;
    std::vector<T> m_storage;
    size_t m_totalCount;
};

template<typename T>
ObjectPool<T>::ObjectPool(size_t poolSize) : m_totalCount(poolSize) {
    m_storage.resize(poolSize);
    m_available.reserve(poolSize);

    for (size_t i = 0; i < poolSize; i++) {
        m_available.push_back(&m_storage[i]);
    }
}

template<typename T>
ObjectPool<T>::~ObjectPool() {
    m_available.clear();
    m_storage.clear();
}

template<typename T>
T* ObjectPool<T>::acquire() {
    if (m_available.empty()) {
        // Pool exhausted, expand
        size_t oldSize = m_storage.size();
        size_t newSize = oldSize * 2;
        m_storage.resize(newSize);
        m_available.reserve(newSize - oldSize);

        for (size_t i = oldSize; i < newSize; i++) {
            m_available.push_back(&m_storage[i]);
        }
        m_totalCount = newSize;
    }

    T* obj = m_available.back();
    m_available.pop_back();
    return obj;
}

template<typename T>
void ObjectPool<T>::release(T* obj) {
    if (obj) {
        m_available.push_back(obj);
    }
}

} // namespace spark
