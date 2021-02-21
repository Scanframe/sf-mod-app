# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the project name.
project("as-app" LANGUAGES CXX)

# Make sure builds do not wind up in the source directory.
list(APPEND CMAKE_PREFIX_PATH "../../com/cmake")
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Add path to angel script
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../script-engines/angelscript_2.35.0/sdk/lib/cmake/Angelscript")
#message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
_SetOutputDirs()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find the needed packages.
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Qml REQUIRED)
find_package(Threads COMPONENTS Threads REQUIRED)
find_package(Angelscript COMPONENTS angelscript REQUIRED)

set(SOURCES
	src/main.cpp
	src/mainwindow.h
	src/mainwindow.cpp
	src/mainwindow.ui
	src/tutorial.cpp
	src/tutorial.h
	src/add_on/scriptarray.h
	src/add_on/scriptarray.cpp
	src/add_on/scriptstdstring.h
	src/add_on/scriptstdstring.cpp
	src/add_on/scriptstdstring_utils.cpp
	)

# Add this for the headers in the autogen directory made by the *.ui files.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Add resource to the application.
qt5_add_resources(SOURCES resource/resource.qrc)
# Set the actual target.
add_executable(${PROJECT_NAME} ${SOURCES})

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_QT)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${PROJECT_NAME} PRIVATE REPORT_TARGET)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

#target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/../bin)
#link_directories(AFTER ${CMAKE_BINARY_DIR}/../bin)

# Include library configuration.
include("libraries.cmake")

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Qml sf-misc Angelscript::angelscript)

# Sets the extension of the generated binary.
_SetBinarySuffix("${PROJECT_NAME}")

