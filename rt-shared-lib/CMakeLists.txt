# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the global project name.
project("rt-shared-lib")

# Add this for the headers in the autogen directory made by the *.ui files.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Add Sub Projects in the right order of dependency.
add_subdirectory(iface)
add_subdirectory(app)
add_subdirectory(impl-a)
add_subdirectory(impl-b)
