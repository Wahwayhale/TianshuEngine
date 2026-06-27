add_executable(SparkRuntime main.cpp)

target_link_libraries(SparkRuntime PRIVATE SparkEngine)

# Copy shaders to output directory
add_custom_command(TARGET SparkRuntime POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets/shaders
    $<TARGET_FILE_DIR:SparkRuntime>/shaders
)
