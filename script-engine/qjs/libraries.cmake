# Import libraries when not added yet depending .
_add_shared_library("sf-misc")

# TODO: Include files should depending on the library which is included.
# Set target additional include directories.
target_include_directories(${PROJECT_NAME}
	# For lib 'sf-misc' headers.
	PUBLIC "../../com"
	# For lib 'iface' headers.
	PUBLIC ".."
	)
