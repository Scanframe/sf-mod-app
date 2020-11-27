# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the project name.
project("rt-lib-app" LANGUAGES CXX)
# Include common configuration.
include("../rt-shared-lib.cmake")

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

include_directories(AFTER "./../../")

set(SOURCES
	src/main.cpp
	src/messagelist.h
	src/messagelist.cpp
	src/mainwindow.h
	src/mainwindow.cpp
	src/mainwindow.ui
	)

#add_subdirectory(source_dir [binary_dir] [EXCLUDE_FROM_ALL])
qt5_add_resources(SOURCES src/main.qrc)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE TARGET_QT)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE REPORT_TARGET)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

#target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/../bin)
#link_directories(AFTER ${CMAKE_BINARY_DIR}/../bin)

add_library(sf-misc SHARED IMPORTED)
set_property(TARGET sf-misc PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libsf-misc.so")

add_library(rt-iface SHARED IMPORTED)
set_property(TARGET rt-iface PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/librt-iface.so")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets sf-misc rt-iface)

if (WIN32)
	set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}" SUFFIX ".exe")
else ()
	set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}" SUFFIX ".bin")
endif ()

