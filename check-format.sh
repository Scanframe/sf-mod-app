#!/usr/bin/env bash

# Get the scripts directory.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Add the directories to check.
arguments=(
	"src/com/misc/gen"
	"src/com/misc/lnx"
	"src/com/misc/win"
	"src/com/misc/tests"
	"${@}"
)
#arguments+=("com/misc/gen" "com/misc/lnx" "com/misc/win")
# Add quiet option when call from the Git pre-commit hook.
if [[ "$(ps -o comm= $PPID)" == "pre-commit" ]]; then
	# Makes the WriteLog function not use colors.
	export TERM="dumb"
	# Report only problems and totals.
	arguments+=('--quiet')
fi
# Make this script directory the current one.
pushd "${script_dir}" >/dev/null || exit 1
# Execute the script for checking.
cmake/lib/bin/clang-format.sh "${arguments[@]}" || exit 1
# Return to the initial directory.
popd || exit 1
