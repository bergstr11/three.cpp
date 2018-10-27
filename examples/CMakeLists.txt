cmake_minimum_required(VERSION 3.7)
project(three_test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)
find_package(Qt5Core REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp
        ShadowMapViewer.h
        resources.qrc
        qml/resources/pontiac_gto/pontiac.qrc)

add_executable(three_test ${SOURCE_FILES})

if(WITH_PHYSICS)
    target_sources(three_test PRIVATE PhysicsTestModelRef.h PhysicsTestModelRef.cpp)
endif(WITH_PHYSICS)

target_include_directories(three_test PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include)

if(ANDROID)
    target_link_libraries(three_test PUBLIC threepp threeppq threereact GLESv3 Qt5::Core Qt5::Gui Qt5::Quick)
elseif(WIN32)
    target_link_libraries(three_test PUBLIC
            threepp_static threeppq_static threereact opengl32 Qt5::Core Qt5::Gui Qt5::Quick)
else(WIN32)
    target_link_libraries(three_test PUBLIC threepp threeppq GL threereact Qt5::Core Qt5::Gui Qt5::Quick)
endif(ANDROID)
