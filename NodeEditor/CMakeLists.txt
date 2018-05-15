project(NodeEditor)

find_package(imgui REQUIRED)
find_package(imguiex REQUIRED)

add_library(NodeEditor STATIC
    include/ax/SafeType.h
    include/ax/SafePointerType.h
    include/NodeEditor.h
    src/NodeEditor.cpp
)
target_include_directories(NodeEditor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(NodeEditor PRIVATE
    imgui
    imguiex
)
