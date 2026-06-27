#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace spark {

class Application;

// Plugin interface
class Plugin {
public:
    virtual ~Plugin() = default;

    // Plugin lifecycle
    virtual bool initialize(Application& app) = 0;
    virtual void shutdown() = 0;
    virtual void update(float deltaTime) = 0;

    // Plugin info
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
};

// Plugin manager
class PluginManager {
public:
    static PluginManager& get();

    // Load plugin from shared library
    bool loadPlugin(const std::string& filepath);

    // Register a plugin directly
    bool registerPlugin(std::shared_ptr<Plugin> plugin);

    // Unload plugin
    void unloadPlugin(const std::string& name);

    // Get plugin
    std::shared_ptr<Plugin> getPlugin(const std::string& name);

    // Initialize all plugins
    bool initializeAll(Application& app);

    // Shutdown all plugins
    void shutdownAll();

    // Update all plugins
    void updateAll(float deltaTime);

    // Get loaded plugins
    const std::vector<std::shared_ptr<Plugin>>& getPlugins() const { return m_plugins; }

private:
    PluginManager() = default;
    ~PluginManager() = default;

    std::vector<std::shared_ptr<Plugin>> m_plugins;
    std::unordered_map<std::string, size_t> m_pluginMap;
};

// Plugin registration macro
#define REGISTER_PLUGIN(PluginClass) \
    static bool PluginClass##_registered = []() { \
        return PluginManager::get().registerPlugin(std::make_shared<PluginClass>()); \
    }();

} // namespace spark
