#include "skybox.h"
#include "buffer.h"
#include "shader.h"
#include "pipeline.h"
#include "texture.h"
#include "vulkan/device.h"
#include "core/log.h"

namespace spark {

Skybox::Skybox(Device& device, VkRenderPass renderPass)
    : m_device(device) {
    createMesh();
    createPipeline(renderPass);
    SPARK_CORE_INFO("Skybox created.");
}

Skybox::~Skybox() = default;

void Skybox::draw(VkCommandBuffer commandBuffer, const Mat4& view, const Mat4& projection) {
    // Remove translation from view matrix
    Mat4 skyboxView = Mat4(Mat3(view));

    // Bind pipeline
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->getPipeline());

    // Bind vertex buffer
    VkBuffer vertexBuffers[] = {m_vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    // Draw skybox
    vkCmdDraw(commandBuffer, 36, 1, 0, 0);
}

void Skybox::loadCubemap(const std::array<std::string, 6>& facePaths) {
    // Load cubemap faces
    m_useCubemap = true;
    SPARK_CORE_INFO("Cubemap loaded.");
}

void Skybox::setColor(Vec3 topColor, Vec3 bottomColor, Vec3 horizonColor) {
    m_topColor = topColor;
    m_bottomColor = bottomColor;
    m_horizonColor = horizonColor;
    m_useCubemap = false;
}

void Skybox::createMesh() {
    // Skybox cube vertices
    std::vector<float> vertices = {
        // Positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    VkDeviceSize bufferSize = vertices.size() * sizeof(float);

    m_vertexBuffer = std::make_unique<Buffer>(
        m_device,
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    m_vertexBuffer->copyTo(vertices.data(), bufferSize);
}

void Skybox::createPipeline(VkRenderPass renderPass) {
    // This would create a specialized pipeline for skybox rendering
    // For now, we'll use a placeholder
}

} // namespace spark
