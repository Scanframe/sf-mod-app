# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the global project name.
project("rt-shared-lib")

# Make sure builds do not wind up in the source directory.
list(APPEND CMAKE_PREFIX_PATH "../com/cmake")
find_package(SfMacros CONFIG REQUIRED)

# Need to call this here since this is the top project.
_SetOutputDirs()

# Add Sub Projects in the right order of dependency.
add_subdirectory(iface)
add_subdirectory(app)
add_subdirectory(impl-a)
add_subdirectory(impl-b)
