set(ENGINE_SOURCES
    # Core
    core/log.cpp
    core/window.cpp
    core/input.cpp
    core/application.cpp

    # Renderer
    renderer/vulkan/instance.cpp
    renderer/vulkan/device.cpp
    renderer/vulkan/swapchain.cpp
    renderer/renderer.cpp
    renderer/shader.cpp
    renderer/pipeline.cpp
    renderer/buffer.cpp
    renderer/mesh.cpp
    renderer/camera.cpp

    # ECS
    ecs/entity.cpp

    # Scene
    scene/scene.cpp
    scene/transform.cpp

    # UI
    ui/ui_system.cpp
    ui/widgets.cpp

    # Math
    math/math_types.cpp
)

add_library(SparkEngine STATIC ${ENGINE_SOURCES})

target_include_directories(SparkEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SparkEngine
    PUBLIC
        Vulkan::Vulkan
        glfw
        glm::glm
        spdlog::spdlog
        stb_lib
        tinygltf_lib
        imgui_lib
)

# Platform specific
if(WIN32)
    target_compile_definitions(SparkEngine PUBLIC PLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(SparkEngine PUBLIC PLATFORM_LINUX)
endif()
