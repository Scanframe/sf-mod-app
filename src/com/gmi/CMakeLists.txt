# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the global project name.
project("sf-gmi")

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
Sf_SetOutputDirs("bin")

# Add Sub Projects in the right order of dependency.
add_subdirectory(iface)
add_subdirectory(emulator)

# Testing only available if this is the main app
if (SF_BUILD_TESTING)
	add_subdirectory(tests)
	Sf_GetSubDirectories(_TestApps "${CMAKE_CURRENT_LIST_DIR}/tests/app/gen" ".+")
	foreach (_TestApp ${_TestApps})
		add_subdirectory("tests/app/gen/${_TestApp}")
	endforeach ()
	if (SF_BUILD_QT AND SF_BUILD_GUI_TESTING)
		Sf_GetSubDirectories(_TestApps "${CMAKE_CURRENT_LIST_DIR}/tests/app/qt/gui" ".+")
		foreach (_TestApp ${_TestApps})
			add_subdirectory("tests/app/qt/gui/${_TestApp}")
		endforeach ()
	endif ()
endif ()
