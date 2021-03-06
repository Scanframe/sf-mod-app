macro(macro_ensure_out_of_source_build MSG)
	string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" insource)
	get_filename_component(PARENTDIR ${CMAKE_SOURCE_DIR} PATH)
	string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PARENTDIR}" insourcesubdir)
	if(insource OR insourcesubdir) 
		message(FATAL_ERROR "${MSG}") 
	endif()
endmacro()

# Ensures that we do an out of source build
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build.")
