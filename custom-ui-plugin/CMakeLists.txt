# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the project name.
project("custom-ui-plugin" LANGUAGES CXX)

# Include common configuration.
include("config.cmake")

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Designer REQUIRED)
find_package(Qt5UiPlugin REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_library(${PROJECT_NAME} SHARED
	src/SfAnalogClock.cpp
	src/SfAnalogClock.h
	src/SfAnalogClockPlugin.cpp
	src/SfAnalogClockPlugin.h
	src/SfPushButton.cpp
	src/SfPushButton.h
	src/SfPushButtonPlugin.cpp
	src/SfPushButtonPlugin.h
	src/SfCustomWidgets.cpp src/SfCustomWidgets.h)

# Include library configuration.
include("libraries.cmake")

# Link the Qt widgets library.
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::UiPlugin)

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_QT)

# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${PROJECT_NAME} PRIVATE QDESIGNER_EXPORT_WIDGETS)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${PROJECT_NAME} PRIVATE REPORT_TARGET)

# Tell the code the library is used as an archive. (So no imports or exports)
#target_compile_definitions(${PROJECT_NAME} PRIVATE _IMPL_A_ARC)

# Tell the code the library is used as dynamic library. (So imports needed)
target_compile_definitions(${PROJECT_NAME} PRIVATE _IMPL_A_PKG)

if (WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME} SUFFIX ".dll")
else ()
	set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME} SUFFIX ".so")
endif ()

# Install designer plugin in plugins directory of current compiler.
install(TARGETS ${PROJECT_NAME} DESTINATION ${QT_PLUGINS_DIR}/designer)
