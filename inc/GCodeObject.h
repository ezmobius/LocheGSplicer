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


#ifndef G_CODE_OBJECT_H
#define G_CODE_OBJECT_H

#include <Constants.h>
#include <QString>
#include <vector>


class GCodeObject
{
public:
   GCodeObject(const PreferenceData& prefs);
   virtual ~GCodeObject();

   /**
    * Load a specified gcode file.
    */
   bool loadFile(const QString &fileName);

   const double* getMinBounds() const;
   const double* getMaxBounds() const;
   const double* getCenter() const;

   void setOffsetPos(double x, double y, double z);
   const double* getOffsetPos() const;

   void setExtruder(int extruderIndex);
   int getExtruder() const;

   int getLevelCount() const;
   const std::vector<GCodeCommand>& getLevel(int levelIndex) const;

   const QString& getError() const;

protected:

private:

   void finalizeTempBuffer(std::vector<GCodeCommand>& tempBuffer, std::vector<GCodeCommand>& finalBuffer, bool cullComments = true);

   const PreferenceData& mPrefs;

   std::vector< std::vector<GCodeCommand> > mData;

   // Bounding Box
   double mMinBounds[AXIS_NUM_NO_E];
   double mMaxBounds[AXIS_NUM_NO_E];
   double mCenter[AXIS_NUM_NO_E];

   double mOffsetPos[AXIS_NUM_NO_E];
   int    mExtruderIndex;

   QString mError;
};

#endif // G_CODE_OBJECT_H