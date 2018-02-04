cmake_minimum_required(VERSION 3.8)
project(three)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(assimp REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)

qt5_add_resources(SHADER_RESOURCES
        renderers/gl/shader/ShaderLib/ShaderLib.qrc
        renderers/gl/shader/ShaderChunk/ShaderChunk.qrc
        renderers/gl/shader/Materials/Materials.qrc)

set(THREE_SRCDIRS
        camera
        controls
        core
        geometry
        helper
        light
        loader
        material
        math
        objects
        scene
        textures
        renderers
        renderers/gl
        renderers/gl/shader)

add_library(threepp SHARED ${SHADER_RESOURCES} ${HEADER_FILES})
target_include_directories(threepp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include)
target_link_libraries(threepp PUBLIC Qt5::Core Qt5::Gui)

foreach(SRC_DIR in ${THREE_SRCDIRS})
    file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)
    target_sources(threepp PUBLIC ${SOURCES})

    file(GLOB_RECURSE HEADERS ${SRC_DIR}/*.h)
    target_sources(threepp PUBLIC ${HEADERS})
endforeach(SRC_DIR in ${THREE_SRCDIRS})

install(TARGETS threepp
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION include)