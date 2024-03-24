# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)

# Set the global project name.
project("document")
# Add doxygen project when SfDoxygen was found.
# On Windows this is only possible when doxygen is installed in Cygwin.
find_package(SfDoxygen QUIET)
if (SfDoxygen_FOUND)
	# Get the markdown files in this project directory.
	file(GLOB _SourceList RELATIVE "${CMAKE_CURRENT_BINARY_DIR}" "*.md")
	# Get all the header files in the src/com module.
	file(GLOB_RECURSE _SourceListTmp RELATIVE "${CMAKE_CURRENT_BINARY_DIR}" "../src/com/*.h" "../src/com/*.md")
	# Remove unwanted header file(s) ending on 'Private.h'.
	list(FILTER _SourcesListTmp EXCLUDE REGEX ".*Private\\.h$")
	list(APPEND _SourceList ${_SourceListTmp})
	# Adds the actual manual target .
	Sf_AddDoxygenDocumentation("${PROJECT_NAME}" "${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/../bin/man" "${_SourceList}" "v1.2024.0")
endif ()
