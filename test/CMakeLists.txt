cmake_minimum_required(VERSION 3.7)
project(three_test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(assimp REQUIRED)

set(CMAKE_CXX_STANDARD 11)

qt5_add_resources(UI_RESOURCES resources.qrc)

set(SOURCE_FILES
        main.cpp
        ThreeDItem.cpp
        ThreeDInteractor.cpp)

set(HEADER_FILES ThreeDItem.h)

add_executable(three_test ${SOURCE_FILES} ${HEADER_FILES} ${UI_RESOURCES})
target_link_libraries(three_test PUBLIC three_qt Qt5::Core Qt5::Gui Qt5::Quick assimp)
