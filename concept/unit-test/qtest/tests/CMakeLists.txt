# Set the target name using the project name as a base name and prefixed.
set(TEST_TARGET "${SF_TEST_NAME_PREFIX}${PROJECT_NAME}")

#message(STATUS "TEST_TARGET: ${TEST_TARGET}")

# Tests need to be added as executables first
add_executable(${TEST_TARGET} testqstring.cpp)

# Sets the extension of the generated binary.
Sf_SetTargetSuffix("${TEST_TARGET}")

# I'm using C++17 in the test
target_compile_features(${TEST_TARGET} PRIVATE cxx_std_17)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(${TEST_TARGET} PRIVATE Qt::Test)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
# Command can be a target
add_test(NAME "${TEST_TARGET}"
	COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../$<IF:$<BOOL:${WIN32}>,$<IF:$<BOOL:${SF_CROSS_WINDOWS}>,win-exec.sh,win-exec.cmd>,lnx-exec.sh>" "$<TARGET_FILE_NAME:${TEST_TARGET}>")
