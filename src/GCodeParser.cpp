/*
 * LocheGSplicer
 * Copyright (C) 2012 Jeff P. Houde (Lochemage)
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
 */


#include <GCodeParser.h>

#include <QDataStream>


////////////////////////////////////////////////////////////////////////////////
GCodeParser::GCodeParser()
   : mCommentPos(-1)
   , mCodePos(-1)
{
   mCommentMarkers.append(';');
   mCommentMarkers.append('(');
   mCommentMarkers.append(')');
}

////////////////////////////////////////////////////////////////////////////////
GCodeParser::~GCodeParser()
{
   closeFile();
}

////////////////////////////////////////////////////////////////////////////////
bool GCodeParser::loadFile(const QString &fileName)
{
   closeFile();

   // Attempt to open the file.
   mFile.setFileName(fileName);
   if (!mFile.open(QIODevice::ReadOnly))
   {
      return false;
   }

   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool GCodeParser::closeFile()
{
   if (mFile.isOpen())
   {
      mFile.close();
   }

   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool GCodeParser::isOpen() const
{
   return mFile.isOpen();
}

////////////////////////////////////////////////////////////////////////////////
bool GCodeParser::parseNext()
{
   if (!mFile.isOpen())
   {
      return false;
   }

   mToken = mFile.readLine();
   mComment.clear();

   // Check for comments
   mCommentPos = -1;
   int count = mCommentMarkers.count();
   for (int index = 0; index < count; ++index)
   {
      int pos = mToken.indexOf(QString(mCommentMarkers.at(index)), 0);
      if (pos > -1 && (mCommentPos == -1 || pos < mCommentPos))
      {
         mCommentPos = pos;
      }
   }

   if (mCommentPos > -1)
   {
      mComment = mToken.right(mToken.length() - mCommentPos);
      mToken = mToken.left(mCommentPos);
   }

   mToken = mToken.toUpper();

   mCodePos = -1;

   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool GCodeParser::codeSeen(char code)
{
   if (!mFile.isOpen())
   {
      return false;
   }

   mCodePos = mToken.indexOf(QString(code), 0);
   if (mCodePos > -1)
   {
      return true;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
double GCodeParser::codeValue()
{
   if (!mFile.isOpen())
   {
      return 0.0;
   }

   if (mCodePos == -1)
   {
      return 0.0;
   }

   char value[80] = {0,};
   for (int i = 0; i < mToken.length() - mCodePos; ++i)
   {
      char c = mToken.at(mCodePos + 1 + i);
      if ((c >= '0' && c <= '9') || c == '.')
      {
         value[i] = c;
      }
      else
      {
         break;
      }
   }

   return QString(value).toDouble();
}

////////////////////////////////////////////////////////////////////////////////
long GCodeParser::codeValueLong()
{
   if (!mFile.isOpen())
   {
      return 0.0;
   }

   if (mCodePos == -1)
   {
      return 0.0;
   }

   char value[80] = {0,};
   for (int i = 0; i < mToken.length() - mCodePos; ++i)
   {
      char c = mToken.at(mCodePos + 1 + i);
      if (c >= '0' && c <= '9')
      {
         value[i] = c;
      }
      else
      {
         break;
      }
   }

   return QString(value).toLong();
}

////////////////////////////////////////////////////////////////////////////////
QString GCodeParser::getLine()
{
   return mToken;
}

////////////////////////////////////////////////////////////////////////////////
QString GCodeParser::getComment()
{
   return mComment;
}

////////////////////////////////////////////////////////////////////////////////
