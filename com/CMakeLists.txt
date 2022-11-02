# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the global project name.
project("sf-com")
# Add Sub Projects in the right order of dependency.
add_subdirectory(test)
add_subdirectory(misc)
add_subdirectory(gsc)
add_subdirectory(gii)
add_subdirectory(rsa)
add_subdirectory(pal)
add_subdirectory(sto)
add_subdirectory(ipj)
add_subdirectory(ami)
add_subdirectory(wgt)
