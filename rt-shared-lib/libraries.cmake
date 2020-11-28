# When the target does not exists add it.
if (NOT TARGET "sf-misc")
	add_library("sf-misc" SHARED IMPORTED)
	set_property(TARGET sf-misc PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libsf-misc.so")
endif()

# When the target does not exists add it.
if (NOT TARGET "rt-iface")
	add_library("rt-iface" SHARED IMPORTED)
	set_property(TARGET rt-iface PROPERTY IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/librt-iface.so")
endif()

# Set target additional include directories.
target_include_directories(${PROJECT_NAME}
	# For lib misc headers.
	PUBLIC "${CMAKE_CURRENT_LIST_DIR}/.."
	# For lib iface headers.
	PUBLIC "${CMAKE_CURRENT_LIST_DIR}"
	)

message("Include Directory: 'file://${CMAKE_CURRENT_LIST_DIR}'")

