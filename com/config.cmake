list(APPEND CMAKE_PREFIX_PATH "cmake")
find_package(SfBuildCheck CONFIG REQUIRED)

# Do not export all by default.
add_definitions("-fvisibility=hidden")
# Generate an error on undefined (imported) symbols on dyn libs
# because the error appears only at load-time.
add_link_options(-Wl,--no-undefined -Wl,--no-allow-shlib-undefined)

# Set output directories only when the current cmake project is also the main project.
if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
	get_filename_component(_BinDir "../bin" REALPATH)
	message(STATUS "Output Dir (${PROJECT_NAME}: 'file://${_BinDir}'" )
	# Set the directories relative to this included file.
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../bin")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../bin")
endif ()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

