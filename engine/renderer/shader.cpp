#include "shader.h"
#include "vulkan/device.h"
#include "core/log.h"
#include <fstream>
#include <stdexcept>

namespace spark {

Shader::Shader(Device& device, const std::string& filepath, VkShaderStageFlagBits stage)
    : m_device(device), m_stage(stage) {

    auto code = readFile(filepath);
    createShaderModule(code);
    SPARK_CORE_INFO("Loaded shader: {0}", filepath);
}

Shader::~Shader() {
    if (m_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(m_device.getDevice(), m_module, nullptr);
    }
}

std::vector<char> Shader::readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + filepath);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

void Shader::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(m_device.getDevice(), &createInfo, nullptr, &m_module) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module!");
    }
}

} // namespace spark
