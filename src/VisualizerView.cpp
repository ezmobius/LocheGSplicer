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


#include <QtGui>
#include <QtOpenGL>

#include <VisualizerView.h>
#include <GCodeObject.h>

#include <math.h>
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

////////////////////////////////////////////////////////////////////////////////
VisualizerView::VisualizerView(const std::vector<ExtruderData>& extruders)
   : QGLWidget(QGLFormat(QGL::SampleBuffers), NULL)
   , mExtruders(extruders)
{
   mRotX = 0;
   mRotY = 0;
   mRotZ = 0;

   // TODO: Add preference to set background and extruder colors.
   mBackgroundColor = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}

////////////////////////////////////////////////////////////////////////////////
VisualizerView::~VisualizerView()
{
   clearObjects();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::addObject(GCodeObject* object, int extruder)
{
   VisualizerObjectData data;
   data.object = object;
   data.extruder = extruder;
   data.vertexBuffer = NULL;
   data.vertexCount = 0;

   visualize(data);

   mObjectList.push_back(data);
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::removeObject(GCodeObject* object)
{
   if (!object)
   {
      return;
   }

   int objectCount = (int)mObjectList.size();
   for (int objectIndex = 0; objectIndex < objectCount; ++objectIndex)
   {
      VisualizerObjectData& data = mObjectList[objectIndex];
      if (data.object == object)
      {
         delete data.object;
         data.object = NULL;

         if (data.vertexBuffer)
         {
            delete [] data.vertexBuffer;
            data.vertexBuffer = NULL;
         }

         mObjectList.erase(mObjectList.begin() + objectIndex);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::clearObjects()
{
   int objectCount = (int)mObjectList.size();
   for (int objectIndex = 0; objectIndex < objectCount; ++objectIndex)
   {
      VisualizerObjectData& data = mObjectList[objectIndex];
      if (data.object)
      {
         delete data.object;
         data.object = NULL;
      }

      if (data.vertexBuffer)
      {
         delete [] data.vertexBuffer;
         data.vertexBuffer = NULL;
      }
   }
   mObjectList.clear();
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
   if (angle != mRotX)
   {
      mRotX = angle;
      emit xRotationChanged(angle);
      updateGL();
   }
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::setYRotation(int angle)
{
   qNormalizeAngle(angle);
   if (angle != mRotY)
   {
      mRotY = angle;
      emit yRotationChanged(angle);
      updateGL();
   }
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::setZRotation(int angle)
{
   qNormalizeAngle(angle);
   if (angle != mRotZ)
   {
      mRotZ = angle;
      emit zRotationChanged(angle);
      updateGL();
   }
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::initializeGL()
{
   qglClearColor(mBackgroundColor.dark());

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glShadeModel(GL_SMOOTH);
   glEnable(GL_MULTISAMPLE);
   glEnable(GL_COLOR_MATERIAL);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, -50.0);
   glRotatef(mRotX / 16.0, 1.0, 0.0, 0.0);
   glRotatef(mRotY / 16.0, 0.0, 1.0, 0.0);
   glRotatef(mRotZ / 16.0, 0.0, 0.0, 1.0);

   int objectCount = (int)mObjectList.size();
   for (int objectIndex = 0; objectIndex < objectCount; ++objectIndex)
   {
      drawObject(mObjectList[objectIndex]);
   }
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::resizeGL(int width, int height)
{
   int side = qMin(width, height);
   glViewport((width - side) / 2, (height - side) / 2, side, side);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   //glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
   gluPerspective(45.0f, width/height, 0.1f, 10000.0f);
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

   if (event->buttons() & (Qt::LeftButton | Qt::RightButton))
   {
      setXRotation(mRotX + 8 * dy);
      setZRotation(mRotZ + 8 * dx);
   }
   mLastPos = event->pos();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::drawObject(const VisualizerObjectData& object)
{
   glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
   glEnableClientState(GL_VERTEX_ARRAY);
   glPushMatrix();

   const double* offset = object.object->getOffsetPos();
   glTranslated(offset[0], offset[1], offset[2]);

   glColor4d(mExtruders[object.extruder].color.redF(),
             mExtruders[object.extruder].color.greenF(),
             mExtruders[object.extruder].color.blueF(),
             1.0);

   glLineWidth(1.0f);
   glVertexPointer(3, GL_DOUBLE, 0, object.vertexBuffer);
   glDrawArrays(GL_LINES, 0, object.vertexCount);

   glPopMatrix();
   glPopClientAttrib();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::visualize(VisualizerObjectData& data)
{
   if (!data.object)
   {
      return;
   }

   if (data.vertexBuffer)
   {
      delete [] data.vertexBuffer;
      data.vertexBuffer = NULL;
   }

   // Before we can allocate memory for our vertex buffers, we first need to
   // determine exactly how many vertices we need.
   data.vertexCount = 0;
   double lastPos[AXIS_NUM] = {0,};
   int levelCount = data.object->getLevelCount();
   for (int levelIndex = 0; levelIndex < levelCount; ++levelIndex)
   {
      const std::vector<GCodeCommand>& levelData = data.object->getLevel(levelIndex);

      int commandCount = (int)levelData.size();
      for (int commandIndex = 0; commandIndex < commandCount; ++commandIndex)
      {
         const GCodeCommand& command = levelData[commandIndex];

         if (command.hasAxis)
         {
            // We only draw a line segment if we are extruding filament on this line.
            if (command.axisValue[E] > 0.0 && command.axisValue[E] > lastPos[E])
            {
               // Two points per line segment.
               data.vertexCount += 2;
            }

            lastPos[E] = command.axisValue[E];
         }
      }
   }

   if (data.vertexCount > 0)
   {
      data.vertexBuffer = new double[data.vertexCount * 3];

      for (int axis = 0; axis < AXIS_NUM; ++axis)
      {
         lastPos[axis] = 0.0;
      }

      // Now fill in our newly allocated buffer space.
      int pointIndex = 0;

      for (int levelIndex = 0; levelIndex < levelCount; ++levelIndex)
      {
         const std::vector<GCodeCommand>& levelData = data.object->getLevel(levelIndex);

         int commandCount = (int)levelData.size();
         for (int commandIndex = 0; commandIndex < commandCount; ++commandIndex)
         {
            const GCodeCommand& command = levelData[commandIndex];

            if (command.hasAxis)
            {
               // We only draw a line segment if we are extruding filament on this line.
               if (command.axisValue[E] > 0.0 && command.axisValue[E] > lastPos[E])
               {
                  // Draw a line from the extruder's current position to the new
                  // position.
                  for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
                  {
                     data.vertexBuffer[pointIndex] = lastPos[axis];
                     pointIndex++;
                  }
                  for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
                  {
                     data.vertexBuffer[pointIndex] = command.axisValue[axis];
                     pointIndex++;
                  }
               }

               for (int axis = 0; axis < AXIS_NUM; ++axis)
               {
                  lastPos[axis] = command.axisValue[axis];
               }
            }
         }
      }

      // Just a simple check to make sure we actually used the proper number of vertices.
      assert(data.vertexCount * 3 == pointIndex);
   }
}

////////////////////////////////////////////////////////////////////////////////
