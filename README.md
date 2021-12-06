# Modular C++ Application &amp; Library (using Qt6)

The concepts-directory contains projects used to proof if what is needed is feasible.
C++ concepts for proofing for me needed concepts using the Qt framework.
Common basic libraries are not Qt dependent.
This makes building non Gui application modules without Qt possible.  

## Prerequisites

Before building some packages need to be installed:

* libopengl0 (Linux) _(Required)_
* libgl1-mesa-dev (Linux) _(Stops cmake warnings)_
* libxkbcommon-dev (Linux) _(Stops cmake warnings)_
* libxkbfile-dev (Linux) _(Stops cmake warnings)
* libvulkan-dev (Linux) _(Stops cmake warnings)_
* doxygen (Linux, Cygwin) _(Required)_
* graphviz (Linux, Cygwin) _(Required)_

### Linux

```bash
sudo apt install libopengl0 libgl1-mesa-dev libxkbcommon-dev libxkbfile-dev libvulkan-dev doxygen graphviz
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


#### Windows Build

Install **Cygwin** to build this project the same way as for Linux using the same bash script.

>For installing **Cygwin** see the git repository [Cygwin Bash Scripts](https://git.scanframe.com/shared/bin-bash)
>This allows to call the bash `build.sh` to be called in side Windows.

## Projects

### Concept Project 1: Shared Library Loading at Runtime

The [`rt-shared-lib`](concept/rt-shared-lib) directory is a proof of 
concept on loading shared libraries at runtime.
Each library containing an different implementation of a virtual interface class. 

### Concept Project 2: Custom UI Plugin

The [`custom-ui-plugin`](concept/custom-ui-plugin) directory is an example from QT to understand how UI 
designer plugin is created, installed (designer) and used (application). 

### Concept Project 3: Serializing UI Forms at Runtime

The [`rt-ui-serialize`](concept/rt-ui-serialize) directory is a proof of
concept that UI elements on a form can be serialized to a file and 
recreated by reading by the same file.

### Concept Project 4: Event Handling by Scripts  

The [`script-engine`](concept/script-engine) directory is a proof of concept that application 
events timed and initiated from within the application can be handled by scripts.
Also demonstrates a highlighting editor for javascript.

### Concept Project 5: Unit Testing in Projects  

The [`unit-test`](concept/unit-test) directory is a proof of concept that application 
can have unit tests in `CMakelists.txt` files using different unit test libraries.

