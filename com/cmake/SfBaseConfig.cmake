# Checks if the required passed file exists.
# When not a useful fatal message is produced.
macro(Sf_CheckFileExists file)
	if (NOT EXISTS "${file}")
		message(FATAL_ERROR "The file \"${file}\" does not exist. Check order of dependent add_subdirectory(...).")
	endif ()
endmacro()

# Gets the version from the Git repository using 'PROJECT_SOURCE_DIR' variable.
# When not found it returns "${_VarOut}-NOTFOUND"
#
function(Sf_GetGitTagVersion _VarOut _SrcDir)
	# Initialize return value.
	set(${_VarOut} "${_VarOut}-NOTFOUND" PARENT_SCOPE)
	# Get git binary location for execution.
	find_program(_GitExe "git")
	if (_Compiler STREQUAL "_GitExe-NOTFOUND")
		message(FATAL_ERROR "Git program not found!")
	endif ()
	execute_process(COMMAND "${_GitExe}" describe --tags --dirty --match "v*"
		# Use the current project directory to find.
		WORKING_DIRECTORY "${_SrcDir}"
		OUTPUT_VARIABLE _Version
		RESULT_VARIABLE _ExitCode
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	# Check the exist code for an error.
	if (_ExitCode GREATER "0")
		message(FATAL_ERROR "Git returned an error on getting the version tag!")
	else ()
		set(_RegEx "^v([0-9]*\\.[0-9]*\\.[0-9]*)")
		string(REGEX MATCH "${_RegEx}" _Dummy_ "${_Version}")
		if ("${CMAKE_MATCH_1}" STREQUAL "")
			message(FATAL_ERROR "Git returned tag does not match regex '${_RegEx}' !")
		else ()
			set(${_VarOut} "${CMAKE_MATCH_1}" PARENT_SCOPE)
		endif ()
	endif ()
endfunction()

# Sets the passed target version properties according the version of the sub project
# or by default use the tag in vN.N.N format
#
function(Sf_SetTargetVersion _Target)
	# Get version. from Git when possible.
	Sf_GetGitTagVersion(_Version "${CMAKE_CURRENT_BINARY_DIR}")
	# Check if the git version was found.
	if (NOT "${_Version}" STREQUAL "_Version-NOTFOUND")
		message("${CMAKE_CURRENT_FUNCTION}(${_Target}) using Git version (${_Version})")
	# Check the target version is set.
	elseif (NOT "${CMAKE_PROJECT_VERSION}" STREQUAL "")
		set(_Version "${${_Target}_VERSION}")
		message("${CMAKE_CURRENT_FUNCTION}(${_Target}) using Sub-Project(${_Target}) version (${_Version})")
	# Check the project version is set.
	elseif (NOT "${CMAKE_PROJECT_VERSION}" STREQUAL "")
		# Try using the main project version
		set(_Version "${CMAKE_PROJECT_VERSION}")
		message("${CMAKE_CURRENT_FUNCTION}(${_Target}) using Main-Project version (${_Version})")
	endif ()
	# Set the target version properties.
	set_target_properties(${PROJECT_NAME} PROPERTIES VERSION "${_Version}" SOVERSION "${_Version}")
endfunction()

# Adds a dynamic library target and sets the version number on it as well.
macro(Sf_AddSharedLibrary TargetName)
	# Add the library to create.
	add_library(${PROJECT_NAME} SHARED)
	# Set the version of this target.
	Sf_SetTargetVersion(${PROJECT_NAME})
endmacro()

# FetchContent_MakeAvailable was not added until CMake 3.14; use our shim
#
if (${CMAKE_VERSION} VERSION_LESS 3.14)
	macro(FetchContent_MakeAvailable NAME)
		FetchContent_GetProperties(${NAME})
		if (NOT ${NAME}_POPULATED)
			FetchContent_Populate(${NAME})
			add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
		endif ()
	endmacro()
endif ()

