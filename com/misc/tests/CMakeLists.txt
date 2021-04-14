# Set the target name using the project name as a base name.
set(TEST_TARGET "${PROJECT_NAME}-test")

# Make Catch2::Catch2 library available.
find_package(SfCatch2 CONFIG REQUIRED)
# When SF_BUILD_QT is Set link the Qt libraries.
if (SF_BUILD_QT)
	# Find the needed packages.
	find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
	find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Designer REQUIRED)
endif()

set(TEST_LIBRARIES Catch2::Catch2 sf-misc)

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
	file(GLOB TEST_SOURCES_QT "${CMAKE_CURRENT_SOURCE_DIR}/qt/test-*")
	list(APPEND TEST_SOURCES ${TEST_SOURCES_QT})
	list(APPEND TEST_LIBRARIES Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Designer)
endif (SF_BUILD_QT)

# Tests need to be added as executables first
add_executable(${TEST_TARGET}	main.cpp ${TEST_SOURCES})

# Sets the extension of the generated binary.
_SetBinarySuffix("${TEST_TARGET}")

# I'm using C++17 in the test
target_compile_features(${TEST_TARGET} PRIVATE cxx_std_17)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(${TEST_TARGET} PRIVATE ${TEST_LIBRARIES})

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
# Command can be a target
add_test(NAME ${TEST_TARGET}-test${_TestBinSuffix} COMMAND ${TEST_TARGET})

