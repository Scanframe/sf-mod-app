# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the global project name.
project("rt-shared-lib")

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "../../cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Need to call this here since this is the top project.
Sf_SetOutputDirs("bin")

# Add Sub Projects in the right order of dependency.
add_subdirectory(iface)
add_subdirectory(app)
add_subdirectory(impl-a)
add_subdirectory(impl-b)
