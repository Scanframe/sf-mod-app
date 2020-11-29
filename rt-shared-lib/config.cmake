# Make sure builds do not wind up in the source directory.
list(APPEND CMAKE_PREFIX_PATH "../../com/cmake")
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)

# Do not export all by default.
add_definitions("-fvisibility=hidden")

# Generate an error on undefined (imported) symbols on dyn libs
# because the error appears only at load-time.
add_link_options(-Wl,--no-undefined -Wl,--no-allow-shlib-undefined)

# Set output directories only when the current cmake project is also the main project.
if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
	message("Setting Output Dir for Project '${PROJECT_NAME}'")
	# Set the directories relative to this included file.
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../bin")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../bin")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../bin")
endif ()

# Write a information messages.
message("Project name: ${PROJECT_NAME}")
if (TRUE)
	message("Out Directory: 'file://${CMAKE_CURRENT_LIST_DIR}/../bin'")
else ()
	message("Run-Time Out Directory: 'file://${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'")
	message("Library Out Directory:  'file://${CMAKE_LIBRARY_OUTPUT_DIRECTORY}'")
	message("Archive Out Directory:  'file://${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}'")
endif ()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

