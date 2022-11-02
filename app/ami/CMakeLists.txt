# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the project name.
project("sf-ami-app" LANGUAGES CXX)

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../com/cmake")
# Make sure builds do not wind up in the source directory.
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
find_package(SfMisc REQUIRED)
find_package(SfAmi REQUIRED)
find_package(SingleInstance REQUIRED)

# Set the actual executable target.
add_executable(${PROJECT_NAME})
# Set the default compiler options for our own code only.
_SetTargetDefaultCompileOptions()

# RC-files only seem to be compiled when building using MingW.
target_sources(${PROJECT_NAME} PRIVATE
	main.cpp
	Application.cpp Application.h
	MainWindow.cpp MainWindow.h
	res/resource.qrc
	res/windows.rc
	ApplicationPropertyPage.cpp ApplicationPropertyPage.h ApplicationPropertyPage.ui
	MainWindowPropertyPage.cpp MainWindowPropertyPage.h MainWindowPropertyPage.ui
	)

# Add this for the headers in the autogen directory made by the *.ui files.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_QT)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${PROJECT_NAME} PRIVATE REPORT_TARGET)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

#target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/../bin)
#link_directories(AFTER ${CMAKE_BINARY_DIR}/../bin)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Widgets sf-misc sf-ami SingleInstance)

# Sets the extension of the generated binary.
_SetBinarySuffix("${PROJECT_NAME}")

