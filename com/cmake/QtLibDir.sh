#!/bin/bash
#set -x

# Get the bash script directory.
#SCRIPT_DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
# Set the directory the local QT root expected.
LOCAL_QT_ROOT="${HOME}/lib/Qt"

# Find newest local Qt version directory.
#
function GetLocalQtDir()
{
	local LocalQtDir=""
	LocalQtDir="$(find "${LOCAL_QT_ROOT}" -type d -regex ".*\/Qt\/[56].[0-9]+.[0-9]+$" | sort --reverse --version-sort | head -n 1)"
	if [[  -z "${LocalQtDir}" ]] ; then
		WriteLog "Could not find local installed Qt directory."
		exit 1
	fi
	echo -n "${LocalQtDir}"
}

GetLocalQtDir