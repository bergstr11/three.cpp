cmake_minimum_required(VERSION 3.8)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)

qt5_add_resources(SHADER_RESOURCES
        renderers/gl/shader/ShaderLib/ShaderLib.qrc
        renderers/gl/shader/ShaderChunk/ShaderChunk.qrc
        renderers/gl/shader/Materials/Materials.qrc)

file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h _include/*)

add_library(threepp SHARED ${SOURCE_FILES} ${SHADER_RESOURCES} ${HEADER_FILES})
target_include_directories(threepp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(threepp PUBLIC Qt5::Core Qt5::Gui)
