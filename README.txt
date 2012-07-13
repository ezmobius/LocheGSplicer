Introduction:
-------------

LocheGSplicer is a splicer program intended to splice together multiple gcode
files for use with 3D Printers that have multiple extruders.

Installation:
-------------

To build the source code for this application, the following libraries are required:
 - Qt Libraries of version 4.7.4 or later (http://qt.nokia.com/downloads/downloads#qt-lib)
 - CMake version 2.8 or later (http://www.cmake.org/cmake/resources/software.html)

Once the necessary libraries have been installed, open CMakeLists.txt using CMake and
configure the project for your platform.  I recommend building the binaries in a sub
folder "build", this will keep all temporarily generated files from mixing with the
primary source files.