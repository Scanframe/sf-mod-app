#!/bin/bash
#set -x

# Get the bash script directory.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Set the directory the local QT root expects/
LOCAL_QT_ROOT="${HOME}/lib/Qt"
# Amount of CPU cores to use for compiling.
CPU_CORES_TO_USE="$(($(nproc --all) -1))"

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
	WriteLog "Usage: ${0} [<options>] <sub-dir> [<target>]

  -d : Debug: Show executed commands rather then executing them.
  -c : Cleans build targets first (adds build option '--clean-first')
  -t : Add tests to the build configuration.
  -w : Cross compile Windows on Linux using MinGW.
  -s : Build using Visual Studio
  -m : Create build directory and makefiles only.
  -b : Build target only.
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

# Detect windows using the cygwin 'uname' command.

if [[ "$(uname -s)" == "CYGWIN_NT"* ]] ; then
	WriteLog "Windows NT detected..."
	FLAG_WINDOWS=true
	EXEC_SCRIPT="$(mktemp --suffix .bat)"
else
	WriteLog "Linux detected..."
	FLAG_WINDOWS=false
	EXEC_SCRIPT="$(mktemp --suffix .sh)"
	chmod +x "${EXEC_SCRIPT}"
fi

# Initialize arguments and switches.
FLAG_DEBUG=false
FLAG_CONFIG=false
FLAG_BUILD=false
# Flag for cross compiling for Windows from Linux.
FLAG_CROSS_WINDOWS=false
# Flag for when using Visual Studio
FLAG_VISUAL_STUDIO=false
# Initialize the config options.
CONFIG_OPTIONS="-L"
# Initialize the build options.
BUIlD_OPTIONS=
# Initialize the target.
TARGET="all"
# Additional Cmake make command line options.
declare -A CMAKE_DEFS
# Default profile is debug.
CMAKE_DEFS['CMAKE_BUILD_TYPE']='Debug'
# Default build dynamic libraries.
CMAKE_DEFS['BUILD_SHARED_LIBS']='ON'

# Parse all options and arguments.
# ---------------------------------

argument=()
while [ $# -gt 0 ] && [ "$1" != "--" ]; do
	while getopts "dhcbtmws" opt; do
		case $opt in
			h)
				ShowHelp
				exit 0
				;;
			d)
				FLAG_DEBUG=true
				;;
			c)
				WriteLog "Clean first enabled"
				BUIlD_OPTIONS="${BUIlD_OPTIONS} --clean-first"
				;;
			m)
				WriteLog "Create build directory and makefiles only"
				FLAG_CONFIG=true
				;;
			b)
				WriteLog "Build the given target only"
				FLAG_BUILD=true
				;;
			t)
				WriteLog "Include test builds"
				CMAKE_DEFS['SF_BUILD_TESTING']='ON'
				;;
			w)
				if [[ ${FLAG_WINDOWS} = false ]] ; then
					WriteLog "Cross compile for Windows"
					if [[ ! ${FLAG_WINDOWS} = true ]] ; then
						FLAG_CROSS_WINDOWS=true
					fi
				else
					WriteLog "Ignoring Cross compile when in Windows"
				fi
				;;
			s)
				if [[ ${FLAG_WINDOWS} = true ]] ; then
					WriteLog "Using Visual Studio Compiler"
					FLAG_VISUAL_STUDIO=true
				else
					WriteLog "Ignoring Visual Studio switch when in Linux"
				fi
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

# First argument is mandatory.
if [[ -z "${argument[0]}" ]]; then
	ShowHelp
	exit 1
fi

# Initialize the first part of the build directory depending on the build type (Debug, Release etc.).
BUILD_SUBDIR="cmake-build-${CMAKE_DEFS['CMAKE_BUILD_TYPE'],,}"

# Initialize variables.
SOURCE_DIR="${argument[0]}"

# Set the build-dir for the cross compile.
if [[ ${FLAG_CROSS_WINDOWS} = true ]] ; then
	CMAKE_DEFS['SF_CROSS_WINDOWS']='ON'
	BUILD_SUBDIR="${BUILD_SUBDIR}-gw"
else
	# Set the build-dir for the cross compile.
	if [[ ${FLAG_VISUAL_STUDIO} = true ]] ; then
		BUILD_SUBDIR="${BUILD_SUBDIR}-msvc"
	fi
fi

# When second argument is not given all targets are build as the default.
if [[ ! -z ${argument[1]} ]]; then
	TARGET="${argument[1]}"
fi

# When both flags are not set enable them both.
if ! ${FLAG_CONFIG} && ! ${FLAG_BUILD} ; then
	WriteLog 'Performing Config & Build in succession.'
	FLAG_CONFIG=true
	FLAG_BUILD=true
fi

