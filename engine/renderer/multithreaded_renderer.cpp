#include "multithreaded_renderer.h"
#include "renderer/vulkan/device.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

CommandBufferPool::CommandBufferPool(Device& device, uint32_t queueFamilyIndex, uint32_t poolCount)
    : m_device(device) {

    m_pools.resize(poolCount);
    m_commandBuffers.resize(poolCount);

    for (uint32_t i = 0; i < poolCount; i++) {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndex;

        if (vkCreateCommandPool(device.getDevice(), &poolInfo, nullptr, &m_pools[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_pools[i];
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, &m_commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffer!");
        }
    }
}

CommandBufferPool::~CommandBufferPool() {
    for (auto pool : m_pools) {
        vkDestroyCommandPool(m_device.getDevice(), pool, nullptr);
    }
}

VkCommandBuffer CommandBufferPool::allocateCommandBuffer() {
    VkCommandBuffer cmd = m_commandBuffers[m_currentPool];
    m_currentPool = (m_currentPool + 1) % m_pools.size();
    return cmd;
}

void CommandBufferPool::resetPool() {
    for (auto pool : m_pools) {
        vkResetCommandPool(m_device.getDevice(), pool, 0);
    }
    m_currentPool = 0;
}

MultithreadedRenderer::MultithreadedRenderer(Device& device, uint32_t threadCount)
    : m_device(device), m_threadCount(threadCount) {

    if (m_threadCount == 0) {
        m_threadCount = std::max(1u, std::thread::hardware_concurrency() - 1);
    }

    SPARK_CORE_INFO("Multithreaded renderer: {0} threads", m_threadCount);
}

MultithreadedRenderer::~MultithreadedRenderer() {
    m_running = false;
    m_condition.notify_all();

    for (auto& thread : m_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void MultithreadedRenderer::initialize() {
    m_running = true;

    // Create command buffer pools for each thread
    uint32_t queueFamilyIndex = m_device.getQueueFamilyIndices().graphicsFamily.value();

    for (uint32_t i = 0; i < m_threadCount; i++) {
        m_commandBufferPools.push_back(
            std::make_unique<CommandBufferPool>(m_device, queueFamilyIndex, 2)
        );
    }

    // Start worker threads
    for (uint32_t i = 0; i < m_threadCount; i++) {
        m_threads.emplace_back(&MultithreadedRenderer::workerThread, this, i);
    }
}

void MultithreadedRenderer::submitCommand(std::function<void(VkCommandBuffer)> command) {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    m_commandQueue.push({command});
}

void MultithreadedRenderer::executeCommands(VkCommandBuffer mainCommandBuffer) {
    // Wait for all workers to complete
    waitForCompletion();

    // Execute all recorded command buffers on the main thread
    // This would use secondary command buffers in a real implementation
}

void MultithreadedRenderer::waitForCompletion() {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_completionCondition.wait(lock, [this]() {
        return m_commandQueue.empty() && m_activeWorkers == 0;
    });
}

void MultithreadedRenderer::workerThread(uint32_t threadIndex) {
    while (m_running) {
        RenderCommand command;

        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_condition.wait(lock, [this]() {
                return !m_commandQueue.empty() || !m_running;
            });

            if (!m_running) break;

            command = m_commandQueue.front();
            m_commandQueue.pop();
            m_activeWorkers++;
        }

        // Execute command
        VkCommandBuffer cmd = m_commandBufferPools[threadIndex]->allocateCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(cmd, &beginInfo);
        command.execute(cmd);
        vkEndCommandBuffer(cmd);

        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_activeWorkers--;
            if (m_commandQueue.empty() && m_activeWorkers == 0) {
                m_completionCondition.notify_all();
            }
        }
    }
}

ThreadedCommandRecorder::ThreadedCommandRecorder(Device& device)
    : m_device(device) {

    uint32_t threadCount = std::max(1u, std::thread::hardware_concurrency());
    uint32_t queueFamilyIndex = device.getQueueFamilyIndices().graphicsFamily.value();

    m_commandPools.resize(threadCount);
    m_commandBuffers.resize(threadCount);
    m_recording.resize(threadCount, false);

    for (uint32_t i = 0; i < threadCount; i++) {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndex;

        if (vkCreateCommandPool(device.getDevice(), &poolInfo, nullptr, &m_commandPools[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_commandPools[i];
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, &m_commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffer!");
        }
    }
}

ThreadedCommandRecorder::~ThreadedCommandRecorder() {
    for (auto pool : m_commandPools) {
        vkDestroyCommandPool(m_device.getDevice(), pool, nullptr);
    }
}

VkCommandBuffer ThreadedCommandRecorder::beginRecording(uint32_t threadIndex) {
    if (threadIndex >= m_commandBuffers.size()) return VK_NULL_HANDLE;

    VkCommandBuffer cmd = m_commandBuffers[threadIndex];

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT |
                      VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

    vkBeginCommandBuffer(cmd, &beginInfo);
    m_recording[threadIndex] = true;

    return cmd;
}

void ThreadedCommandRecorder::endRecording(uint32_t threadIndex) {
    if (threadIndex >= m_commandBuffers.size()) return;

    vkEndCommandBuffer(m_commandBuffers[threadIndex]);
    m_recording[threadIndex] = false;
}

void ThreadedCommandRecorder::executeAll(VkCommandBuffer mainCommandBuffer) {
    for (uint32_t i = 0; i < m_commandBuffers.size(); i++) {
        if (!m_recording[i]) {
            vkCmdExecuteCommands(mainCommandBuffer, 1, &m_commandBuffers[i]);
        }
    }
}

void ThreadedCommandRecorder::reset() {
    for (auto pool : m_commandPools) {
        vkResetCommandPool(m_device.getDevice(), pool, 0);
    }
    std::fill(m_recording.begin(), m_recording.end(), false);
}

} // namespace spark
