/*
 * LocheGSplicer
 * Copyright (C) 2012 Jeff P. Houde (Lochemage)
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
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
 */


#include <GCodeObject.h>
#include <GCodeParser.h>
#include <Constants.h>


////////////////////////////////////////////////////////////////////////////////
GCodeObject::GCodeObject()
{
   for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
   {
      mOffsetPos[axis] = 0.0;
   }
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
      printf("Failed to load file \'%s\'", fileName.toStdString().c_str());
      return false;
   }

   std::vector<GCodeCommand> layer;
   GCodeCommand code;
   
   double dValue = 0.0;
   long lValue = 0;
   double coordConversion = 1.0;

   // TODO: Add global preferences to specify the
   // default values for the following attributes.
   bool absoluteMode = true;
   bool absoluteEMode = true;
   double currentPos[AXIS_NUM] = {0.0,};
   double offsetPos[AXIS_NUM] = {0.0,};
   double homeOffset[AXIS_NUM_NO_E] = {0.0,};

   // Parse the gcode file, at the same time any codes we care about will have
   // special treatment while any codes we don't care about will simply be preserved
   // and included in the final product as is.
   double lastZ = currentPos[Z];
   double lastE = currentPos[E];

   while (parser.parseNext())
   {
      code.clear();
      code.command = parser.getLine();
      code.comment = parser.getComment();

      bool changeLayers = false;

      if (parser.codeSeen('G'))
      {
         lValue = 1000 + parser.codeValueLong();

         code.type = lValue;

         // Codes we care about:
         // 0 or 1: Extruder movement.
         if (lValue == GCODE_EXTRUDER_MOVEMENT0 || lValue == GCODE_EXTRUDER_MOVEMENT1)
         {
            code.hasAxis = true;

            for (int axis = 0; axis < AXIS_NUM; ++axis)
            {
               if (parser.codeSeen(AXIS_NAME[axis]))
               {
                  if (axis == E? absoluteEMode: absoluteMode)
                  {
                     currentPos[axis] = (parser.codeValue() * coordConversion) + offsetPos[axis];
                  }
                  else
                  {
                     currentPos[axis] += (parser.codeValue() * coordConversion);
                  }
               }
               code.axisValue[axis] = currentPos[axis];
            }

            if (parser.codeSeen('F'))
            {
               code.f = parser.codeValue();
               code.hasF = true;
            }

            // If we are extruding some material,
            // determine if the layer has changed.
            if (lastE < currentPos[E])
            {
               lastE = currentPos[E];

               if (lastZ < currentPos[Z])
               {
                  lastZ = currentPos[Z];
                  changeLayers = true;
               }
            }
            else if (currentPos[E] == 0.0)
            {
               lastE = 0.0;
            }
         }

         // 4: Dwell
         if (lValue == GCODE_DWELL)
         {
            if (parser.codeSeen('S'))
            {
               code.s = parser.codeValue();
               code.hasS = true;
            }
            if (parser.codeSeen('P'))
            {
               code.p = parser.codeValue();
               code.hasP = true;
            }
         }

         // 28: Home axes.
         if (lValue == GCODE_HOME)
         {
            code.hasAxis = true;

            // The extruder position does not change from this command.
            code.axisValue[E] = currentPos[E];

            bool foundAny = false;
            for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
            {
               if (parser.codeSeen(AXIS_NAME[axis]))
               {
                  foundAny = true;

                  currentPos[axis] = 0.0;
                  offsetPos[axis] = 0.0;

                  // Not sure if this is correct, as it is contrary to Marlin's
                  // documentation, but according to the source code, this is what
                  // happens when a value is specified along with the axis to home.
                  dValue = parser.codeValue();
                  if (dValue != 0.0)
                  {
                     offsetPos[axis] = dValue + homeOffset[axis];
                  }
               }

               code.axisValue[axis] = currentPos[axis];
            }

            // If the code was used without specifying any
            // particular axis, reset all axes instead.
            if (!foundAny)
            {
               for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
               {
                  code.axisValue[axis] = 0.0;
                  currentPos[axis] = 0.0;
                  offsetPos[axis] = 0.0;
               }
            }
         }

         // 90: Absolute coordinate system.
         if (lValue == GCODE_ABSOLUTE_COORDS)
         {
            absoluteMode = true;

            // Everything is converted to absolute coordinates, so any
            // gcode that changes the coordinate system will not be
            // propagated to the final result.
            continue;
         }

         // 91: Relative coordinate system.
         if (lValue == GCODE_RELATIVE_COORDS)
         {
            absoluteMode = false;

            // Everything is converted to absolute coordinates, so any
            // gcode that changes the coordinate system will not be
            // propagated to the final result.
            continue;
         }

         // 92: Set current position to coordinates given.
         if (lValue == GCODE_CURRENT_POSITION)
         {
            for (int axis = 0; axis < AXIS_NUM; ++axis)
            {
               offsetPos[axis] = 0.0;

               if (parser.codeSeen(AXIS_NAME[axis]))
               {
                  // The offset is always the difference between our actual
                  // current position and the position our gcode is
                  // specifying as the current, since we don't care what
                  // the gcode thinks is the current position, we only
                  // care about our current actual position in a global
                  // sense.  Our offset is used to convert any subsequent
                  // position given to an actual global position.
                  offsetPos[axis] = currentPos[axis] - (parser.codeValue() * coordConversion);
               }
            }

            // This command is not propagated to our final result as everything
            // is converted to global absolute coordinates.
            continue;
         }

         // Codes we care about, but are not supported by Marlin:
         // 20: Set units to inches.
         if (lValue == GCODE_INCHES_MODE)
         {
            // If we find this code, it means we need to convert any subsequent
            // gcode coordinate from inches back to millimeters since all of
            // our internal coordinates are in mm.
            coordConversion = INCHES_TO_MM;
         }

         // 21: Set units to millimeters.
         if (lValue == GCODE_MILLIMETERS_MODE)
         {
            // If we find this code, we can undo our conversion from inches
            // to millimeters if necessary.
            coordConversion = 1.0;
         }
      }
      else if (parser.codeSeen('M'))
      {
         lValue = 2000 + parser.codeValueLong();

         code.type = lValue;

         // Codes we care about:
         // Marlin custom codes:
         // 82: Set E codes absolute
         if (lValue == MCODE_E_ABSOLUTE_COORDS)
         {
            absoluteEMode = true;
         }

         // 83: Set E codes relative while in Absolute Coordinates (G90) mode
         if (lValue == MCODE_E_RELATIVE_COORDS)
         {
            absoluteEMode = false;
         }

         // 206: Set additional homing offset.
         if (lValue == MCODE_SET_HOMING_OFFSET)
         {
            for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
            {
               if (parser.codeSeen(AXIS_NAME[axis]))
               {
                  homeOffset[axis] = parser.codeValue();
               }
            }
         }

         // 221: Set extruder speed factor.
         if (lValue == MCODE_SET_EXTRUDE_FACTOR)
         {
            if (parser.codeSeen('S'))
            {
               // factor in percentage.
               // It's possible that I may have to account for this
               // when calculating my own custom retraction values.
            }
         }

         // Codes we care about, but are not supported by Marlin:
         // 0: Stop
         if (lValue == MCODE_EMERGENCY_STOP)
         {
            // Not sure if this code will actually appear in any gcode file,
            // but just in case we don't want it to be included in our
            // final splice.
            continue;
         }
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
      // If we get here then we found no valid code type,
      // if we also don't have a comment then it is a blank line
      // and can be discarded.
      else if (code.comment.isEmpty())
      {
         continue;
      }

      // If we have changed layers, put the current layer's data into the
      // layer stack and begin a new one.
      if (changeLayers)
      {
         mData.push_back(layer);
         layer.clear();
      }

      // Add the current code value to the current layer.
      layer.push_back(code);
   }

   // Add our final layer.
   if (!layer.empty())
   {
      mData.push_back(layer);
   }

   return true;
}

////////////////////////////////////////////////////////////////////////////////
void GCodeObject::setOffsetPos(double x, double y, double z)
{
   mOffsetPos[X] = x;
   mOffsetPos[Y] = y;
   mOffsetPos[Z] = z;
}

////////////////////////////////////////////////////////////////////////////////
const double* GCodeObject::getOffsetPos() const
{
   return mOffsetPos;
}

////////////////////////////////////////////////////////////////////////////////
int GCodeObject::getLevelCount() const
{
   return (int)mData.size();
}

////////////////////////////////////////////////////////////////////////////////
const std::vector<GCodeCommand>& GCodeObject::getLevel(int levelIndex)
{
   return mData[levelIndex];
}

////////////////////////////////////////////////////////////////////////////////
