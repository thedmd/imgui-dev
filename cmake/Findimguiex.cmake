set(_imguiex_SourceDir ${CMAKE_SOURCE_DIR}/imguiex)
set(_imguiex_BinaryDir ${CMAKE_BINARY_DIR}/imguiex)

if (NOT TARGET imguiex)
    add_subdirectory(
        ${_imguiex_SourceDir}
        ${_imguiex_BinaryDir}
    )
endif()

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    imguiex
    REQUIRED_VARS
        _imguiex_SourceDir
)

unset(_imguiex_SourceDir)
unset(_imguiex_BinaryDir)