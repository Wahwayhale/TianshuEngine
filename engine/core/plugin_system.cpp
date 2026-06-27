#include "plugin_system.h"
#include "log.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace spark {

PluginManager& PluginManager::get() {
    static PluginManager instance;
    return instance;
}

bool PluginManager::loadPlugin(const std::string& filepath) {
#ifdef PLATFORM_WINDOWS
    HMODULE handle = LoadLibraryA(filepath.c_str());
    if (!handle) {
        SPARK_CORE_ERROR("Failed to load plugin: {0}", filepath);
        return false;
    }

    // Get plugin factory function
    using CreatePluginFunc = std::shared_ptr<Plugin>(*)();
    auto createFunc = (CreatePluginFunc)GetProcAddress(handle, "createPlugin");

    if (!createFunc) {
        FreeLibrary(handle);
        SPARK_CORE_ERROR("Plugin missing createPlugin function: {0}", filepath);
        return false;
    }

    auto plugin = createFunc();
    if (plugin) {
        return registerPlugin(plugin);
    }
#else
    void* handle = dlopen(filepath.c_str(), RTLD_LAZY);
    if (!handle) {
        SPARK_CORE_ERROR("Failed to load plugin: {0} - {1}", filepath, dlerror());
        return false;
    }

    using CreatePluginFunc = std::shared_ptr<Plugin>(*)();
    auto createFunc = (CreatePluginFunc)dlsym(handle, "createPlugin");

    if (!createFunc) {
        dlclose(handle);
        SPARK_CORE_ERROR("Plugin missing createPlugin function: {0}", filepath);
        return false;
    }

    auto plugin = createFunc();
    if (plugin) {
        return registerPlugin(plugin);
    }
#endif

    return false;
}

bool PluginManager::registerPlugin(std::shared_ptr<Plugin> plugin) {
    if (!plugin) return false;

    const std::string& name = plugin->getName();

    // Check if already registered
    if (m_pluginMap.find(name) != m_pluginMap.end()) {
        SPARK_CORE_WARN("Plugin already registered: {0}", name);
        return false;
    }

    m_plugins.push_back(plugin);
    m_pluginMap[name] = m_plugins.size() - 1;

    SPARK_CORE_INFO("Plugin registered: {0} v{1}", name, plugin->getVersion());
    return true;
}

void PluginManager::unloadPlugin(const std::string& name) {
    auto it = m_pluginMap.find(name);
    if (it == m_pluginMap.end()) return;

    size_t index = it->second;
    auto& plugin = m_plugins[index];

    plugin->shutdown();

    // Remove from vector
    m_plugins.erase(m_plugins.begin() + index);

    // Rebuild map
    m_pluginMap.clear();
    for (size_t i = 0; i < m_plugins.size(); i++) {
        m_pluginMap[m_plugins[i]->getName()] = i;
    }

    SPARK_CORE_INFO("Plugin unloaded: {0}", name);
}

std::shared_ptr<Plugin> PluginManager::getPlugin(const std::string& name) {
    auto it = m_pluginMap.find(name);
    if (it == m_pluginMap.end()) return nullptr;
    return m_plugins[it->second];
}

bool PluginManager::initializeAll(Application& app) {
    bool success = true;

    for (auto& plugin : m_plugins) {
        if (!plugin->initialize(app)) {
            SPARK_CORE_ERROR("Failed to initialize plugin: {0}", plugin->getName());
            success = false;
        }
    }

    return success;
}

void PluginManager::shutdownAll() {
    for (auto& plugin : m_plugins) {
        plugin->shutdown();
    }
    m_plugins.clear();
    m_pluginMap.clear();
}

void PluginManager::updateAll(float deltaTime) {
    for (auto& plugin : m_plugins) {
        plugin->update(deltaTime);
    }
}

} // namespace spark
