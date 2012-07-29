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

#include <QtGui/QtGui>

#include <math.h>


////////////////////////////////////////////////////////////////////////////////
GCodeObject::GCodeObject(const PreferenceData& prefs)
   : mPrefs(prefs)
   , mExtruderIndex(0)
   , mAverageLayerHeight(0.0)
{
   for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
   {
      mMinBounds[axis] = 0.0;
      mMaxBounds[axis] = 0.0;
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
      mError = "File not found.";
      return false;
   }

   QWidget* progressWindow = new QWidget();
   progressWindow->setWindowFlags(Qt::CustomizeWindowHint);
   QVBoxLayout* progressLayout = new QVBoxLayout();
   QProgressDialog* progressDialog = new QProgressDialog("Importing...", "Cancel", 0, 100);
   progressDialog->setWindowModality(Qt::WindowModal);
   progressLayout->addWidget(progressDialog);
   progressWindow->setLayout(progressLayout);
   progressWindow->setFixedSize(progressWindow->sizeHint());
   progressWindow->show();

   bool queueFinalizeTempBuffer = false;
   std::vector<GCodeCommand> tempLayerBuffer;
   std::vector<GCodeCommand> layer;
   GCodeCommand code;

   double dValue = 0.0;
   long lValue = 0;
   double coordConversion = 1.0;
   bool findingFirstLayer = true;
   bool firstBounds = true;
   int averageCount = 0;

   bool absoluteMode = mPrefs.exportAbsoluteMode;
   bool absoluteEMode = mPrefs.exportAbsoluteEMode;
   double currentPos[AXIS_NUM] = {0.0,};
   double offsetPos[AXIS_NUM] = {0.0,};
   double homeOffset[AXIS_NUM_NO_E] = {0.0,};

   double layerZ = currentPos[Z];
   double lastZ = currentPos[Z];
   double lastE = currentPos[E];
   double mostE = currentPos[E];

   // Parse the gcode file, at the same time any codes we care about will have
   // special treatment while any codes we don't care about will simply be preserved
   // and included in the final product as is.
   while (parser.parseNext())
   {
      progressDialog->setValue(int(parser.getProgress() * 100.0));
      progressDialog->show();

      if (progressDialog->wasCanceled())
      {
         if (progressWindow)
         {
            progressWindow->hide();
            delete progressWindow;
         }

         mError = "Import was cancelled.";
         return false;
      }

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
            code.axisValue[E] = code.axisValue[E] - lastE;
            lastE = currentPos[E];

            if (parser.codeSeen('F'))
            {
               code.f = parser.codeValue();
               code.hasF = true;
            }

            // Our first extruder move command should
            // not be part of our header data.
            if (mData.empty())
            {
               // Move our temp code to our current layer code
               // and iterate to our next layer.
               finalizeTempBuffer(tempLayerBuffer, layer);
               changeLayers = true;
            }
            // If we are extruding some material,
            // determine if the layer has changed.
            else if (mostE < currentPos[E])
            {
               mostE = currentPos[E];

               if (layerZ < currentPos[Z])
               {
                  double height = currentPos[Z] - layerZ;
                  mAverageLayerHeight += height;
                  averageCount++;

                  layerZ = currentPos[Z];
                  lastZ = layerZ;

                  // Because we don't start the extruder's first layer at
                  // position 0, we need to assume that our first height
                  // above 0 is all part of the first layer, so don't start
                  // a new one just yet.
                  if (findingFirstLayer)
                  {
                     findingFirstLayer = false;
                  }
                  else
                  {
                     changeLayers = true;
                  }
               }

               // Update the bounding volume.
               if (!firstBounds)
               {
                  for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
                  {
                     if (mMinBounds[axis] > currentPos[axis])
                     {
                        mMinBounds[axis] = currentPos[axis];
                     }
                     if (mMaxBounds[axis] < currentPos[axis])
                     {
                        mMaxBounds[axis] = currentPos[axis];
                     }
                  }
               }
               else
               {
                  for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
                  {
                     mMinBounds[axis] = currentPos[axis];
                     mMaxBounds[axis] = currentPos[axis];
                  }
                  firstBounds = false;
               }
            }
            // Extruder has increased height.
            else if (layerZ < currentPos[Z])
            {
               if (lastZ < currentPos[Z])
               {
                  lastZ = currentPos[Z];
                  finalizeTempBuffer(tempLayerBuffer, layer);
               }
            }
            else
            {
               queueFinalizeTempBuffer = true;
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
            code.axisValue[E] = 0.0;

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
            continue;
         }

         // 21: Set units to millimeters.
         if (lValue == GCODE_MILLIMETERS_MODE)
         {
            // If we find this code, we can undo our conversion from inches
            // to millimeters if necessary.
            coordConversion = 1.0;
            continue;
         }
      }
      else if (parser.codeSeen('M'))
      {
         lValue = 2000 + parser.codeValueLong();

         code.type = lValue;

         // Codes we care about:
         // 104 & 109: Set Extruder temp.
         if (lValue == MCODE_SET_EXTRUDER_TEMP ||
             lValue == MCODE_SET_EXTRUDER_TEMP_WAIT)
         {
            // Temperature changes are handled by the splicer.
            continue;
         }

         // Marlin custom codes:
         // 82: Set E codes absolute
         if (lValue == MCODE_E_ABSOLUTE_COORDS)
         {
            absoluteEMode = true;
            continue;
         }

         // 83: Set E codes relative while in Absolute Coordinates (G90) mode
         if (lValue == MCODE_E_RELATIVE_COORDS)
         {
            absoluteEMode = false;
            continue;
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
            if (progressWindow)
            {
               progressWindow->hide();
               delete progressWindow;
            }

            // We should not find any extruder change commands as we are assuming
            // all the gcode in any given file are for a single extruder.
            // We are unequipped to deal with this case so we must fail the load.
            mError = "Import does not support gcode files that already contain extruder changes.";
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
         changeLayers = false;

         if (!layer.empty())
         {
            addLayer(layer);
            layer.clear();
         }
      }

      // Add the current code value to the current layer.
      tempLayerBuffer.push_back(code);

      if (queueFinalizeTempBuffer)
      {
         queueFinalizeTempBuffer = false;
         finalizeTempBuffer(tempLayerBuffer, layer);
      }
   }

   // Finalize any remaining temp codes.
   finalizeTempBuffer(tempLayerBuffer, layer, false);

   // Add our final layer.
   if (!layer.empty())
   {
      addLayer(layer);
   }

   // Calculate our bounding center.
   for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
   {
      mCenter[axis] = mMinBounds[axis] + ((mMaxBounds[axis] - mMinBounds[axis]) / 2.0);
   }

   // Offset the object so it is in the center of the build platform.
   mOffsetPos[X] = (mPrefs.platformWidth / 2.0) - mCenter[X];
   mOffsetPos[Y] = (mPrefs.platformHeight / 2.0) - mCenter[Y];
   mOffsetPos[Z] = 0.0;

   if (averageCount > 1)
   {
      mAverageLayerHeight /= averageCount;
   }

   if (progressWindow)
   {
      progressWindow->hide();
      delete progressWindow;
   }

   // We need to 'heal' our layers to remove any extruder
   // retractions and primes that may have been separated
   // between multiple layers.  Since they have been
   // separated, we need to remove them entirely because
   // we can't guarantee that those two layers will be
   // spliced together consecutively again.
   return healLayerRetraction();
}

