cmake_minimum_required(VERSION 3.8)
project(three_cpp)


set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES three.cpp three.h)
add_library(three_cpp SHARED ${SOURCE_FILES})