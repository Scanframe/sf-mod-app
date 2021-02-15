# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the project name.
project("as-app" LANGUAGES CXX)

# Include common configuration.
include("config.cmake")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets Designer Qml REQUIRED)
find_package(Angelscript COMPONENTS angelscript REQUIRED)
find_package(Threads COMPONENTS Threads REQUIRED)

set(SOURCES
	src/main.cpp
	src/mainwindow.h
	src/mainwindow.cpp
	src/mainwindow.ui
	src/tutorial.cpp
	src/tutorial.h
	src/add_on/scriptarray.h
	src/add_on/scriptarray.cpp
	src/add_on/scriptstdstring.h
	src/add_on/scriptstdstring.cpp
	src/add_on/scriptstdstring_utils.cpp
	)

# Add this for the headers in the autogen directory made by the *.ui files.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Add resource to the application.
qt5_add_resources(SOURCES resource/resource.qrc)
# Set the actual target.
add_executable(${PROJECT_NAME} ${SOURCES})

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_QT)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${PROJECT_NAME} PRIVATE REPORT_TARGET)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

#target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/../bin)
#link_directories(AFTER ${CMAKE_BINARY_DIR}/../bin)

# Include library configuration.
include("libraries.cmake")

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Designer Qt${QT_VERSION_MAJOR}::Qml sf-misc Angelscript::angelscript)


if (WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}" SUFFIX ".exe")
else ()
	set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}" SUFFIX ".bin")
endif ()
