# Set the target name using the project name as a base name and prefixed.
set(TEST_TARGET "${SF_TEST_NAME_PREFIX}${PROJECT_NAME}")

# Make Catch2::Catch2 library available.
find_package(SfCatch2 CONFIG REQUIRED)

set(TEST_LIBRARIES Catch2::Catch2 sf-misc sf-gii)

# Find the test sources.
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/gen/test-*")

# Add platform dependent tests.
if (WIN32)
	file(GLOB TEST_SOURCES_WIN "${CMAKE_CURRENT_SOURCE_DIR}/win/test-*")
	list(APPEND TEST_SOURCES ${TEST_SOURCES_WIN})
else (WIN32)
	file(GLOB TEST_SOURCES_LNX "${CMAKE_CURRENT_SOURCE_DIR}/lnx/test-*")
	list(APPEND TEST_SOURCES ${TEST_SOURCES_LNX})
endif (WIN32)

# When Qt libraries are linked.
if (SF_BUILD_QT)
	# Add the console unittests.
	file(GLOB TEST_SOURCES_QT "${CMAKE_CURRENT_SOURCE_DIR}/qt/test-*")
	list(APPEND TEST_SOURCES ${TEST_SOURCES_QT})
	list(APPEND TEST_LIBRARIES Qt::Widgets Qt::Designer)
endif (SF_BUILD_QT)

# Tests need to be added as executables first
add_executable("${TEST_TARGET}")

target_sources("${TEST_TARGET}" PRIVATE main.cpp "${TEST_SOURCES}")

# Adds the compiler and linker options for coverage.
Sf_AddTargetForCoverage("${TEST_TARGET}")

# Sets the extension of the generated binary.
Sf_SetTargetSuffix("${TEST_TARGET}")

if (SF_BUILD_QT)
	# Explicitly tell the compiled code the QT libraries are included.
	target_compile_definitions(${TEST_TARGET} PRIVATE TARGET_QT)
endif (SF_BUILD_QT)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries("${TEST_TARGET}" PRIVATE ${TEST_LIBRARIES})

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
# Command can be a target
add_test(NAME "${TEST_TARGET}"
	COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../$<IF:$<BOOL:${WIN32}>,$<IF:$<BOOL:${SF_CROSS_WINDOWS}>,win-exec.sh,win-exec.cmd>,lnx-exec.sh>" "$<TARGET_FILE_NAME:${TEST_TARGET}>")

# Make sure this test is executed before coverage test.
Sf_AddAsCoverageTest("${TEST_TARGET}")


