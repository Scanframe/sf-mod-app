# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the global project name.
project("manual")
# Add doxygen project when doxygen was found
find_package(SfDoxyGen REQUIRED)


Sf_AddManual("${PROJECT_NAME}" "${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/../bin/man")
