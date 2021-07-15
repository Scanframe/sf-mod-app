#!/bin/bash
#set -x

# Get the script directory.
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Location of the users Qt installed files
LOCAL_QT_ROOT="${HOME}/lib/Qt"

# Writes to stderr.
#
function WriteLog()
{
	echo "$@" 1>&2;
}

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

# Form the actual plugin target directory
TARGET="$(GetLocalQtDir)/gcc_64/plugins/designer"

if [[ ! -d "${TARGET}" ]] ; then
	WriteLog "Qt Designer plugin directory not found!"
	exit 1
else
	WriteLog "Plugin directory is: ${TARGET}"
fi

# Declare an array of string with type
declare -a MODULES=("libsf-misc.so" "libsf-gii.so" "libcustom-ui-plugin.so")
# Iterate the string array using for loop
for fn in "${MODULES[@]}" ; do

	if [[ -f "${TARGET}/${fn}" ]] ; then
		WriteLog "Skipping existing: ${fn}"
	else
		ln -s "${DIR}/bin/${fn}" "${TARGET}/${fn}"
	fi

done

