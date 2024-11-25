# Set the target name using the current directory name and the test prefix.
get_filename_component(SF_PROJECT_SUFFIX ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TEST_APP_TARGET "${SF_TEST_NAME_PREFIX}${PROJECT_NAME}-${SF_PROJECT_SUFFIX}")

# Make Catch2::Catch2 library available.
find_package(SfCatch2 CONFIG REQUIRED)

# Find the test sources.
file(GLOB TEST_APP_SOURCES "test-*")

# Tests need to be added as executables first
Sf_AddExecutable("${TEST_APP_TARGET}")

target_sources("${TEST_APP_TARGET}" PRIVATE main.cpp "${TEST_APP_SOURCES}")

# Adds the compiler and linker options for coverage.
Sf_AddTargetForCoverage("${TEST_APP_TARGET}")

# Sets the extension of the generated binary.
Sf_SetTargetSuffix("${TEST_APP_TARGET}")

# Should be linked to the main library, as well as the Catch2 testing library.
target_link_libraries("${TEST_APP_TARGET}" PRIVATE pthread sf-misc)

# Add this application as a test for all possible builds.
Sf_AddTest(${TEST_APP_TARGET} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/..")

# Make sure this test is executed before coverage test.
Sf_AddAsCoverageTest("${TEST_APP_TARGET}")
