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


#ifndef G_CODE_BUILDER_H
#define G_CODE_BUILDER_H

#include <Constants.h>
#include <QString>
#include <vector>


class GCodeObject;

class GCodeSplicer
{
public:
   GCodeSplicer(const PreferenceData& prefs);
   virtual ~GCodeSplicer();

   /**
    * Inserts an object into the build list.
    */
   bool addObject(const GCodeObject* object);

   /**
    *	Builds the final gcode file and outputs it to a file.
    *
    * @param[in]  fileName  The name of the file to save.
    */
   bool build(const QString& fileName);

#ifdef BUILD_DEBUG_CONTROLS
   /**
    * Debug method to export the layer debug data.
    */
   bool debugBuildLayerData(const QString& fileName);
#endif

   const QString& getError() const;

protected:

private:

   /**
    * Retrieves whether there is a next layer above the given height.
    * and outputs the exact height of the next layer.
    */
   bool getNextLayer(double height, double& outHeight);

   const PreferenceData& mPrefs;

   std::vector<const GCodeObject*> mObjectList;

   QString mError;
};

#endif // G_CODE_BUILDER_H