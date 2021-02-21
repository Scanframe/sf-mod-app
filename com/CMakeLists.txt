# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the project name.
project("sf-misc" LANGUAGES CXX VERSION 0.1.0)

list(APPEND CMAKE_PREFIX_PATH "cmake")
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
	misc/global.h
	misc/dynamicbuffer.h
	misc/profile.h
	misc/profile.cpp
	misc/class_reg.h
	misc/class_reg.cpp
	misc/fifoclass.h
	misc/target.h
	misc/dbgutils.h
	misc/dbgutils.cpp
	misc/qt_utils.h
	misc/qt_utils.cpp
	misc/linebuffer.h
	misc/linebuffer.cpp
	misc/capturelistmodel.h
	misc/capturelistmodel.cpp
	misc/genutils.h
	misc/genutils.cpp
	misc/exception.h
	misc/exception.cpp
	misc/class_reg.cpp
	misc/resource.h
	misc/resource.cpp
	misc/formwriter.cpp
	misc/formwriter.h
	misc/objectextension.cpp
	misc/objectextension.h
	)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_add_resources(SOURCES misc/resource/resource.qrc)

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
