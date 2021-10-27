# Notify that this file was loaded.
message("Linux compiling for Windows.")

# Allow cmake to find the Qt library (this should be automated and version independent)
list(APPEND CMAKE_PREFIX_PATH "/mnt/server/userdata/project/PROG/Qt/6.2.1/mingw81_64/lib/cmake/")

# Targeted operating system.
set(CMAKE_SYSTEM_NAME Windows)

# Use mingw 64-bit compilers.
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc-posix)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-c++-posix)

SET(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_RANLIB x86_64-w64-mingw32-ranlib)

set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32/)

# Adjust the default behavior of the find commands:
# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

# Search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)


