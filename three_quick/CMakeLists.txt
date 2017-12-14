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

add_library(three_quick ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(three_quick PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(three_quick PUBLIC threepp Qt5::Core Qt5::Gui Qt5::Quick assimp)
