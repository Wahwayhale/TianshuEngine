# WebGL 平台 CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# 设置 Web 特定选项
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Web 特定定义
add_definitions(-DPLATFORM_WEB)
add_definitions(-DVK_USE_PLATFORM_WAYLAND_KHR)

# Emscripten 设置
set(CMAKE_EXECUTABLE_SUFFIX ".js")

# 引擎源文件
set(ENGINE_SOURCES
    # Core
    ../../engine/core/log.cpp
    ../../engine/core/window.cpp
    ../../engine/core/input.cpp
    ../../engine/core/application.cpp
    ../../engine/core/memory_allocator.cpp

    # Renderer (WebGL 版本)
    ../../engine/renderer/renderer.cpp
    ../../engine/renderer/shader.cpp
    ../../engine/renderer/pipeline.cpp
    ../../engine/renderer/buffer.cpp
    ../../engine/renderer/mesh.cpp
    ../../engine/renderer/camera.cpp
    ../../engine/renderer/material.cpp
    ../../engine/renderer/light.cpp

    # ECS
    ../../engine/ecs/entity.cpp

    # Scene
    ../../engine/scene/scene.cpp
    ../../engine/scene/transform.cpp

    # Platform (Web)
    ../../engine/platform/web/web_platform.cpp
)

# 创建可执行文件
add_executable(TianshuEngine ${ENGINE_SOURCES})

# 包含目录
target_include_directories(TianshuEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../engine
)

# Emscripten 链接选项
target_link_options(TianshuEngine PRIVATE
    -sUSE_WEBGL2=1
    -sFULL_ES3=1
    -sWASM=1
    -sALLOW_MEMORY_GROWTH=1
    -sMAXIMUM_MEMORY=2147483648
    -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']
    -sEXPORTED_FUNCTIONS=['_main','_malloc','_free']
    --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/shell.html
)

# 链接库
target_link_libraries(TianshuEngine
    idbfs.js
)

# 设置输出属性
set_target_properties(TianshuEngine PROPERTIES
    OUTPUT_NAME "TianshuEngine"
    SUFFIX ".js"
)
