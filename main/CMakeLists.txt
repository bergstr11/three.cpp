cmake_minimum_required(VERSION 3.8)
project(three_cpp)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h _include/*)

add_library(three_cpp SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(three_cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/_include)
target_link_libraries(three_cpp PUBLIC Qt5::Core Qt5::Gui)
