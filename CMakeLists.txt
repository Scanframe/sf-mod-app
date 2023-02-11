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

# Add top target for displaying info on the compiled target where Sf_AddExifTarget() is called on.
add_custom_target("exif" ALL)

# Enables including Qt libraries when building (libraries).
set(SF_BUILD_QT ON)
#set(SF_BUILD_QT OFF)

# Use faster linker for Windows maybe?
if (WIN32)
	# Adding option '-mwindows' as a linker flag will remove the console.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=bfd")
endif ()

# Make sure builds do not wind up in the source directory.
find_package(SfMacros CONFIG REQUIRED)
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
Sf_SetOutputDirs()

if (FALSE)
	# For testing only.
	add_subdirectory(com/test)
	#add_subdirectory(manual)
else ()
	# Add Sub Projects in the right order of dependency.
	add_subdirectory(com)
	add_subdirectory(app)
	if (NOT WIN32)
		add_subdirectory(doc)
	endif ()
	# Concepts only when test flag is set.
	if (SF_BUILD_TESTING)
		add_subdirectory(concept)
	endif ()
endif ()
