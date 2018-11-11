set(ENV{QT_OPENGL_ES_3_1} True)
set(QT_TARGET $ENV{HOME}/Qt/5.12/android_arm64)
set(CMAKE_STAGING_PREFIX $ENV{HOME}/work/android-toolchain/arm64_24)
set(CMAKE_INSTALL_PREFIX ${CMAKE_STAGING_PREFIX})
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 24) # API level
set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
set(CMAKE_ANDROID_NDK $ENV{HOME}/Android/Sdk/ndk-bundle)

#look for dependencies here
SET(CMAKE_FIND_ROOT_PATH "${CMAKE_STAGING_PREFIX};${QT_TARGET}")

# search for programs in the build host and target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#some specialties for OpenCV. Requires manipulation in OpenCV's CMakeLists.txt,
#in particular adding AND NOT OPENCV_ANDROID_NATIVE clauses in multiple places
set(OPENCV_ANDROID_NATIVE True)
add_definitions(-DANDROID -DBUILD_ANDROID)
SET(ANDROID_NDK_ABI_NAME ${CMAKE_ANDROID_ARCH_ABI})

#required by AddQtAndroidApk
set(QT_ANDROID_SDK_ROOT $ENV{HOME}/Android/Sdk)
set(QT_ANDROID_ANT $ENV{HOME}/work/apache-ant-1.9.7/bin/ant)
set(QT_ANDROID_NDK_ROOT ${CMAKE_ANDROID_NDK})
set(QT_ANDROID_ABI ${CMAKE_ANDROID_ARCH_ABI})

