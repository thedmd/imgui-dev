project(NodeEditor)

set(_sources
    include/ax/SafeType.h
    include/ax/SafePointerType.h
    include/NodeEditor.h
    src/NodeEditor_Internal.h
    src/NodeEditor_Internal.inl
    src/NodeEditor.cpp
    src/Debug.cpp
    src/Selection.cpp
    src/Editor.h
    src/Editor.cpp
    src/Actions.h
    src/Actions/Action.h
    src/Actions/Action.cpp
    src/Actions/DragAction.h
    src/Actions/DragAction.cpp
    src/Actions/NavigateAction.h
    src/Actions/NavigateAction.cpp
    src/Actions/SelectAction.h
    src/Actions/SelectAction.cpp
    src/Builders/NodeBuilder.h
    src/Builders/NodeBuilder.cpp
    src/Utilities/BitmaskType.h
    src/Utilities/Drawing.h
    src/Utilities/Drawing.cpp
)

source_group("" FILES ${_sources})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_sources})


find_package(imgui REQUIRED)
find_package(imguiex REQUIRED)

add_library(NodeEditor STATIC ${_sources})

target_include_directories(NodeEditor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(NodeEditor PUBLIC #PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(NodeEditor PRIVATE
    imgui
    imguiex
)
