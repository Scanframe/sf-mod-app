# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.27)

# Make it so our own packages are found and also the ones in the sub-module library.
# The full directory path using 'CMAKE_CURRENT_LIST_DIR' is only needed for building from Windows.
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" "${CMAKE_CURRENT_LIST_DIR}/cmake/lib")

# Package needed for Sf_GetGitTagVersion().
find_package(SfBase CONFIG REQUIRED)
# Package needed for adding function Sf_SetToolChain().
find_package(SfToolChain CONFIG REQUIRED)

# Get the Git versions from the repository of this files directory.
Sf_GetGitTagVersion(_Versions "${CMAKE_CURRENT_LIST_DIR}")
# Report the found Git tag found version.
Sf_ReportGitTagVersion("${_Versions}")
# Split the list into separate values.
list(GET _Versions 0 SF_GIT_TAG_VERSION)
list(GET _Versions 1 SF_GIT_TAG_RC)
list(GET _Versions 2 SF_GIT_TAG_COMMITS)

# Set the global project name.
project("qt-concepts"
	VERSION "${SF_GIT_TAG_VERSION}"
	DESCRIPTION "Scanframe Modular Application"
	HOMEPAGE_URL "https://git.scanframe.com/shared/qt-concepts"
)

# Check if the cmake is available and include it to maybe overrule CMakePresets.json cache variables.
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/user.cmake")
	include("${CMAKE_CURRENT_LIST_DIR}/user.cmake")
endif ()

# Add top target for displaying info on the compiled targets where Sf_AddExifTarget() is called on.
add_custom_target("exif" ALL)

# Use faster linker for Windows maybe?
if (WIN32)
	# Adding option '-mwindows' as a linker flag will remove the console.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=bfd")
	# Prevent error when configuring for cross compile for Windows in Linux.
	if ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
		# Make some cmake files happy so they do report "Not found".
		set(Vulkan_INCLUDE_DIR "/tmp")
	endif ()
endif ()

# Make sure builds do not wind up in the source directory.
find_package(SfMacros CONFIG REQUIRED)
find_package(SfBuildCheck CONFIG REQUIRED)
if (SF_BUILD_QT)
	# Install/fetch the Qt Libraries when ENV{QT_VER_DIR} is not set.
	find_package(SfQtLibrary 6.7.2 CONFIG REQUIRED)
endif ()

# Set the C++ standard to 17 for all projects to allow building with
# Ubuntu 22.04 and the Qt provided 'libstdc++-6.dll'.
# When building with Ubuntu 24.04 g++ version 13 it gets a bit complicated
# for running on Windows where 'libstdc++-6.dll' is needed from Linux MinGW.
set(CMAKE_CXX_STANDARD 17)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
Sf_SetOutputDirs("bin")

if (SF_BUILD_TESTING)
	# Prevents Catch2 from adding targets.
	set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
	# Enable the tests added with add_test.
	enable_testing()
	# Include CDash dashboard testing module and it sets the BUILD_TESTING to 'ON'.
	include(CTest)
endif ()

# Configure the rpath.
if (SF_BUILD_QT)
	# When the Qt directory is available append it.
	Sf_GetQtVersionDirectory(_QtVer)
	if (NOT _QtVer STREQUAL "")
		Sf_SetRPath("\${ORIGIN}:\${ORIGIN}/lib:${_QtVer}/gcc_64/lib")
	endif ()
else ()
	Sf_SetRPath("\${ORIGIN}:\${ORIGIN}/lib")
endif ()

# Satisfy cmake to prevent warning.
if (CMAKE_VERBOSE_MAKEFILE)
	message(STATUS "Verbosity enabled.")
endif ()

# Clear the tests from previous by passing an empty string.
Sf_AddAsCoverageTest("")

# Add Sub Projects in the right order of dependency.
add_subdirectory(src/com)
# The application is QT only so skip it.
if (SF_BUILD_QT)
	add_subdirectory(src/app)
endif ()

# Add Doxygen document project when .
if (NOT WIN32 AND NOT CMAKE_BUILD_TYPE STREQUAL "Coverage")
	add_subdirectory(doc)
endif ()

# Concepts only when test flag is set.
if (SF_BUILD_CONCEPTS)
	add_subdirectory(concept)
endif ()

# Coverage report generator in the form af a test is added.
# Only when testing is enabled and the build type is 'Coverage'.
# This must be the last test added since it relies on previous the calls
# to 'Sf_AddAsCoverageTest()'.
list(APPEND _CoverageDirs
	"src/com/misc/gen"
	"src/com/misc/qt"
	"src/com/misc/lnx"
	"src/com/gii/gen"
	"src/com/gii/qt"
	"src/com/math"
	"src/com/gmi/iface"
)
Sf_AddTestCoverageReport("coverage-report" "${CMAKE_CURRENT_LIST_DIR}/bin/gcov" "--cleanup --html nested" "${_CoverageDirs}")

# Add package build config when not building coverage.
if (NOT CMAKE_BUILD_TYPE STREQUAL "Coverage")
	if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/cmake/cpack/CPackConfig.cmake")
		include(cmake/cpack/CPackConfig.cmake)
	endif ()
endif ()


