# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the global project name.
project("script-engine")

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "../../com/cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
Sf_SetOutputDirs()

# Add Sub Projects in the right order of dependency.
if (NOT WIN32)
	add_subdirectory(as)
endif()
add_subdirectory(qjs)
