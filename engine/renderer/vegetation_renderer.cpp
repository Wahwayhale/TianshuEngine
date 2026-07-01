#include "vegetation_renderer.h"
#include "mesh.h"
#include "texture.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <random>

namespace spark {

VegetationRenderer::VegetationRenderer(Device& device, VkRenderPass renderPass)
    : m_device(device) {

    createPipeline(renderPass);
    SPARK_CORE_INFO("Vegetation renderer initialized.");
}

VegetationRenderer::~VegetationRenderer() {
    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device.getDevice(), m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
    }
}

void VegetationRenderer::addVegetationType(const VegetationType& type) {
    m_vegetationTypes.push_back(type);
    m_instances.resize(m_vegetationTypes.size());
    SPARK_CORE_INFO("Vegetation type added: {0}", type.name);
}

void VegetationRenderer::generateVegetation(const Vec3& center, float radius) {
    std::default_random_engine rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (size_t typeIdx = 0; typeIdx < m_vegetationTypes.size(); typeIdx++) {
        const auto& type = m_vegetationTypes[typeIdx];
        auto& instances = m_instances[typeIdx];
        instances.clear();

        // 计算实例数量
        float area = 3.14159f * radius * radius;
        int count = static_cast<int>(area * type.density);

        for (int i = 0; i < count; i++) {
            float x = dist(rng) * radius;
            float z = dist(rng) * radius;

            VegetationInstance instance;
            instance.position = center + Vec3(x, 0.0f, z);
            instance.rotation = Vec3(0.0f, dist(rng) * 3.14159f * 2.0f, 0.0f);

            float scale = type.minScale + std::abs(dist(rng)) * (type.maxScale - type.minScale);
            instance.scale = Vec3(scale);

            instance.windPhase = dist(rng) * 3.14159f * 2.0f;

            instances.push_back(instance);
        }

        SPARK_CORE_INFO("Generated {0} instances of {1}", instances.size(), type.name);
    }
}

void VegetationRenderer::render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix) {
    // TODO: 实例化渲染植被
}

void VegetationRenderer::clearVegetation() {
    for (auto& instances : m_instances) {
        instances.clear();
    }
}

void VegetationRenderer::createPipeline(VkRenderPass renderPass) {
    // TODO: 创建植被渲染管线
    SPARK_CORE_INFO("Vegetation pipeline created (simplified).");
}

} // namespace spark
