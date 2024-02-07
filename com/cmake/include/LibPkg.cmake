# When the target exists ignore it.
if (TARGET ${TargetName})
	#message(STATUS "Target '${PROJECT_NAME}': Library '${TargetName}' is already part of build.")
else ()
	message(STATUS "Target '${PROJECT_NAME}': Adding library '${TargetName}'.")
	add_library(${TargetName} SHARED IMPORTED)
	if (WIN32)
		Sf_PopulateTargetProperties(${TargetName} DEBUG "lib${TargetName}.dll" "lib${TargetName}.dll.a")
	else ()
		Sf_PopulateTargetProperties(${TargetName} DEBUG "lib${TargetName}.so" "")
	endif ()
	get_filename_component(_include_location "${CMAKE_CURRENT_LIST_DIR}/.." REALPATH)
	set_property(TARGET ${TargetName} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${_include_location}")
endif ()
