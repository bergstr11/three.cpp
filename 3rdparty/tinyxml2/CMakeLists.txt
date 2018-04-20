cmake_minimum_required(VERSION 3.7)
project(tinyxml2)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTINYXML2_DEBUG")

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES tinyxml2.cpp tinyxml2.h)

if(MSVC)
    target_compile_definitions(tinyxml2 PUBLIC -D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

add_library(tinyxml2 OBJECT ${SOURCE_FILES})
target_include_directories(tinyxml2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(tinyxml2 PROPERTIES POSITION_INDEPENDENT_CODE ON)
