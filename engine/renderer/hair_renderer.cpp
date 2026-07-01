#include "hair_renderer.h"
#include "buffer.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <random>
#include <cmath>

namespace spark {

HairRenderer::HairRenderer(Device& device, VkRenderPass renderPass)
    : m_device(device) {

    createPipeline(renderPass);
    SPARK_CORE_INFO("Hair renderer initialized.");
}

HairRenderer::~HairRenderer() {
    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device.getDevice(), m_pipeline, nullptr);
    }
    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device.getDevice(), m_pipelineLayout, nullptr);
    }
}

void HairRenderer::generateHair(const Vec3& position, const HairSettings& settings) {
    m_settings = settings;
    m_strands.clear();

    std::default_random_engine rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (int i = 0; i < settings.strandCount; i++) {
        HairStrand strand;

        // 根部位置（圆形分布）
        float angle = dist(rng) * 3.14159f;
        float radius = std::abs(dist(rng)) * 0.5f;
        Vec3 root = position + Vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);

        // 生成发丝点
        float length = settings.length + dist(rng) * settings.lengthVariance;
        Vec3 direction = glm::normalize(Vec3(dist(rng) * 0.1f, 1.0f, dist(rng) * 0.1f));

        for (int j = 0; j < settings.segmentsPerStrand; j++) {
            float t = (float)j / (settings.segmentsPerStrand - 1);
            Vec3 point = root + direction * (length * t);

            // 添加一些随机弯曲
            point.x += sin(t * 3.14159f * 2.0f) * 0.05f * dist(rng);
            point.z += cos(t * 3.14159f * 2.0f) * 0.05f * dist(rng);

            strand.points.push_back(point);
        }

        m_strands.push_back(strand);
    }

    // 创建顶点缓冲
    std::vector<float> vertices;
    for (const auto& strand : m_strands) {
        for (const auto& point : strand.points) {
            vertices.push_back(point.x);
            vertices.push_back(point.y);
            vertices.push_back(point.z);
        }
    }

    m_vertexCount = static_cast<uint32_t>(vertices.size() / 3);

    VkDeviceSize bufferSize = vertices.size() * sizeof(float);
    m_vertexBuffer = std::make_unique<Buffer>(
        m_device, bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    m_vertexBuffer->copyTo(vertices.data(), bufferSize);

    SPARK_CORE_INFO("Hair generated: {0} strands", settings.strandCount);
}

void HairRenderer::render(VkCommandBuffer commandBuffer, const Mat4& viewMatrix, const Mat4& projMatrix) {
    if (m_strands.empty() || !m_vertexBuffer) return;

    // TODO: 渲染头发
    // 需要头发着色器
}

void HairRenderer::createPipeline(VkRenderPass renderPass) {
    // TODO: 创建头发渲染管线
    SPARK_CORE_INFO("Hair pipeline created (simplified).");
}

} // namespace spark
