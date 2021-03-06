# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the project name.
project("sf-misc" LANGUAGES CXX VERSION 0.1.0)

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "../cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
_SetOutputDirs()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find the needed packages.
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Designer REQUIRED)

set(SOURCES
	global.h
	value.h
	value.cpp
	arith.h
	arith.cpp
	dynamicbuffer.h
	profile.h
	profile.cpp
	class_reg.h
	class_reg.cpp
	fifoclass.h
	target.h
	dbgutils.h
	dbgutils.cpp
	qt_utils.h
	qt_utils.cpp
	linebuffer.h
	linebuffer.cpp
	capturelistmodel.h
	capturelistmodel.cpp
	genutils.h
	genutils.cpp
	exception.h
	exception.cpp
	class_reg.cpp
	resource.h
	resource.cpp
	formwriter.cpp
	formwriter.h
	objectextension.cpp
	objectextension.h
	range.h
	range.cpp
	csf.h
	csf.cpp
	resource/resource.qrc
	)

file(GLOB GII_SOURCES "gii/*.cpp" "gii/*.h")
list(APPEND SOURCES ${GII_SOURCES})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Link the Qt widgets library.
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Designer)

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_QT)

# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_DYNAMIC_LIB)

# Tell the code the 'sf-misc' library is used as an archive. (So no imports or exports)
#target_compile_definitions(${PROJECT_NAME} PRIVATE _MISC_ARC)

# Tell the code the 'misc' library is used as dynamic library. (So imports needed)
target_compile_definitions(${PROJECT_NAME} PRIVATE _MISC_PKG)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${PROJECT_NAME} PRIVATE REPORT_TARGET)

# Set the include directory for this library when it is imported by another sub project.
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/..")
