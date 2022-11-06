cmake_minimum_required(VERSION 3.23)
project(wine-exec)

# Add target.
Sf_AddExecutable(${PROJECT_NAME})
# Set the sources for this target.
target_sources(${PROJECT_NAME} PRIVATE exec-wine.cpp)
# SEt the suffix for the produced target binary.
Sf_SetBinarySuffix("${PROJECT_NAME}")

