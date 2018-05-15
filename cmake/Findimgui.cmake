set(_imgui_SourceDir ${CMAKE_SOURCE_DIR}/imgui)

if (NOT TARGET imgui)
    add_library(imgui STATIC
        ${_imgui_SourceDir}/imgui.h
        ${_imgui_SourceDir}/imconfig.h
        ${_imgui_SourceDir}/imgui_internal.h
        ${_imgui_SourceDir}/imgui.cpp
        ${_imgui_SourceDir}/imgui_draw.cpp
        ${_imgui_SourceDir}/imgui_demo.cpp
    )
    target_include_directories(imgui PUBLIC
        ${_imgui_SourceDir}
    )
endif()

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    imgui
    REQUIRED_VARS
        _imgui_SourceDir
)

unset(_imgui_SourceDir)