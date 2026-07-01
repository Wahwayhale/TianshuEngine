# 第三方库配置 - 使用本地源码

set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# GLFW - Window and Input
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${THIRD_PARTY_DIR}/glfw glfw EXCLUDE_FROM_ALL)

# GLM - Math library
add_subdirectory(${THIRD_PARTY_DIR}/glm glm EXCLUDE_FROM_ALL)

# spdlog - Logging
add_subdirectory(${THIRD_PARTY_DIR}/spdlog spdlog EXCLUDE_FROM_ALL)

# Dear ImGui
add_library(imgui_lib STATIC
    ${THIRD_PARTY_DIR}/imgui/imgui.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp
    ${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_vulkan.cpp
)
target_include_directories(imgui_lib PUBLIC
    ${THIRD_PARTY_DIR}/imgui
    ${THIRD_PARTY_DIR}/imgui/backends
)
target_link_libraries(imgui_lib PUBLIC glfw Vulkan::Vulkan)

# stb - Image loading
add_library(stb_lib INTERFACE)
target_include_directories(stb_lib INTERFACE ${THIRD_PARTY_DIR}/stb)

# tinygltf - glTF model loading
add_library(tinygltf_lib INTERFACE)
target_include_directories(tinygltf_lib INTERFACE ${THIRD_PARTY_DIR}/tinygltf)
target_compile_definitions(tinygltf_lib INTERFACE TINYGLTF_IMPLEMENTATION STB_IMAGE_IMPLEMENTATION STB_IMAGE_WRITE_IMPLEMENTATION)
