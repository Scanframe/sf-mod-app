#!/usr/bin/env bash
#set -x

# Writes to stderr.
#
function WriteLog()
{
	echo "$@" 1>&2;
}

# Get this script's directory.
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
# Form the binary target directory for cross Windows builds.
DIR_BIN_WIN="$(realpath "${DIR}/binwin")"
# Location of MinGW DLLs.
DIR_MINGW_DLL="/usr/x86_64-w64-mingw32/lib"
# Location of MinGW DLLs 2.
DIR_MINGW_DLL2="/usr/lib/gcc/x86_64-w64-mingw32/9.3-posix"
# Location of Qt DLLs 2.
DIR_QT_DLL="$(realpath "${DIR}/../../project/PROG/Qt/6.2.0/mingw81_64/bin")"
# Wine command.
WINE_BIN="wine64"

if [[ -z "$1" ]]; then
	WriteLog \
	"Executes a cross-compiled Windows binary from the target directory.
Usage: $0 <win-exe-in-binwin-dir> [[<options>]...]

Available exe-files:
$(cd "${DIR_BIN_WIN}" && ls *.exe)
	"
	exit 1
fi

# Check if the command is available/installed.
if ! command -v "${WINE_BIN}" &> /dev/null ; then
	WriteLog "Missing '${WINE_BIN}', probably not installed."
	exit 1
fi

# Check if all directories exist.
for DIR_NAME in  "${DIR_BIN_WIN}" "${DIR_MINGW_DLL}" "${DIR_MINGW_DLL2}" "${DIR_QT_DLL}" ; do
	if [[ ! -d "${DIR_NAME}" ]]; then
		WriteLog "Missing directory '${DIR_NAME}', probably something is not installed."
		exit 1
	fi
done

# Path to executable and its DLL's.
WDIR_EXE_DLL="$(winepath -w "${DIR_BIN_WIN}")"
# Path to mingw runtime DLL's
WDIR_MINGW_DLL="$(winepath -w "${DIR_MINGW_DLL}")"
# Path to mingw runtime DLL's second path.
WDIR_MINGW_DLL2="$(winepath -w "${DIR_MINGW_DLL2}")"
# Path to QT runtime DLL's
WDIR_QT_DLL="$(winepath -w "${DIR_QT_DLL}")"
# Export the path to find the needed DLLs in.
export WINEPATH="${WDIR_EXE_DLL};${WDIR_QT_DLL};${WDIR_MINGW_DLL};${WDIR_MINGW_DLL2}"

# Execute in in its own shell to contain the temp dir change.
(cd "${DIR_BIN_WIN}" && wine "$@")