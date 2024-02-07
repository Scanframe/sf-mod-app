# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" "${CMAKE_CURRENT_LIST_DIR}/cmake/lib")

# Package needed for Sf_GetGitTagVersion().
find_package(SfBase CONFIG REQUIRED)
# Package needed for adding function Sf_SetToolChain().
find_package(SfToolChain CONFIG REQUIRED)
# Install tool chain for Linux or Windows.
Sf_SetToolChain()

# Get the Git version number from the repository of this files directory.
Sf_GetGitTagVersion(SF_VERSION "${CMAKE_CURRENT_LIST_DIR}")
# Set the global project name.
project("qt-concepts"
	VERSION "${SF_VERSION}"
	DESCRIPTION "Scanframe Modular Application"
	HOMEPAGE_URL "https://git.scanframe.com/shared/qt-concepts"
	)

# Add top target for displaying info on the compiled targets where Sf_AddExifTarget() is called on.
add_custom_target("exif" ALL)

# Enables including Qt libraries when building (libraries) and needs to be defined.
if (NOT DEFINED SF_BUILD_QT)
	message(FATAL_ERROR "Variable 'SF_BUILD_QT' is not defined!")
	#set(SF_BUILD_QT ON)
endif ()

# Use faster linker for Windows maybe?
if (WIN32)
	# Adding option '-mwindows' as a linker flag will remove the console.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=bfd")
	# Prevent error when configuring for cross compile for Windows in Linux.
	if ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
		# Make some cmake files happy so they do report "Not found".
		set(Vulkan_INCLUDE_DIR "/tmp")
	endif()
endif ()

# Make sure builds do not wind up in the source directory.
find_package(SfMacros CONFIG REQUIRED)
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
Sf_SetOutputDirs("bin")

if (SF_BUILD_TESTING)
	# Prevents Catch2 from adding targets.
	set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
	# Enable the tests added with add_test.
	enable_testing()
	# Include CDash dashboard testing module
	include(CTest)
endif ()

# Configure the rpath.
if (SF_BUILD_QT STREQUAL "ON")
	# When the Qt directory is available append it.
	Sf_GetQtVersionDirectory(_QtVer)
	if (NOT _QtVer STREQUAL "")
		Sf_SetRPath("\${ORIGIN}:\${ORIGIN}/lib:${_QtVer}/gcc_64/lib")
	endif ()
else ()
	Sf_SetRPath("\${ORIGIN}:\${ORIGIN}/lib")
endif ()

# Add Sub Projects in the right order of dependency.
add_subdirectory(com)
add_subdirectory(app)
if (NOT WIN32)
	add_subdirectory(doc)
endif ()
# Concepts only when test flag is set.
if (SF_BUILD_CONCEPTS)
	add_subdirectory(concept)
endif ()
