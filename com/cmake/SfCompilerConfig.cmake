# Gets the Qt directory given by the bash script 'QtLibDir.sh'
#
function(_GetQtLibDir VarOut)
	execute_process(COMMAND "bash" "${SfMacros_DIR}/QtLibDir.sh" OUTPUT_VARIABLE Result)
	set(${VarOut} ${Result} PARENT_SCOPE)
endfunction()

# Set the Qt Library location variable.
if (NOT DEFINED QT_DIRECTORY)
	_GetQtLibDir(QT_DIRECTORY)
	message(STATUS "Qt Directory: ${QT_DIRECTORY}")
	# When changing this CMAKE_PREFIX_PATH remove the 'cmake-build-xxxx' directory
	# since it weirdly keeps the previous selected CMAKE_PREFIX_PATH
	# Variable 'QT_DIRECTORY' is set by 'SfCompiler'
	list(PREPEND CMAKE_PREFIX_PATH "${QT_DIRECTORY}/gcc_64")
endif ()

# Set the Qt directory variable.
if (NOT DEFINED QT_PLUGINS_DIR)
	if (NOT QT_DIRECTORY STREQUAL "")
		set(QT_PLUGINS_DIR "${QT_DIRECTORY}/gcc_64/plugins")
	endif ()
	message(STATUS "Designer Plugins Dir: '${QT_PLUGINS_DIR}'")
endif ()

if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
	# Do not export all by default.
	add_definitions("-fvisibility=hidden")
	# Generate an error on undefined (imported) symbols on dyn libs
	# because the error appears only at load-time.
	add_link_options(-Wl,--no-undefined -Wl,--no-allow-shlib-undefined)
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	# When GNU compiler is used set some options.
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
		if (WIN32)
			# Needed for Windows since Catch2 is creating a huge obj file.
			add_compile_options(-m64 -Wa,-mbig-obj)
		endif ()
	endif ()
endif ()
