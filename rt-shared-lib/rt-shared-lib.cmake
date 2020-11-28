if (WIN32)
	message("Platform: Windows")
else ()
	message("Platform: Linux ${CMAKE_SYSTEM_VERSION}")
endif ()

# Make sure builds do not wind up in the source directory.
list(APPEND CMAKE_PREFIX_PATH "../../com/cmake")
find_package(SfBuildCheck CONFIG REQUIRED)

# Do not export all by default.
add_definitions("-fvisibility=hidden")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../bin)

# Write a message.
message("Project name: ${CMAKE_PROJECT_NAME}")
message("Run-Time Directory: ${CMAKE_BINARY_DIR}")
message("Library Out Directory: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message("Archive Out Directory: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate an error on undefined (imported) symbols on dyn libs
# because the error appears only at load-time.
add_link_options(-Wl,--no-undefined -Wl,--no-allow-shlib-undefined)


