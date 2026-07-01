#include "resource_manager.h"
#include "renderer/texture.h"
#include "renderer/mesh.h"
#include "renderer/model.h"
#include "renderer/vulkan/device.h"
#include "core/log.h"
#include <tiny_gltf.h>

namespace spark {

// =============================================
// TextureResource
// =============================================

TextureResource::~TextureResource() = default;

bool TextureResource::load(Device& device) {
    if (m_loaded) return true;

    try {
        m_texture = std::make_unique<Texture>(device, m_path);
        m_loaded = true;
        SPARK_CORE_INFO("Loaded texture: {0}", m_path);
        return true;
    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to load texture {0}: {1}", m_path, e.what());
        return false;
    }
}

// =============================================
// ModelResource
// =============================================

ModelResource::~ModelResource() = default;

bool ModelResource::load(Device& device) {
    if (m_loaded) return true;

    try {
        // 使用 tinygltf 加载模型
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        std::string err, warn;

        bool result;
        if (m_path.ends_with(".glb")) {
            result = loader.LoadBinaryFromFile(&model, &err, &warn, m_path);
        } else {
            result = loader.LoadASCIIFromFile(&model, &err, &warn, m_path);
        }

        if (!result) {
            SPARK_CORE_ERROR("Failed to load model {0}: {1}", m_path, err);
            return false;
        }

        if (!warn.empty()) {
            SPARK_CORE_WARN("Model loading warning: {0}", warn);
        }

        // 处理网格
        for (const auto& mesh : model.meshes) {
            for (const auto& primitive : mesh.primitives) {
                std::vector<PBRVertex> vertices;
                std::vector<uint32_t> indices;

                // 获取顶点数据
                const float* positionBuffer = nullptr;
                const float* normalBuffer = nullptr;
                const float* texCoordBuffer = nullptr;

                auto posIt = primitive.attributes.find("POSITION");
                if (posIt != primitive.attributes.end()) {
                    const auto& accessor = model.accessors[posIt->second];
                    const auto& bufferView = model.bufferViews[accessor.bufferView];
                    positionBuffer = reinterpret_cast<const float*>(
                        &model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]);
                }

                auto normIt = primitive.attributes.find("NORMAL");
                if (normIt != primitive.attributes.end()) {
                    const auto& accessor = model.accessors[normIt->second];
                    const auto& bufferView = model.bufferViews[accessor.bufferView];
                    normalBuffer = reinterpret_cast<const float*>(
                        &model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]);
                }

                auto texIt = primitive.attributes.find("TEXCOORD_0");
                if (texIt != primitive.attributes.end()) {
                    const auto& accessor = model.accessors[texIt->second];
                    const auto& bufferView = model.bufferViews[accessor.bufferView];
                    texCoordBuffer = reinterpret_cast<const float*>(
                        &model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]);
                }

                // 构建顶点
                size_t vertexCount = 0;
                if (posIt != primitive.attributes.end()) {
                    vertexCount = model.accessors[posIt->second].count;
                }

                for (size_t i = 0; i < vertexCount; i++) {
                    PBRVertex vertex{};

                    if (positionBuffer) {
                        vertex.position[0] = positionBuffer[i * 3 + 0];
                        vertex.position[1] = positionBuffer[i * 3 + 1];
                        vertex.position[2] = positionBuffer[i * 3 + 2];
                    }

                    if (normalBuffer) {
                        vertex.normal[0] = normalBuffer[i * 3 + 0];
                        vertex.normal[1] = normalBuffer[i * 3 + 1];
                        vertex.normal[2] = normalBuffer[i * 3 + 2];
                    } else {
                        vertex.normal[0] = 0.0f;
                        vertex.normal[1] = 1.0f;
                        vertex.normal[2] = 0.0f;
                    }

                    if (texCoordBuffer) {
                        vertex.texCoord[0] = texCoordBuffer[i * 2 + 0];
                        vertex.texCoord[1] = texCoordBuffer[i * 2 + 1];
                    }

                    vertices.push_back(vertex);
                }

                // 获取索引
                if (primitive.indices >= 0) {
                    const auto& accessor = model.accessors[primitive.indices];
                    const auto& bufferView = model.bufferViews[accessor.bufferView];
                    const auto& buffer = model.buffers[bufferView.buffer];

                    indices.reserve(accessor.count);

                    switch (accessor.componentType) {
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                            const uint16_t* buf = reinterpret_cast<const uint16_t*>(
                                &buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                            for (size_t i = 0; i < accessor.count; i++) {
                                indices.push_back(buf[i]);
                            }
                            break;
                        }
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
                            const uint32_t* buf = reinterpret_cast<const uint32_t*>(
                                &buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                            for (size_t i = 0; i < accessor.count; i++) {
                                indices.push_back(buf[i]);
                            }
                            break;
                        }
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                            const uint8_t* buf = reinterpret_cast<const uint8_t*>(
                                &buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                            for (size_t i = 0; i < accessor.count; i++) {
                                indices.push_back(buf[i]);
                            }
                            break;
                        }
                    }
                }

                // 创建 Mesh
                std::shared_ptr<Mesh> meshPtr;
                if (!indices.empty()) {
                    meshPtr = std::make_shared<Mesh>(device, vertices, indices);
                } else {
                    meshPtr = std::make_shared<Mesh>(device, vertices);
                }
                m_meshes.push_back(meshPtr);
            }
        }

        m_loaded = true;
        SPARK_CORE_INFO("Loaded model: {0} ({1} meshes)", m_path, m_meshes.size());
        return true;

    } catch (const std::exception& e) {
        SPARK_CORE_ERROR("Failed to load model {0}: {1}", m_path, e.what());
        return false;
    }
}

// =============================================
// ResourceManager
// =============================================

ResourceManager& ResourceManager::get() {
    static ResourceManager instance;
    return instance;
}

void ResourceManager::initialize(Device* device) {
    m_device = device;
    SPARK_CORE_INFO("ResourceManager initialized.");
}

void ResourceManager::shutdown() {
    unloadAll();
    m_device = nullptr;
    SPARK_CORE_INFO("ResourceManager shutdown.");
}

std::shared_ptr<TextureResource> ResourceManager::loadTexture(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // 检查缓存
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        it->second->addRef();
        return it->second;
    }

    // 创建新资源
    auto resource = std::make_shared<TextureResource>(path);
    resource->m_id = m_nextID++;
    resource->addRef();

    // 加载资源
    if (m_device && resource->load(*m_device)) {
        m_textures[path] = resource;
        return resource;
    }

    return nullptr;
}

std::shared_ptr<ModelResource> ResourceManager::loadModel(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // 检查缓存
    auto it = m_models.find(path);
    if (it != m_models.end()) {
        it->second->addRef();
        return it->second;
    }

    // 创建新资源
    auto resource = std::make_shared<ModelResource>(path);
    resource->m_id = m_nextID++;
    resource->addRef();

    // 加载资源
    if (m_device && resource->load(*m_device)) {
        m_models[path] = resource;
        return resource;
    }

    return nullptr;
}

void ResourceManager::unload(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_textures.erase(path);
    m_models.erase(path);
}

void ResourceManager::unloadAll() {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_textures.clear();
    m_models.clear();
}

std::shared_ptr<TextureResource> ResourceManager::getTexture(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<ModelResource> ResourceManager::getModel(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_models.find(path);
    if (it != m_models.end()) {
        return it->second;
    }
    return nullptr;
}

size_t ResourceManager::getResourceCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_textures.size() + m_models.size();
}

size_t ResourceManager::getTextureCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_textures.size();
}

size_t ResourceManager::getModelCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_models.size();
}

} // namespace spark
