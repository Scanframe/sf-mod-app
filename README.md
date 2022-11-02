# Modular C++ Application &amp; Library (using Qt6)

The concepts-directory contains projects used to proof if what is needed is feasible.
C++ concepts for proofing for me needed concepts using the Qt framework.
Common basic libraries are not Qt dependent.
This makes building non Gui application modules without Qt possible.  

## Prerequisites

Before building some packages need to be installed:

* cmake (Linux) _(Required)_
* libopengl0 (Linux) _(Required)_
* libgl1-mesa-dev (Linux) _(Stops cmake warnings)_
* libxkbcommon-dev (Linux) _(Stops cmake warnings)_
* libxkbfile-dev (Linux) _(Stops cmake warnings)
* libvulkan-dev (Linux) _(Stops cmake warnings)_
* libssl-dev (Linux) _(Required)_
* gcc or gcc-12(Linux) _(Required)_
* g++ or g++-12 (Linux) _(Required)_
* doxygen (Linux, Cygwin) _(Required)_
* graphviz (Linux, Cygwin) _(Required)_

### Linux

```bash
sudo apt install libopengl0 libgl1-mesa-dev libxkbcommon-dev libxkbfile-dev libvulkan-dev doxygen graphviz
```

### Windows Build (Cross Compile & Run from Wine)

To cross compile for Windows-64bit the MinGW 64bit c++ compiler must be installed.

```bash
sudo apt install g++-mingw-w64-x86-64
```

#### Work-around "9.3-posix" error (Ubuntu 22.04 LTS)

For some reason WineHQ-staging (stable not present yet) references to directory `/usr/lib/gcc/x86_64-w64-mingw32/9.3-posix` 
which is not  it is not available but `/usr/lib/gcc/x86_64-w64-mingw32/10-posix` is.

To fix this a symbolic link need to be created for the `9.3-posix` directory pointing to `10-posix` for now.

```bash
sudo ln -s  10-posix  /usr/lib/gcc/x86_64-w64-mingw32/9.3-posix
```


#### Preparing Windows Qt6 for Linux 

To access the needed Windows build DLL's and binaries for compiling the next directory is only copied to a 
Linux directory (`~/lib/QtWin/<qtversion>/mingw_64`).

The script `cross-windows-fix.sh` which creates links to the needed executable binaries required by the CMake files.
It also appends some to Cmake files to allow them to compile.

Running the application from wine using the `wine-exec.sh` script a Wine version 7 or later is required.
For this run the next line to accomplish that.
```bash 
wget -q https://dl.winehq.org/wine-builds/winehq.key -O- | apt-key add -
apt-add-repository "deb https://dl.winehq.org/wine-builds/$(lsb_release -is | tr '[:upper:]' '[:lower:]')/ $(lsb_release -cs) main"
apt install --install-recommends winehq-stable
```

### Windows Cygwin

```bash
apt-cyg install doxygen graphviz
```

For Cygwin a script `apt-cyg` it there to install packages from the command line. 

## General Building (Compiling/Linking) 

### Linux Build

Use the `build.sh` to call the `cmake` command having the needed options. 
All projects are configured to drop the result into the **`./bin`** directory.

The usage is `./build.sh <directory>`.<br>
Where the directory is any with a `CMakelists.txt` project file in it.

On Ubuntu 20.04 LTS in order to use the GCC 10 compiler the next packages need to be installed.

```bash
sudo apt install gcc-10 gcc-10-base gcc-10-doc g++-10 libstdc++-10-dev libstdc++-10-doc
```
Check with this command the C++20 GNU defined values.  
```bash
g++-10 -x c++ -std=c++2a -dM -E - </dev/null | grep -i GNU
```

### Windows Build

Install **Cygwin** to build this project the same way as for Linux using the same bash script.

>For installing **Cygwin** see the git repository [Cygwin Bash Scripts](https://git.scanframe.com/shared/bin-bash)
>This allows to call the bash `build.sh` to be called in side Windows.

