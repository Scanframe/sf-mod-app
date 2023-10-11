#!/bin/bash

SCRIPT_DIR="$(cd "$( dirname "${BASH_SOURCE[0]}")" && pwd)"
export WINEPATH="lib"
source "${SCRIPT_DIR}/../cmake/lib/bin/WineExec.sh"


