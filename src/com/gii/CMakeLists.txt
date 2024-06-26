# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the project name.
project("sf-gii" LANGUAGES CXX)

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "../cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
Sf_SetOutputDirs("bin")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include Scanframe misc library.
set(LIBRARIES sf-misc)

# Generic sources.
set(SOURCES
	global.h
	gen/InformationBase.h
	gen/VariableTypes.h
	gen/VariableStatic.cpp gen/VariableStatic.h
	gen/Variable.cpp gen/Variable.h
	gen/VariableIdList.cpp gen/VariableIdList.h
	gen/VariableHandler.cpp gen/VariableHandler.h
	gen/VariableReference.cpp gen/VariableReference.h
	gen/VariableScriptObject.cpp gen/VariableScriptObject.h
	gen/GiiUtils.cpp gen/GiiUtils.h
	gen/UnitConversionServer.cpp gen/UnitConversionServer.h
	gen/UnitConversionServerEx.cpp gen/UnitConversionServerEx.h
	gen/UnitConversion.cpp gen/UnitConversion.h
	gen/FileMappedStorage.cpp gen/FileMappedStorage.h
	gen/ResultDataTypes.cpp gen/ResultDataTypes.h
	gen/ResultData.cpp gen/ResultData.h
	gen/ResultDataReference.cpp gen/ResultDataReference.h
	gen/ResultDataHandler.cpp gen/ResultDataHandler.h
	gen/ResultDataRequester.cpp gen/ResultDataRequester.h
	gen/ResultDataStatic.cpp gen/ResultDataStatic.h
	gen/GiiScriptInterpreter.cpp gen/GiiScriptInterpreter.h
	gen/ResultDataScriptObject.cpp gen/ResultDataScriptObject.h
	gen/InformationServer.cpp gen/InformationServer.h
	LibMain.cpp)

if (NOT WIN32)

set(SOURCES_LNX
	)
list(APPEND SOURCES ${SOURCES_LNX})

else (NOT WIN32)

set(SOURCES_WIN
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
	qt/res/icons.qrc
	qt/Namespace.h
	qt/Macros.h
	qt/VariableWidgetBase.cpp qt/VariableWidgetBase.h	qt/VariableWidgetBasePrivate.h
	qt/InformationSelectDialog.cpp qt/InformationSelectDialog.h qt/InformationSelectDialog.ui
	qt/VariableEdit.cpp qt/VariableEdit.h
	qt/InformationIdEdit.cpp qt/InformationIdEdit.h
	qt/InformationIdEditPlugin.cpp qt/InformationIdEditPlugin.h
	qt/VariableEdit.cpp qt/VariableEdit.h
	qt/VariableEditPlugin.cpp qt/VariableEditPlugin.h
	qt/VariableComboBox.cpp qt/VariableComboBox.h
	qt/VariableComboBoxPlugin.cpp qt/VariableComboBoxPlugin.h
	qt/VariableCheckBox.cpp qt/VariableCheckBox.h
	qt/VariableCheckBoxPlugin.cpp qt/VariableCheckBoxPlugin.h
	qt/VariableBar.cpp qt/VariableBar.h
	qt/VariableBarPlugin.cpp qt/VariableBarPlugin.h
	qt/InformationItemModel.cpp qt/InformationItemModel.h
	qt/LayoutData.cpp qt/LayoutData.h
	qt/GiiCustomWidgets.cpp qt/GiiCustomWidgets.h
	qt/IdPropertyList.cpp qt/IdPropertyList.h
	qt/InformationMonitor.cpp qt/InformationMonitor.h qt/InformationMonitor.ui
	qt/VariableIdPropertyPage.cpp qt/VariableIdPropertyPage.h qt/VariableIdPropertyPage.ui
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
	# Explicitly tell the compiled code the QT libraries are included.
	target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_QT)
endif (SF_BUILD_QT)

# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_DYNAMIC_LIB)

# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${PROJECT_NAME} PRIVATE QDESIGNER_EXPORT_WIDGETS)

# Tell the code the 'sf-gii' library is used as an archive. (So no imports or exports)
#target_compile_definitions(${PROJECT_NAME} PRIVATE _GII_ARC)

# Tell the code the 'gii' library is used as dynamic library. (So imports needed)
target_compile_definitions(${PROJECT_NAME} PRIVATE _GII_PKG)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${PROJECT_NAME} PRIVATE REPORT_TARGET)

# Set the include directory for this library when it is imported by another sub project.
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}")

# Testing only available if this is the main app
if (SF_BUILD_TESTING)
	add_subdirectory(tests)
	Sf_GetSubDirectories(_TestApps "${CMAKE_CURRENT_LIST_DIR}/tests/app/gen" ".+")
	foreach (_TestApp ${_TestApps})
		add_subdirectory("tests/app/gen/${_TestApp}")
	endforeach ()
	if (SF_BUILD_QT AND SF_BUILD_GUI_TESTING)
		Sf_GetSubDirectories(_TestApps "${CMAKE_CURRENT_LIST_DIR}/tests/app/qt/gui" ".+")
		foreach (_TestApp ${_TestApps})
			add_subdirectory("tests/app/qt/${_TestApp}")
		endforeach ()
	endif ()
endif ()
