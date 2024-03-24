# Set the target name using the project name as a base name and prefixed.
set(TEST_TARGET "${SF_TEST_NAME_PREFIX}${PROJECT_NAME}")

# Make Catch2::Catch2 library available.
find_package(SfCatch2 CONFIG REQUIRED)

# Tests need to be added as executables first
add_executable(${TEST_TARGET} test-it.cpp ../functions.cpp)

# Sets the extension of the generated binary.
Sf_SetTargetSuffix("${TEST_TARGET}")

# I'm using C++17 in the test
target_compile_features("${TEST_TARGET}" PRIVATE cxx_std_20)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries("${TEST_TARGET}" PRIVATE Catch2::Catch2WithMain)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
# Command can be a target
add_test(NAME "${TEST_TARGET}"
	COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../$<IF:$<BOOL:${WIN32}>,win-exec.sh,lnx-exec.sh>" "$<TARGET_FILE_NAME:${TEST_TARGET}>")
