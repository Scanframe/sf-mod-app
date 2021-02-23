#!/bin/bash
#set -x

# Get the bash script directory.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Set the directory the local QT root expected.
LOCAL_QT_ROOT="${HOME}/lib/Qt"
# Initialize the build options.
BUIlD_OPTIONS=
# Initialize the config options.
CONFIG_OPTIONS=

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
	WriteLog "Usage: ${0} <sub-dir> [<target>] [-l]

  -l : Build using local user QT version (needs ${LOCAL_QT_ROOT} directory).
  -c : Cleans build targets first (adds build option '--clean-first')
  -t : Add tests to the build config.
  Where <sub-dir> is:
    '.', 'com', 'rt-shared-lib/app', 'rt-shared-lib/iface',
    'rt-shared-lib/impl-a', 'rt-shared-lib', 'custom-ui-plugin'
  When the <target> argument is omitted it defaults to 'all'.
  The <sub-dir> is also the directory where cmake will create its 'cmake-build-???' directory.

  Examples:
    Build all projects: ${0} .
    Same as above: ${0} . all
    Clean all projects: ${0} . clean
    Install all projects: ${0} . install
    Show all projects to be build: ${0} . help
    Build 'sf-misc' project in 'com' sub-dir only: ${0} . sf-misc
    Build 'com' project and all sub-projects: ${0} com
    Build 'rt-shared-lib' project and all sub-projects: ${0} rt-shared-lib
	"
}

# Parse all options and arguments.
# ---------------------------------

# Initialize arguments and switches.
argument=()
LOCAL_QT=false
while [ $# -gt 0 ] && [ "$1" != "--" ]; do
	while getopts "hlct" opt; do
		case $opt in
			h)
				ShowHelp
				exit 0
				;;
			l)
				WriteLog "Using local Qt version from: ${LOCAL_QT_ROOT}"
				LOCAL_QT=true
				;;
			c)
			 	WriteLog "Clean first enabled"
				BUIlD_OPTIONS="${BUIlD_OPTIONS} --clean-first"
				;;
			t)
			 	WriteLog "Include test builds"
				CONFIG_OPTIONS="${CONFIG_OPTIONS} -DBUILD_TESTING=ON"
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

# When second argument is not given all targets are build as the default.
if [[ ! -z ${argument[1]} ]]; then
	TARGET="${argument[1]}"
fi

if [[ "$(uname -s)" == "CYGWIN_NT"* ]]; then
	echo "Windows NT detected."
	# "P:\Qt\5.15.2\mingw81_64\bin"
	# CMAKE_BIN="/cygdrive/p/Qt/Tools/CMake_64/bin/cmake"
	CMAKE_BIN="CMD /C P:\Qt\Tools\CMake_64\bin\cmake.exe"
	BUILD_DIR="$(cygpath -aw "${SCRIPT_DIR}/${BUILD_SUBDIR}")"
	BUILD_GENERATOR="MinGW Makefiles"
	SOURCE_DIR="$(cygpath -aw "${SOURCE_DIR}")"
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

	#CMAKE_BIN="${HOME}/lib/clion/bin/cmake/linux/bin/cmake"
	CMAKE_BIN="cmake"
	BUILD_DIR="${SCRIPT_DIR}/${BUILD_SUBDIR}"
	BUILD_GENERATOR="CodeBlocks - Unix Makefiles"
fi

# Initialize configuration options
CONFIG_OPTIONS="${CONFIG_OPTIONS} -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=$(realpath "${SCRIPT_DIR}/..") -L"

# Configure debug
${CMAKE_BIN} -B "${BUILD_DIR}" --config "${SOURCE_DIR}" -G "${BUILD_GENERATOR}" ${CONFIG_OPTIONS}
# Build
${CMAKE_BIN} --build "${BUILD_DIR}" --target "${TARGET}" ${BUIlD_OPTIONS}