if [[ ${FLAG_WINDOWS} = true ]] ; then
	CMAKE_BIN="cmake.exe"
	BUILD_DIR="$(cygpath -aw "${SCRIPT_DIR}/${BUILD_SUBDIR}")"
	if ${FLAG_VISUAL_STUDIO} ; then
		BUILD_GENERATOR="Ninja"
		# Unable to test if the cmake application is availablesince the 'VSINSTALLDIR' variable is available.
		CMAKE_BIN="%VSINSTALLDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
		# Set some needed cmake command line options for make files.
		CMAKE_DEFS['CMAKE_MAKE_PROGRAM:FILEPATH']="%VSINSTALLDIR%Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
		#CMAKE_DEFS['CMAKE_C_COMPILER:FILEPATH']="%VCToolsInstallDir%bin\Hostx64\x64\cl.exe"
		CMAKE_DEFS['CMAKE_CXX_COMPILER:FILEPATH']="%VCToolsInstallDir%bin\Hostx64\x64\cl.exe"
	else
		BUILD_GENERATOR="MinGW Makefiles"
	fi
	SOURCE_DIR="$(cygpath -aw "${SOURCE_DIR}")"
else
	# Try to use the CLion installed version of the cmake command.
	CMAKE_BIN="${HOME}/lib/clion/bin/cmake/linux/bin/cmake"
	if ! command -v "${CMAKE_BIN}" &> /dev/null ; then
		# Try to use the Qt installed version of the cmake command.
		CMAKE_BIN="${LOCAL_QT_ROOT}/Tools/CMake/bin/cmake"
		if ! command -v "${CMAKE_BIN}" &> /dev/null ; then
			CMAKE_BIN="cmake"
		fi
	fi
	BUILD_DIR="${SCRIPT_DIR}/${BUILD_SUBDIR}"
	BUILD_GENERATOR="CodeBlocks - Unix Makefiles"
fi

# Build execution script depending on the OS.
if [[ ${FLAG_WINDOWS} = true ]] ; then
	# Set time stamp at beginning of file.
	echo ":: Timestamp: $(date '+%Y-%m-%dT%T.%N')" > "${EXEC_SCRIPT}"
	# Start of echo capturing.
	{
		if ${FLAG_VISUAL_STUDIO} ; then	cat <<EOF
if not defined VisualStudioVersion (
	call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64
	echo :: MSVC v%VisualStudioVersion% vars have been set now.
) else (
	echo :: MSVC v%VisualStudioVersion% vars have been set before.
)
EOF
		fi
		# Configure
		if ${FLAG_CONFIG} ; then
			echo ":: CMake Configure"
			echo "\"${CMAKE_BIN}\" ^"
			echo "-B \"${BUILD_DIR}\" ^"
			echo "-G \"${BUILD_GENERATOR}\" ${CONFIG_OPTIONS} ^"
			for key in "${!CMAKE_DEFS[@]}" ; do
				echo "-D ${key}=\"${CMAKE_DEFS[${key}]}\" ^"
			done
			echo "\"${SOURCE_DIR}\""
		fi
		# Build/Compile
		if ${FLAG_BUILD} ; then
			echo ":: CMake Build Target"
			echo "\"${CMAKE_BIN}\" ^"
			echo "--build \"${BUILD_DIR}\" ^"
			echo "--target \"${TARGET}\" ${BUIlD_OPTIONS} ^"
			echo "-- -j ${CPU_CORES_TO_USE}"
		fi
	} >> "${EXEC_SCRIPT}"
else
	# Set time stamp at beginning of file.
	echo "# Timestamp: $(date '+%Y-%m-%dT%T.%N')" > "${EXEC_SCRIPT}"
	# Start of echo capturing.
	{
		# Configure
		if [[ ${FLAG_CONFIG} = true ]] ; then
			echo "# CMake Configure"
			echo "\"${CMAKE_BIN}\" \\"
			echo "	-B \"${BUILD_DIR}\" \\"
			echo "	-G \"${BUILD_GENERATOR}\" ${CONFIG_OPTIONS} \\"
			for key in "${!CMAKE_DEFS[@]}" ; do
				echo "	-D ${key}=\"${CMAKE_DEFS[${key}]}\" \\"
			done
			echo "	\"${SOURCE_DIR}\""
		fi
		# Build/Compile
		if [[ ${FLAG_BUILD} = true ]] ; then
			echo "# CMake Build Target"
			echo "\"${CMAKE_BIN}\" \\" ;
			echo "	--build \"${BUILD_DIR}\" \\"
			echo "	--target \"${TARGET}\" ${BUIlD_OPTIONS} \\"
			echo "	-- -j ${CPU_CORES_TO_USE}"
		fi
	} >> "${EXEC_SCRIPT}"
fi

# Execute the script or write it to the log out when debugging.
if [[ ${FLAG_DEBUG} = true ]] ; then
	WriteLog "Script content ${EXEC_SCRIPT}"
	WriteLog '------------------------------'
	WriteLog "$(cat "${EXEC_SCRIPT}")"
	WriteLog '------------------------------'
else
	WriteLog "Executing Script: ${EXEC_SCRIPT}"
	if [[ ${FLAG_WINDOWS} = true ]] ; then
		CMD /C "$(cygpath -w "${EXEC_SCRIPT}")"
	else
		exec "${EXEC_SCRIPT}"
	fi
fi
