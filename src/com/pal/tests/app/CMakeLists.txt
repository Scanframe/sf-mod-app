# Set the target name using the project name as a base name.
set(TEST_APP_TARGET "${PROJECT_NAME}-test-app")

# Uses directory Set by Sf_SetOutputDirs().
find_package(SfMisc REQUIRED)
find_package(SfGii REQUIRED)
find_package(SfPal REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find the needed packages.
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

# Find the test sources.
file(GLOB TEST_APP_SOURCES "test-*")

# Tests need to be added as executables first
add_executable("${TEST_APP_TARGET}")

target_sources("${TEST_APP_TARGET}" PRIVATE main.cpp "${TEST_APP_SOURCES}")

# Adds the compiler and linker options for coverage.
Sf_AddTargetForCoverage("${TEST_APP_TARGET}")

# Sets the extension of the generated binary.
Sf_SetTargetSuffix("${TEST_APP_TARGET}")

# Should be linked to the main library, as well as the Catch2 testing library.
target_link_libraries("${TEST_APP_TARGET}" PRIVATE Qt::Widgets sf-misc sf-gii sf-pal sf-pal-simple sf-pal-amplitude)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
# Command can be a target
add_test(NAME "${TEST_APP_TARGET}"
	COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../$<IF:$<BOOL:${WIN32}>,$<IF:$<BOOL:${SF_CROSS_WINDOWS}>,win-exec.sh,win-exec.cmd>,lnx-exec.sh>" "$<TARGET_FILE_NAME:${TEST_APP_TARGET}>")


# Make sure this test is executed before coverage test.
Sf_AddAsCoverageTest("${TEST_APP_TARGET}")

