# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the global project name.
project("qt-concepts" VERSION 0.1.0 HOMEPAGE_URL "https://git.scanframe.com/shared/qt-concepts")

# Set output directories only when the current cmake project is also the main project.
if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
	message(STATUS "Output Dir (${PROJECT_NAME}): 'file://${CMAKE_CURRENT_LIST_DIR}/bin'")
	# Set the directories relative to this included file.
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin")
endif ()

# Add Sub Projects in the right order of dependency.
add_subdirectory(com)
add_subdirectory(rt-shared-lib)
add_subdirectory(custom-ui-plugin)
add_subdirectory(rt-ui-serialize)
add_subdirectory(script-engine)

# Add this for the headers in the autogen directory made by the *.ui files.
set(CMAKE_INCLUDE_CURRENT_DIR ON)
