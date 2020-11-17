cmake_minimum_required(VERSION 3.14)

project(rt-lib-impl-a LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core LinguistTools REQUIRED)

set(TS_FILES rt-lib-impl-a_en_US.ts)

add_library(rt-lib-impl-a SHARED
  rt-lib-impl-a_global.h
  rt-lib-impl-a.cpp
  rt-lib-impl-a.h
  ${TS_FILES}
)

target_link_libraries(rt-lib-impl-a PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(rt-lib-impl-a PRIVATE RTLIBIMPLA_LIBRARY)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
