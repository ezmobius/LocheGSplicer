Licensing:
----------

Copyright (C) 2012 Jeff P. Houde (Lochemage)

This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA


Introduction:
-------------

LocheGSplicer is a splicer program intended to splice together multiple gcode
files for use with 3D Printers that have multiple extruders.


Building from Source:
---------------------
Download the source here: https://github.com/Lochemage/LocheGSplicer

LocheGSplicer uses CMake (http://www.cmake.org) to generate the project files.

Once the necessary dependancies (see below) have been installed, use CMake to
configure the project for your platform.  I recommend building the binaries in a sub
folder such as "build", this will keep all generated files from mixing with the
primary source files.


Dependancies:
-------------
 - Qt Libraries of version 4.7.4 or later (http://qt.nokia.com/downloads/downloads#qt-lib)
