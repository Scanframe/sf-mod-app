cmake_minimum_required(VERSION 3.14)

project(rt-iface-lib LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core LinguistTools REQUIRED)

set(TS_FILES rt-iface-lib_en_US.ts)

add_library(rt-iface-lib SHARED
  rt-iface-lib_global.h
  rt-iface.cpp
  rt-iface.h
  ${TS_FILES}
)

target_link_libraries(rt-iface-lib PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(rt-iface-lib PRIVATE RTIFACELIB_LIBRARY)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
