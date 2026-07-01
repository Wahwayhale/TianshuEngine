#include "model_loader.h"
#include "mesh.h"
#include "material.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <tiny_gltf.h>
#include <algorithm>

namespace spark {

ModelLoader& ModelLoader::get() {
    static ModelLoader instance;
    return instance;
}

bool ModelLoader::initialize(Device* device) {
    m_device = device;
    SPARK_CORE_INFO("Model loader initialized.");
    return true;
}

void ModelLoader::shutdown() {
    m_device = nullptr;
    SPARK_CORE_INFO("Model loader shutdown.");
}

ModelLoadResult ModelLoader::loadModel(const std::string& filepath) {
    std::string extension = filepath.substr(filepath.find_last_of('.'));
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension == ".gltf" || extension == ".glb") {
        return loadGLTF(filepath);
    } else if (extension == ".obj") {
        return loadOBJ(filepath);
    } else if (extension == ".fbx") {
        return loadFBX(filepath);
    }

    ModelLoadResult result;
    result.error = "Unsupported format: " + extension;
    return result;
}

bool ModelLoader::isFormatSupported(const std::string& extension) const {
    std::string ext = extension;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".gltf" || ext == ".glb" || ext == ".obj" || ext == ".fbx";
}

std::vector<std::string> ModelLoader::getSupportedFormats() const {
    return {".gltf", ".glb", ".obj", ".fbx"};
}

ModelLoadResult ModelLoader::loadGLTF(const std::string& filepath) {
    ModelLoadResult result;

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool loaded;
    if (filepath.ends_with(".glb")) {
        loaded = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);
    } else {
        loaded = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);
    }

    if (!loaded) {
        result.error = "Failed to load glTF: " + err;
        return result;
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

                for (size_t i = 0; i < accessor.count; i++) {
                    switch (accessor.componentType) {
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                            const uint16_t* buf = reinterpret_cast<const uint16_t*>(
                                &buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                            indices.push_back(buf[i]);
                            break;
                        }
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
                            const uint32_t* buf = reinterpret_cast<const uint32_t*>(
                                &buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                            indices.push_back(buf[i]);
                            break;
                        }
                    }
                }
            }

            // 创建网格
            std::shared_ptr<Mesh> meshPtr;
            if (!indices.empty()) {
                meshPtr = std::make_shared<Mesh>(*m_device, vertices, indices);
            } else {
                meshPtr = std::make_shared<Mesh>(*m_device, vertices);
            }
            result.meshes.push_back(meshPtr);
        }
    }

    result.success = true;
    SPARK_CORE_INFO("glTF model loaded: {0} ({1} meshes)", filepath, result.meshes.size());
    return result;
}

ModelLoadResult ModelLoader::loadOBJ(const std::string& filepath) {
    ModelLoadResult result;
    result.error = "OBJ loader not implemented yet";
    return result;
}

ModelLoadResult ModelLoader::loadFBX(const std::string& filepath) {
    ModelLoadResult result;
    result.error = "FBX loader not implemented yet";
    return result;
}

} // namespace spark
