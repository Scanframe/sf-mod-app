# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the global project name.
project("sf-com")
# Add Sub Projects in the right order of dependency.
add_subdirectory(misc)
add_subdirectory(gii)

# Testing only available if this is the main app
#if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)	AND BUILD_TESTING)
if (BUILD_TESTING)
	add_subdirectory(tests)
endif()