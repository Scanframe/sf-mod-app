# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the global project name.
project("ami")

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "../cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
Sf_SetOutputDirs()

# Add Sub Projects in the right order of dependency.
add_subdirectory(iface)
add_subdirectory(editor)
add_subdirectory(layout)
add_subdirectory(script)
add_subdirectory(units)
add_subdirectory(project)
add_subdirectory(service)
