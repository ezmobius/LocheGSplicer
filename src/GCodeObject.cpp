/*
 * LocheGSplicer
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author: Lochemage
 */


#include <GCodeObject.h>
#include <GCodeParser.h>

////////////////////////////////////////////////////////////////////////////////
GCodeObject::GCodeObject()
{
}

////////////////////////////////////////////////////////////////////////////////
GCodeObject::~GCodeObject()
{
}

////////////////////////////////////////////////////////////////////////////////
bool GCodeObject::loadFile(const QString &fileName)
{
   GCodeParser parser;

   if (!parser.loadFile(fileName))
   {
      return false;
   }

   double value = 0.0;

   // Parse the gcode file, at the same time any codes we care about will have
   // special treatment while any codes we don't care about will simply be preserved
   // and included in the final product as is.

   while (parser.parseNext())
   {
      if (parser.codeSeen('G'))
      {
         value = parser.codeValue();

         // Codes we care about:
         // 0 or 1: Extruder movement.
         // 2 or 3: Not sure what this does yet.
         // 4: Dwell
         // 28: Home all axes.
         // 90: Absolute coordinate system.
         // 91: Relative coordinate system.
         // 92: Set current position to coordinates given.

         // Codes we care about, but are not supported by Marlin:
         // 20: Set units to inches.
         // 21: Set units to millimeters.
      }
      else if (parser.codeSeen('M'))
      {
         value = parser.codeValue();

         // Codes we care about:
         // 104 or 109: Setting nozzle temp.
         // 106 or 107: Fan on/off.
         
         // Marlin custom codes:
         // 82: Set E codes absolute
         // 83: Set E codes relative while in Absolute Coordinates (G90) mode

         // Codes we care about, but are not supported by Marlin:
         // 0: Stop
      }
      else if (parser.codeSeen('T'))
      {
         // A T0 code is valid as it does not change extruders...
         if (parser.codeValueLong() != 0)
         {
            // We should not find any extruder change commands as we are assuming
            // all the gcode in any given file are for a single extruder.
            // We are unequipped to deal with this case so we must fail the load.
            return false;
         }
      }
   }

   return true;
}

////////////////////////////////////////////////////////////////////////////////
