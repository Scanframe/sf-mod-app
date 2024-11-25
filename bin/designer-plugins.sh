#!/bin/bash
#set -x

# Get the script directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

# Location of the users Qt installed files
local_qt_root="${HOME}/lib/Qt"

# Writes to stderr.
#
function WriteLog() {
	echo "$@" 1>&2
}

# Find newest local Qt version directory.
#
function GetLocalQtDir() {
	local local_qt_dir
	local_qt_dir="$(find -L "${local_qt_root}" -type d -regex ".*\/Qt\/[56]\\.[0-9]+\\.[0-9]+$" | sort --reverse --version-sort | head -n 1)"
	if [[ -z "${local_qt_dir}" ]]; then
		WriteLog "Could not find local installed Qt directory."
		exit 1
	fi
	echo "${local_qt_dir}"
}

# Form the actual plugin target directory
target="$(GetLocalQtDir)/gcc_64/plugins/designer"

if [[ ! -d "${target}" ]]; then
	WriteLog "Qt Designer plugin directory not found!"
	exit 1
else
	WriteLog "Plugin directory is: ${target}"
fi

# Declare an array of string with type
declare -a dynlibs=(
	"libsf-misc.so"
	"libsf-gii.so"
	"libsf-wgt-ascan.so"
	"libsf-wgt-bscan.so"
	"libsf-wgt-layout.so"
	"libsf-wgt-acq-ctrl.so"
	"libsf-gmi-iface.so"
	"libcustom-ui-plugin.so"
	"libtask-menu.so"
)
# Iterate the string array using for loop
for fn in "${dynlibs[@]}"; do
	# Form the source library path.
	src="${script_dir}/lnx64/lib/${fn}"
	# Checking if the source exists.
	if [[ ! -f "${src}" ]]; then
		WriteLog "Skipping non-existing: ${fn}"
	else
		ln --symbolic --force "${script_dir}/lnx64/lib/${fn}" "${target}/${fn}"
	fi

done
