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

# Include pthread library.
set(LIBRARIES pthread)

# Generic sources only STL dependent.
set(SOURCES
	global.h
	gen/target.h
	gen/Sustain.h gen/Sustain.cpp
	gen/IFileMapper.cpp gen/IFileMapper.h
	gen/gen_utils.h	gen/gen_utils.cpp
	gen/dbgutils.h gen/dbgutils.cpp
	gen/TimeSpec.h gen/TimeSpec.cpp
	gen/IntervalTimer.h gen/IntervalTimer.cpp
	gen/ElapseTimer.h gen/ElapseTimer.cpp
	gen/PerformanceTimer.h gen/PerformanceTimer.cpp
	gen/Value.h	gen/Value.cpp
	gen/ScriptObject.h gen/ScriptObject.cpp
	gen/ScriptEngine.h gen/ScriptEngine.cpp
	gen/ScriptInterpreter.cpp gen/ScriptInterpreter.h gen/ScriptInterpreter_p.h
	gen/TDynamicBuffer.h
	gen/TDynamicArray.h
	gen/IniProfile.h gen/IniProfile.cpp
	gen/DynamicLibraryInfo.h gen/DynamicLibraryInfo.cpp
	gen/TClassRegistration.h
	gen/LineBuffer.h gen/LineBuffer.cpp
	gen/Exception.h gen/Exception.cpp
	gen/Range.h	gen/Range.cpp
	gen/RangeManager.h gen/RangeManager.cpp
	gen/csf.h gen/csf.cpp
	gen/Mutex.h gen/Mutex.cpp
	gen/Thread.h gen/Thread.cpp
	gen/Condition.h gen/Condition.cpp
	gen/Sync.h
	gen/TVector.h
	gen/TListener.h
	gen/TDeque.h
	gen/TFifoClass.h
	gen/TStaticSync.h
	gen/TEnvelope.h
	gen/TPointer.h
	gen/TBitSet.h
	)

if (NOT WIN32)

find_package(OpenSSL REQUIRED)

set(SOURCES_LNX
	lnx/lnx_utils.h lnx/lnx_utils.cpp
	lnx/EventCounter.h lnx/EventCounter.cpp
	lnx/File.h lnx/File.cpp
	lnx/FileMapper.h lnx/FileMapper.cpp
	lnx/SegmentFaultHandler.h lnx/SegmentFaultHandler.cpp
	)

list(APPEND SOURCES ${SOURCES_LNX})
list(APPEND LIBRARIES OpenSSL::Crypto)

else (NOT WIN32)

set(SOURCES_WIN
	win/win_utils.cpp win/win_utils.h
	win/WinTimer.cpp win/WinTimer.h
	win/FileMapper.h win/FileMapper.cpp
	)
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
		qt/qt_utils.cpp qt/qt_utils.h
		qt/CaptureListModel.cpp qt/CaptureListModel.h
		qt/Resource.h	qt/Resource.cpp
		qt/FormBuilder.cpp qt/FormBuilder.h
		qt/ObjectExtension.cpp qt/ObjectExtension.h
		qt/FileMapper.cpp qt/FileMapper.h
		qt/MiscCustomWidgets.cpp qt/MiscCustomWidgets.h
		qt/DrawWidgetPlugin.cpp qt/DrawWidgetPlugin.h
		qt/Draw.cpp qt/Draw.h
		qt/FormDialog.cpp qt/FormDialog.h
		qt/ActionButton.cpp qt/ActionButton.h
		qt/ActionButtonPlugin.cpp qt/ActionButtonPlugin.h
		qt/DrawWidget.cpp qt/DrawWidget.h
		qt/ModuleConfiguration.cpp qt/ModuleConfiguration.h
		qt/ModuleConfigurationDialog.cpp qt/ModuleConfigurationDialog.h qt/ModuleConfigurationDialog.ui
		qt/PropertySheetDialog.cpp qt/PropertySheetDialog.h qt/PropertySheetDialog.ui
		qt/PropertyPage.cpp qt/PropertyPage.h
		qt/PropertyPagePlugin.cpp qt/PropertyPagePlugin.h
		qt/ScriptModelList.cpp qt/ScriptModelList.h
		qt/ScriptHighlighter.cpp qt/ScriptHighlighter.h
		qt/Editor.cpp qt/Editor.h
		qt/EditorPropertyPage.cpp qt/EditorPropertyPage.h qt/EditorPropertyPage.ui
		qt/EditorPlugin.cpp qt/EditorPlugin.h
		qt/Graph.cpp qt/Graph.h
		qt/res/resource.qrc
		qt/res/icons.qrc
		)

	list(APPEND SOURCES ${SOURCES_QT})
	list(APPEND LIBRARIES Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Designer)

endif (SF_BUILD_QT)

# Add the library to create.
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (SF_BUILD_QT)
	# Explicitly tell the compiled code the QT libraries are included.
	target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_QT)
endif (SF_BUILD_QT)

# Link the Qt widgets library.
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})

# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_DYNAMIC_LIB)

# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${PROJECT_NAME} PRIVATE QDESIGNER_EXPORT_WIDGETS)

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

# Testing only available if this is the main app
#if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)	AND SF_BUILD_TESTING)
if (SF_BUILD_TESTING)
	add_subdirectory(tests)
	add_subdirectory(tests/app/threads)
	add_subdirectory(tests/app/script)
endif(SF_BUILD_TESTING)

# Install designer plugin in plugins directory of current compiler.
install(TARGETS ${PROJECT_NAME} DESTINATION ${QT_PLUGINS_DIR}/designer)
