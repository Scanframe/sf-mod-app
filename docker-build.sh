#!/bin/bash

# Get the script directory.
SCRIPT_DIR="$(cd "$( dirname "${BASH_SOURCE[0]}")" && pwd)"
# Build directory used for Docker to prevent mixing.
BUILD_DIR="${SCRIPT_DIR}/cmake-build/docker"
# Set the image name to be used.
IMG_NAME="nexus.scanframe.com:8090/gnu-cpp:dev"
# Hostname for the docker container.
HOSTNAME="cpp-builder"
# Create the docker binary build directory.
mkdir -p "${SCRIPT_DIR}/cmake-build/docker"
# Function which runs the docker build.sh script.
function docker_run {
	local cmd
	cmd=(docker run)
	cmd+=(--rm --interactive --tty)
	cmd+=(--privileged)
	cmd+=(--net=host)
	cmd+=(--env LOCAL_USER="$(id -u):$(id -g)")
#	cmd+=(--env DISPLAY)
#	cmd+=(--volume "${HOME}/.Xauthority:/home/user/.Xauthority:ro")
	cmd+=(--volume "${SCRIPT_DIR}:/mnt/project:rw")
	cmd+=(--volume "${BUILD_DIR}:/mnt/project/cmake-build:rw")
	cmd+=(--workdir "/mnt/project/")
	cmd+=("${IMG_NAME}")
	"${cmd[@]}"	 "${@}"
}

if [[ $# -eq 0 ]]; then
	echo "Same as 'build.sh' script but running from Docker image '${IMG_NAME}'.

To pull the latest image for use the 'pull' command explicitly for this script.
Now entering container using bash to allow inspection.
"
	# Execute the build script from the Docker image.
	docker_run bash
else
	# When only pull has been passed pull the latest image.
	if [[ $# -eq 1  && "${1}" == "pull" ]]; then
		docker pull "${IMG_NAME}"
	fi
	# Execute the build script from the Docker image.
	docker_run /mnt/project/build.sh "${@}"
fi
