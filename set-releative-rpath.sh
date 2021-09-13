#!/bin/bash
#set -x

# Get the script directory.
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
# Binary directory.
BIN_DIR="${DIR}/bin"
# Command which is executed.
CMD="chrpath"
# Other command is patchelf
#   patchelf --set-rpath RPATH '$ORIGIN/path/to/library' <executable>

# Location of the users Qt installed files
LOCAL_QT_ROOT="${HOME}/lib/Qt"

# Writes to stderr.
#
function WriteLog()
{
	echo "$@" 1>&2;
}

if ! command -v "${CMD}" &> /dev/null
then
    echo "<the_command> could not be found and my should be installed. (apt install ${CMD})"
    exit
fi

# Find newest local Qt version directory.
#
function GetLocalQtDir()
{
	local LocalQtDir
	LocalQtDir="$(find "${LOCAL_QT_ROOT}" -type d -regex ".*\/Qt\/[56].[0-9]+.[0-9]+$" | sort --reverse --version-sort | head -n 1)"
	if [[  -z "${LocalQtDir}" ]] ; then
		WriteLog "Could not find local installed Qt directory."
		exit 1
	fi
	echo "${LocalQtDir}"
}

# Check if Qt library exists relative to the binary directory.
if [[ -f "${BIN_DIR}/libqt" ]] ; then
	WriteLog "Could '${BIN_DIR}/libqt' local installed Qt directory or symlink."
		exit
fi

# Replace the RUNPATH in the ELF header to relative directories of the binary.
#
for fn in "${BIN_DIR}"/*.bin "${BIN_DIR}"/*.so
do
	WriteLog "Modfying file: ${fn}"
	# ${CMD} -l "${fn}"
	${CMD} -r '$ORIGIN:$ORIGIN/libqt/lib' "${fn}"
done

