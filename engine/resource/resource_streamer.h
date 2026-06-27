#pragma once

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
#include <unordered_map>
#include <future>

namespace spark {

enum class ResourceState {
    Unloaded,
    Loading,
    Loaded,
    Failed
};

struct ResourceRequest {
    std::string filepath;
    std::function<void(std::shared_ptr<void>)> callback;
    int priority = 0;  // Higher = more important
};

class ResourceStreamer {
public:
    ResourceStreamer(uint32_t threadCount = 2);
    ~ResourceStreamer();

    // Request a resource to be loaded asynchronously
    std::future<std::shared_ptr<void>> requestResource(const std::string& filepath,
                                                         int priority = 0);

    // Request a resource with callback
    void requestResource(const std::string& filepath,
                         std::function<void(std::shared_ptr<void>)> callback,
                         int priority = 0);

    // Cancel a pending request
    void cancelRequest(const std::string& filepath);

    // Get resource state
    ResourceState getResourceState(const std::string& filepath) const;

    // Check if a resource is loaded
    bool isResourceLoaded(const std::string& filepath) const;

    // Get a loaded resource (blocks if still loading)
    std::shared_ptr<void> getResource(const std::string& filepath);

    // Update (process completed loads)
    void update();

    // Statistics
    uint32_t getPendingCount() const;
    uint32_t getLoadingCount() const;

private:
    void workerThread();
    void processRequest(const ResourceRequest& request);

    std::vector<std::thread> m_threads;
    std::priority_queue<ResourceRequest> m_requestQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;

    std::unordered_map<std::string, ResourceState> m_resourceStates;
    std::unordered_map<std::string, std::shared_ptr<void>> m_loadedResources;
    std::unordered_map<std::string, std::promise<std::shared_ptr<void>>> m_promises;

    bool m_running = false;
};

// Priority comparison for resource requests
bool operator<(const ResourceRequest& a, const ResourceRequest& b);

} // namespace spark
