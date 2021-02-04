if (WIN32)
	message(STATUS "Platform: Windows")
#	set(CMAKE_MAKE_PROGRAM "P:/Qt/Tools/CMake_64/bin/cmake.exe" FORCE)
#	set(CMAKE_CXX_COMPILER "P:/Qt/Tools/mingw810_64/bin/g++.exe")
#	set(CMAKE_C_COMPILER "P:/Qt/Tools/mingw810_64/bin/gcc.exe")
	set(QT_PLUGINS_DIR "P:/Qt/5.15.2/mingw81_64/plugins/")
else ()
	message(STATUS "Platform: Linux ${CMAKE_SYSTEM_VERSION}")
	#set(QT_PLUGINS_DIR "/usr/lib/x86_64-linux-gnu/qt5/plugins")
	set(QT_PLUGINS_DIR "$ENV{HOME}/lib/Qt/5.15.1/gcc_64/plugins")
endif ()

message(STATUS "Plugins Dir: '${QT_PLUGINS_DIR}'" )