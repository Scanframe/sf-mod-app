# Set the target name using the project name as a base name.
set(MY_TEST_TARGET "${PROJECT_NAME}-test")

#message(STATUS "MY_TEST_TARGET: ${MY_TEST_TARGET}")

# Tests need to be added as executables first
add_executable(${MY_TEST_TARGET} testqstring.cpp)

# Sets the extension of the generated binary.
_SetBinarySuffix("${MY_TEST_TARGET}")

# I'm using C++17 in the test
target_compile_features(${MY_TEST_TARGET} PRIVATE cxx_std_17)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(${MY_TEST_TARGET} PRIVATE Qt${QT_VERSION_MAJOR}::Test)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
# Command can be a target
add_test(NAME ${MY_TEST_TARGET}-test${_TestBinSuffix} COMMAND ${MY_TEST_TARGET})
