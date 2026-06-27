#include "ssao.h"
#include "buffer.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <random>
#include <array>

namespace spark {

// 全屏四边形顶点
struct FullscreenVertex {
    float position[2];
    float texCoord[2];
};

static const FullscreenVertex quadVertices[] = {
    {{-1.0f, -1.0f}, {0.0f, 0.0f}},
    {{ 1.0f, -1.0f}, {1.0f, 0.0f}},
    {{ 1.0f,  1.0f}, {1.0f, 1.0f}},
    {{-1.0f, -1.0f}, {0.0f, 0.0f}},
    {{ 1.0f,  1.0f}, {1.0f, 1.0f}},
    {{-1.0f,  1.0f}, {0.0f, 1.0f}},
};

SSAO::SSAO(Device& device, uint32_t width, uint32_t height)
    : m_device(device), m_width(width), m_height(height) {

    generateSampleKernel();
    createResources();
    createPipelines();

    SPARK_CORE_INFO("SSAO initialized: {0}x{1}", width, height);
}

SSAO::~SSAO() {
    vkDeviceWaitIdle(m_device.getDevice());
    cleanupResources();
    SPARK_CORE_INFO("SSAO destroyed.");
}

void SSAO::recreate(uint32_t width, uint32_t height) {
    vkDeviceWaitIdle(m_device.getDevice());
    m_width = width;
    m_height = height;
    cleanupResources();
    createResources();
}

void SSAO::generateSampleKernel() {
    m_sampleKernel.resize(64);

    std::default_random_engine generator;
    std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);

    for (int i = 0; i < 64; i++) {
        Vec3 sample(
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);

        // 将采样点聚集在原点附近
        float scale = static_cast<float>(i) / 64.0f;
        scale = glm::mix(0.1f, 1.0f, scale * scale);
        sample *= scale;

        m_sampleKernel[i] = Vec4(sample, 0.0f);
    }
}

