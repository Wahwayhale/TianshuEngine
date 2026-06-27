#pragma once

#include "math/math_types.h"
#include <memory>
#include <string>
#include <array>

namespace spark {

class Device;
class Buffer;
class Texture;
class Shader;
class Pipeline;

class Skybox {
public:
    Skybox(Device& device, VkRenderPass renderPass);
    ~Skybox();

    void draw(VkCommandBuffer commandBuffer, const Mat4& view, const Mat4& projection);

    void loadCubemap(const std::array<std::string, 6>& facePaths);
    void setColor(Vec3 topColor, Vec3 bottomColor, Vec3 horizonColor);

private:
    void createMesh();
    void createPipeline(VkRenderPass renderPass);

    Device& m_device;
    std::unique_ptr<Buffer> m_vertexBuffer;
    std::unique_ptr<Buffer> m_indexBuffer;
    std::shared_ptr<Texture> m_cubemapTexture;
    std::unique_ptr<Pipeline> m_pipeline;
    std::unique_ptr<Shader> m_vertShader;
    std::unique_ptr<Shader> m_fragShader;

    Vec3 m_topColor = Vec3(0.2f, 0.3f, 0.8f);
    Vec3 m_bottomColor = Vec3(0.1f, 0.1f, 0.2f);
    Vec3 m_horizonColor = Vec3(0.5f, 0.6f, 0.9f);
    bool m_useCubemap = false;
};

} // namespace spark
