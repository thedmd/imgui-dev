
project(NATVIS)

add_custom_target(NATVIS SOURCES
    custom.natvis
)

set_property(TARGET NATVIS PROPERTY FOLDER "CMakePredefinedTargets")