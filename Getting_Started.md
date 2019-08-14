
# The HISSTools Impulse Response Toolbox (HIRT) — Getting Started


Provided you are already set up with a development environment and the Max SDK (http://cycling74.com/downloads/sdk/) the following should be enough to get you started.
If you have any questions please contact: ajharker@gmail.com.


## Compiling the Max Externals in XCode:

Only two steps should be necessary to compile the code on your machine.

1. Set the correct path to the c74support folder of the Max SDK - this is done in the HIRT.xconfig file (currently expected at ../max-sdk-8.0.3/source/c74support).
2. If the SDK has changed location then you will need to relocate the MaxAudioAPI.framework from the XCode project (Get Info -> Choose).


## Compiling the Max Externals in Visual Studio:

You must make sure the SDK components are in the search path in the right location, or that the relevant locations are identified correctly in the search paths, or by editing the .props files.
The easiest way to do this is to copy the c74support folder from the Max SDK to ../Max6_support/c74support.

Otherwise you must make sure that these two folders are in the search path:

c74support/max-includes
c74support/msp-includes

and also that dllmain_win.c is correctly added to each project.


## Using the Source Outside of XCode or Microsoft Visual Studio:

If trying to compile any of the source without the included XCode/VS Projects, you need to add the following paths to your header search paths:

HISSTools_IR_Toolbox_Dependencies
HISSTools_IR_Toolbox_Dependencies/HISSTools_FFT
HISSTools_IR_Toolbox_Dependencies/HISSTools_Multichannel_Convolution

The HISSTools_FFT and HISSTools_Multichannel_Convolution folders will compile without the Max SDK, but have some dependencies on the enclosing folder.

The following files have dependencies on the Max SDK.

- The HIRT object source files
- HIRT_Buffer_Access.c
- HIRT_Buffer_Access.h
- HIRT_Common_Attribute_Setup.h
- HIRT_Common_Attribute_Vars.h

The other core files with a HIRT_ prefix should compile on pretty much anything.
