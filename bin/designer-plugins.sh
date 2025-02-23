#!/bin/bash
#set -x

# Bailout on first error.
set -e

# Get the script directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

# Writes to stderr.
#
function WriteLog() {
	echo "$@" 1>&2
}

# Form the actual plugin target directory
target="$("${script_dir}/../cmake/lib/bin/QtLibDir.sh")/gcc_64/plugins/designer"

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
