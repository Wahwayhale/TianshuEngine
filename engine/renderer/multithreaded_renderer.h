#include "vulkan/fwd.h"
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <memory>

namespace spark {



struct RenderCommand {
    std::function<void(VkCommandBuffer)> execute;
};

class CommandBufferPool {
public:
    CommandBufferPool(Device& device, uint32_t queueFamilyIndex, uint32_t poolCount = 2);
    ~CommandBufferPool();

    VkCommandBuffer allocateCommandBuffer();
    void resetPool();

private:
    Device& m_device;
    std::vector<VkCommandPool> m_pools;
    std::vector<VkCommandBuffer> m_commandBuffers;
    uint32_t m_currentPool = 0;
};

class MultithreadedRenderer {
public:
    MultithreadedRenderer(Device& device, uint32_t threadCount = 0);
    ~MultithreadedRenderer();

    // Initialize thread pool
    void initialize();

    // Submit render commands
    void submitCommand(std::function<void(VkCommandBuffer)> command);

    // Execute all submitted commands
    void executeCommands(VkCommandBuffer mainCommandBuffer);

    // Wait for all threads to complete
    void waitForCompletion();

    // Get thread count
    uint32_t getThreadCount() const { return m_threadCount; }

private:
    void workerThread(uint32_t threadIndex);

    Device& m_device;
    uint32_t m_threadCount;

    std::vector<std::thread> m_threads;
    std::vector<std::unique_ptr<CommandBufferPool>> m_commandBufferPools;

    std::queue<RenderCommand> m_commandQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
    std::condition_variable m_completionCondition;

    bool m_running = false;
    uint32_t m_activeWorkers = 0;
};

// Thread-safe command buffer recorder
class ThreadedCommandRecorder {
public:
    ThreadedCommandRecorder(Device& device);
    ~ThreadedCommandRecorder();

    // Begin recording on a thread
    VkCommandBuffer beginRecording(uint32_t threadIndex);

    // End recording on a thread
    void endRecording(uint32_t threadIndex);

    // Execute all recorded commands
    void executeAll(VkCommandBuffer mainCommandBuffer);

    // Reset for next frame
    void reset();

private:
    Device& m_device;
    std::vector<VkCommandPool> m_commandPools;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<bool> m_recording;
};

} // namespace spark
