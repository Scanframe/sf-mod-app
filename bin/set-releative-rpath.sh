#!/bin/bash
#set -x

# Binary directory.
#BIN_DIR="${DIR}/bin/lnx64"
BIN_DIR="$(pwd)"
# Command which is executed.
CMD="chrpath"
# Other command is patchelf
#   patchelf --set-rpath RPATH '$ORIGIN/path/to/library' <executable>

# Replace the RUNPATH in the ELF header to relative directories of the binary.
#

for fn in "${BIN_DIR}"/*
do
	type="$(file -ib "${fn}" | pcregrep -i -o1 '^([0-9a-z\-/]*);' )"
	if [[ "${type}" == "application/x-sharedlib" || "${type}" == "application/x-pie-executable" ]]; then
		if ${CMD} -l "${fn}" > /dev/null ; then
			echo "Modifiable rpath in (${type}): ${fn}"
			# shellcheck disable=SC2016
			${CMD} -r '${ORIGIN}/lib' "${fn}"
		else
			echo "No rpath found in (${type}): ${fn}"
		fi
	fi
done
