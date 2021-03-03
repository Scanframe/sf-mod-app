# Qt Concepts

C++ concepts for proofing for me needed concepts using the Qt framework.

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

#### Windows Build

Install **Cygwin** to build this project the same way as for Linux using the same bash script.

>For installing **Cygwin** see the git repository [Cygwin Bash Scripts](https://git.scanframe.com/shared/bin-bash)
>This allows to call the bash `build.sh` to be called in side Windows.

## Projects

### Concept Project 1: Shared Library Loading at Runtime

The [`rt-shared-lib`](rt-shared-lib) directory is a proof of 
concept on loading shared libraries at runtime.
Each library containing an different implementation of a virtual interface class. 

### Concept Project 2: Custom UI Plugin

The [`custom-ui-plugin`](custom-ui-plugin) directory is an example from QT to understand how UI 
designer plugin is created, installed (designer) and used (application). 

### Concept Project 3: Serializing UI Forms at Runtime

The [`rt-ui-serialize`](rt-ui-serialize) directory is a proof of
concept that UI elements on a form can be serialized to a file and 
recreated by reading by the same file.

### Concept Project 4: Event Handling by Scripts  

The [`script-engine`](script-engine) directory is a proof of concept that application 
events timed and initiated from within the application can be handled by scripts.
Also demonstrates a highlighting editor for javascript.

### Concept Project 5: Unit Testing in Projects  

The [`unit-test`](unit-test) directory is a proof of concept that application 
can have unit tests in `CMakelists.txt` files using different unit test libraries.

