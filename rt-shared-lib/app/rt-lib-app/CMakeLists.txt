cmake_minimum_required(VERSION 3.5)

# Make sure builds do not wind up in the source directory.
set(BuildCheck_DIR "../../../cmake-mods")
find_package(BuildCheck CONFIG REQUIRED)

# Set the project name.
project(rt-lib-app LANGUAGES CXX)

# Write a message.
message("CMAKE_PROJECT_NAME: ${CMAKE_PROJECT_NAME}")
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("CMAKE_LIBRARY_OUTPUT_DIRECTORY: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/archive)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/library)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/binairy)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(TS_FILES rt-lib-app_en_US.ts)
set(SOURCES
	main.cpp
	mainwindow.cpp
	mainwindow.h
	mainwindow.ui
)

qt5_add_resources(SOURCES main.qrc)

add_executable(rt-lib-app
	${SOURCES}
	${TS_FILES}
)

target_link_libraries(rt-lib-app PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})


