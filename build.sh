#!/bin/bash
#set -x

if [ -z "$1" ]; then
	echo "Usage: ${0} <sub-dir>"
	echo "	Where sub-dir is 'app', 'iface' and 'impl-a'"
	exit 1
fi

TARGET_DIR="$1"

# Configure
cmake -B "${TARGET_DIR}/cmake-build-debug" --config "${TARGET_DIR}" -DCMAKE_BUILD_TYPE=Debug 

# Build debug
cmake --build "${TARGET_DIR}/cmake-build-debug" --target all --clean-first

