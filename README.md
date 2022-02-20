HISSTools Impulse Response Toolbox (HIRT)
=========================================

A toolbox for convolution, deconvolution and other impulse response related tasks.

![](HISSTools_Packaging/HISSTools%20Impulse%20Response%20Toolbox%20(HIRT)/icon.png)

By **Alex Harker** and **Pierre Alexandre Tremblay** with **Pete Dowling**<br/>
**CeReNeM** - The University of Huddersfield

## About

The HISSTools Impulse Response Toolbox (HIRT) is a set of tools for working with convolution and impulse responses in Max. This set of object addresses various tasks, including measuring impulse responses, spectral display from realtime data/ buffers, and buffer-based convolution, deconvolution and inversion.

The HIRT is distributed freely, under a Modified BSD License.<br>

## Release / Installation

The HISSTools Impulse Response Toolbox is now released via Cycling74 package manager for Max.<br>
If you are looking to install the current release you are advised to do so via the Max Package Manager.<br>
The current version is 2.1.0. 

## Compiling the Max Externals in XCode / MSVS:

There are provided projects/solutions for XCode and Microsoft Visual Studio.<br>
Once the requirements are satisfied building from the projects/solutions should be straightforward.

### Requirements

The projects are configured to expect the max-base-sdk repo (https://github.com/Cycling74/max-sdk-base) to be located in a parallel location (i.e. with the two repos placed in the same parent folder). 

If required this configuration can be altered in:

Config_HIRT.xcconfig (Xcode)
Config_HIRT_Win_Debug_64bit.props (MSVS)
Config_HIRT_Win_Release_64bit.props (MSVS)

## Compiling in other Environments:

- There are a number of required library and search paths to compile the externals as they are not single file sources.
- We would be happy to consider submissions of methods for other build systems (e.g. CMake).

## Compiling outside of the HIRT:

The following files have dependencies on the Max SDK.

- HISSTools_IR_Toolbox_Objects/*
- HIRT_Buffer_Access.cpp
- HIRT_Buffer_Access.hpp
- HIRT_Common_Attribute_Setup.hpp
- HIRT_Common_Attribute_Vars.hpp
- HISSTools_IR_Toolbox_Dependencies/AH_Atomic.h
- HISSTools_IR_Toolbox_Dependencies/AH_Memory_Swap.h
- HISSTools_IR_Toolbox_Dependencies/ibuffer.hpp
- HISSTools_IR_Toolbox_Dependencies/ibuffer_access.hpp
- HISSTools_IR_Toolbox_Dependencies/ibuffer_access.cpp

Other files should compile on Mac OS / Windows / Linux.

## Contact:

* a.harker@hud.ac.uk / p.a.tremblay@hud.ac.uk
* http://thehiss.org/

Enjoy,

Alex Harker, Pierre Alexandre Tremblay and Pete Dowling
