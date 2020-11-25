cmake_minimum_required(VERSION 3.14)
# Set the project name.
project(rt-iface LANGUAGES CXX)

if (WIN32)
	message("Platform: Windows")
else ()
	message("Platform: Linux ${CMAKE_SYSTEM_VERSION}")
endif ()

# Make sure builds do not wind up in the source directory.
set(BuildCheck_DIR "../../cmake-mods")
find_package(BuildCheck CONFIG REQUIRED)

# Do not export all by default.
add_definitions("-fvisibility=hidden")

# Explicitly tell the compiled code the QT libraries are included.
add_definitions(-DTARGET_QT)
# Tell the compiled code a dynamic library (DL) is being build.
add_definitions(-DTARGET_DYNAMIC_LIB)
# Tell the code the 'misc' library is used as an archive. (So no imports or exports)
add_definitions(-D_MISC_ARC)
# Tell the code the 'misc' library is used as dynamic library. (So imports needed)
#add_definitions(-D_MISC_PKG)
# The 'DDEBUG_LEVEL' controls the debug output macro's.
add_definitions(-DDEBUG_LEVEL=1)
# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#add_definitions(-DREPORT_TARGET)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../bin)

# Write a message.
message("Project name: ${CMAKE_PROJECT_NAME}")
message("Run-Time Directory: ${CMAKE_BINARY_DIR}")
message("Library Out Directory: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message("Archive Out Directory: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core LinguistTools REQUIRED)

#set(TS_FILES rt-iface_en_US.ts)

add_library(rt-iface SHARED
	com/misc/defs.h
	com/misc/target.h
	com/misc/dbgutils.h
	com/misc/dbgutils.cpp
	src/rt-iface_global.h
	src/rt-iface.h
	src/rt-iface.cpp
	${TS_FILES}
	)

target_link_libraries(rt-iface PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if (WIN32)
	set_target_properties(rt-iface PROPERTIES OUTPUT_NAME "rt-iface" SUFFIX ".dll")
else ()
	set_target_properties(rt-iface PROPERTIES OUTPUT_NAME "rt-iface" SUFFIX ".so")
endif ()

target_compile_definitions(rt-iface PRIVATE RTIFACELIB_LIBRARY)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
