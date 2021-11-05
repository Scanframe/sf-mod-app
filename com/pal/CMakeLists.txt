# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the global project name.
project("pal")

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../com/cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
_SetOutputDirs()

# Add Sub Projects in the right order of dependency.
add_subdirectory(iface)
add_subdirectory(simple)
add_subdirectory(amplitude)

# Testing only available if this is the main app
#if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)	AND SF_BUILD_TESTING)
if (SF_BUILD_TESTING)
	add_subdirectory(tests/app)
endif(SF_BUILD_TESTING)
