#!/bin/bash
#set -x

# Set the directory the local QT root expected.
LOCAL_QT_ROOT="${HOME}/lib/Qt"

# Writes to stderr.
#
function WriteLog()
{
	echo "$@" 1>&2;
}

# Prints the help to stderr.
#
function ShowHelp()
{
	WriteLog "Usage: ${0} <sub-dir> [<target>] [-l]"
	WriteLog "-l : Local QT version (needs ${LOCAL_QT_ROOT} directory)."
	WriteLog "	Where sub-dir is 'com', 'rt-shared-lib/app', 'rt-shared-lib/iface', 'rt-shared-lib/impl-a', 'rt-shared-lib'"
}

# Parse all options and arguments.
# ---------------------------------
# Initialize arguments and switches.
argument=()
LOCAL_QT=false
while [ $# -gt 0 ] && [ "$1" != "--" ]; do
	while getopts "hl" opt; do
		case $opt in
			 h)
				ShowHelp
				exit 0
				;;
			 l)
			 	WriteLog "Using local Qt version from: ${LOCAL_QT_ROOT}"
			 	LOCAL_QT=true
			 	;;
			 \?)
			 	WriteLog "Invalid option: -$OPTARG"
				ShowHelp
			 	exit 1
			 	;;
	esac
	done
	shift $((OPTIND-1))
	while [ $# -gt 0 ] && ! [[ "$1" =~ ^- ]]; do
		argument=("${argument[@]}" "$1")
		shift
	done
done
if [ "$1" == "--" ]; then
	shift
	argument=("${argument[@]}" "$@")
fi
# ---------------------------------

# First argument is mandatory.
if [[ -z "${argument[0]}" ]]; then
	ShowHelp
	exit 1
fi

# Initialize variables.
SOURCE_DIR="${argument[0]}"
TARGET="all"
BUILD_SUBDIR="cmake-build-debug"
BUIlD_OPTIONS="--clean-first"

# When second argument is not given all targets are build as the default.
if [[ ! -z ${argument[1]} ]]; then
	TARGET="${argument[1]}"
	BUIlD_OPTIONS=""
fi

if [[ "$(uname -s)" == "CYGWIN_NT"* ]]; then
	echo "Windows NT detected."
	# "P:\Qt\5.15.2\mingw81_64\bin"
	# CMAKE_BIN="/cygdrive/p/Qt/Tools/CMake_64/bin/cmake"
	CMAKE_BIN="CMD /C P:\Qt\Tools\CMake_64\bin\cmake.exe"
	BUILD_DIR="$(cygpath -aw "${SOURCE_DIR}/${BUILD_SUBDIR}")"
	# Configure
	${CMAKE_BIN} -B "${BUILD_DIR}" --config "$(cygpath -aw "${SOURCE_DIR}")" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
	# Build debug
	${CMAKE_BIN} --build "${BUILD_DIR}" --target "${TARGET}" ${BUIlD_OPTIONS}
else
	# Make cmake find local Qt version.
	if $LOCAL_QT ; then
		QT_LIB_DIR="$(find "${LOCAL_QT_ROOT}" -maxdepth 2 -type d -name "gcc_64")"
		if [[ ! -z "${QT_LIB_DIR}" ]] ; then
			BUILD_SUBDIR="${BUILD_SUBDIR}-local"
			WriteLog "QT library directory '${QT_LIB_DIR}'"
			export CMAKE_PREFIX_PATH="${QT_LIB_DIR}:${CMAKE_PREFIX_PATH}"
		else
			WriteLog "No local QT library directory found in '${LOCAL_QT_ROOT}'"
		fi
	fi
	CMAKE_BIN="cmake"
	BUILD_DIR="${SOURCE_DIR}/${BUILD_SUBDIR}"
	# Configure
	${CMAKE_BIN} -B "${BUILD_DIR}" --config "${SOURCE_DIR}" -DCMAKE_BUILD_TYPE=Debug
	# Build debug
	${CMAKE_BIN} --build "${BUILD_DIR}" --target "${TARGET}" ${BUIlD_OPTIONS}
fi
