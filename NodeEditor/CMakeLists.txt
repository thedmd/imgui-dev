project(NodeEditor)

set(_sources
    include/ax/SafeType.h
    include/ax/SafePointerType.h
    include/NodeEditor.h
    src/NodeEditor.cpp
)

source_group("" FILES ${_sources})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_sources})


find_package(imgui REQUIRED)
find_package(imguiex REQUIRED)

add_library(NodeEditor STATIC ${_sources})

target_include_directories(NodeEditor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(NodeEditor PRIVATE
    imgui
    imguiex
)
