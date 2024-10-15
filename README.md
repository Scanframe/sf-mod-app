# Modular C++ Application &amp; Library (using Qt6)

The concepts-directory contains projects used to proof if what is needed is feasible.
C++ concepts for proofing for me needed concepts using the Qt framework.
Common basic libraries are not Qt dependent.
This makes building non Gui application modules without Qt is also possible.  

## Prerequisites

Easiest is to prepare a Linux system by following the 
[cpp.Dockerfile](https://git.scanframe.com/docker/gitlab-runner/-/blob/main/builder/cpp.Dockerfile "Link to repository")
from the [gitlab-runner repository](https://git.scanframe.com/docker/gitlab-runner "Docker image build repository."). 

An image can be pulled from `nexus.scanframe.com/gnu-cpp:24.04-6.7.2` or [docker.com**](https://hub.docker.com/u/avolphen) 
by `avolphen/gnu-cpp:latest`.  
Where the tag **24.04** is the Ubuntu-LTS version and **6.7.2** the Qt-version.

The `build.sh` a symbolic link to `cmake/lib/bin/build.sh`, part of the [`cmake-lib`](https://git.scanframe.com/library/cmake-lib) 
common repository to build Qt projects. 

To install required packages for non-docker usage `./build.sh --required` is called also from a Cygwin-bash command line for 
Windows packages using `winget`. (see [Cygwin bash scripts](https://git.scanframe.com/shared/bin-bash)) 

The `docker-build.sh` script forwards commands to the `build.sh` script to the container.  
To operate faster a running container can be started using `./docker-build.sh start` command.  
The script check if the container is running attaches to it to execute the command instead of running it.  
To use **JetBrains Gateway** start the container using command `./docker-build.sh sshd` which starts an 
ssh-service on port 3022 which can be connected to with user "**user**" and password "**user**".

Listing of `docker-build.sh` commands.

```text
Same as 'build.sh' script but running from Docker image 'nexus.scanframe.com/gnu-cpp:24.04-6.7.2' but allows Docker specific commands.

Usage: docker-build.sh [command] <args...>
  pull      : Pulls the docker image 'nexus.scanframe.com/gnu-cpp:24.04-6.7.2' from the Docker registry.
  run       : Runs a command as user 'user' in the container using Docker command
              'run' or 'exec' depending on a running container in the background.
  start     : Starts/Detaches a container named 'cpp_builder' in the background.
  attach    : Attaches to the  in the background running container named 'cpp_builder'.
  status    : Returns info of the running container 'cpp_builder' in the background.
  stop      : Stops the container named 'cpp_builder' running in the background.
  kill      : Kills the container named 'cpp_builder' running in the background.
  versions  : Shows versions of most installed applications within the container.
  sshd      : Starts sshd service on port 3022 to allow remote control.

Set environment variable 'DOCKER_BUILD=1' for using 'docker' as offset in the build directory to prevent mixing host build directories.
When a the container is detached it executes the 'build.sh' script by attaching to the container which is much faster.
```

Listing of `build.sh` options.  
This script is used to execute the workflows from the GitLab pipelines.

```text
Executes CMake commands using the 'CMakePresets.json' and 'CMakeUserPresets.json' files
of which the first is mandatory to exist.

Usage: build.sh [<options>] [<presets> ...]
  -h, --help       : Shows this help.
  -d, --debug      : Debug: Show executed commands rather then executing them.
  -i, --info       : Return information on all available build, test and package presets.
  -s, --submodule  : Return branch information on all Git submodules of last commit.
  -p, --package    : Create packages using a preset.
  --required       : Install required Linux packages using debian apt package manager.
  -m, --make       : Create build directory and makefiles only.
  -f, --fresh      : Configure a fresh build tree, removing any existing cache file.
  -C, --wipe       : Wipe clean build tree directory by removing all contents from the build directory.
  -c, --clean      : Cleans build targets first (adds build option '--clean-first')
  -b, --build      : Build target and make config when it does not exist.
  -B, --build-only : Build target only and fail when the configuration does note exist.
  -t, --test       : Runs the ctest application using a test-preset.
  -r, --regex      : Regular expression on which test names are to be executed.
  -w, --workflow   : Runs the passed work flow presets.
  -l, --list-only  : Lists the ctest test defined application by the project and selected preset.
  -n, --target     : Overrides the build targets set in the preset by a single target.
  Where <sub-dir> is the directory used as build root for the CMakeLists.txt in it.
  This is usually the current directory '.'.
  When the <target> argument is omitted it defaults to 'all'.
  The <sub-dir> is also the directory where cmake will create its 'cmake-build-???' directory.

  Examples:
    Get all project presets info: ./build.sh -i
    Make/Build project: ./build.sh -b my-build-preset1 my-build-preset2
    Test project: ./build.sh -t my-test-preset1 my-test-preset2
    Make/Build/Test/Pack project: ./build.sh -w my-workflow-preset
```

## Cygwin for Builds from Windows

Install **Cygwin** to build this project the same way as for Linux using the same bash script.

>For installing **Cygwin** see the git repository [Cygwin Bash Scripts](https://git.scanframe.com/shared/bin-bash).  
>This allows to call the bash `build.sh` to be called in side Windows.


## Compiler Issues and DLL

Check with these commands if the C++20 is supported on compilers.

```bash
g++ -x c++ -std=c++20 -dM -E - </dev/null | grep __cplusplus
x86_64-w64-mingw32-g++ -x c++ -std=c++20 -dM -E - </dev/null | grep __cplusplus 
```

The correct version of `libstdc++-6.dll` is required and depends on the compiler.
Qt on Windows as of (2024-10-09) only supports GNU compiler version 10 and 11 
which comes with a different `libstdc++-6.dll` then for version 13.

On Linux running Wine this is no problem setting the `WINEPATH` environment 
variable to look for the correct location first but running the Linux compiled
version on actual Windows requires `libstdc++-6.dll` to be copied into the 
application or its `lib` directory.