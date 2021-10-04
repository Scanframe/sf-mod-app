# Set the target name using the project name as a base name.
set(TEST_APP_TARGET "${PROJECT_NAME}-test-app-script")

# Make Catch2::Catch2 library available.
find_package(SfCatch2 CONFIG REQUIRED)

# Find the test sources.
file(GLOB TEST_APP_SOURCES "test-*")

# Tests need to be added as executables first
add_executable(${TEST_APP_TARGET}
	main.cpp
	${TEST_APP_SOURCES}
	)

# Sets the extension of the generated binary.
_SetBinarySuffix("${TEST_APP_TARGET}")

# I'm using C++17 in the test.
target_compile_features(${TEST_APP_TARGET} PRIVATE cxx_std_17)

# Should be linked to the main library, as well as the Catch2 testing library.
target_link_libraries(${TEST_APP_TARGET} PRIVATE Qt::Designer pthread sf-misc)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
# Command can be a target
add_test(NAME ${TEST_APP_TARGET}-test${_TestBinSuffix} COMMAND ${TEST_APP_TARGET})
