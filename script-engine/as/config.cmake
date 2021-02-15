# Make sure builds do not wind up in the source directory.
list(APPEND CMAKE_PREFIX_PATH "../../com/cmake")
# Add path to angel script
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../script-engines/angelscript_2.35.0/sdk/lib/cmake/Angelscript")
#message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)

# Do not export all by default.
add_definitions("-fvisibility=hidden")

# Generate an error on undefined (imported) symbols on dyn libs
# because the error appears only at load-time.
add_link_options(-Wl,--no-undefined -Wl,--no-allow-shlib-undefined)

# Set output directories only when the current cmake project is also the main project.
if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
	get_filename_component(_BinDir "../../bin" REALPATH)
	message(STATUS "Output Dir (${PROJECT_NAME}): 'file://${_BinDir}'" )
	# Set the directories relative to this included file.
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../../bin")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../../bin")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../../bin")
endif ()

# Write a information messages.
message(STATUS "Project name: ${PROJECT_NAME}")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
