# Qt Concepts

Concepts for proofing for me needed concepts using the Qt framework.

## Concept 1: Shared Library Loading at Runtime

The `rt-shared-lib` directory is a proof of concept on loading shared libraries at runtime.
This consists of 3 builds:
1) The interface class (pure virtual) shared library.
2) The application linking the interface class shared library.
3) The implementation of a derived class of the interface class in a shared library.

### Interface Class Shared Library 

The interface shared library implements a C++ virtual base class with some static member functions 
for registering implementation classes and retrieving information loaded implemention libraries.

### Implmentation Shared Library

The implementation library is derived from the virtual base class in the interface library 
implementing all virtual member functions.

### Building This Concept

Use the `build.sh` to call the `cmake` command having the needed options.
Usage is `./build.sh <sub-dir>`.

The script builds projects  
```bash
# Builds bin/libsf-misc.so
./build.sh com
# Builds bin/librt-iface.so,  bin/librt-impl-a.so and bin/librt-lib-app 
./build.sh rt-shared-lib
```

## Concept 2: Serializing UI Forms at Runtime

This proof of concept that UI elements on a form can be serialized to a file and recreated by reading the same file.
