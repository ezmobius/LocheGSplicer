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


#ifndef G_CODE_PARSER_H
#define G_CODE_PARSER_H

#include <QString>
#include <QFile>


class GCodeParser
{
public:
   GCodeParser();
   virtual ~GCodeParser();

   /**
    * Load a specified gcode file.
    *
    * @param[in]  fileName  The name of the gcode file to load.
    */
   bool loadFile(const QString &fileName);
   bool closeFile();
   bool isOpen() const;

   /**
    * Parses the file for the next gcode command line.
    *
    * @return  Returns false when the end of file has been reached.
    */
   bool parseNext();

   /**
    * Retrieves whether a given code type belongs to this line.
    */
   bool codeSeen(char code);

   /**
    * Retrieves the value of the last seen code type
    */
   double codeValue();
   long   codeValueLong();

   /**
    * Retrieves the entire gcode line value.
    */
   QString getLine();

   /**
    * Retrieves the currently parsed comment value if found.
    */
   QString getComment();

protected:

private:

   QFile mFile;

   QByteArray mCommentMarkers;

   QByteArray mToken;
   QByteArray mComment;
   int mCommentPos;

   int mCodePos;
};


#endif // G_CODE_PARSER_H
