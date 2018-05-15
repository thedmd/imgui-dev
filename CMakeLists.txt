cmake_minimum_required(VERSION 3.8)
project(imgui-dev)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD            17)
set(CMAKE_CXX_STANDARD_REQUIRED   YES)

if (DEFINED MSVC_VERSION AND (MSVC_VERSION GREATER_EQUAL 1500))
    unset(CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
endif()
if (ALPPE OR UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

add_subdirectory(sample-browser)
add_subdirectory(support)

set_directory_properties(PROPERTIES
    VS_STARTUP_PROJECT sample-browser
)
