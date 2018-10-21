cmake_minimum_required(VERSION 3.7)
project(three_reactphysics)

find_package(reactphysics3d REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(TREACT_SRCDIRS core)
set(TREACT_HDRDIRS core)

file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)

add_library(threereact_static STATIC ${SOURCE_FILES} ${HEADER_FILES})
add_library(threereact SHARED ${SOURCE_FILES} ${HEADER_FILES})

target_compile_definitions(threereact PRIVATE -DCOMPILE_THREEREACT_DLL -DUSE_THREEREPP_DLL)

foreach(TARGET threereact_static threereact)
    target_include_directories(${TARGET} PUBLIC
            $<BUILD_INTERFACE:${THREE_ROOT}>
            $<INSTALL_INTERFACE:include>)
endforeach(TARGET)

target_link_libraries(threereact PUBLIC reactphysics3d)

set_target_properties(threereact PROPERTIES SOVERSION ${THREE_VERSION})

install(TARGETS threereact threereact_static
        EXPORT threereact
        INCLUDES DESTINATION include
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(EXPORT threereact
        DESTINATION lib/cmake/threereact
        FILE threereact-config.cmake)

foreach(DIR in ${TREACT_HDRDIRS})
    file(GLOB HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${DIR}/*.h)
    install(FILES ${HEADERS} DESTINATION include/threepp/reactphysics/${DIR})
endforeach(DIR in ${THREE_HDRDIRS})

file(GLOB HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)
install(FILES ${HEADERS} DESTINATION include/threepp/reactphysics)
