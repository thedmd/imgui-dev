set(_gl3w_SourceDir ${CMAKE_SOURCE_DIR}/imgui/examples/libs/gl3w)

if (NOT TARGET imgui)
    add_library(imgui STATIC
        ${_gl3w_SourceDir}/GL/gl3w.h
        ${_gl3w_SourceDir}/GL/glcorearb.h
        ${_gl3w_SourceDir}/GL/gl3w.c
    )
    target_include_directories(imgui PUBLIC
        ${_gl3w_SourceDir}/imgui
    )
endif()

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    gl3w
    REQUIRED_VARS
        _gl3w_SourceDir
)

unset(_gl3w_SourceDir)
