set(EDITOR_SOURCES
    main.cpp
    editor_commands.cpp
    content_browser.cpp
    editor_selection.cpp
    material_editor.cpp
)

add_executable(SparkEditor ${EDITOR_SOURCES})

target_include_directories(SparkEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SparkEditor PRIVATE SparkEngine)
