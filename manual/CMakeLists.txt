
# Add doxygen project when doxygen was found
find_package(Doxygen QUIET)
if (Doxygen_FOUND)
	message(STATUS "Doxygen ${DOXYGEN_VERSION} found and documentation is build.")
	#[[
		set(DOXYGEN_EXTRACT_ALL YES)
		Set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
		doxygen_add_docs(manual
			mainpage.md
			com/misc/fifoclass.h
			com/misc/closure.h
			com/misc/dynamicbuffer.h
			com/misc/profile.h
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
			OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/man"
			COMMENT "Generate manual pages on interface"
		)

		find_program(CYGPATH cygpath ${CMAKE_CURRENT_BINARY_DIR})
		if (NOT CYGPATH)
			message(FATAL_ERROR "Can't find 'cygpath'!")
		else ()
			execute_process(COMMAND "${CYGPATH}" "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE PROJECT_SOURCE_DIR)
			string(REGEX REPLACE "\n$" "" PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
		endif ()
	]]
	# For cygwin only relative path are working.
	file(RELATIVE_PATH LOGO_FILE "${CMAKE_CURRENT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/com/misc/qt/res/logo/scanframe.ico")
	# Path to images.
	file(RELATIVE_PATH IMAGE_PATH "${CMAKE_CURRENT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}")
	# Enable when to change the output directory.
	if (FALSE)
		file(RELATIVE_PATH OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/bin/man")
	endif()
	# Get the markdown files in this project directory.
	file(GLOB DOXYGEN_SOURCES RELATIVE "${CMAKE_CURRENT_BINARY_DIR}" "*.md")
	# Get all the header files in the ../com module.
	file(GLOB_RECURSE DOXYGEN_SOURCES_TMP RELATIVE "${CMAKE_CURRENT_BINARY_DIR}" "../com/*.h" "../com/*.md")
	# Remove unwanted header file(s) ending on 'Private.h'.
	list(FILTER DOXYGEN_SOURCES_TMP EXCLUDE REGEX ".*Private\\.h$")
	list(APPEND DOXYGEN_SOURCES ${DOXYGEN_SOURCES_TMP})
	# Replace the list separator ';' with space in the list.
	list(JOIN DOXYGEN_SOURCES " " DOXYGEN_SRC)
	# Enable when generating Zen styling output.
	if (FALSE)
		set(HTML_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/theme/zen/header.html")
		set(HTML_FOOTER "${CMAKE_CURRENT_SOURCE_DIR}/theme/zen/footer.html")
		set(HTML_STYLESHEET "${CMAKE_CURRENT_SOURCE_DIR}/theme/zen/stylesheet.css")
		set(HTML_EXTRA_STYLESHEET "")
	else()
		# Fixes source file viewing.
		set(HTML_EXTRA_STYLESHEET "${CMAKE_CURRENT_SOURCE_DIR}/custom.css")
	endif ()
	# Set the example path to this parent directory.
	file(RELATIVE_PATH EXAMPLE_PATH "${CMAKE_CURRENT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}")
	# Set input and output files for the generation of the actual config file.
	set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.conf)
	set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doxyfile.conf)
	# Generate the configure the file for doxygen.
	configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
	# Note the option ALL which allows to build the docs together with the application.
	add_custom_target(manual ALL
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating documentation with Doxygen"
		VERBATIM)
else ()
	message(NOTICE "Doxygen not found, not building documentation.
Install needed Linux packages 'doxygen' and 'graphviz'.")
endif ()
