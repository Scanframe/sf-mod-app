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

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(LIBRARIES "")
# Generic sources only STL dependent.
set(SOURCES
	global.h
	gen/Value.h
	gen/Value.cpp
	gen/ScriptCalc.h
	gen/ScriptCalc.cpp
	gen/TDynamicBuffer.h
	gen/IniProfile.h
	gen/IniProfile.cpp
	gen/DynamicLibraryInfo.h
	gen/DynamicLibraryInfo.cpp
	gen/TClassRegistration.h
	gen/TFifoClass.h
	gen/target.h
	gen/dbgutils.h
	gen/dbgutils.cpp
	gen/LineBuffer.h
	gen/LineBuffer.cpp
	gen/genutils.h
	gen/genutils.cpp
	gen/Exception.h
	gen/Exception.cpp
	gen/Range.h
	gen/Range.cpp
	gen/csf.h
	gen/csf.cpp
	)

# When SF_LINK_QT is Set link the Qt libraries.
if (SF_BUILD_QT)

	# Find the needed packages.
	find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
	find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Designer REQUIRED)

	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)

	# Qt dependent sources.
	set(SOURCES_QT
	qt/qt_utils.h
	qt/qt_utils.cpp
	qt/capturelistmodel.h
	qt/capturelistmodel.cpp
	qt/resource.h
	qt/resource.cpp
	qt/formwriter.cpp
	qt/formwriter.h
	qt/objectextension.cpp
	qt/objectextension.h
	qt/res/resource.qrc
	)

	list(APPEND SOURCES ${SOURCES_QT})
	list(APPEND LIBRARIES Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Designer)

endif(SF_BUILD_QT)

# Add the library to create.
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (SF_BUILD_QT)
	# Explicitly tell the compiled code the QT libraries are included.
	target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_QT)
endif(SF_BUILD_QT)

# Link the Qt widgets library.
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})

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
