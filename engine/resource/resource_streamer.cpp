#include "resource_streamer.h"
#include "core/log.h"
#include <algorithm>

namespace spark {

bool operator<(const ResourceRequest& a, const ResourceRequest& b) {
    return a.priority < b.priority;
}

ResourceStreamer::ResourceStreamer(uint32_t threadCount) {
    m_running = true;

    for (uint32_t i = 0; i < threadCount; i++) {
        m_threads.emplace_back(&ResourceStreamer::workerThread, this);
    }

    SPARK_CORE_INFO("Resource streamer started with {0} threads", threadCount);
}

ResourceStreamer::~ResourceStreamer() {
    m_running = false;
    m_condition.notify_all();

    for (auto& thread : m_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

std::future<std::shared_ptr<void>> ResourceStreamer::requestResource(const std::string& filepath, int priority) {
    std::lock_guard<std::mutex> lock(m_queueMutex);

    // Check if already loaded
    auto it = m_loadedResources.find(filepath);
    if (it != m_loadedResources.end()) {
        std::promise<std::shared_ptr<void>> promise;
        promise.set_value(it->second);
        return promise.get_future();
    }

    // Check if already loading
    if (m_resourceStates.find(filepath) != m_resourceStates.end() &&
        m_resourceStates[filepath] == ResourceState::Loading) {
        // Return existing promise
        return m_promises[filepath].get_future();
    }

    // Create new request
    ResourceRequest request;
    request.filepath = filepath;
    request.priority = priority;

    m_resourceStates[filepath] = ResourceState::Loading;
    m_promises[filepath] = std::promise<std::shared_ptr<void>>();

    m_requestQueue.push(request);
    m_condition.notify_one();

    return m_promises[filepath].get_future();
}

void ResourceStreamer::requestResource(const std::string& filepath,
                                        std::function<void(std::shared_ptr<void>)> callback,
                                        int priority) {
    std::lock_guard<std::mutex> lock(m_queueMutex);

    // Check if already loaded
    auto it = m_loadedResources.find(filepath);
    if (it != m_loadedResources.end()) {
        callback(it->second);
        return;
    }

    ResourceRequest request;
    request.filepath = filepath;
    request.callback = callback;
    request.priority = priority;

    m_resourceStates[filepath] = ResourceState::Loading;
    m_requestQueue.push(request);
    m_condition.notify_one();
}

void ResourceStreamer::cancelRequest(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    m_resourceStates.erase(filepath);
}

ResourceState ResourceStreamer::getResourceState(const std::string& filepath) const {
    auto it = m_resourceStates.find(filepath);
    if (it != m_resourceStates.end()) {
        return it->second;
    }
    return ResourceState::Unloaded;
}

bool ResourceStreamer::isResourceLoaded(const std::string& filepath) const {
    return m_loadedResources.find(filepath) != m_loadedResources.end();
}

std::shared_ptr<void> ResourceStreamer::getResource(const std::string& filepath) {
    // Check if loaded
    auto it = m_loadedResources.find(filepath);
    if (it != m_loadedResources.end()) {
        return it->second;
    }

    // Request and wait
    auto future = requestResource(filepath);
    return future.get();
}

void ResourceStreamer::update() {
    // Process completed loads
}

uint32_t ResourceStreamer::getPendingCount() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_queueMutex));
    return static_cast<uint32_t>(m_requestQueue.size());
}

uint32_t ResourceStreamer::getLoadingCount() const {
    uint32_t count = 0;
    for (const auto& pair : m_resourceStates) {
        if (pair.second == ResourceState::Loading) {
            count++;
        }
    }
    return count;
}

void ResourceStreamer::workerThread() {
    while (m_running) {
        ResourceRequest request;

        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_condition.wait(lock, [this]() {
                return !m_requestQueue.empty() || !m_running;
            });

            if (!m_running) break;

            request = m_requestQueue.top();
            m_requestQueue.pop();
        }

        processRequest(request);
    }
}

void ResourceStreamer::processRequest(const ResourceRequest& request) {
    SPARK_CORE_TRACE("Loading resource: {0}", request.filepath);

    // Simulate resource loading
    // In a real implementation, this would load textures, models, etc.
    auto resource = std::make_shared<int>(42);  // Placeholder

    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_loadedResources[request.filepath] = resource;
        m_resourceStates[request.filepath] = ResourceState::Loaded;

        // Notify promise if exists
        auto promiseIt = m_promises.find(request.filepath);
        if (promiseIt != m_promises.end()) {
            promiseIt->second.set_value(resource);
            m_promises.erase(promiseIt);
        }
    }

    // Call callback if provided
    if (request.callback) {
        request.callback(resource);
    }

    SPARK_CORE_TRACE("Resource loaded: {0}", request.filepath);
}

} // namespace spark
