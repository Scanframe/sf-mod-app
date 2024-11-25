cmake_minimum_required(VERSION 3.23)
project(wine-exec)

set(CMAKE_CXX_STANDARD 20)

Sf_AddExecutable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	exec-wine.cpp
)
