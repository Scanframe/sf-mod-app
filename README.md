# Qt Concepts

C++ concepts for proofing for me needed concepts using the Qt framework.

## General Building (Compiling/Linking) 

### Linux Build

Use the `build.sh` to call the `cmake` command having the needed options. 
All projects are configured to drop the result into the **`./bin`** directory.

Usage is `./build.sh <directory>`.<br>
Where the directory is any with a `CMakelists.txt` project file in it.

#### Windows Build

Install **Cygwin** to build this project the same way as for Linux using the same bash script.

>For installing **Cygwin** see the git repository [Cygwin Bash Scripts](https://git.scanframe.com/shared/bin-bash)
>This allows to call the bash `build.sh` to be called in side Windows.

# Projects

## Concept Project 1: Shared Library Loading at Runtime


The [`rt-shared-lib`](./-/tree/master/rt-shared-lib) directory is a proof of 
concept on loading shared libraries at runtime.
Each library containing an different implementation of a virtual interface class. 

## Concept Project 2: Serializing UI Forms at Runtime

The [`rt-ui-serialize`](./-/tree/master/rt-ui-serialize) directory is a proof of
This proof of concept that UI elements on a form can be serialized to a file and 
recreated by reading by the same file.
