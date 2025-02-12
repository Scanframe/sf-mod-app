# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the project name.
project("sf-gmi-iface" LANGUAGES CXX)

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../../cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
Sf_SetOutputDirs("bin")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include misc library.
set(LIBRARIES sf-misc sf-gii sf-math)

# Generic sources.
set(SOURCES
	global.h
	GmiTypes.cpp GmiTypes.h
	AxesCoord.cpp AxesCoord.h
	GmiInterface.cpp GmiInterface.h
	Controller.cpp Controller.h
	GmiUtils.cpp GmiUtils.h
)

if (NOT WIN32)
	set(SOURCES_LNX)
	list(APPEND SOURCES ${SOURCES_LNX})
else (NOT WIN32)
	set(SOURCES_WIN)
	list(APPEND SOURCES ${SOURCES_WIN})
endif (NOT WIN32)

# When SF_BUILD_QT is Set link the Qt libraries.
if (SF_BUILD_QT)
	# Find the needed packages.
	find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
	find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Designer REQUIRED)

	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)

	# Qt dependent sources.
	set(SOURCES_QT
		qt/res/icons.qrc
		qt/Namespace.h
		qt/AxesCoordEdit.cpp qt/AxesCoordEdit.h
		qt/AxesCoordEditPlugin.cpp qt/AxesCoordEditPlugin.h
		qt/GmiCustomWidgets.cpp qt/GmiCustomWidgets.h
	)

	list(APPEND SOURCES ${SOURCES_QT})
	list(APPEND LIBRARIES Qt::Widgets Qt::Designer)

endif (SF_BUILD_QT)

# Generic sources.
list(APPEND SOURCES)

# Add shared library and also sets the dynamic library version.
Sf_AddSharedLibrary("${PROJECT_NAME}")

# Adds the compiler and linker options for coverage.
Sf_AddTargetForCoverage("${PROJECT_NAME}")

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

# Link the Qt widgets library.
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})

if (SF_BUILD_QT)
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		# Explicitly tell the compiled code the QT libraries are included.
		TARGET_QT
		# Tell the compiled code a dynamic library (DL) is being build.
		QDESIGNER_EXPORT_WIDGETS
		REPORT_TARGET
	)
endif (SF_BUILD_QT)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	# Tell the compiled code a dynamic library (DL) is being build.
	TARGET_DYNAMIC_LIB
	# Tell the code the 'sf-gmi' library is used as an archive. (So no imports or exports)
	#_GMI_ARC)
	# Tell the code the 'gmi' library is used as dynamic library. (So imports needed)
	_GMI_PKG
	# The 'DEBUG_LEVEL' controls the debug output macro's.
	_DEBUG_LEVEL=1
	# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
	#REPORT_TARGET
)

# Set the include directory for this library when it is imported by another sub project.
target_include_directories("${PROJECT_NAME}"
	INTERFACE "${CMAKE_CURRENT_LIST_DIR}/../.."
	PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../.."
)
