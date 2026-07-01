set(ENGINE_SOURCES
    # Core
    core/log.cpp
    core/window.cpp
    core/input.cpp
    core/application.cpp
    core/memory_allocator.cpp

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
    renderer/light.cpp
    renderer/descriptor_set.cpp

    # ECS
    ecs/entity.cpp

    # Scene
    scene/scene.cpp
    scene/transform.cpp
    scene/scene_serializer.cpp

    # Physics
    physics/jolt_physics.cpp
    physics/joint.cpp
    physics/vehicle.cpp
    physics/debug_draw.cpp
    physics/cloth.cpp
    physics/destruction.cpp
    physics/spatial_partition.cpp

    # Renderer Advanced
    renderer/volumetric.cpp
    renderer/terrain_system.cpp
    renderer/deferred_pipeline.cpp
    renderer/global_illumination.cpp
    renderer/ray_tracing.cpp
    renderer/culling.cpp
    renderer/instanced_renderer.cpp

    # Resource
    resource/resource_manager.cpp
    resource/asset_pipeline.cpp

    # UI
    ui/ui_system.cpp
    ui/widgets.cpp
    ui/imgui_integration.cpp

    # AI
    ai/ai_manager.cpp
    ai/ai_code_generator.cpp
    ai/ai_asset_generator.cpp
    ai/behavior_tree.cpp
    ai/navigation.cpp

    # Math
    math/math_types.cpp
)

# 平台特定源文件
if(PLATFORM_ANDROID)
    list(APPEND ENGINE_SOURCES
        platform/android/android_platform.cpp
    )
elseif(PLATFORM_WEB)
    list(APPEND ENGINE_SOURCES
        platform/web/web_platform.cpp
    )
else()
    # 桌面平台
    list(APPEND ENGINE_SOURCES
        platform/platform.cpp
    )
endif()

add_library(SparkEngine STATIC ${ENGINE_SOURCES})

target_include_directories(SparkEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 链接库
if(NOT PLATFORM_WEB)
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
else()
    # Web 平台使用不同的库
    target_link_libraries(SparkEngine
        PUBLIC
            glm::glm
            spdlog::spdlog
            stb_lib
            tinygltf_lib
    )
endif()

# Platform specific
if(PLATFORM_ANDROID)
    target_compile_definitions(SparkEngine PUBLIC PLATFORM_ANDROID)
    target_link_libraries(SparkEngine PUBLIC android log)
elseif(PLATFORM_WEB)
    target_compile_definitions(SparkEngine PUBLIC PLATFORM_WEB)
elseif(WIN32)
    target_compile_definitions(SparkEngine PUBLIC PLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(SparkEngine PUBLIC PLATFORM_LINUX)
elseif(APPLE)
    target_compile_definitions(SparkEngine PUBLIC PLATFORM_MACOS)
endif()
