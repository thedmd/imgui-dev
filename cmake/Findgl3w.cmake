set(_gl3w_SourceDir ${CMAKE_SOURCE_DIR}/imgui/examples/libs/gl3w)

if (NOT TARGET gl3w)
    add_library(gl3w STATIC
        ${_gl3w_SourceDir}/GL/gl3w.h
        ${_gl3w_SourceDir}/GL/glcorearb.h
        ${_gl3w_SourceDir}/GL/gl3w.c
    )
    target_include_directories(gl3w PUBLIC
        ${_gl3w_SourceDir}
    )
endif()

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    gl3w
    REQUIRED_VARS
        _gl3w_SourceDir
)

unset(_gl3w_SourceDir)
