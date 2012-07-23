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


#include <GCodeSplicer.h>
#include <GCodeParser.h>
#include <GCodeObject.h>

////////////////////////////////////////////////////////////////////////////////
GCodeSplicer::GCodeSplicer(const PreferenceData& prefs)
   : mPrefs(prefs)
{
}

////////////////////////////////////////////////////////////////////////////////
GCodeSplicer::~GCodeSplicer()
{
}

////////////////////////////////////////////////////////////////////////////////
bool GCodeSplicer::addObject(const GCodeObject* object)
{
   if (!object)
   {
      return false;
   }

   // First make sure we don't add the same object twice.
   int count = (int)mObjectList.size();
   for (int index = 0; index < count; ++index)
   {
      if (object == mObjectList[index])
      {
         return false;
      }
   }

   // Now insert the new object into the list, sorted by extruder index.
   int extruderIndex = object->getExtruder();
   for (int index = 0; index < count; ++index)
   {
      const GCodeObject* other = mObjectList[index];
      if (other && other->getExtruder() > object->getExtruder())
      {
         mObjectList.insert(mObjectList.begin() + index, object);
         return true;
      }
   }

   // If we get here, it means the object has not been entered into a
   // sorted position into the list.  This can be either because
   // the list was empty or the new object extruder value was higher
   // than everything already in the list.  Therefore, any objects
   // that have not been inserted yet go to the end.
   mObjectList.push_back(object);

   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool GCodeSplicer::build(const QString& fileName)
{
   if (mObjectList.empty())
   {
      mError = "No objects to export.";
      return false;
   }

   QFile file;
   file.setFileName(fileName);
   if (!file.open(QIODevice::WriteOnly))
   {
      mError = "Could not open file \'" + fileName + "\' for writing.";
      return false;
   }

   file.write("; Spliced using LocheGSplicer BETA\n");

   // Start by assembling the initialization code.  Start with
   // the header code from the first object, then include our
   // custom header code, and last include our final settings.
   const GCodeObject* object = mObjectList[0];
   if (object && object->getLevelCount())
   {
      const std::vector<GCodeCommand>& header = object->getLevel(0).codes;
      int count = (int)header.size();
      for (int index = 0; index < count; ++index)
      {
         const GCodeCommand& code = header[index];

         // We only care about certain codes.
         if (code.type == GCODE_COMMENT ||
             code.type == GCODE_DWELL ||
             code.type == GCODE_HOME ||
             code.type == MCODE_FAN_ENABLE ||
             code.type == MCODE_FAN_DISABLE)
         {
            file.write(code.command.toAscii());

            if (mPrefs.exportComments && !code.comment.isEmpty())
            {
               file.write(code.comment.toAscii());
            }
            file.write("\n");
         }
      }
   }

   if (!mPrefs.customPrefixCode.isEmpty())
   {
      file.write(mPrefs.customPrefixCode.toAscii());
      file.write("\n");
   }

   file.write("G21");
   if (mPrefs.exportComments) file.write("; Set units to millimeters");
   file.write("\n");

   // Now append our constant header codes.
   if (mPrefs.exportAbsoluteMode)
   {
      file.write("G90");
      if (mPrefs.exportComments) file.write("; Use absolute coordinates");
   }
   else
   {
      file.write("G91");
      if (mPrefs.exportComments) file.write("; Use relative coordinates");
   }
   file.write("\n");

   if (mPrefs.exportAbsoluteEMode)
   {
      file.write("M82");
      if (mPrefs.exportComments) file.write("; Use absolute E coordinates");
   }
   else
   {
      file.write("M83");
      if (mPrefs.exportComments) file.write("; Use relative E coordinates");
   }
   file.write("\n");

   double currentPos[AXIS_NUM] = {0.0,};

   double currentLayerHeight = 0.0;
   int lastExtruder = 0;
   int layerIndex = 1;

   // Iterate through each layer.
   while (getNextLayer(currentLayerHeight, currentLayerHeight))
   {
      if (mPrefs.exportComments)
      {
         file.write("; ++++++++++++++++++++++++++++++++++++++\n; Begin Layer ");
         file.write(QString::number(layerIndex).toAscii());
         file.write(" with height = ");
         file.write(QString::number(currentLayerHeight).toAscii());
         file.write("\n; ++++++++++++++++++++++++++++++++++++++\n");
      }

      int currentExtruder = lastExtruder;
      double offset[AXIS_NUM] = {0,};

      //file.write("G92 E0");
      //if (mPrefs.exportComments) file.write("; Reset extrusion");
      //file.write("\n");
      //offset[E] = 0.0;

      // Iterate through each extruder.  We try to start with the last
      // extruder we used previously in an attempt to reduce the total
      // number of extruder changes done throughout the print.
      for (int extruderIndex = 0; extruderIndex < (int)mPrefs.extruderList.size(); ++extruderIndex)
      {
         int objectCount = (int)mObjectList.size();
         for (int objectIndex = 0; objectIndex < objectCount; ++objectIndex)
         {
            const GCodeObject* object = mObjectList[objectIndex];
            if (object && object->getExtruder() == currentExtruder)
            {
               std::vector<GCodeCommand> layer;
               object->getLevelAtHeight(layer, currentLayerHeight);

               // If we found some codes for this layer using our current extruder...
               if (!layer.empty())
               {
                  // Begin by processing the extruder change if necessary.
                  if (lastExtruder != currentExtruder)
                  {
                     if (mPrefs.exportComments)
                     {
                        file.write("; ++++++++++++++++++++++++++++++++++++++\n; Swap from extruder ");
                        file.write(QString::number(lastExtruder).toAscii());
                        file.write(" to ");
                        file.write(QString::number(currentExtruder).toAscii());
                        file.write("\n; ++++++++++++++++++++++++++++++++++++++\n");
                     }

                     // TODO: Perform an extruder swap.
                     // Not entirely sure what this will involve in the end,
                     // but my guess is probably something around:
                     // - Retract the current extruder
                     // - Wipe the current extruder
                     // - Set the current extruder to an idle temp
                     // - Swap to the next extruder
                     // - Heat up the next extruder
                     // - Prime the next extruder
                     // - begin printing
                     //
                     // My hope is to find a way two swap extruders without
                     // needing to wait for temperature changes.

                     //file.write("T");
                     //file.write(QString::number(currentExtruder).toAscii());
                     //if (mPrefs.exportComments) file.write("; Perform the extruder swap");
                     //file.write("\n");

                     file.write("G92 E0");
                     if (mPrefs.exportComments) file.write("; Reset extrusion");
                     file.write("\n");
                     offset[E] = 0.0;

                     lastExtruder = currentExtruder;
                  }

                  // Setup the offset based on the offset of the current extruder
                  // and the offset position to place the object.
                  for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
                  {
                     offset[axis] = object->getOffsetPos()[axis] + mPrefs.extruderList[lastExtruder].offset[axis];
                  }

                  int codeCount = (int)layer.size();
                  for (int codeIndex = 0; codeIndex < codeCount; ++codeIndex)
                  {
                     const GCodeCommand& code = layer[codeIndex];

                     if (code.type == GCODE_EXTRUDER_MOVEMENT0 ||
                         code.type == GCODE_EXTRUDER_MOVEMENT1)
                     {
                        QString output;
                        if (code.type == GCODE_EXTRUDER_MOVEMENT0) output = "G0 ";
                        else                                       output = "G1 ";

                        bool hasChanged = false;
                        for (int axis = 0; axis < AXIS_NUM; ++axis)
                        {
                           // Only export this axis if it has changed, or if we
                           // have the preference to re-export duplicate axes.
                           if (mPrefs.exportDuplicateAxisPositions ||
                              (axis != E && code.axisValue[axis] != currentPos[axis]) ||
                              (axis == E && code.axisValue[axis] != 0.0))
                           {
                              output += QString(AXIS_NAME[axis]).toAscii();

                              double value = code.axisValue[axis];
                              if (axis == E)
                              {
                                 // Offset the extrusion value by our extruders flow ratio.
                                 value *= mPrefs.extruderList[currentExtruder].flow;
                              }
                              value += offset[axis];
                              if (axis == E)
                              {
                                 offset[E] = value;
                              }

                              output += QString::number(value).toAscii() + " ";
                              hasChanged = true;
                           }

                           currentPos[axis] = code.axisValue[axis];
                        }

                        if (code.hasF)
                        {
                           output += "F" + QString::number(code.f).toAscii();
                           hasChanged = true;
                        }

                        if (hasChanged)
                        {
                           file.write(output.toAscii());
                        }
                     }
                     else
                     {
                        file.write(code.command.toAscii());
                     }

                     if (mPrefs.exportComments) file.write(code.comment.toAscii());
                     file.write("\n");
                  }
               }
            }
         }

         // Iterate to the next extruder.
         currentExtruder++;
         if (currentExtruder >= (int)mPrefs.extruderList.size())
         {
            currentExtruder = 0;
         }
      }
      //layer.codes.clear();
   
      layerIndex++;
   }

   file.close();

   return true;
}

#ifdef BUILD_DEBUG_CONTROLS
////////////////////////////////////////////////////////////////////////////////
bool GCodeSplicer::debugBuildLayerData(const QString& fileName)
{
   if (mObjectList.empty())
   {
      mError = "No objects to export.";
      return false;
   }

   QFile file;
   file.setFileName(fileName);
   if (!file.open(QIODevice::WriteOnly))
   {
      mError = "Could not open file \'" + fileName + "\' for writing.";
      return false;
   }

   double extrusionOffset = 0.0;
   double currentPos[AXIS_NUM] = {0.0,};

   const GCodeObject* object = mObjectList[0];
   if (object)
   {
      int levelCount = object->getLevelCount();
      for (int levelIndex = 0; levelIndex < levelCount; ++levelIndex)
      {
         const LayerData& layer = object->getLevel(levelIndex);

         file.write("; ++++++++++++++++++++++++++++++++++++++\n; Begin Layer ");
         file.write(QString::number(levelIndex).toAscii());
         file.write(" with height = ");
         file.write(QString::number(layer.height).toAscii());
         file.write("\n; ++++++++++++++++++++++++++++++++++++++\n");

         if (levelIndex > 0)
         {
            file.write("G92 E0; Reset extruder position\n");
            extrusionOffset = 0.0;
         }

         int codeCount = (int)layer.codes.size();
         for (int codeIndex = 0; codeIndex < codeCount; ++codeIndex)
         {
            const GCodeCommand& code = layer.codes[codeIndex];

            if (code.type == GCODE_EXTRUDER_MOVEMENT0 ||
               code.type == GCODE_EXTRUDER_MOVEMENT1)
            {
               QString output;
               if (code.type == GCODE_EXTRUDER_MOVEMENT0) output = "G0 ";
               else                                       output = "G1 ";

               bool hasChanged = false;
               for (int axis = 0; axis < AXIS_NUM; ++axis)
               {
                  // Only export this axis if it has changed, or if we
                  // have the preference to re-export duplicate axes.
                  if (mPrefs.exportDuplicateAxisPositions ||
                     (axis != E && code.axisValue[axis] != currentPos[axis]) ||
                     (axis == E && code.axisValue[axis] != 0.0))
                  {
                     output += QString(AXIS_NAME[axis]).toAscii();

                     double value = code.axisValue[axis];

                     if (axis == E)
                     {
                        value += extrusionOffset;
                        extrusionOffset = value;
                     }
                     output += QString::number(value).toAscii() + " ";
                     hasChanged = true;
                  }

                  currentPos[axis] = code.axisValue[axis];
               }

               if (code.hasF)
               {
                  output += "F" + QString::number(code.f).toAscii();
                  hasChanged = true;
               }

               if (hasChanged)
               {
                  file.write(output.toAscii());
               }
            }
            else
            {
               file.write(code.command.toAscii());
            }
            file.write(code.comment.toAscii());
            file.write("\n");
         }
      }
   }
}
#endif

////////////////////////////////////////////////////////////////////////////////
const QString& GCodeSplicer::getError() const
{
   return mError;
}

////////////////////////////////////////////////////////////////////////////////
bool GCodeSplicer::getNextLayer(double height, double& outHeight)
{
   double lowestHeight = 0.0;

   // Determine the next layer by getting the next
   // highest layer between all of the objects.
   int count = (int)mObjectList.size();
   for (int index = 0; index < count; ++index)
   {
      const GCodeObject* object = mObjectList[index];
      if (object)
      {
         const LayerData* data = NULL;
         if (object->getLevelAboveHeight(data, height) && data)
         {
            if (lowestHeight == 0.0 || lowestHeight > data->height)
            {
               lowestHeight = data->height;
            }
         }
      }
   }

   if (lowestHeight > 0.0)
   {
      outHeight = lowestHeight;
      return true;
   }
   return false;
}

////////////////////////////////////////////////////////////////////////////////
