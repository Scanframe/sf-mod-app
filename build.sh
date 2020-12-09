#!/bin/bash
#set -x

if [[ -z "$1" ]]; then
	echo "Usage: ${0} <sub-dir> [<target>]"
	echo "	Where sub-dir is 'com', 'rt-shared-lib/app', 'rt-shared-lib/iface', 'rt-shared-lib/impl-a', rt-shared-lib"
	exit 1
fi

SOURCE_DIR="$1"

if [[ -z $2 ]]; then

	TARGET="all"
	BUIlD_OPTIONS="--clean-first"

else

	TARGET="$2"
	BUIlD_OPTIONS=""

fi

if [[ "$(uname -s)" == "CYGWIN_NT"* ]]; then

	echo "Windows NT detected."
	# "P:\Qt\5.15.2\mingw81_64\bin"

	# CMAKE_BIN="/cygdrive/p/Qt/Tools/CMake_64/bin/cmake"
	CMAKE_BIN="CMD /C P:\Qt\Tools\CMake_64\bin\cmake.exe"
	BUILD_DIR="$(cygpath -aw "${SOURCE_DIR}/cmake-build-debug")"
	# Configure 
	${CMAKE_BIN} -B "${BUILD_DIR}" --config "$(cygpath -aw "${SOURCE_DIR}")" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
	# Build debug
	${CMAKE_BIN} --build "${BUILD_DIR}" --target "${TARGET}" ${BUIlD_OPTIONS}

else

	CMAKE_BIN="cmake"
	# Configure
	${CMAKE_BIN} -B "${SOURCE_DIR}/cmake-build-debug" --config "${SOURCE_DIR}" -DCMAKE_BUILD_TYPE=Debug 
	# Build debug
	${CMAKE_BIN} --build "${SOURCE_DIR}/cmake-build-debug" --target "${TARGET}" ${BUIlD_OPTIONS}

fi




