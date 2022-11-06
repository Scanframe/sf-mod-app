cmake_minimum_required(VERSION 3.23)
project(wine-exec)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} exec-wine.cpp)

Sf_SetBinarySuffix("${PROJECT_NAME}")