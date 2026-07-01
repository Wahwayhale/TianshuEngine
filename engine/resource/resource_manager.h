#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <functional>
#include <mutex>
#include "renderer/vulkan/fwd.h"

namespace spark {
class Texture;
class Mesh;

using ResourceID = uint64_t;

// 资源基类
class Resource {
public:
    Resource(const std::string& path) : m_path(path) {}
    virtual ~Resource() = default;

    const std::string& getPath() const { return m_path; }
    ResourceID getID() const { return m_id; }
    int getRefCount() const { return m_refCount; }

    void addRef() { m_refCount++; }
    void release() { m_refCount--; }

    bool isLoaded() const { return m_loaded; }

protected:
    std::string m_path;
    ResourceID m_id = 0;
    int m_refCount = 0;
    bool m_loaded = false;

    friend class ResourceManager;
};

// 纹理资源
class TextureResource : public Resource {
public:
    TextureResource(const std::string& path) : Resource(path) {}
    ~TextureResource() override;

    bool load(Device& device);
    Texture* getTexture() const { return m_texture.get(); }

private:
    std::unique_ptr<Texture> m_texture;
};

// 模型资源
class ModelResource : public Resource {
public:
    ModelResource(const std::string& path) : Resource(path) {}
    ~ModelResource() override;

    bool load(Device& device);
    const std::vector<std::shared_ptr<Mesh>>& getMeshes() const { return m_meshes; }

private:
    std::vector<std::shared_ptr<Mesh>> m_meshes;
};

// 资源管理器
class ResourceManager {
public:
    static ResourceManager& get();

    // 初始化（需要设备引用）
    void initialize(Device* device);
    void shutdown();

    // 加载资源（自动缓存）
    std::shared_ptr<TextureResource> loadTexture(const std::string& path);
    std::shared_ptr<ModelResource> loadModel(const std::string& path);

    // 手动卸载
    void unload(const std::string& path);
    void unloadAll();

    // 获取已加载的资源
    std::shared_ptr<TextureResource> getTexture(const std::string& path);
    std::shared_ptr<ModelResource> getModel(const std::string& path);

    // 统计
    size_t getResourceCount() const;
    size_t getTextureCount() const;
    size_t getModelCount() const;

private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    Device* m_device = nullptr;

    ResourceID m_nextID = 1;
    std::unordered_map<std::string, std::shared_ptr<TextureResource>> m_textures;
    std::unordered_map<std::string, std::shared_ptr<ModelResource>> m_models;

    mutable std::mutex m_mutex;
};

} // namespace spark
