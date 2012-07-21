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


#include <GCodeBuilder.h>
#include <GCodeParser.h>
#include <GCodeObject.h>

////////////////////////////////////////////////////////////////////////////////
GCodeBuilder::GCodeBuilder(const PreferenceData& prefs)
   : mPrefs(prefs)
{
}

////////////////////////////////////////////////////////////////////////////////
GCodeBuilder::~GCodeBuilder()
{
}

////////////////////////////////////////////////////////////////////////////////
bool GCodeBuilder::addObject(const GCodeObject* object)
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
bool GCodeBuilder::build(const QString& fileName)
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

   if (DEBUG_OUTPUT_LAYER_CODE)
   {
      const GCodeObject* object = mObjectList[0];
      if (object)
      {
         int levelCount = object->getLevelCount();
         for (int levelIndex = 0; levelIndex < levelCount; ++levelIndex)
         {
            file.write("; ++++++++++++++++++++++++++++++++++++++ Begin Layer ");
            file.write(QString::number(levelIndex).toAscii());
            file.write(" ++++++++++++++++++++++++++++++++++++++\n");

            const std::vector<GCodeCommand>& layer = object->getLevel(levelIndex);
            int codeCount = (int)layer.size();
            for (int codeIndex = 0; codeIndex < codeCount; ++codeIndex)
            {
               const GCodeCommand& code = layer[codeIndex];
               file.write(code.command.toAscii());
               file.write(code.comment.toAscii());
               file.write("\n");
            }
         }
      }

      return true;
   }

   // Start by assembling the initialization code.  Start with
   // our own custom prefix if available, then we'll add the
   // header code from our first object.
   if (!mPrefs.customPrefixCode.isEmpty())
   {
      if (mPrefs.exportComments)
      {
         file.write("; *** Begin Custom Prefix GCode ***\n");
      }

      file.write(mPrefs.customPrefixCode.toAscii());
      file.write("\n");

      if (mPrefs.exportComments)
      {
         file.write("; *** End Custom Prefix GCode ***\n");
      }
   }

   const GCodeObject* object = mObjectList[0];
   if (object && object->getLevelCount())
   {
      const std::vector<GCodeCommand>& header = object->getLevel(0);
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

   // Now append our constant header codes.

   file.close();

   mError = "Not implemented yet.";
   return false;
}

////////////////////////////////////////////////////////////////////////////////
const QString& GCodeBuilder::getError() const
{
   return mError;
}

////////////////////////////////////////////////////////////////////////////////