////////////////////////////////////////////////////////////////////////////////
const double* GCodeObject::getMinBounds() const
{
   return mMinBounds;
}

////////////////////////////////////////////////////////////////////////////////
const double* GCodeObject::getMaxBounds() const
{
   return mMaxBounds;
}

////////////////////////////////////////////////////////////////////////////////
const double* GCodeObject::getCenter() const
{
   return mCenter;
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
void GCodeObject::setExtruder(int extruderIndex)
{
   mExtruderIndex = extruderIndex;
}

////////////////////////////////////////////////////////////////////////////////
int GCodeObject::getExtruder() const
{
   return mExtruderIndex;
}

////////////////////////////////////////////////////////////////////////////////
double GCodeObject::getAverageLayerHeight() const
{
   return mAverageLayerHeight;
}

////////////////////////////////////////////////////////////////////////////////
int GCodeObject::getLayerCount() const
{
   return (int)mData.size();
}

////////////////////////////////////////////////////////////////////////////////
const LayerData& GCodeObject::getLayer(int levelIndex) const
{
   return mData[levelIndex];
}

////////////////////////////////////////////////////////////////////////////////
bool GCodeObject::getLayerAtHeight(std::vector<GCodeCommand>& outLayer, double height) const
{
   int layerCount = (int)mData.size();
   for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex)
   {
      const LayerData& data = mData[layerIndex];
      if (fabs(data.height - height) < 0.001)
      {
         outLayer.insert(outLayer.end(), data.codes.begin(), data.codes.end());
         return true;
      }
      else if (data.height > height)
      {
         return false;
      }
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool GCodeObject::getLayerAboveHeight(const LayerData*& outLayer, double height) const
{
   int layerCount = (int)mData.size();
   for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex)
   {
      const LayerData& data = mData[layerIndex];
      if (data.height > height)
      {
         outLayer = &data;
         return true;
      }
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
const QString& GCodeObject::getError() const
{
   return mError;
}

////////////////////////////////////////////////////////////////////////////////
void GCodeObject::finalizeTempBuffer(std::vector<GCodeCommand>& tempBuffer, std::vector<GCodeCommand>& finalBuffer, bool cullComments)
{
   if (tempBuffer.empty())
   {
      return;
   }

   // All comments at the end are ignored.
   int lastCommentIndex = (int)tempBuffer.size() - 1;

   if (cullComments)
   {
      for (; lastCommentIndex >= 0; --lastCommentIndex)
      {
         GCodeCommand& code = tempBuffer[lastCommentIndex];
         if (code.type == GCODE_COMMENT)
         {
            continue;
         }
         break;
      }
   }

   if (lastCommentIndex >= 0)
   {
      finalBuffer.insert(finalBuffer.end(), tempBuffer.begin(), tempBuffer.begin() + lastCommentIndex + 1);
      tempBuffer.clear();
   }
}

////////////////////////////////////////////////////////////////////////////////
void GCodeObject::addLayer(std::vector<GCodeCommand>& layer)
{
   LayerData data;
   data.codes = layer;

   double height = 0.0;

   double eValue = 0.0;
   bool firstEChange = false;
   int count = (int)layer.size();
   for (int index = 0; index < count; ++index)
   {
      GCodeCommand& code = layer[index];
      if (code.type == GCODE_EXTRUDER_MOVEMENT0 ||
          code.type == GCODE_EXTRUDER_MOVEMENT1)
      {
         if (!firstEChange)
         {
            eValue = code.axisValue[E];
            firstEChange = true;
         }
         else
         {
            if (code.axisValue[E] > eValue)
            {
               height = code.axisValue[Z];
               break;
            }
         }
      }
   }

   data.height = height;
   mData.push_back(data);
}

////////////////////////////////////////////////////////////////////////////////
bool GCodeObject::healLayerRetraction()
{
   bool findPrimer = false;
   int layerCount = (int)mData.size();
   for (int layerIndex = 0; layerIndex < layerCount; ++layerIndex)
   {
      LayerData& layer = mData[layerIndex];
      double extrusionValue = 0.0;

      // If a previous layer is missing its primer, attempt
      // to find it.
      if (findPrimer)
      {
         findPrimer = false;

         int codeCount = (int)layer.codes.size();
         for (int codeIndex = 0; codeIndex < codeCount; ++codeIndex)
         {
            GCodeCommand& code = layer.codes[codeIndex];

            if (code.type == GCODE_EXTRUDER_MOVEMENT0 ||
               code.type == GCODE_EXTRUDER_MOVEMENT1)
            {
               extrusionValue += code.axisValue[E];

               // If we find another retraction that happens before
               // we have primed, there may be something wrong with
               // the slicer that made this gcode file as this
               // should not happen.
               if (code.axisValue[E] < 0.0)
               {
                  mError = "A retraction was found that was not followed properly by a primer.";
                  return false;
               }

               double retractionAmount = mPrefs.retraction;
               if (mPrefs.retraction < 0.0)
               {
                  retractionAmount = -code.axisValue[E];
               }
               double primeAmount = mPrefs.primer;
               if (mPrefs.primer < 0.0)
               {
                  primeAmount = -code.axisValue[E];
               }

               // Determine if we have found our primer, and then
               // erase it because we have already removed the
               // retraction.
               if (extrusionValue >= primeAmount - retractionAmount)
               {
                  layer.codes.erase(layer.codes.begin() + codeIndex);
                  break;
               }
            }
         }
      }

      // Search this layer, starting from the end, for any
      // retraction that does not get primed again.
      extrusionValue = 0.0;
      int codeCount = (int)layer.codes.size();
      for (int codeIndex = codeCount-1; codeIndex >= 0; --codeIndex)
      {
         GCodeCommand& code = layer.codes[codeIndex];

         if (code.type == GCODE_EXTRUDER_MOVEMENT0 ||
             code.type == GCODE_EXTRUDER_MOVEMENT1)
         {
            extrusionValue += code.axisValue[E];

            // Once we find our first instance of retraction,
            // we are done iterating through this layer.  If
            // we find that this retraction is more than what
            // gets extruded after within this layer, then we
            // need to check the next layer for the prime.
            if (code.axisValue[E] < 0.0)
            {
               double retractionAmount = mPrefs.retraction;
               if (mPrefs.retraction < 0.0)
               {
                  retractionAmount = -code.axisValue[E];
               }
               double primeAmount = mPrefs.primer;
               if (mPrefs.primer < 0.0)
               {
                  primeAmount = -code.axisValue[E];
               }
               // If we are still in the negative, meaning we
               // are still retracted and not fully primed,
               // then we need to remove this retraction
               // from the layer and then search the next
               // layer for the primer that matches this.
               extrusionValue += primeAmount;
               if (extrusionValue < primeAmount - retractionAmount)
               {
                  layer.codes.erase(layer.codes.begin() + codeIndex);
                  findPrimer = true;
               }
               break;
            }
         }
      }
   }

   return true;
}

////////////////////////////////////////////////////////////////////////////////
