cmake_minimum_required(VERSION 3.14)
# Set the project name.
project("rt-iface" LANGUAGES CXX)

if (WIN32)
	message("Platform: Windows")
else ()
	message("Platform: Linux ${CMAKE_SYSTEM_VERSION}")
endif ()

# Make sure builds do not wind up in the source directory.
list(APPEND CMAKE_PREFIX_PATH "../../com/cmake")
find_package(SfBuildCheck CONFIG REQUIRED)

# Do not export all by default.
add_definitions("-fvisibility=hidden")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../bin)

# Write a message.
message("Project name: ${CMAKE_PROJECT_NAME}")
message("Run-Time Directory: ${CMAKE_BINARY_DIR}")
message("Library Out Directory: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message("Archive Out Directory: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

include_directories(AFTER "./../../")

add_library(${CMAKE_PROJECT_NAME} SHARED
	src/rt-iface_global.h
	src/rt-iface.h
	src/rt-iface.cpp
	)

# Link the Qt widgets library.
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE TARGET_QT)

# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE TARGET_DYNAMIC_LIB)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE REPORT_TARGET)

target_compile_definitions(rt-iface PRIVATE RTIFACELIB_LIBRARY)

if (WIN32)
	set_target_properties(rt-iface PROPERTIES OUTPUT_NAME "rt-iface" SUFFIX ".dll")
else ()
	set_target_properties(rt-iface PROPERTIES OUTPUT_NAME "rt-iface" SUFFIX ".so")
endif ()

