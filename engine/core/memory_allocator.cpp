#include "memory_allocator.h"
#include "log.h"
#include <cstdlib>
#include <cstring>

namespace spark {

// =============================================
// LinearAllocator
// =============================================

LinearAllocator::LinearAllocator(size_t size) : m_capacity(size), m_used(0) {
    m_memory = std::malloc(size);
    if (!m_memory) {
        SPARK_CORE_ERROR("Failed to allocate linear allocator memory: {0} bytes", size);
    }
}

LinearAllocator::~LinearAllocator() {
    if (m_memory) {
        std::free(m_memory);
    }
}

void* LinearAllocator::allocate(size_t size) {
    if (m_used + size > m_capacity) {
        SPARK_CORE_WARN("Linear allocator out of memory: {0} bytes requested, {1} used, {2} capacity",
                        size, m_used, m_capacity);
        return nullptr;
    }

    void* ptr = static_cast<char*>(m_memory) + m_used;
    m_used += size;
    return ptr;
}

void LinearAllocator::reset() {
    m_used = 0;
}

// =============================================
// FrameAllocator
// =============================================

FrameAllocator::FrameAllocator(size_t frameSize, int frameCount)
    : m_frameCount(frameCount) {

    m_frames.resize(frameCount);
    for (int i = 0; i < frameCount; i++) {
        m_frames[i].memory = std::malloc(frameSize);
        m_frames[i].used = 0;
        m_frames[i].capacity = frameSize;
    }

    SPARK_CORE_INFO("Frame allocator created: {0} frames, {1} bytes each", frameCount, frameSize);
}

FrameAllocator::~FrameAllocator() {
    for (auto& frame : m_frames) {
        if (frame.memory) {
            std::free(frame.memory);
        }
    }
}

void* FrameAllocator::allocate(size_t size, int frame) {
    if (frame < 0 || frame >= m_frameCount) return nullptr;

    auto& f = m_frames[frame];
    if (f.used + size > f.capacity) {
        SPARK_CORE_WARN("Frame allocator out of memory for frame {0}", frame);
        return nullptr;
    }

    void* ptr = static_cast<char*>(f.memory) + f.used;
    f.used += size;
    return ptr;
}

void FrameAllocator::freeFrame(int frame) {
    if (frame >= 0 && frame < m_frameCount) {
        m_frames[frame].used = 0;
    }
}

// =============================================
// MemoryManager
// =============================================

MemoryManager& MemoryManager::get() {
    static MemoryManager instance;
    return instance;
}

bool MemoryManager::initialize() {
    if (m_initialized) return true;

    // 创建帧分配器（每帧 16MB）
    m_frameAllocator = std::make_unique<LinearAllocator>(16 * 1024 * 1024);

    // 重置统计
    resetStats();

    m_initialized = true;
    SPARK_CORE_INFO("Memory manager initialized.");
    return true;
}

void MemoryManager::shutdown() {
    if (!m_initialized) return;

    m_frameAllocator.reset();

    m_initialized = false;
    SPARK_CORE_INFO("Memory manager shutdown.");
}

void MemoryManager::resetStats() {
    m_stats = {};
}

void MemoryManager::logMemoryUsage() const {
    SPARK_CORE_INFO("Memory Stats:");
    SPARK_CORE_INFO("  Total Allocated: {0} bytes", m_stats.totalAllocated);
    SPARK_CORE_INFO("  Total Used: {0} bytes", m_stats.totalUsed);
    SPARK_CORE_INFO("  Allocations: {0}", m_stats.allocationCount);
    SPARK_CORE_INFO("  Deallocations: {0}", m_stats.deallocationCount);

    if (m_frameAllocator) {
        SPARK_CORE_INFO("  Frame Allocator: {0}/{1} bytes ({2:.1f}%)",
                        m_frameAllocator->getUsed(),
                        m_frameAllocator->getCapacity(),
                        m_frameAllocator->getUsagePercent());
    }
}

} // namespace spark
