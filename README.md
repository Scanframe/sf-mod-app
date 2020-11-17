# Qt Concepts

Concepts for proofing needed concepts using the Qt framework.


## Concept 1: Shared Library Loading at Runtime

The `rt-shared-lib` directory is a proof of concept on loading shared libraries at runtime.
This consists of 3 builds:
1) The Shared Library.
2) The application linking the interface shared library.
3) The implementation shared library.

#### Interface Shared Library 

The interface shared library implements a C++ virtual base class with some static member functions 
for registering implementation classes and retrieving information loaded implemention libraries.

#### Implmentation Shared Library

The implementation library is derived from the virtual base class in the interface library 
implementing all virtual member functions.


## Concept 2: Serializing UI Forms at Runtime

This proof of concept that UI elements on a form can be serialized to a file and recreated by reading the same file.
