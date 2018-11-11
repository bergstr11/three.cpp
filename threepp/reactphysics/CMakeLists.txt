cmake_minimum_required(VERSION 3.7)
project(three_reactphysics)

find_package(reactphysics3d REQUIRED)
find_package(Qt5Gui REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(TREACT_SRCDIRS )
set(TREACT_HDRDIRS )

file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)

if(ANDROID)
    add_compile_definitions(QT_OPENGL_ES_3_2)
endif(ANDROID)

add_library(threereact OBJECT ${SOURCE_FILES} ${HEADER_FILES})

target_compile_definitions(threereact PRIVATE -DCOMPILE_THREEREPP_DLL)

target_include_directories(threereact PUBLIC
        $<BUILD_INTERFACE:${THREE_ROOT}>
        $<INSTALL_INTERFACE:include>)

target_link_libraries(threereact PUBLIC reactphysics3d Qt5::Gui)

set_target_properties(threereact PROPERTIES SOVERSION ${THREE_VERSION})

install(TARGETS threereact INCLUDES DESTINATION include)

foreach(DIR in ${TREACT_HDRDIRS})
    file(GLOB HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${DIR}/*.h)
    install(FILES ${HEADERS} DESTINATION include/threepp/reactphysics/${DIR})
endforeach(DIR in ${THREE_HDRDIRS})

file(GLOB HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)
install(FILES ${HEADERS} DESTINATION include/threepp/reactphysics)
