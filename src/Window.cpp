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


#include <VisualizerView.h>
#include <GCodeObject.h>
#include <Window.h>

#include <QtGui>


////////////////////////////////////////////////////////////////////////////////
Window::Window()
{
   mVisualizerView = new VisualizerView(mPrefs);

   //mXSlider = createSliderWidget();
   //mYSlider = createSliderWidget();
   //mZSlider = createSliderWidget();

   //connect(mXSlider, SIGNAL(valueChanged(int)), mVisualizerView, SLOT(setXRotation(int)));
   //connect(mVisualizerView, SIGNAL(xRotationChanged(int)), mXSlider, SLOT(setValue(int)));
   //connect(mYSlider, SIGNAL(valueChanged(int)), mVisualizerView, SLOT(setYRotation(int)));
   //connect(mVisualizerView, SIGNAL(yRotationChanged(int)), mYSlider, SLOT(setValue(int)));
   //connect(mZSlider, SIGNAL(valueChanged(int)), mVisualizerView, SLOT(setZRotation(int)));
   //connect(mVisualizerView, SIGNAL(zRotationChanged(int)), mZSlider, SLOT(setValue(int)));

   QHBoxLayout *mainLayout = new QHBoxLayout;
   mainLayout->addWidget(mVisualizerView);
   //mainLayout->addWidget(mXSlider);
   //mainLayout->addWidget(mYSlider);
   //mainLayout->addWidget(mZSlider);
   setLayout(mainLayout);

   //mXSlider->setValue(15 * 16);
   //mYSlider->setValue(345 * 16);
   //mZSlider->setValue(0 * 16);
   setWindowTitle(tr("LocheGSplicer"));

   // TEMP
   GCodeObject* object1 = new GCodeObject(mPrefs);
   GCodeObject* object2 = new GCodeObject(mPrefs);
   object1->loadFile("../data/ColorBase.gcode");
   object2->loadFile("../data/ColorPatern.gcode");
   object2->setExtruder(1);

   mVisualizerView->addObject(object1);
   mVisualizerView->addObject(object2);

   //object1->setOffsetPos(-18.0, -18.0, 0.0);
   //object2->setOffsetPos(-16.0, -16.0, 0.0);
   //object1->setOffsetPos(0.0, -16.0, 0.0);
   //object2->setOffsetPos(-26.0, -16.0, 0.0);
   //object2->setOffsetPos(object2->getOffsetPos()[X] - 2.0, object2->getOffsetPos()[Y] - 2.0, 0.0);

   mObjectList.push_back(object1);
   mObjectList.push_back(object2);
}

//////////////////////////////////////////////////////////////////////////////////
//QSlider *Window::createSliderWidget()
//{
//   QSlider *slider = new QSlider(Qt::Vertical);
//   slider->setRange(0, 360 * 16);
//   slider->setSingleStep(16);
//   slider->setPageStep(15 * 16);
//   slider->setTickInterval(15 * 16);
//   slider->setTickPosition(QSlider::TicksRight);
//   return slider;
//}

////////////////////////////////////////////////////////////////////////////////
void Window::keyPressEvent(QKeyEvent *e)
{
   if (e->key() == Qt::Key_Escape)
      close();
   else
      QWidget::keyPressEvent(e);
}

////////////////////////////////////////////////////////////////////////////////
