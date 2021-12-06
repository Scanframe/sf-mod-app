# Notify that this file was loaded.
message(STATUS "Linux compiling for Windows.")

# Check the existence of the of the directory.
if (NOT EXISTS "$ENV{SF_QT_VERSION_CMAKE_DIR}")
	# Fatally bailout. This is a show stopper.
	message(FATAL_ERROR "Environment variable 'SF_QT_VERSION_CMAKE_DIR' assigned directory '$ENV{SF_QT_VERSION_CMAKE_DIR}' does not exist!")
endif()

# Append the directory to the list if not already.
if("$ENV{SF_QT_VERSION_CMAKE_DIR}" IN_LIST CMAKE_PREFIX_PATH)
	list(APPEND CMAKE_PREFIX_PATH "$ENV{SF_QT_VERSION_CMAKE_DIR}")
endif()

# Targeted operating system.
set(CMAKE_SYSTEM_NAME Windows)

# Use mingw 64-bit compilers.
set(CMAKE_C_COMPILER "x86_64-w64-mingw32-gcc-posix")
set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-c++-posix")

SET(CMAKE_RC_COMPILER "x86_64-w64-mingw32-windres")
set(CMAKE_RANLIB "x86_64-w64-mingw32-ranlib")

set(CMAKE_FIND_ROOT_PATH "/usr/x86_64-w64-mingw32")

# Adjust the default behavior of the find commands:
# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

# Search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)


