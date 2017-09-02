cmake_minimum_required(VERSION 3.8)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h _include/*)

add_library(three_qt SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(three_qt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(three_qt PUBLIC Qt5::Core Qt5::Gui)
