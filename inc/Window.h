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


#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>


QT_BEGIN_NAMESPACE
class QSlider;
QT_END_NAMESPACE
class VisualizerView;
class GCodeObject;

class Window : public QWidget
{
   Q_OBJECT

public:
   Window();

protected:
   void keyPressEvent(QKeyEvent *event);

private:
   QSlider *createSliderWidget();

   VisualizerView *mVisualizerView;
   QSlider *mXSlider;
   QSlider *mYSlider;
   QSlider *mZSlider;

   // TEMP
   GCodeObject* mTestObject1;
   GCodeObject* mTestObject2;
};

#endif // WINDOW_H
