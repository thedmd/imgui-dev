set(_imgui_SourceDir ${CMAKE_SOURCE_DIR}/imgui)

if (NOT TARGET imgui)
    set(_imgui_Sources
        ${_imgui_SourceDir}/imgui.h
        ${_imgui_SourceDir}/imconfig.h
        ${_imgui_SourceDir}/imgui_internal.h
        ${_imgui_SourceDir}/imgui.cpp
        ${_imgui_SourceDir}/imgui_widgets.cpp
        ${_imgui_SourceDir}/imgui_draw.cpp
        ${_imgui_SourceDir}/imgui_demo.cpp
    )

    source_group("" FILES ${_imgui_Sources})
    source_group(TREE ${_imgui_SourceDir} FILES ${_imgui_Sources})

    add_library(imgui STATIC
        ${_imgui_Sources}
    )
    target_include_directories(imgui PUBLIC
        ${_imgui_SourceDir}
    )
    unset(_imgui_Sources)
endif()

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    imgui
    REQUIRED_VARS
        _imgui_SourceDir
)

unset(_imgui_SourceDir)