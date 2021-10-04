# Set the target name using the project name as a base name.
set(TEST_TARGET "${PROJECT_NAME}-test")

# Make Catch2::Catch2 library available.
find_package(SfCatch2 CONFIG REQUIRED)

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
	list(APPEND TEST_SOURCES ${TEST_SOURCES_QT}
		)
	list(APPEND TEST_LIBRARIES Qt::Widgets Qt::Designer)
endif (SF_BUILD_QT)

# Tests need to be added as executables first
add_executable(${TEST_TARGET}	main.cpp ${TEST_SOURCES})

# Sets the extension of the generated binary.
_SetBinarySuffix("${TEST_TARGET}")

# I'm using C++17 in the test
target_compile_features(${TEST_TARGET} PRIVATE cxx_std_17)

if (SF_BUILD_QT)
	# Explicitly tell the compiled code the QT libraries are included.
	target_compile_definitions(${TEST_TARGET} PRIVATE TARGET_QT)
endif (SF_BUILD_QT)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(${TEST_TARGET} PRIVATE ${TEST_LIBRARIES})

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
# Command can be a target
add_test(NAME ${TEST_TARGET}-test${_TestBinSuffix} COMMAND ${TEST_TARGET})

