cmake_minimum_required(VERSION 3.8)
project(three_base)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(assimp REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(SHADER_RESOURCES
        renderers/gl/shader/ShaderLib/ShaderLib.qrc
        renderers/gl/shader/ShaderChunk/ShaderChunk.qrc
        renderers/gl/shader/Materials/Materials.qrc)

set(THREE_SRCDIRS
        camera
        controls
        core
        geometry
        extras
        extras/shape
        extras/curve
        helper
        light
        loader
        material
        math
        objects
        scene
        textures
        util
        util/impl
        renderers
        renderers/gl
        renderers/gl/shader)

set(THREE_HDRDIRS
        camera
        controls
        core
        geometry
        extras
        extras/shape
        extras/curve
        helper
        light
        loader
        material
        math
        objects
        scene
        textures
        renderers
        util)

CONFIGURE_FILE("${THREE_ROOT}/etc/threepp.pc.in" "threepp.pc" @ONLY)

message("using Assimp: ${ASSIMP_LIBRARIES} ${ASSIMP_LIBRARY_DIRS}")

add_subdirectory(quick)

add_library(threepp SHARED Constants.h ${SHADER_RESOURCES} $<TARGET_OBJECTS:tinyxml2>)
add_dependencies(threepp tinyxml2)

target_include_directories(threepp PUBLIC
        $<BUILD_INTERFACE:${THREE_ROOT}>
        $<INSTALL_INTERFACE:include>)

if(ANDROID)
    target_link_libraries(threepp PUBLIC
            z
            ${ASSIMP_LIBRARY_DIRS}/libassimpd.a
            ${ASSIMP_LIBRARY_DIRS}/libIrrXMLd.a
            Qt5::Core Qt5::Gui)
else(ANDROID)
    target_link_libraries(threepp PUBLIC assimp Qt5::Core Qt5::Gui)
endif(ANDROID)

target_include_directories(threepp PRIVATE ${ASSIMP_INCLUDE_DIRS})

set_target_properties(threepp PROPERTIES SOVERSION ${THREE_VERSION})

foreach(DIR in ${THREE_SRCDIRS})
    file(GLOB SOURCES ${DIR}/*.cpp)
    target_sources(threepp PRIVATE ${SOURCES})

    file(GLOB HEADERS ${DIR}/*.h)
    target_sources(threepp PRIVATE ${HEADERS})
endforeach(DIR in ${THREE_SRCDIRS})
target_sources(threepp PRIVATE core/impl/raycast.h)

install(TARGETS threepp
        EXPORT threepp
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(EXPORT threepp
        DESTINATION lib/cmake/threepp
        FILE threepp-config.cmake)

foreach(DIR in ${THREE_HDRDIRS})
    file(GLOB HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${DIR}/*.h)
    install(FILES ${HEADERS} DESTINATION include/threepp/${DIR})
endforeach(DIR in ${THREE_HDRDIRS})
file(GLOB HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)
install(FILES ${HEADERS} DESTINATION include/threepp)

install(FILES
        renderers/gl/shader/ShaderLib.h
        renderers/gl/shader/ShaderID.h
        renderers/gl/shader/UniformsLib.h
        DESTINATION include/threepp/renderers/gl/shader)
install(FILES
        renderers/gl/Uniforms.h
        renderers/gl/Lights.h
        DESTINATION include/threepp/renderers/gl)