#pragma once

#include <string>
#include <vector>
#include <memory>
#include "math/math_types.h"

namespace spark {

class Device;
class Mesh;
class Material;

// 模型加载结果
struct ModelLoadResult {
    bool success = false;
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<std::shared_ptr<Material>> materials;
    std::string error;
};

// 模型加载器
class ModelLoader {
public:
    static ModelLoader& get();

    // 初始化
    bool initialize(Device* device);
    void shutdown();

    // 加载模型
    ModelLoadResult loadModel(const std::string& filepath);

    // 支持的格式
    bool isFormatSupported(const std::string& extension) const;
    std::vector<std::string> getSupportedFormats() const;

private:
    ModelLoader() = default;

    ModelLoadResult loadGLTF(const std::string& filepath);
    ModelLoadResult loadOBJ(const std::string& filepath);
    ModelLoadResult loadFBX(const std::string& filepath);

    Device* m_device = nullptr;
};

} // namespace spark
