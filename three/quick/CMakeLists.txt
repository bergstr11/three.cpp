cmake_minimum_required(VERSION 3.7)
project(three_quick)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(assimp REQUIRED)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE SOURCE_FILES . *.cpp)
file(GLOB_RECURSE HEADER_FILES . *.h)

add_library(threeppq SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(threepp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:include)
target_link_libraries(threeppq PUBLIC threepp Qt5::Core Qt5::Gui Qt5::Quick assimp)

install(TARGETS threeppq
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION include/three)