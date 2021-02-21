# Set the target name using the project name as a base name.
set(MY_TEST_TARGET "${PROJECT_NAME}-test")

# Import Catch2 library for testing.
FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v2.13.4)
# Adds Catch2::Catch2
FetchContent_MakeAvailable(Catch2)

# Tests need to be added as executables first
add_executable(${MY_TEST_TARGET} test-it.cpp ../src/functions.cpp)

# Sets the extension of the generated binary.
_SetBinarySuffix("${MY_TEST_TARGET}")

# I'm using C++17 in the test
target_compile_features(${MY_TEST_TARGET} PRIVATE cxx_std_17)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(${MY_TEST_TARGET} PRIVATE Catch2::Catch2)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
# Command can be a target
add_test(NAME ${MY_TEST_TARGET}-test${_TestBinSuffix} COMMAND ${MY_TEST_TARGET})

