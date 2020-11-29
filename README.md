# Qt Concepts

C++ concepts for proofing for me needed concepts using the Qt framework.

## General Building (Compiling/Linking) 

Use the `build.sh` to call the `cmake` command having the needed options. 
All projects are configured to drop the result into the **`./bin`** directory.

Usage is `./build.sh <directory>`.<br>
Where the directory is any with a CMakelists.txt project file in it.


## Concept 1: Shared Library Loading at Runtime

The `rt-shared-lib` directory is a proof of concept on loading shared libraries at runtime.
This consists of 3 builds:
1) The interface class (pure virtual) shared library.
2) The application linking the interface class shared library.
3) The implementation of a derived class of the interface class in a shared library.

### Interface Class Shared Library 

The interface shared library implements a C++ virtual base class with some static member functions 
for registering implementation classes and retrieving information loaded implemention libraries.

### Implementation Shared Library

The implementation library is derived from the virtual base class in the interface library 
implementing all virtual member functions.

### Building

To build this concept 2 steps are needed.

#### Step 1 
First build the common miscellaneous dependent dynamic 
library '**libsf-misc**' with `./build.sh com` from the root.

#### Step 2 
Second to build are the three sub projects '**iface**', '**app**' and '**impl-a**' by building 
the overarching project directory '**rt-shared-lib**' with `./build.sh rt-shared-lib`.      


## Concept 2: Serializing UI Forms at Runtime

This proof of concept that UI elements on a form can be serialized to a file and 
recreated by reading the same file.
