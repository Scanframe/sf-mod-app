# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the project name.
project("sf-misc" LANGUAGES CXX)
# Include common configuration.
include("cmake/SfCommonInc.cmake")

# Finde the needed packages.
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

include_directories(AFTER "./../../" "./../")

add_library(${CMAKE_PROJECT_NAME} SHARED
	misc/global.h
	misc/target.h
	misc/dbgutils.h
	misc/dbgutils.cpp
	misc/genutils.h
	misc/genutils.cpp
	misc/exception.h
	misc/exception.cpp
	)

# Link the Qt widgets library.
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE TARGET_QT)

# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE TARGET_DYNAMIC_LIB)

# Tell the code the 'misc' library is used as an archive. (So no imports or exports)
#target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE _MISC_ARC)

# Tell the code the 'misc' library is used as dynamic library. (So imports needed)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE _MISC_PKG)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE REPORT_TARGET)

if(WIN32)
	set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME} SUFFIX ".dll")
else()
	set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME} SUFFIX ".so")
endif()
