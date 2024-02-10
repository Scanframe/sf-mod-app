#!/bin/bash

SCRIPT_DIR="$(cd "$( dirname "${BASH_SOURCE[0]}")" && pwd)"

# Function which runs the docker build.sh script.
function docker_run {
	local IMG_NAME HOSTNAME
	# Set the image name to be used.
	IMG_NAME="nexus.scanframe.com:8090/gnu-cpp:dev"
	# Hostname for the docker container.
	HOSTNAME="cpp-builder"
#		--env DISPLAY \
#		--volume "${HOME}/.Xauthority:/home/user/.Xauthority:ro" \
	docker run \
		--rm \
		--interactive \
		--tty \
		--privileged \
		--net=host \
		--env LOCAL_USER="$(id -u):$(id -g)" \
		--volume "${SCRIPT_DIR}:/mnt/project:rw" \
		--volume "${SCRIPT_DIR}/cmake-build-docker:/mnt/project/cmake-build:rw" \
		--workdir "/mnt/project/" \
		"${IMG_NAME}" "${@}"
}


# Execute the build script from the Docker image.
docker_run /bin/bash
#docker_run /mnt/project/build.sh "${@}"