void SSAO::createResources() {
    // 创建 SSAO 纹理（R8 格式）
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_width;
    imageInfo.extent.height = m_height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8_UNORM;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    // SSAO 纹理
    vkCreateImage(m_device.getDevice(), &imageInfo, nullptr, &m_ssaoImage);
    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(m_device.getDevice(), m_ssaoImage, &memReq);
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_ssaoMemory);
    vkBindImageMemory(m_device.getDevice(), m_ssaoImage, m_ssaoMemory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_ssaoImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8_UNORM;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;
    vkCreateImageView(m_device.getDevice(), &viewInfo, nullptr, &m_ssaoImageView);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    vkCreateSampler(m_device.getDevice(), &samplerInfo, nullptr, &m_ssaoSampler);

    // 模糊后纹理
    vkCreateImage(m_device.getDevice(), &imageInfo, nullptr, &m_blurredImage);
    vkGetImageMemoryRequirements(m_device.getDevice(), m_blurredImage, &memReq);
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_blurredMemory);
    vkBindImageMemory(m_device.getDevice(), m_blurredImage, m_blurredMemory, 0);

    viewInfo.image = m_blurredImage;
    vkCreateImageView(m_device.getDevice(), &viewInfo, nullptr, &m_blurredImageView);
    vkCreateSampler(m_device.getDevice(), &samplerInfo, nullptr, &m_blurredSampler);

    // 噪声纹理（4x4）
    std::default_random_engine generator;
    std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
    std::vector<Vec4> noiseData(16);
    for (int i = 0; i < 16; i++) {
        noiseData[i] = Vec4(randomFloats(generator) * 2.0f - 1.0f,
                            randomFloats(generator) * 2.0f - 1.0f,
                            0.0f, 0.0f);
    }

    VkImageCreateInfo noiseImageInfo{};
    noiseImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    noiseImageInfo.imageType = VK_IMAGE_TYPE_2D;
    noiseImageInfo.extent.width = 4;
    noiseImageInfo.extent.height = 4;
    noiseImageInfo.extent.depth = 1;
    noiseImageInfo.mipLevels = 1;
    noiseImageInfo.arrayLayers = 1;
    noiseImageInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    noiseImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    noiseImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    noiseImageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    vkCreateImage(m_device.getDevice(), &noiseImageInfo, nullptr, &m_noiseImage);
    vkGetImageMemoryRequirements(m_device.getDevice(), m_noiseImage, &memReq);
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_noiseMemory);
    vkBindImageMemory(m_device.getDevice(), m_noiseImage, m_noiseMemory, 0);

    viewInfo.image = m_noiseImage;
    viewInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vkCreateImageView(m_device.getDevice(), &viewInfo, nullptr, &m_noiseImageView);

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    vkCreateSampler(m_device.getDevice(), &samplerInfo, nullptr, &m_noiseSampler);

    // 创建 UBO
    m_ssaoUBO = std::make_unique<Buffer>(m_device, sizeof(SSAOUBO),
                                          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_ssaoUBO->map();

    m_blurUBO = std::make_unique<Buffer>(m_device, sizeof(BlurUBO),
                                          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_blurUBO->map();

    m_kernelUBO = std::make_unique<Buffer>(m_device, sizeof(Vec4) * 64,
                                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_kernelUBO->map();
    m_kernelUBO->copyTo(m_sampleKernel.data(), sizeof(Vec4) * 64);

    // 创建全屏四边形顶点缓冲
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(quadVertices);
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vkCreateBuffer(m_device.getDevice(), &bufferInfo, nullptr, &m_quadVertexBuffer);
    vkGetBufferMemoryRequirements(m_device.getDevice(), m_quadVertexBuffer, &memReq);
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = m_device.findMemoryType(memReq.memoryTypeBits,
                                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vkAllocateMemory(m_device.getDevice(), &allocInfo, nullptr, &m_quadVertexMemory);
    vkBindBufferMemory(m_device.getDevice(), m_quadVertexBuffer, m_quadVertexMemory, 0);
    void* data;
    vkMapMemory(m_device.getDevice(), m_quadVertexMemory, 0, sizeof(quadVertices), 0, &data);
    memcpy(data, quadVertices, sizeof(quadVertices));
    vkUnmapMemory(m_device.getDevice(), m_quadVertexMemory);

    // 创建渲染通道
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = VK_FORMAT_R8_UNORM;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    vkCreateRenderPass(m_device.getDevice(), &renderPassInfo, nullptr, &m_renderPass);

    // 创建帧缓冲
    VkFramebufferCreateInfo fbInfo{};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.renderPass = m_renderPass;
    fbInfo.attachmentCount = 1;
    fbInfo.width = m_width;
    fbInfo.height = m_height;
    fbInfo.layers = 1;

    fbInfo.pAttachments = &m_ssaoImageView;
    vkCreateFramebuffer(m_device.getDevice(), &fbInfo, nullptr, &m_ssaoFramebuffer);

    fbInfo.pAttachments = &m_blurredImageView;
    vkCreateFramebuffer(m_device.getDevice(), &fbInfo, nullptr, &m_blurFramebuffer);

    SPARK_CORE_INFO("SSAO resources created.");
}

void SSAO::cleanupResources() {
    vkDestroySampler(m_device.getDevice(), m_ssaoSampler, nullptr);
    vkDestroyImageView(m_device.getDevice(), m_ssaoImageView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_ssaoImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_ssaoMemory, nullptr);

    vkDestroySampler(m_device.getDevice(), m_blurredSampler, nullptr);
    vkDestroyImageView(m_device.getDevice(), m_blurredImageView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_blurredImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_blurredMemory, nullptr);

    vkDestroySampler(m_device.getDevice(), m_noiseSampler, nullptr);
    vkDestroyImageView(m_device.getDevice(), m_noiseImageView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_noiseImage, nullptr);
    vkFreeMemory(m_device.getDevice(), m_noiseMemory, nullptr);

    vkDestroyBuffer(m_device.getDevice(), m_quadVertexBuffer, nullptr);
    vkFreeMemory(m_device.getDevice(), m_quadVertexMemory, nullptr);

    vkDestroyFramebuffer(m_device.getDevice(), m_ssaoFramebuffer, nullptr);
    vkDestroyFramebuffer(m_device.getDevice(), m_blurFramebuffer, nullptr);
    vkDestroyRenderPass(m_device.getDevice(), m_renderPass, nullptr);
}

void SSAO::createPipelines() {
    // 简化实现：创建 SSAO 和模糊管线
    // 实际需要加载着色器并创建完整的图形管线
    SPARK_CORE_INFO("SSAO pipelines created (simplified).");
}

void SSAO::updateSettings(const SSAOSettings& settings) {
    m_settings = settings;

    // 更新 SSAO UBO
    SSAOUBO ssaoUBO{};
    ssaoUBO.radius = settings.radius;
    ssaoUBO.bias = settings.bias;
    ssaoUBO.power = settings.power;
    ssaoUBO.kernelSize = settings.kernelSize;
    ssaoUBO.screenSize = Vec2(static_cast<float>(m_width), static_cast<float>(m_height));
    ssaoUBO.noiseScale = static_cast<float>(m_width) / 4.0f;
    m_ssaoUBO->copyTo(&ssaoUBO, sizeof(SSAOUBO));

    // 更新模糊 UBO
    BlurUBO blurUBO{};
    blurUBO.screenSize = Vec2(static_cast<float>(m_width), static_cast<float>(m_height));
    blurUBO.blurSize = settings.blurSize;
    m_blurUBO->copyTo(&blurUBO, sizeof(BlurUBO));
}

void SSAO::render(VkCommandBuffer commandBuffer, VkImageView depthImageView, const Mat4& projection, const Mat4& view) {
    if (!m_settings.enabled) return;

    // 更新 UBO
    SSAOUBO ssaoUBO{};
    ssaoUBO.projection = projection;
    ssaoUBO.view = view;
    ssaoUBO.radius = m_settings.radius;
    ssaoUBO.bias = m_settings.bias;
    ssaoUBO.power = m_settings.power;
    ssaoUBO.kernelSize = m_settings.kernelSize;
    ssaoUBO.screenSize = Vec2(static_cast<float>(m_width), static_cast<float>(m_height));
    ssaoUBO.noiseScale = static_cast<float>(m_width) / 4.0f;
    m_ssaoUBO->copyTo(&ssaoUBO, sizeof(SSAOUBO));

    // 实际渲染需要完整的管线和描述符集
    // 这里是简化实现
}

} // namespace spark
