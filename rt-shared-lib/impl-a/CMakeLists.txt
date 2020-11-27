# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the project name.
project("rt-impl-a" LANGUAGES CXX)
# Include common configuration.
include("../rt-shared-lib.cmake")

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

include_directories(AFTER "./../../" "./../")

add_library(rt-impl-a SHARED
	src/global.h
	src/rt-impl-a.h
	src/rt-impl-a.cpp
)

add_library(sf-misc SHARED IMPORTED)
set_property(TARGET sf-misc PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libsf-misc.so")
add_library(rt-iface SHARED IMPORTED)
set_property(TARGET rt-iface PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/librt-iface.so")

# Link the Qt widgets library.
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets sf-misc rt-iface)

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE TARGET_QT)

# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE TARGET_DYNAMIC_LIB)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE REPORT_TARGET)

# Tell the code the library is used as an archive. (So no imports or exports)
#target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE _IMPL_A_ARC)

# Tell the code the library is used as dynamic library. (So imports needed)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE _IMPL_A_PKG)

if (WIN32)
	set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME} SUFFIX ".dll")
else ()
	set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME} SUFFIX ".so")
endif ()
