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

#include <QtGui>
#include <QtOpenGL>

#include <math.h>
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

////////////////////////////////////////////////////////////////////////////////
VisualizerView::VisualizerView(const PreferenceData& prefs)
   : QGLWidget(QGLFormat(QGL::SampleBuffers), NULL)
   , mPrefs(prefs)
{
   for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
   {
      mCameraRot[axis] = 0.0;
      mCameraRotTarget[axis] = 0.0;
      mCameraTrans[axis] = 0.0;
      mCameraTransTarget[axis] = 0.0;
   }

   mCameraZoom = -50.0f;
   mCameraZoomTarget = -50.0f;

   mCameraTrans[X] = mPrefs.platformWidth / 2.0;
   mCameraTrans[Y] = mPrefs.platformHeight / 2.0;
   mCameraTransTarget[X] = mCameraTrans[X];
   mCameraTransTarget[Y] = mCameraTrans[Y];

   mUpdateTimer = new QTimer(this);
   connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(updateTick()));
   mUpdateTimer->start(100);
}

////////////////////////////////////////////////////////////////////////////////
VisualizerView::~VisualizerView()
{
   clearObjects();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::addObject(GCodeObject* object)
{
   VisualizerObjectData data;
   data.object = object;
   data.vertexBuffer = NULL;
   data.vertexCount = 0;

   generateGeometry(data);

   mObjectList.push_back(data);

   updateGL();
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
         if (data.vertexBuffer)
         {
            delete [] data.vertexBuffer;
            data.vertexBuffer = NULL;
         }

         mObjectList.erase(mObjectList.begin() + objectIndex);
         break;
      }
   }

   updateGL();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::clearObjects()
{
   int objectCount = (int)mObjectList.size();
   for (int objectIndex = 0; objectIndex < objectCount; ++objectIndex)
   {
      VisualizerObjectData& data = mObjectList[objectIndex];
      if (data.vertexBuffer)
      {
         delete [] data.vertexBuffer;
         data.vertexBuffer = NULL;
      }
   }
   mObjectList.clear();

   updateGL();
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
void VisualizerView::setXTranslation(double pos)
{
   mCameraTransTarget[X] = pos;
   emit xTranslationChanged(pos);
   updateGL();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::setYTranslation(double pos)
{
   mCameraTransTarget[Y] = pos;
   emit yTranslationChanged(pos);
   updateGL();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::setZTranslation(double pos)
{
   mCameraTransTarget[Z] = pos;
   emit zTranslationChanged(pos);
   updateGL();
}

////////////////////////////////////////////////////////////////////////////////
static void normalizeAngle(double &angle)
{
   while (angle < 0.0)
      angle += 360.0;
   while (angle > 360.0)
      angle -= 360.0;
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::setXRotation(double angle)
{
   normalizeAngle(angle);
   if (angle != mCameraRotTarget[X])
   {
      mCameraRotTarget[X] = angle;
      emit xRotationChanged(angle);
      updateGL();
   }
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::setYRotation(double angle)
{
   normalizeAngle(angle);
   if (angle != mCameraRotTarget[Y])
   {
      mCameraRotTarget[Y] = angle;
      emit yRotationChanged(angle);
      updateGL();
   }
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::setZRotation(double angle)
{
   normalizeAngle(angle);
   if (angle != mCameraRotTarget[Z])
   {
      mCameraRotTarget[Z] = angle;
      emit zRotationChanged(angle);
      updateGL();
   }
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::setZoom(double zoom)
{
   if (zoom > -1.0) zoom = -1.0f;

   mCameraZoomTarget = zoom;
   emit zoomChanged(zoom);
   updateGL();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::updateTick()
{
   if (updateCamera())
   {
      updateGL();
   }
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::initializeGL()
{
   qglClearColor(mPrefs.backgroundColor);

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glShadeModel(GL_SMOOTH);
   //glEnable(GL_LIGHTING);
   //glEnable(GL_LIGHT0);
   glEnable(GL_MULTISAMPLE);
   glEnable(GL_COLOR_MATERIAL);
   //static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
   //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

////////////////////////////////////////////////////////////////////////////////
bool VisualizerView::updateCamera()
{
   bool changed = false;
   double dist = 0.0;

   // Update camera translation towards target.
   for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
   {
      dist  = mCameraRotTarget[axis] - mCameraRot[axis];
      double dist2 = mCameraRotTarget[axis] - (mCameraRot[axis] + 360.0);
      if (fabs(dist) > fabs(dist2)) dist = dist2;
      if (dist) changed = true;
      mCameraRot[axis] += dist / 2.0;

      dist = (mCameraTransTarget[axis] - mCameraTrans[axis]) / 2.0;
      if (dist) changed = true;
      mCameraTrans[axis] += dist;
   }
   dist = (mCameraZoomTarget - mCameraZoom) / 2.0;
   if (dist) changed = true;
   mCameraZoom += dist;

   return changed;
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::paintGL()
{
   updateCamera();

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, mCameraZoom);

   glPushMatrix();

   // Apply camera rotation.
   glRotatef(mCameraRot[X], 1.0, 0.0, 0.0);
   glRotatef(mCameraRot[Y], 0.0, 1.0, 0.0);
   glRotatef(mCameraRot[Z], 0.0, 0.0, 1.0);

   // Apply camera translation.
   glTranslatef(-mCameraTrans[X], -mCameraTrans[Y], -mCameraTrans[Z]);

   int objectCount = (int)mObjectList.size();
   for (int objectIndex = 0; objectIndex < objectCount; ++objectIndex)
   {
      drawObject(mObjectList[objectIndex]);
   }
   glPopMatrix();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::resizeGL(int width, int height)
{
   glViewport(0, 0, width, height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   //glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
   gluPerspective(45.0f, width/(double)height, 0.1f, 10000.0f);
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

   if (event->buttons() & Qt::LeftButton)
   {
      QMatrix4x4 mat;
      mat.rotate(mCameraRotTarget[Z], 0.0, 0.0, 1.0);
      QVector3D pos;
      pos.setX(0.1 * -dx);
      pos.setY(0.1 * dy);

      pos = pos * mat;

      setXTranslation(mCameraTransTarget[X] + pos.x());
      setYTranslation(mCameraTransTarget[Y] + pos.y());
   }
   if (event->buttons() & Qt::RightButton)
   {
      setZRotation(mCameraRotTarget[Z] + dx);
      setXRotation(mCameraRotTarget[X] + dy);
   }
   mLastPos = event->pos();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::wheelEvent(QWheelEvent* event)
{
   setZoom(mCameraZoomTarget + (-mCameraZoomTarget * event->delta() * 0.001));
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::drawObject(const VisualizerObjectData& object)
{
   glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
   glEnableClientState(GL_VERTEX_ARRAY);
   glPushMatrix();

   const double* offset = object.object->getOffsetPos();
   glTranslated(offset[0], offset[1], offset[2]);

   int extruderIndex = object.object->getExtruder();
   if (extruderIndex < 0 || extruderIndex >= (int)mPrefs.extruderList.size())
   {
      // Default to extruder index 0 if our desired index is out of bounds.
      extruderIndex = 0;
   }

   glColor4d(mPrefs.extruderList[extruderIndex].color.redF(),
             mPrefs.extruderList[extruderIndex].color.greenF(),
             mPrefs.extruderList[extruderIndex].color.blueF(),
             1.0);

   glLineWidth(1.0f);
   glVertexPointer(3, GL_DOUBLE, 0, object.vertexBuffer);
   glDrawArrays(GL_LINES, 0, object.vertexCount);

   glPopMatrix();
   glPopClientAttrib();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::generateGeometry(VisualizerObjectData& data)
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
