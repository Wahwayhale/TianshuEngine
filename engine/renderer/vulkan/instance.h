#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace spark::vulkan {

class Instance {
public:
    Instance(const std::string& appName, bool enableValidation = true);
    ~Instance();

    VkInstance getInstance() const { return m_instance; }

private:
    void createInstance(const std::string& appName);
    void setupDebugMessenger();
    void destroyDebugMessenger();

    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();

    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    bool m_enableValidation;

    const std::vector<const char*> m_validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
};

} // namespace spark::vulkan
