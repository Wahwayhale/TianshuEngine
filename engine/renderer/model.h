#pragma once

#include "mesh.h"
#include "material.h"
#include <memory>
#include <vector>
#include <string>

namespace spark {

class Device;

class Model {
public:
    Model(Device& device, const std::string& filepath);
    ~Model();

    void draw(VkCommandBuffer commandBuffer);

    const std::vector<std::shared_ptr<Mesh>>& getMeshes() const { return m_meshes; }
    const std::vector<std::shared_ptr<Material>>& getMaterials() const { return m_materials; }

private:
    void loadModel(const std::string& filepath);

    Device& m_device;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::vector<std::shared_ptr<Material>> m_materials;
};

} // namespace spark
