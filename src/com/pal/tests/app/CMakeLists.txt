# Set the target name using the test prefix.
set(TEST_APP_TARGET "${SF_TEST_NAME_PREFIX}${PROJECT_NAME}-test-app")

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
Sf_AddExecutable("${TEST_APP_TARGET}")

target_sources("${TEST_APP_TARGET}" PRIVATE main.cpp "${TEST_APP_SOURCES}")

# Adds the compiler and linker options for coverage.
Sf_AddTargetForCoverage("${TEST_APP_TARGET}")

# Sets the extension of the generated binary.
Sf_SetTargetSuffix("${TEST_APP_TARGET}")

# Should be linked to the main library, as well as the Catch2 testing library.
target_link_libraries("${TEST_APP_TARGET}" PRIVATE Qt::Widgets sf-misc sf-gii sf-pal sf-pal-simple sf-pal-amplitude)

# Add this application as a test for all possible builds.
Sf_AddTest(${TEST_APP_TARGET} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/..")

# Make sure this test is executed before coverage test.
Sf_AddAsCoverageTest("${TEST_APP_TARGET}")
