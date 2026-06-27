set(ENGINE_SOURCES
    # Core
    core/log.cpp
    core/window.cpp
    core/input.cpp
    core/application.cpp
    core/profiler.cpp
    core/debug_console.cpp
    core/serialization.cpp
    core/plugin_system.cpp

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
    renderer/material.cpp
    renderer/texture.cpp
    renderer/model.cpp
    renderer/shadow_map.cpp
    renderer/post_process.cpp
    renderer/render_system.cpp
    renderer/particle_system.cpp
    renderer/terrain.cpp
    renderer/skybox.cpp
    renderer/animation.cpp
    renderer/frustum_cull.cpp
    renderer/lod_system.cpp
    renderer/instanced_renderer.cpp
    renderer/animation_system.cpp
    renderer/ssao.cpp
    renderer/multithreaded_renderer.cpp
    renderer/uniform_buffer.cpp
    renderer/descriptor_set.cpp

    # ECS
    ecs/entity.cpp

    # Scene
    scene/scene.cpp
    scene/transform.cpp

    # Physics
    physics/physics_world.cpp
    physics/rigid_body.cpp
    physics/collider.cpp
    physics/debug_draw.cpp
    physics/physics_system.cpp
    physics/jolt_physics.cpp

    # Renderer
    renderer/deferred_renderer.cpp

    # Platform
    platform/platform.cpp

    # AI
    ai/ai_manager.cpp
    ai/ai_code_generator.cpp
    ai/ai_asset_generator.cpp

    # Audio
    audio/audio_engine.cpp
    audio/audio_source.cpp
    audio/audio_listener.cpp
    audio/audio_system.cpp

    # Script
    script/script_engine.cpp
    script/script_system.cpp

    # UI
    ui/ui_system.cpp

    # Resource
    resource/resource_manager.cpp
    resource/resource_streamer.cpp
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
        Jolt
        imgui_lib
)

# Platform specific
if(WIN32)
    target_compile_definitions(SparkEngine PUBLIC PLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(SparkEngine PUBLIC PLATFORM_LINUX)
endif()

# Enable validation layers in debug
if(ENABLE_VALIDATION AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(SparkEngine PUBLIC ENABLE_VULKAN_VALIDATION)
endif()
