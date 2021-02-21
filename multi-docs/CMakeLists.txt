# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the project name.
project("multi-docs" LANGUAGES CXX)

# Make sure builds do not wind up in the source directory.
list(APPEND CMAKE_PREFIX_PATH "../com/cmake")
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
_SetOutputDirs()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(SOURCES
	src/main.cpp
	src/Highlighter.h
	src/Highlighter.cpp
	src/MainWindow.h
	src/MainWindow.cpp
	src/TextEditor.h
	src/TextEditor.cpp
	src/CodeEditor.h
	src/CodeEditor.cpp
	src/MdiChild.cpp src/MdiChild.h)

# Add this for the headers in the autogen directory made by the *.ui files.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Add a resource file.
qt5_add_resources(SOURCES resource/resource.qrc)
# Set the actual target.
add_executable(${PROJECT_NAME} ${SOURCES})

# Include library configuration.
include("libraries.cmake")

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_QT)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${PROJECT_NAME} PRIVATE REPORT_TARGET)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

#target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/../bin)
#link_directories(AFTER ${CMAKE_BINARY_DIR}/../bin)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets sf-misc)

# Sets the extension of the generated binary.
_SetBinarySuffix("${PROJECT_NAME}")

