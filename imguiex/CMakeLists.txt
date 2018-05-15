project(imguiex)

find_package(imgui REQUIRED)

add_library(imguiex STATIC
    imguiex.h
    imguiex_internal.h
    imguiex.cpp
    imguiex_api.cpp
    imguiex_canvas.cpp
)
target_include_directories(imguiex PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(imguiex PUBLIC
    imgui
)
