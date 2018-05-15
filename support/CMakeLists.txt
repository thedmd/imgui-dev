
project(NATVIS)

add_custom_target(NATVIS SOURCES
    ${CMAKE_SOURCE_DIR}/imgui/misc/natvis/imgui.natvis
    custom.natvis
)

set_property(TARGET NATVIS PROPERTY FOLDER "CMakePredefinedTargets")