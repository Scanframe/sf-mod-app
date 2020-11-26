cmake_minimum_required(VERSION 3.14)
# Set the project name.
project("rt-lib-app" LANGUAGES CXX)

if (WIN32)
	message("Platform: Windows")
else ()
	message("Platform: Linux ${CMAKE_SYSTEM_VERSION}")
endif ()

# Make sure builds do not wind up in the source directory.
#set(BuildCheck_DIR "../../cmake-mods")
list(APPEND CMAKE_PREFIX_PATH "../../com/cmake")
find_package(SfBuildCheck CONFIG REQUIRED)

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

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

include_directories(AFTER "./../../")

#set(TS_FILES ${CMAKE_PROJECT_NAME}_en_US.ts)

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

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${TS_FILES})

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE TARGET_QT)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE REPORT_TARGET)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

link_directories(${CMAKE_BINARY_DIR}/../bin)

add_library(rt-iface SHARED IMPORTED)
set_property(TARGET rt-iface PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/librt-iface.so")

add_library(sf-misc SHARED IMPORTED)
set_property(TARGET sf-misc PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libsf-misc.so")

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets rt-iface sf-misc)

if (WIN32)
	set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}" SUFFIX ".exe")
else ()
	set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}" SUFFIX ".bin")
endif ()

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
