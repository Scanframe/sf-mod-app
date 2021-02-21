# Unit Testing - Catch2


See: [How it should work](https://gitlab.com/CLIUtils/modern-cmake/-/tree/master/examples/extended-project)
Import Boost library by fetching from [GitHub](https://github.com/Orphis/boost-cmake)

```cmake

find_package(Catch2 QUIET)
if (NOT Catch2_FOUND)
	message("Catch2 not found")
	include(FetchContent)

	FetchContent_Declare(
		catch2
		GIT_REPOSITORY https://github.com/catchorg/Catch2.git
		GIT_TAG        v2.11.0
	)

	FetchContent_MakeAvailable(catch2)
	list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/contrib")

	message(STATUS "Catch2_SOURCE_DIR: ${Catch2_SOURCE_DIR}/contrib")
endif()
```