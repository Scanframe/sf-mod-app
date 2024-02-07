# Generated from taskmenuextension.pro.

cmake_minimum_required(VERSION 3.16)
project(task-menu LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Designer)

# Add shared library and also sets the dynamic library version.
Sf_AddSharedLibrary("${PROJECT_NAME}")

target_link_libraries(${PROJECT_NAME} PUBLIC Qt::Core Qt::Designer Qt::Gui Qt::Widgets)

target_sources(${PROJECT_NAME} PRIVATE
	TicTacToe.cpp TicTacToe.h
	TicTacToeDialog.cpp TicTacToeDialog.h
	TicTacToePlugin.cpp TicTacToePlugin.h
	TicTacToeTaskMenu.cpp TicTacToeTaskMenu.h
	)


