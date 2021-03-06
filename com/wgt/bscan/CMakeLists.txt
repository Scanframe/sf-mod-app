# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the project name.
project("sf-wgt-bscan" LANGUAGES CXX)

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "../../cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)
find_package(SfMisc CONFIG REQUIRED)
find_package(SfGii CONFIG REQUIRED)
find_package(SfPal CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
_SetOutputDirs()

# Uses directory Set by _SetOutputDirs().
find_package(SfMisc REQUIRED)
find_package(SfAmi REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find the needed packages.
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Designer UiTools REQUIRED)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} PRIVATE
	#res/resource.qrc
	GiiCustomWidgets.cpp GiiCustomWidgets.h
	BscanGraph.cpp BscanGraph.h
	BscanGraphPrivate.cpp BscanGraphPrivate.h
	BscanPropertyPage.cpp BscanPropertyPage.h BscanPropertyPage.ui
	BscanGraphPlugin.cpp BscanGraphPlugin.h
)

# Link the Qt widgets library.
target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt::Widgets
	Qt::Designer
	sf-misc
	sf-gii
	sf-pal
	)
	
# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${PROJECT_NAME} PRIVATE QDESIGNER_EXPORT_WIDGETS)
