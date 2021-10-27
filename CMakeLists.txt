# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)

# Check if this is a cross compile for windows.
if (SF_CROSS_WINDOWS)
	set(CMAKE_TOOLCHAIN_FILE com/cmake/SfWindowsToolChain.cmake)
endif ()

# Set the global project name.
project("qt-concepts"
	VERSION 0.1.0
	DESCRIPTION "Scanframe Modular Application"
	HOMEPAGE_URL "https://git.scanframe.com/shared/qt-concepts"
	)

# Enable test builds from the root file by default for now.
# So the test targets are listed and can be executed.
if (WIN32)
	# Disabled for Windows for now since MingW linker takes minutes to link Catch2 archive.
	set(SF_BUILD_TESTING OFF)
else ()
	set(SF_BUILD_TESTING ON)
endif ()

# Enables including Qt libraries when building (libraries).
set(SF_BUILD_QT ON)
#set(SF_BUILD_QT OFF)

# Use faster linker for Windows maybe?
if (WIN32)
	# Adding option '-mwindows' as a linker flag will remove the console.
	set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=bfd")
endif (WIN32)

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/com/cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
_SetOutputDirs()

# Add Sub Projects in the right order of dependency.
add_subdirectory(com)
add_subdirectory(app)
add_subdirectory(manual)

add_subdirectory(concept/custom-ui-plugin)
add_subdirectory(concept/treemodel)

