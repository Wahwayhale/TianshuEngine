#include "vulkan/fwd.h"
#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>


namespace spark {


class Shader {
public:
    Shader(Device& device, const std::string& filepath, VkShaderStageFlagBits stage);
    ~Shader();

    VkShaderModule getModule() const { return m_module; }
    VkShaderStageFlagBits getStage() const { return m_stage; }

private:
    static std::vector<char> readFile(const std::string& filepath);
    void createShaderModule(const std::vector<char>& code);

    Device& m_device;
    VkShaderModule m_module = VK_NULL_HANDLE;
    VkShaderStageFlagBits m_stage;
};

} // namespace spark
