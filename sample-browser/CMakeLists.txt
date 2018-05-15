project(sample-browser)

set(_sources
    samples/samples.h
    samples/layouts-basic.cpp
    samples/layouts-blueprint-node.cpp
    samples/drawlist-transformation.cpp
    samples/imguiex-canvas.cpp

    support/picojson.h
    support/property.h
    support/settings.h
    support/settings.inl
    support/settings.cpp
)

set(_sources_glfw
    ${CMAKE_SOURCE_DIR}/imgui/examples/opengl3_example/imgui_impl_glfw_gl3.h
    ${CMAKE_SOURCE_DIR}/imgui/examples/opengl3_example/imgui_impl_glfw_gl3.cpp
    main-glfw.cpp
)

set(_sources_dx11
    imgui_impl_dx11.h
    imgui_impl_dx11.cpp
    main.cpp
)

find_package(imgui REQUIRED)
find_package(imguiex REQUIRED)

if (WIN32)
    add_executable(${PROJECT_NAME}
        WIN32
        ${_sources}
        ${_sources_dx11}
    )

    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        d3d11.lib
        d3dcompiler.lib
    )

elseif (APPLE)
    find_package(gl3w REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(OpenGL REQUIRED)

    add_executable(${PROJECT_NAME}
        MACOSX_BUNDLE
        ${_sources}
        ${_sources_glfw}
    )

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${OPENGL_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/imgui/examples/opengl3_example
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        gl3w
        ${OPENGL_gl_LIBRARY}
        "-framework Cocoa"
        #-framework OpenGL
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui
    imguiex
)
