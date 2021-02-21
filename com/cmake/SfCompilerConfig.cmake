if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")

	if (WIN32)
		#message(STATUS "Platform: Windows ${CMAKE_SYSTEM_VERSION}")
		#	set(CMAKE_MAKE_PROGRAM "P:/Qt/Tools/CMake_64/bin/cmake.exe" FORCE)
		#	set(CMAKE_CXX_COMPILER "P:/Qt/Tools/mingw810_64/bin/g++.exe")
		#	set(CMAKE_C_COMPILER "P:/Qt/Tools/mingw810_64/bin/gcc.exe")
		set(QT_PLUGINS_DIR "P:/Qt/5.15.2/mingw81_64/plugins/")
	else ()
		#message(STATUS "Platform: Linux ${CMAKE_SYSTEM_VERSION}")
		#set(QT_PLUGINS_DIR "/usr/lib/x86_64-linux-gnu/qt5/plugins")
		set(QT_PLUGINS_DIR "$ENV{HOME}/lib/Qt/5.15.1/gcc_64/plugins")
	endif ()

	message(STATUS "Designer Plugins Dir: '${QT_PLUGINS_DIR}'")

	# Do not export all by default.
	add_definitions("-fvisibility=hidden")
	# Generate an error on undefined (imported) symbols on dyn libs
	# because the error appears only at load-time.
	add_link_options(-Wl,--no-undefined -Wl,--no-allow-shlib-undefined)

	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)

endif ()
