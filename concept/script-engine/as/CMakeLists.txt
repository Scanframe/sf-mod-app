# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the project name.
project("as-app" LANGUAGES CXX)

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "../../../cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Add path to angel script
list(APPEND CMAKE_PREFIX_PATH "../../../../script-engines/angelscript_2.35.0/sdk/lib/cmake/Angelscript")
#message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
Sf_SetOutputDirs("bin")

# Uses directory Set by Sf_SetOutputDirs().
find_package(SfMisc REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find the needed packages.
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Qml REQUIRED)
find_package(Threads COMPONENTS Threads REQUIRED)
find_package(Angelscript COMPONENTS angelscript REQUIRED)

set(SOURCES
	main.cpp
	MainWindow.h
	MainWindow.cpp
	MainWindow.ui
	tutorial.cpp
	tutorial.h
	add_on/scriptarray.h
	add_on/scriptarray.cpp
	add_on/scriptstdstring.h
	add_on/scriptstdstring.cpp
	add_on/scriptstdstring_utils.cpp
	res/resource.qrc
	)

# Add this for the headers in the autogen directory made by the *.ui files.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set the actual executable target.
add_executable(${PROJECT_NAME})

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

target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Widgets Qt::Qml sf-misc Angelscript::angelscript)

# Sets the extension of the generated binary.
Sf_SetTargetSuffix("${PROJECT_NAME}")

