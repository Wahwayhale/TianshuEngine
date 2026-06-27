cmake_minimum_required(VERSION 3.20)
project(TianshuEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_EDITOR "Build the editor" ON)

# Find Vulkan
find_package(Vulkan REQUIRED)

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
