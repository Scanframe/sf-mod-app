cmake_minimum_required(VERSION 3.5)

# Set the project name.
project(rt-lib-app LANGUAGES CXX)

# Make sure builds do not wind up in the source directory.
set(BuildCheck_DIR "../../../cmake-mods")
find_package(BuildCheck CONFIG REQUIRED)

add_definitions(-DDEBUG_LEVEL=1)
#add_definitions(-DREPORT_TARGET)
add_definitions(-DTARGET_QT)
#add_definitions(-D_TARGET_SHARED)

# Write a message.
message("CMAKE_PROJECT_NAME: ${CMAKE_PROJECT_NAME}")
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("CMAKE_LIBRARY_OUTPUT_DIRECTORY: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../bin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

#set(TS_FILES rt-lib-app_en_US.ts)

set(SOURCES
	com/misc/defs.h
	com/misc/target.h
	com/misc/dbgutils.cpp
	com/misc/dbgutils.h
	src/main.cpp
	src/messagelist.h
	src/messagelist.cpp
	src/mainwindow.cpp
	src/mainwindow.h
	src/mainwindow.ui
)

qt5_add_resources(SOURCES src/main.qrc)

add_executable(rt-lib-app
	${SOURCES}
	${TS_FILES}
)

target_link_libraries(rt-lib-app PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
set_target_properties(
	rt-lib-app
	PROPERTIES
	OUTPUT_NAME "rt-lib-app"
	SUFFIX ".bin"
)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
