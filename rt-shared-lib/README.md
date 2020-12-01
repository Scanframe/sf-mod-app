# Shared Library Loading at Runtime

This project directory is a proof of concept on loading shared libraries at runtime.
It consists of 3 builds:

1) The interface class (pure virtual) shared library  
2) The application linking the interface class shared library (`rt-shared-lib.bin`).
3) The implementation of a derived class of the interface class in a shared library (`lbrt-iface.so`).

## Interface Class Shared Library (iface) 

The interface shared library implements a C++ pure virtual base class with some static member functions 
for registering implementation classes and retrieving information loaded implementation libraries.

## Implementation Shared Library (app)

Application loading the dynamic library and creating a implementation 
class of which a member function is called from. 

## Implementation Shared Library (impl-a)

The implementation library is derived class from the virtual base class in the interface library. 
It implements all virtual member functions from the interface class.

## Building the Projects

To build this concept 2 steps are needed.
Because of a common miscellaneous library (`sf-misc`) for all projects is used. 

#### Step 1 
First build the common miscellaneous dependent dynamic 
library '**libsf-misc**' with `./build.sh com` from the root of this git repository.

#### Step 2 
Second to build are the three sub projects '**iface**', '**app**' and '**impl-a**' by building 
the overarching project directory '**rt-shared-lib**' with `./build.sh rt-shared-lib`.      
