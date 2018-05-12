cmake_minimum_required(VERSION 3.8)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)
set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk)
project(imgui-dev)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD            17)
set(CMAKE_CXX_STANDARD_REQUIRED   YES)

if (DEFINED MSVC_VERSION AND (MSVC_VERSION GREATER_EQUAL 1500))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
endif()
if (ALPPE OR UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# imgui
add_library(imgui STATIC
    imgui/imgui.h
    imgui/imconfig.h
    imgui/imgui_internal.h
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
)
target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
)

# imguiex
add_library(imguiex STATIC
    imguiex/imguiex.h
    imguiex/imguiex_internal.h
    imguiex/imguiex.cpp
    imguiex/imguiex_api.cpp
    imguiex/imguiex_canvas.cpp
)
target_include_directories(imguiex PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiex
)
target_link_libraries(imguiex PUBLIC
    imgui
)

# gl3w
add_library(gl3w STATIC
    imgui/examples/libs/gl3w/GL/gl3w.h
    imgui/examples/libs/gl3w/GL/glcorearb.h
    imgui/examples/libs/gl3w/GL/gl3w.c
)
target_include_directories(gl3w PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples/libs/gl3w
)

# example
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
add_executable(example
    example/samples/samples.h
    example/samples/layouts.cpp
    example/samples/canvas.cpp
    example/support/picojson.h
    example/support/property.h
    example/support/settings.h
    example/support/settings.inl
    example/support/settings.cpp
    imgui/examples/opengl3_example/imgui_impl_glfw_gl3.h
    imgui/examples/opengl3_example/imgui_impl_glfw_gl3.cpp
    example/main-glfw.cpp
)
target_include_directories(example PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/example
    ${OPENGL_INCLUDE_DIR}
    imgui/examples/opengl3_example/
)
target_link_libraries(example PRIVATE
    imgui
    imguiex
    glfw
    gl3w
    ${OPENGL_gl_LIBRARY}
    "-framework Cocoa"
    #-framework OpenGL
    "-framework IOKit"
    "-framework CoreVideo"
)
