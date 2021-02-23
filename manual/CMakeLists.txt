# Add doxygen project when doxygen was found
find_package(Doxygen QUIET)
if (Doxygen_FOUND)
	message(STATUS "Doxygen ${DOXYGEN_VERSION} found and documentation is build.")
	#[[
		set(DOXYGEN_EXTRACT_ALL YES)
		set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
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
	]]
	# Get the markdown files in this project directory.
	file(GLOB DOXYGEN_SOURCES *.md)
	# Get all the header files in the ../com module.
	file(GLOB_RECURSE DOXYGEN_SOURCES_TMP ../com/*.h)
	list (APPEND DOXYGEN_SOURCES ${DOXYGEN_SOURCES_TMP})
	# Replace the list separator ';' with space in the list.
	list(JOIN DOXYGEN_SOURCES " " DOXYGEN_SRC)
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
