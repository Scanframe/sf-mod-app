# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the global project name.
project("sf-pal")

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
Sf_SetOutputDirs("bin")

# Add Sub Projects in the right order of dependency.
add_subdirectory(iface)
add_subdirectory(simple)
add_subdirectory(amplitude)

# Testing only available if this is the main app
if (SF_BUILD_TESTING AND SF_BUILD_GUI_TESTING)
	add_subdirectory(tests/app)
endif()
