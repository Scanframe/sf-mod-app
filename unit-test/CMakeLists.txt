# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)
# Set the global project name.
project("unit-test")

# Add Sub Projects in the right order of dependency.
add_subdirectory(qtest)
add_subdirectory(catch2)
