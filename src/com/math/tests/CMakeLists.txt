# Set the target name using the project name as a base name and prefixed.
set(TEST_TARGET "${SF_TEST_NAME_PREFIX}${PROJECT_NAME}")

# Make Catch2::Catch2 library available.
find_package(SfCatch2 CONFIG REQUIRED)

set(TEST_LIBRARIES Catch2::Catch2 sf-misc sf-math)

# Crypto library is not needed anymore since MD5 is deprecated and a solution is provided.
#list(APPEND TEST_LIBRARIES OpenSSL::Crypto)

# Find the test sources.
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test-*")

# Tests need to be added as executables first
Sf_AddExecutable("${TEST_TARGET}")

target_sources("${TEST_TARGET}" PRIVATE main.cpp "${TEST_SOURCES}")

# Adds the compiler and linker options for coverage.
Sf_AddTargetForCoverage("${TEST_TARGET}")

# Sets the extension of the generated binary.
Sf_SetTargetSuffix("${TEST_TARGET}")

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries("${TEST_TARGET}" PRIVATE "${TEST_LIBRARIES}")

# Add this application as a test for all possible builds.
Sf_AddTest(${TEST_TARGET} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/..")

# Make sure this test is executed before coverage test.
Sf_AddAsCoverageTest("${TEST_TARGET}")
