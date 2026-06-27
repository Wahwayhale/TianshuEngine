#pragma once

#include <cstddef>
#include <vector>
#include <memory>
#include <mutex>

namespace spark {

// 线性分配器（帧内存）
class LinearAllocator {
public:
    LinearAllocator(size_t size);
    ~LinearAllocator();

    // 分配内存
    void* allocate(size_t size);

    // 重置分配器（每帧调用）
    void reset();

    // 获取使用量
    size_t getUsed() const { return m_used; }
    size_t getCapacity() const { return m_capacity; }
    float getUsagePercent() const { return (float)m_used / m_capacity * 100.0f; }

private:
    void* m_memory;
    size_t m_capacity;
    size_t m_used;
};

// 对象池
template<typename T>
class ObjectPool {
public:
    ObjectPool(size_t initialSize = 64) {
        m_pool.reserve(initialSize);
        for (size_t i = 0; i < initialSize; i++) {
            m_pool.push_back(std::make_unique<T>());
        }
    }

    ~ObjectPool() = default;

    // 获取对象
    T* acquire() {
        if (m_available.empty()) {
            // 扩展池
            m_pool.push_back(std::make_unique<T>());
            return m_pool.back().get();
        }

        T* obj = m_available.back();
        m_available.pop_back();
        return obj;
    }

    // 释放对象
    void release(T* obj) {
        if (obj) {
            m_available.push_back(obj);
        }
    }

    // 获取统计
    size_t getTotal() const { return m_pool.size(); }
    size_t getAvailable() const { return m_available.size(); }
    size_t getUsed() const { return m_pool.size() - m_available.size(); }

private:
    std::vector<std::unique_ptr<T>> m_pool;
    std::vector<T*> m_available;
};

// 分帧分配器
class FrameAllocator {
public:
    FrameAllocator(size_t frameSize, int frameCount = 3);
    ~FrameAllocator();

    // 分配内存
    void* allocate(size_t size, int frame);

    // 释放帧内存
    void freeFrame(int frame);

    // 获取当前帧
    int getCurrentFrame() const { return m_currentFrame; }
    void nextFrame() { m_currentFrame = (m_currentFrame + 1) % m_frameCount; }

private:
    struct FrameMemory {
        void* memory;
        size_t used;
        size_t capacity;
    };

    std::vector<FrameMemory> m_frames;
    int m_frameCount;
    int m_currentFrame = 0;
};

// 内存统计
struct MemoryStats {
    size_t totalAllocated;
    size_t totalUsed;
    size_t allocationCount;
    size_t deallocationCount;
};

// 内存管理器
class MemoryManager {
public:
    static MemoryManager& get();

    // 初始化
    bool initialize();
    void shutdown();

    // 帧分配器
    LinearAllocator& getFrameAllocator() { return *m_frameAllocator; }

    // 对象池
    template<typename T>
    ObjectPool<T>& getPool() {
        static ObjectPool<T> pool;
        return pool;
    }

    // 统计
    const MemoryStats& getStats() const { return m_stats; }
    void resetStats();

    // 调试
    void logMemoryUsage() const;

private:
    MemoryManager() = default;

    std::unique_ptr<LinearAllocator> m_frameAllocator;
    MemoryStats m_stats;
    bool m_initialized = false;
};

} // namespace spark
