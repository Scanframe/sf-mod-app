if (WIN32)
	message("Platform: Windows")
#	set(CMAKE_MAKE_PROGRAM "P:/Qt/Tools/CMake_64/bin/cmake.exe" FORCE)
#	set(CMAKE_CXX_COMPILER "P:/Qt/Tools/mingw810_64/bin/g++.exe")
#	set(CMAKE_C_COMPILER "P:/Qt/Tools/mingw810_64/bin/gcc.exe")
else ()
	message("Platform: Linux ${CMAKE_SYSTEM_VERSION}")
endif ()


