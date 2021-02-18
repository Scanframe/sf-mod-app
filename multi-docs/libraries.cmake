# Import libraries when not added yet depending .
_add_shared_library("sf-misc")
#_add_shared_library("custom-ui-plugin")

# TODO: Include files should depending on the library which is included.
# Set target additional include directories.
target_include_directories(${PROJECT_NAME}
	# For lib 'sf-misc' headers.
	PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../com"
	)