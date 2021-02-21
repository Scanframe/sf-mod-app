# FetchContent added in CMake 3.11, downloads during the configure step
include(FetchContent)
# Import Catch2 library for testing.
FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v2.13.4)
# Adds Catch2::Catch2
FetchContent_MakeAvailable(Catch2)
