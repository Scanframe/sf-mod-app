# Gets the Qt directory given by the bash script 'QtLibDir.sh'
#
function(_GetQtLibDir VarOut)
	if (WIN32)
		set(_Command "C:\\cygwin64\\bin\\bash.exe")
		string(REPLACE "/" "\\" _Script "${SfMacros_DIR}/QtLibDir.sh")
	else ()
		set(_Command "bash")
		set(_Script "${SfMacros_DIR}/QtLibDir.sh")
	endif ()
	execute_process(COMMAND "${_Command}" "${_Script}" OUTPUT_VARIABLE _Result RESULT_VARIABLE _ExitCode)
	# Validate the exit code.
	if(_ExitCode GREATER "0")
		message(FATAL_ERROR "Failed execution of script: ${_Script}")
	endif()
	set(${VarOut} "${_Result}" PARENT_SCOPE)
endfunction()

#TODO: This QT stuff should probably be in its own cmake package file so it can be omitted in non Qt builds.
# Set the Qt Library location variable.
if (NOT DEFINED QT_DIRECTORY)
	_GetQtLibDir(QT_DIRECTORY)
	message(STATUS "Qt Directory: ${QT_DIRECTORY}")
	# When changing this CMAKE_PREFIX_PATH remove the 'cmake-build-xxxx' directory
	# since it weirdly keeps the previous selected CMAKE_PREFIX_PATH
	if (WIN32)
		list(PREPEND CMAKE_PREFIX_PATH "${QT_DIRECTORY}/mingw81_64")
	else ()	
		list(PREPEND CMAKE_PREFIX_PATH "${QT_DIRECTORY}/gcc_64")
	endif ()
endif ()

# Set the Qt directory variable.
if (NOT DEFINED QT_PLUGINS_DIR)
	if (NOT QT_DIRECTORY STREQUAL "")
		if (WIN32)
			set(QT_PLUGINS_DIR "${QT_DIRECTORY}/mingw81_64/plugins")
		else ()
			set(QT_PLUGINS_DIR "${QT_DIRECTORY}/gcc_64/plugins")
		endif ()
	endif ()
	message(STATUS "Designer Plugins Dir: ${QT_PLUGINS_DIR}")
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
	# Do not export all by default.
	add_definitions("-fvisibility=hidden")
	# Generate an error on undefined (imported) symbols on dyn libs
	# because the error appears only at load-time.
	add_link_options(-Wl,--no-undefined -Wl,--no-allow-shlib-undefined)
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	#set_property(TARGET "${PROJECT_NAME}" PROPERTY CXX_STANDARD 17)
	# When GNU compiler is used Set some options.
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
		if (WIN32)
			# Needed for Windows since Catch2 is creating a huge obj file.
			add_compile_options(-m64 -Wa,-mbig-obj)
		endif ()
	endif ()
endif ()
