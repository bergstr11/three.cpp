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
        extras/core
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
        extras/core
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

if(WITH_REACTPHYSICS)
    add_compile_definitions(COMPILE_REACTPHYSICS)
    add_subdirectory(reactphysics)
endif(WITH_REACTPHYSICS)

add_subdirectory(quick)

add_library(threepp_static STATIC Constants.h ${SHADER_RESOURCES} $<TARGET_OBJECTS:tinyxml2>)
add_library(threepp SHARED Constants.h ${SHADER_RESOURCES} $<TARGET_OBJECTS:tinyxml2> util/osdecl.h)

target_compile_definitions(threepp PRIVATE -DCOMPILE_THREEPP_DLL)

foreach(TARGET threepp_static threepp)
    target_include_directories(${TARGET} PUBLIC
            $<BUILD_INTERFACE:${THREE_ROOT}>
            $<INSTALL_INTERFACE:include>)

    if(ANDROID)
        target_link_libraries(${TARGET} PUBLIC
                z
                ${ASSIMP_LIBRARY_DIRS}/libassimpd.a
                ${ASSIMP_LIBRARY_DIRS}/libIrrXMLd.a
                Qt5::Core Qt5::Gui)
    elseif(WIN32)
        target_link_libraries(${TARGET} PUBLIC
                ${ASSIMP_LIBRARY_DIRS}/${ASSIMP_LIBRARIES} opengl32.lib
                Qt5::Core Qt5::Gui)
    else(WIN32)
        target_link_libraries(${TARGET} PUBLIC assimp Qt5::Core Qt5::Gui)
    endif(ANDROID)

    target_include_directories(${TARGET} PRIVATE ${ASSIMP_INCLUDE_DIRS})

    set_target_properties(${TARGET} PROPERTIES SOVERSION ${THREE_VERSION})

    foreach(DIR in ${THREE_SRCDIRS})
        file(GLOB SOURCES ${DIR}/*.cpp)
        target_sources(${TARGET} PRIVATE ${SOURCES})

        file(GLOB HEADERS ${DIR}/*.h)
        target_sources(${TARGET} PRIVATE ${HEADERS})
    endforeach(DIR in ${THREE_SRCDIRS})
    target_sources(${TARGET} PRIVATE core/impl/raycast.h)

    add_dependencies(${TARGET} tinyxml2)
endforeach(TARGET)

install(TARGETS threepp threepp_static
        EXPORT threepp
        INCLUDES DESTINATION include
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
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