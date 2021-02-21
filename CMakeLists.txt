# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.14)

# Set the global project name.
project("qt-concepts"
	VERSION 0.1.0
	HOMEPAGE_URL "https://git.scanframe.com/shared/qt-concepts"
	)

# Make sure builds do not wind up in the source directory.
list(APPEND CMAKE_PREFIX_PATH "com/cmake")
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
_SetOutputDirs()

# Add Sub Projects in the right order of dependency.
add_subdirectory(com)
add_subdirectory(rt-shared-lib)
add_subdirectory(custom-ui-plugin)
add_subdirectory(rt-ui-serialize)
add_subdirectory(script-engine)
add_subdirectory(multi-docs)
add_subdirectory(unit-test)
