cmake_minimum_required(VERSION 3.7)
project(Three.pp)

set(THREE_VERSION 0.9)
set(THREE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

message("path: ${CMAKE_PREFIX_PATH}")
if(WIN32)
    add_definitions(-DNOMINMAX)
    set(ASSIMP_PREFIX $ENV{ASSIMP_PATH})
    if(NOT IS_DIRECTORY ${ASSIMP_PREFIX})
        message(FATAL_ERROR "ASSIMP_PATH environment variable must point to a valid directory")
    endif(NOT IS_DIRECTORY ${ASSIMP_PREFIX})
    set(CMAKE_PREFIX_PATH "${ASSIMP_PREFIX};${CMAKE_PREFIX_PATH}")
endif(WIN32)

add_subdirectory(threepp)
add_subdirectory(examples)
add_subdirectory(3rdparty/tinyxml2)
