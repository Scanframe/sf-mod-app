# Set the target name using the project name as a base name.
set(TEST_TARGET "${PROJECT_NAME}-test")

#[[
# Make Catch2::Catch2 library available.
find_package(SfCatch2 CONFIG REQUIRED)
#find_package(SfGsc REQUIRED)
]]

find_package(SfMisc REQUIRED)

set(TEST_LIBRARIES Catch2::Catch2 sf-misc sf-gsc)

# Find the test sources.
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test-*")

list(APPEND TEST_LIBRARIES Qt::Widgets)

# Tests need to be added as executables first
add_executable(${TEST_TARGET}	main.cpp ${TEST_SOURCES})

# Sets the extension of the generated binary.
Sf_SetTargetSuffix("${TEST_TARGET}")

if (SF_BUILD_QT)
	# Explicitly tell the compiled code the QT libraries are included.
	target_compile_definitions(${TEST_TARGET} PRIVATE TARGET_QT)
endif (SF_BUILD_QT)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(${TEST_TARGET} PRIVATE ${TEST_LIBRARIES})

#[[
# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
# Command can be a target
add_test(NAME ${TEST_TARGET}-test${_TestBinSuffix} COMMAND ${TEST_TARGET})
]]

