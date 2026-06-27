set(EDITOR_SOURCES
    main.cpp
    core/editor_state.cpp
    core/command_system.cpp
    core/entity_picker.cpp
    panels/viewport_panel.cpp
    panels/inspector_panel.cpp
    panels/scene_hierarchy_panel.cpp
    panels/asset_browser_panel.cpp
    panels/toolbar_panel.cpp
    panels/status_bar.cpp
    panels/console_panel.cpp
    panels/settings_panel.cpp
    panels/ai_panel.cpp
)

add_executable(SparkEditor ${EDITOR_SOURCES})

target_include_directories(SparkEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SparkEditor PRIVATE SparkEngine)
