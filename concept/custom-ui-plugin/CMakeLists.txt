# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the project name.
project("custom-ui-plugin" LANGUAGES CXX)

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

# Find the needed packages.
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets UiPlugin REQUIRED)

add_library(${PROJECT_NAME} SHARED
	namespace.h
	SfAnalogClock.cpp
	SfAnalogClock.h
	SfAnalogClockPlugin.cpp
	SfAnalogClockPlugin.h
	SfPushButton.cpp
	SfPushButton.h
	SfPushButtonPlugin.cpp
	SfPushButtonPlugin.h
	SfCustomWidgets.cpp
	SfCustomWidgets.h
)

# Link the Qt widgets library.
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::UiPlugin sf-misc)

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_QT)

# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${PROJECT_NAME} PRIVATE QDESIGNER_EXPORT_WIDGETS)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${PROJECT_NAME} PRIVATE REPORT_TARGET)

# Sets the extension of the generated binary.
#_SetBinarySuffix("${PROJECT_NAME}")

# Install designer plugin in plugins directory of current compiler.
install(TARGETS ${PROJECT_NAME} DESTINATION ${QT_PLUGINS_DIR}/designer)
