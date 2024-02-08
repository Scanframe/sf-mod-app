# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the project name.
project("rt-iface" LANGUAGES CXX)

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "../../../../cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
Sf_SetOutputDirs("bin")

# Uses directory Set by Sf_SetOutputDirs().
find_package(SfMisc REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find the needed packages.
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

# Add shared library and also sets the dynamic library version.
Sf_AddSharedLibrary("${PROJECT_NAME}")

target_sources("${PROJECT_NAME}" PRIVATE
	global.h
	RuntimeIface.h
	RuntimeIface.cpp
	)

# Link the Qt widgets library.
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Widgets sf-misc)

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_QT)

# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_DYNAMIC_LIB)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${PROJECT_NAME} PRIVATE REPORT_TARGET)

# Tell the code the library is used as an archive. (So no imports or exports)
#target_compile_definitions(${PROJECT_NAME} PRIVATE _IFACE_ARC)

# Tell the code the library is used as dynamic library. (So imports needed)
target_compile_definitions(${PROJECT_NAME} PRIVATE _IFACE_PKG)

# Set the include directory for this library when it is imported by another sub project.
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/")

