# Create variable holding the libraries filename.
set(TargetName "sf-misc")
# When the target exists ignore it.
if (TARGET ${TargetName})
	message(STATUS "Target '${PROJECT_NAME}': Library '${TargetName}' is already part of build.")
else ()
	message(STATUS "Target '${PROJECT_NAME}': Adding library '${TargetName}'.")
	add_library(${TargetName} SHARED IMPORTED)
	if (WIN32)
		_populate_target_props(${TargetName} DEBUG "lib${TargetName}.dll" "lib${TargetName}.dll.a")
	else ()
		_populate_target_props(${TargetName} DEBUG "lib${TargetName}.so" "")
	endif ()
	get_filename_component(_include_location "${CMAKE_CURRENT_LIST_DIR}/.." REALPATH)
	set_property(TARGET ${TargetName} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${_include_location}")
endif ()

#[[

#set_property(TARGET ${TargetName} PROPERTY IMPORTED_IMPLIB_DEBUG ${PROJECT_SOURCE_DIR}/lib/nana_v141_Debug_x64.lib)
#set_property(TARGET ${TargetName} PROPERTY IMPORTED_IMPLIB_RELEASE ${PROJECT_SOURCE_DIR}/lib/nana_v141_Release_x64.lib)
#set_property(TARGET ${TargetName} PROPERTY INTERFACE_INCLUDE_DIRECTORIES PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../com")
#set_property(TARGET ${TargetName} PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include)

target_include_directories(${TargetName}
	INTERFACE
#	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/single_include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	)

# Set the include directory for this library when it is imported by another sub project.
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}")

# Set target additional include directories.
target_include_directories("sf-misc" PUBLIC "../../com")

]]

