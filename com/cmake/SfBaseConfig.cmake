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

# Checks if the required passed file exists.
# When not a useful fatal message is produced.
#
macro(Sf_CheckFileExists file)
	if (NOT EXISTS "${file}")
		message(FATAL_ERROR "The file \"${file}\" does not exist. Check order of dependent add_subdirectory(...).")
	endif ()
endmacro()

# Works around for Catch2 which does not allow us to set the compiler switch (-fvisibility=hidden)
#
function(Sf_SetTargetDefaultCompileOptions _Target)
	#message(STATUS "Setting target '${_Target}' compiler option -fvisibility=hidden")
	target_compile_options("${_Target}" PRIVATE "-fvisibility=hidden")
endfunction()

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
		message(VERBOSE "${CMAKE_CURRENT_FUNCTION}(${_Target}) using Git version (${_Version})")
		# Check the target version is set.
	elseif (NOT "${CMAKE_PROJECT_VERSION}" STREQUAL "")
		set(_Version "${${_Target}_VERSION}")
		message(VERBOSE "${CMAKE_CURRENT_FUNCTION}(${_Target}) using Sub-Project(${_Target}) version (${_Version})")
		# Check the project version is set.
	elseif (NOT "${CMAKE_PROJECT_VERSION}" STREQUAL "")
		# Try using the main project version
		set(_Version "${CMAKE_PROJECT_VERSION}")
		message(VERBOSE "${CMAKE_CURRENT_FUNCTION}(${_Target}) using Main-Project version (${_Version})")
	else ()
		# Clear the version variable.
		set(_Version "")
	endif ()
	# When the version string was resolved apply the properties.
	if (NOT "${_Version}" STREQUAL "")
		# Set the target version properties.
		set_target_properties("${_Target}" PROPERTIES
			VERSION "${_Version}"
			SOVERSION "${_Version}"
			)
	endif ()
endfunction()

# Adds an executable application target and also sets the default compile options.
#
macro(Sf_AddExecutable _Target)
	# Add the executable.
	add_executable("${_Target}")
	# Set the default compiler options for our own code only.
	Sf_SetTargetDefaultCompileOptions("${_Target}")
endmacro()

# Adds a dynamic library target and sets the version number on it as well.
macro(Sf_AddSharedLibrary _Target)
	# Add the library to create.
	add_library("${_Target}" SHARED)
	# Set the version of this target.
	Sf_SetTargetVersion("${_Target}")
	# Set the default compiler options for our own code only.
	Sf_SetTargetDefaultCompileOptions("${_Target}")
endmacro()

# Adds an exif custom target for reporting the resource stored versions.
#
macro(Sf_AddExifTarget _Target)
	# Add "exif-<target>" custom target when main 'exif' target exist.
	if (TARGET "exif")
		add_custom_target("exif-${_Target}" ALL
			#COMMAND echo "Exif Target: $<TARGET_NAME:${_Target}>"
			COMMAND exiftool "$<TARGET_FILE:${_Target}>" | egrep -i "^(File Name|Product Version|File Version)\\s*:"
			WORKING_DIRECTORY "$<TARGET_FILE_DIR:${_Target}>"
			DEPENDS "$<TARGET_FILE:${_Target}>"
			COMMENT "Reading resource information from '$<TARGET_DIR:${_Target}>'."
			VERBATIM
			)
		add_dependencies("exif" "exif-${_Target}")
	endif ()
endmacro()

# Add version resource.
#
macro(Sf_SetVersion _Target)
endmacro()

