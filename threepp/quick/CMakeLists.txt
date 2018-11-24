cmake_minimum_required(VERSION 3.7)
project(three_quick)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Quick REQUIRED)
find_package(Qt5Core REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(THREEQ_HDRDIRS
        cameras
        elements
        interact
        materials
        objects
        lights
        qutil
        scene
        loader
        textures)

file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)

add_library(threeppq_static STATIC ${SOURCE_FILES} ${HEADER_FILES})
add_library(threeppq SHARED ${SOURCE_FILES} ${HEADER_FILES})

target_compile_definitions(threeppq PRIVATE -DCOMPILE_THREEPPQ_DLL -DUSE_THREEPP_DLL)

foreach(TARGET threeppq_static threeppq)
    target_include_directories(${TARGET} PUBLIC
            $<BUILD_INTERFACE:${THREE_ROOT}>
            $<INSTALL_INTERFACE:include>)
endforeach(TARGET)

if(WITH_REACTPHYSICS)
    target_include_directories(threeppq PRIVATE $<TARGET_PROPERTY:threereact,INCLUDE_DIRECTORIES>)
    target_include_directories(threeppq_static PRIVATE $<TARGET_PROPERTY:threereact,INTERFACE_INCLUDE_DIRECTORIES>)

    target_sources(threeppq PRIVATE scene/R3dPhysics.cxx scene/R3dPhysics.hxx)
    target_sources(threeppq_static PRIVATE scene/R3dPhysics.cxx scene/R3dPhysics.hxx)

    target_sources(threeppq PRIVATE $<TARGET_OBJECTS:threereact>)
    target_sources(threeppq_static PRIVATE $<TARGET_OBJECTS:threereact>)
endif(WITH_REACTPHYSICS)

target_link_libraries(threeppq_static PUBLIC threepp_static Qt5::Core Qt5::Gui Qt5::Quick)
target_link_libraries(threeppq PUBLIC threepp Qt5::Core Qt5::Gui Qt5::Quick)

set_target_properties(threeppq PROPERTIES SOVERSION ${THREE_VERSION})

install(TARGETS threeppq threeppq_static
        EXPORT threeppq
        INCLUDES DESTINATION include
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(EXPORT threeppq
        CONFIGURATIONS Debug Release
        DESTINATION lib/cmake/threeppq
        FILE threeppq-config.cmake)

foreach(DIR in ${THREEQ_HDRDIRS})
    file(GLOB HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${DIR}/*.h)
    install(FILES ${HEADERS} DESTINATION include/threepp/quick/${DIR})
endforeach(DIR in ${THREE_HDRDIRS})

file(GLOB HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)
install(FILES ${HEADERS} DESTINATION include/threepp/quick)
