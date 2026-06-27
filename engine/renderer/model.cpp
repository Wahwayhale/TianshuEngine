#include "model.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <tiny_gltf.h>
#include <stdexcept>

namespace spark {

Model::Model(Device& device, const std::string& filepath)
    : m_device(device) {
    loadModel(filepath);
}

Model::~Model() = default;

void Model::draw(VkCommandBuffer commandBuffer) {
    for (auto& mesh : m_meshes) {
        mesh->draw(commandBuffer);
    }
}

void Model::loadModel(const std::string& filepath) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool result;
    if (filepath.ends_with(".glb")) {
        result = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);
    } else {
        result = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);
    }

    if (!result) {
        throw std::runtime_error("Failed to load model: " + filepath + "\n" + err);
    }

    if (!warn.empty()) {
        SPARK_CORE_WARN("Model loading warning: {0}", warn);
    }

    // Process meshes
    for (const auto& mesh : model.meshes) {
        for (const auto& primitive : mesh.primitives) {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;

            // Get position accessor
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

            // Build vertices
            size_t vertexCount = 0;
            if (posIt != primitive.attributes.end()) {
                vertexCount = model.accessors[posIt->second].count;
            }

            for (size_t i = 0; i < vertexCount; i++) {
                Vertex vertex{};

                if (positionBuffer) {
                    vertex.position[0] = positionBuffer[i * 3 + 0];
                    vertex.position[1] = positionBuffer[i * 3 + 1];
                    vertex.position[2] = positionBuffer[i * 3 + 2];
                }

                if (normalBuffer) {
                    vertex.color[0] = normalBuffer[i * 3 + 0] * 0.5f + 0.5f;
                    vertex.color[1] = normalBuffer[i * 3 + 1] * 0.5f + 0.5f;
                    vertex.color[2] = normalBuffer[i * 3 + 2] * 0.5f + 0.5f;
                } else {
                    vertex.color[0] = 1.0f;
                    vertex.color[1] = 1.0f;
                    vertex.color[2] = 1.0f;
                }

                if (texCoordBuffer) {
                    vertex.texCoord[0] = texCoordBuffer[i * 2 + 0];
                    vertex.texCoord[1] = texCoordBuffer[i * 2 + 1];
                }

                vertices.push_back(vertex);
            }

            // Get indices
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

            // 创建 Mesh（带索引缓冲）
            std::shared_ptr<Mesh> meshPtr;
            if (!indices.empty()) {
                meshPtr = std::make_shared<Mesh>(m_device, vertices, indices);
            } else {
                meshPtr = std::make_shared<Mesh>(m_device, vertices);
            }
            m_meshes.push_back(meshPtr);

            // Create material
            auto material = std::make_shared<Material>();
            if (primitive.material >= 0) {
                const auto& mat = model.materials[primitive.material];
                material->albedo = Vec3(
                    mat.pbrMetallicRoughness.baseColorFactor[0],
                    mat.pbrMetallicRoughness.baseColorFactor[1],
                    mat.pbrMetallicRoughness.baseColorFactor[2]
                );
                material->metallic = static_cast<float>(mat.pbrMetallicRoughness.metallicFactor);
                material->roughness = static_cast<float>(mat.pbrMetallicRoughness.roughnessFactor);
            }
            m_materials.push_back(material);
        }
    }

    SPARK_CORE_INFO("Loaded model: {0} ({1} meshes)", filepath, m_meshes.size());
}

} // namespace spark
