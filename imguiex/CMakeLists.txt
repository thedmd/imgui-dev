project(imguiex)

set(_sources
    imguiex.h
    imguiex_internal.h
    imguiex.cpp
    imguiex_api.cpp
    imguiex_canvas.h
    imguiex_canvas.cpp
)

source_group("" FILES ${_sources})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_sources})

find_package(imgui REQUIRED)

add_library(imguiex STATIC ${_sources})

target_include_directories(imguiex PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(imguiex PUBLIC
    imgui
)
