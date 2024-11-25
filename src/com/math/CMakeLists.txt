# Required first entry checking the cmake version.
cmake_minimum_required(VERSION 3.18)
# Set the project name.
project("sf-math" LANGUAGES CXX)

# Make it so our own packages are found.
list(APPEND CMAKE_PREFIX_PATH "../../../../cmake")
# Make sure builds do not wind up in the source directory.
find_package(SfBuildCheck CONFIG REQUIRED)
find_package(SfMacros CONFIG REQUIRED)
find_package(SfCompiler CONFIG REQUIRED)

# Set the 3 CMAKE_xxxxx_OUTPUT_DIRECTORY variables.
Sf_SetOutputDirs("bin")

# Uses directory Set by Sf_SetOutputDirs().
find_package(SfMisc REQUIRED)

# Add shared library and also sets the dynamic library version.
Sf_AddSharedLibrary("${PROJECT_NAME}")

# Adds the compiler and linker options for coverage.
Sf_AddTargetForCoverage("${PROJECT_NAME}")

target_sources(${PROJECT_NAME} PRIVATE
	global.h
	LibMain.cpp
	TVector2D.h TVector2D.hpp
	TRectangle2D.h TRectangle2D.hpp
	TMatrix22.h TMatrix22.hpp
	TVector3D.h TVector3D.hpp
	TMatrix44.h TMatrix44.hpp
	TQuaternion.h TQuaternion.hpp
)

# Link the Qt widgets library.
target_link_libraries(${PROJECT_NAME} PRIVATE sf-misc)

# Tell the compiled code a dynamic library (DL) is being build.
target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_DYNAMIC_LIB)

# The 'DEBUG_LEVEL' controls the debug output macro's.
target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG_LEVEL=1)

# When enabled the 'target.h' file enables reporting of the current target being build using 'pragma message'.
#target_compile_definitions(${PROJECT_NAME} PRIVATE REPORT_TARGET)

# Tell the code the library is used as an archive. (So no imports or exports)
#target_compile_definitions(${PROJECT_NAME} PRIVATE _GMI_ARC)

# Tell the code the library is used as dynamic library. (So imports needed)
target_compile_definitions(${PROJECT_NAME} PRIVATE _MATH_PKG)

# Set the include directory for this library when it is imported by another sub project.
target_include_directories("${PROJECT_NAME}"
	INTERFACE "${CMAKE_CURRENT_LIST_DIR}/.."
	PRIVATE "${CMAKE_CURRENT_LIST_DIR}/.."
)

# Add Sub Projects in the right order of dependency.
if (SF_BUILD_TESTING)
	add_subdirectory(tests)
endif ()
