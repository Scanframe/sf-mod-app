# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the project name.
project("sf-unwind" LANGUAGES CXX)

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)
# Requires Linux package 'pkg-config' to be installed.
find_package(PkgConfig REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
Sf_SetOutputDirs("bin")

# Uses directory Set by Sf_SetOutputDirs().
#find_package(SfMisc REQUIRED)

# Check if the unwindw package exists.
pkg_check_modules(UNWIND REQUIRED libunwind)

# Generic sources only STL dependent.
set(SOURCES
	global.h
	LibMain.cpp
	Backtrace.h Backtrace.cpp
)

# Add shared library and also sets the dynamic library version.
Sf_AddSharedLibrary("${PROJECT_NAME}")

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${UNWIND_LIBRARIES})

target_include_directories(${PROJECT_NAME} PRIVATE ${UNWIND_INCLUDE_DIRS})

# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_DYNAMIC_LIB)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${PROJECT_NAME} PRIVATE REPORT_TARGET)

# Tell the code the library is used as an archive. (So no imports or exports)
#target_compile_definitions(${PROJECT_NAME} PRIVATE _UNW_ARC)

# Tell the code the library is used as dynamic library. (So imports needed)
target_compile_definitions(${PROJECT_NAME} PRIVATE _UNW_PKG)

# Set the include directory for this library when it is imported by another sub project.
target_include_directories("${PROJECT_NAME}"
	INTERFACE "${CMAKE_CURRENT_LIST_DIR}/.."
	PRIVATE "${CMAKE_CURRENT_LIST_DIR}/.."
)

if (SF_BUILD_TESTING)
	add_subdirectory(tests)
endif ()