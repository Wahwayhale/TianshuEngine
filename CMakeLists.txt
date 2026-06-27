cmake_minimum_required(VERSION 3.20)
project(SparkEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build options
option(BUILD_EDITOR "Build the editor" ON)
option(ENABLE_VALIDATION "Enable Vulkan validation layers" ON)
option(COMPILE_SHADERS "Compile shaders during build" ON)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Find glslc for shader compilation
find_program(GLSLC glslc HINTS $ENV{VULKAN_SDK}/Bin)

# Third party libraries
add_subdirectory(third_party)

# Engine library
add_subdirectory(engine)

# Runtime executable
add_subdirectory(runtime)

# Editor executable
if(BUILD_EDITOR)
    add_subdirectory(editor)
endif()

# Shader compilation
if(COMPILE_SHADERS AND GLSLC)
    file(GLOB SHADERS "assets/shaders/*.glsl")
    foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
        set(SPIRV "${CMAKE_SOURCE_DIR}/assets/shaders/${SHADER_NAME}.spv")
        add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSLC} ${SHADER} -o ${SPIRV}
            DEPENDS ${SHADER}
            COMMENT "Compiling shader: ${SHADER_NAME}"
        )
        list(APPEND SPIRV_FILES ${SPIRV})
    endforeach()
    add_custom_target(Shaders ALL DEPENDS ${SPIRV_FILES})
endif()
