# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the project name.
project("multi-docs" LANGUAGES CXX)

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "../../com/cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
_SetOutputDirs()

# Uses directory Set by _SetOutputDirs().
find_package(SfMisc REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(SOURCES
	main.cpp
	Highlighter.h
	Highlighter.cpp
	MainWindow.h
	MainWindow.cpp
	TextEditor.h
	TextEditor.cpp
	CodeEditor.h
	CodeEditor.cpp
	MdiChild.cpp MdiChild.h
	res/resource.qrc
	)

# Add this for the headers in the autogen directory made by the *.ui files.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set the actual executable target.
add_executable(${PROJECT_NAME})
# Set the default compiler options for our own code only.
_SetTargetDefaultCompileOptions()
# Set the sources for this target.
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_QT)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${PROJECT_NAME} PRIVATE REPORT_TARGET)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

#target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/../bin)
#link_directories(AFTER ${CMAKE_BINARY_DIR}/../bin)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Widgets sf-misc)

# Sets the extension of the generated binary.
_SetBinarySuffix("${PROJECT_NAME}")

