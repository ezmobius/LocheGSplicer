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


#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include <VisualizerView.h>
#include <GCodeObject.h>
#include <qtlogo.h>

////////////////////////////////////////////////////////////////////////////////
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

////////////////////////////////////////////////////////////////////////////////
VisualizerView::VisualizerView(QWidget *parent)
   : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
   mLogo = 0;
   mRotX = 0;
   mRotY = 0;
   mRotZ = 0;

   // TODO: Add preference to set background and extruder colors.
   mBackgroundColor = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);

   mExtruderColors.push_back(Qt::red);
   mExtruderColors.push_back(Qt::green);
   mExtruderColors.push_back(Qt::blue);
   mExtruderColors.push_back(Qt::yellow);
}

////////////////////////////////////////////////////////////////////////////////
VisualizerView::~VisualizerView()
{
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::addObject(GCodeObject* object, int extruder)
{
   VisualizerObjectData data;
   data.object = object;
   data.extruder = extruder;

   mObjectList.push_back(data);
}

////////////////////////////////////////////////////////////////////////////////
QSize VisualizerView::minimumSizeHint() const
{
   return QSize(50, 50);
}

////////////////////////////////////////////////////////////////////////////////
QSize VisualizerView::sizeHint() const
{
   return QSize(400, 400);
}

////////////////////////////////////////////////////////////////////////////////
static void qNormalizeAngle(int &angle)
{
   while (angle < 0)
      angle += 360 * 16;
   while (angle > 360 * 16)
      angle -= 360 * 16;
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::setXRotation(int angle)
{
   qNormalizeAngle(angle);
   if (angle != mRotX) {
      mRotX = angle;
      emit xRotationChanged(angle);
      updateGL();
   }
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::setYRotation(int angle)
{
   qNormalizeAngle(angle);
   if (angle != mRotY) {
      mRotY = angle;
      emit yRotationChanged(angle);
      updateGL();
   }
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::setZRotation(int angle)
{
   qNormalizeAngle(angle);
   if (angle != mRotZ) {
      mRotZ = angle;
      emit zRotationChanged(angle);
      updateGL();
   }
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::initializeGL()
{
   qglClearColor(mBackgroundColor.dark());

   mLogo = new QtLogo(this, 64);
   mLogo->setColor(Qt::darkGreen);

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glShadeModel(GL_SMOOTH);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_MULTISAMPLE);
   static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
   glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, -1.0);
   glRotatef(mRotX / 16.0, 1.0, 0.0, 0.0);
   glRotatef(mRotY / 16.0, 0.0, 1.0, 0.0);
   glRotatef(mRotZ / 16.0, 0.0, 0.0, 1.0);
   mLogo->draw();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::resizeGL(int width, int height)
{
   int side = qMin(width, height);
   glViewport((width - side) / 2, (height - side) / 2, side, side);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   //glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
   gluPerspective(45.0f, width/height, 0.1f, 100.0f);
   glMatrixMode(GL_MODELVIEW);
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::mousePressEvent(QMouseEvent *event)
{
   mLastPos = event->pos();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::mouseMoveEvent(QMouseEvent *event)
{
   int dx = event->x() - mLastPos.x();
   int dy = event->y() - mLastPos.y();

   if (event->buttons() & Qt::LeftButton) {
      setXRotation(mRotX + 8 * dy);
      setYRotation(mRotY + 8 * dx);
   } else if (event->buttons() & Qt::RightButton) {
      setXRotation(mRotX + 8 * dy);
      setZRotation(mRotZ + 8 * dx);
   }
   mLastPos = event->pos();
}

////////////////////////////////////////////////////////////////////////////////
