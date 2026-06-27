# Android 平台 CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# 设置 Android 特定选项
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Android 特定定义
add_definitions(-DPLATFORM_ANDROID)
add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)

# 引擎源文件
set(ENGINE_SOURCES
    # Core
    ../../engine/core/log.cpp
    ../../engine/core/window.cpp
    ../../engine/core/input.cpp
    ../../engine/core/application.cpp
    ../../engine/core/memory_allocator.cpp

    # Renderer
    ../../engine/renderer/vulkan/instance.cpp
    ../../engine/renderer/vulkan/device.cpp
    ../../engine/renderer/vulkan/swapchain.cpp
    ../../engine/renderer/renderer.cpp
    ../../engine/renderer/shader.cpp
    ../../engine/renderer/pipeline.cpp
    ../../engine/renderer/buffer.cpp
    ../../engine/renderer/mesh.cpp
    ../../engine/renderer/camera.cpp
    ../../engine/renderer/material.cpp
    ../../engine/renderer/light.cpp
    ../../engine/renderer/texture.cpp

    # ECS
    ../../engine/ecs/entity.cpp

    # Scene
    ../../engine/scene/scene.cpp
    ../../engine/scene/transform.cpp

    # Platform (Android)
    ../../engine/platform/android/android_platform.cpp
)

# 创建共享库
add_library(TianshuEngine SHARED ${ENGINE_SOURCES})

# 包含目录
target_include_directories(TianshuEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../engine
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/imgui
)

# 链接库
target_link_libraries(TianshuEngine
    android
    log
    vulkan
    native_app_glue
)

# Android 特定设置
set_target_properties(TianshuEngine PROPERTIES
    OUTPUT_NAME "TianshuEngine"
    PREFIX "lib"
    SUFFIX ".so"
)

# 导出符号
set_target_properties(TianshuEngine PROPERTIES
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
)
