# Set the target name using the project name as a base name and prefixed.
set(TEST_TARGET "${SF_TEST_NAME_PREFIX}${PROJECT_NAME}")

find_package(SfMisc REQUIRED)

set(TEST_LIBRARIES sf-unwind)

# Find the test sources.
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test-*")

# Tests need to be added as executables first
Sf_AddExecutable("${TEST_TARGET}")

target_sources("${TEST_TARGET}" PRIVATE main.cpp "${TEST_SOURCES}")

# Sets the extension of the generated binary.
Sf_SetTargetSuffix("${TEST_TARGET}")

# Explicitly tell the compiled code the QT libraries are included.
target_compile_definitions(${TEST_TARGET} PRIVATE TARGET_QT)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries("${TEST_TARGET}" PRIVATE ${TEST_LIBRARIES})

#[[
# Add this application as a test for all possible builds.
Sf_AddTest(${TEST_APP_TARGET} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/..")

# Make sure this test is executed before coverage test.
Sf_AddAsCoverageTest("${TEST_TARGET}")
]]

