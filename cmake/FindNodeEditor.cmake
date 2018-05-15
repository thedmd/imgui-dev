set(_NodeEditor_SourceDir ${CMAKE_SOURCE_DIR}/NodeEditor)
set(_NodeEditor_BinaryDir ${CMAKE_BINARY_DIR}/NodeEditor)

if (NOT TARGET NodeEditor)
    add_subdirectory(
        ${_NodeEditor_SourceDir}
        ${_NodeEditor_BinaryDir}
    )
endif()

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    NodeEditor
    REQUIRED_VARS
        _NodeEditor_SourceDir
)

unset(_NodeEditor_SourceDir)
unset(_NodeEditor_BinaryDir)