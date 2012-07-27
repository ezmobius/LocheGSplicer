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

   mCameraZoom = -50.0;
   mCameraZoomTarget = -50.0;

   mCameraTrans[X] = mPrefs.platformWidth / 2.0;
   mCameraTrans[Y] = mPrefs.platformHeight / 2.0;
   mCameraTransTarget[X] = mCameraTrans[X];
   mCameraTransTarget[Y] = mCameraTrans[Y];

   mCameraRot[X] = -45.0;
   mCameraRotTarget[X] = mCameraRot[X];
   mCameraRot[Z] = 25.0;
   mCameraRotTarget[Z] = mCameraRot[Z];

   mUpdateTimer = new QTimer(this);
   connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(updateTick()));
   mUpdateTimer->start(100);
}

////////////////////////////////////////////////////////////////////////////////
VisualizerView::~VisualizerView()
{
   clearObjects();
   mUpdateTimer->stop();
   delete mUpdateTimer;
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::addObject(GCodeObject* object)
{
   VisualizerObjectData data;
   data.object = object;
   data.vertexBuffer = NULL;
   data.normalBuffer = NULL;
   data.indexBuffer = NULL;
   data.vertexCount = 0;
   data.quadCount = 0;

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
         freeObjectBuffers(data);
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
      freeObjectBuffers(data);
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
void VisualizerView::regenerateGeometry()
{
   int objectCount = (int)mObjectList.size();
   for (int objectIndex = 0; objectIndex < objectCount; ++objectIndex)
   {
      VisualizerObjectData& data = mObjectList[objectIndex];
      generateGeometry(data);
   }

   updateGL();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::onBackgroundColorChanged(const QColor& color)
{
   qglClearColor(color);
   updateGL();
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
   //normalizeAngle(angle);
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
   //normalizeAngle(angle);
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
   glEnable(GL_MULTISAMPLE);
   glEnable(GL_COLOR_MATERIAL);
   //glEnable(GL_POLYGON_OFFSET_FILL);
   //glPolygonOffset(1.0, 1.0);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

   static GLfloat lightPosition[4] = { 1.0, 2.0, 1.0, 1.0 };
   glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
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

   // Set up perspective view.
   GLdouble xmin, xmax, ymin, ymax, aspect;

   aspect = width/(double)height;
   ymax = 0.1f * tan( 45.0f * M_PI / 360.0 );
   ymin = -ymax;
   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum(xmin, xmax, ymin, ymax, 0.1, 1000.0);

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

      if ((mCameraRot[X] < -90 && mCameraRot[X] > -270) ||
         (mCameraRot[X] > 90 && mCameraRot[X] < 270)) dy = -dy;

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
   glPushAttrib(GL_LIGHTING_BIT);
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

   switch (mPrefs.drawQuality)
   {
   case DRAW_QUALITY_LOW:
      {
         glEnableClientState(GL_VERTEX_ARRAY);

         glLineWidth(1.0f);
         glVertexPointer(3, GL_DOUBLE, 0, object.vertexBuffer);
         glDrawArrays(GL_LINES, 0, object.vertexCount);
      }
      break;
   case DRAW_QUALITY_MED:
   case DRAW_QUALITY_HIGH:
      {
         glEnable(GL_LIGHTING);
         glEnable(GL_LIGHT0);

         glEnableClientState(GL_VERTEX_ARRAY);
         glEnableClientState(GL_NORMAL_ARRAY);

         glVertexPointer(3, GL_DOUBLE, 0, object.vertexBuffer);
         glNormalPointer(GL_DOUBLE, 0, object.normalBuffer);
         glDrawElements(GL_QUADS, object.quadCount * 4, GL_UNSIGNED_INT, object.indexBuffer);
      }
      break;
   }

   glPopMatrix();
   glPopAttrib();
   glPopClientAttrib();
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::freeObjectBuffers(VisualizerObjectData& data)
{
   if (data.vertexBuffer)
   {
      delete [] data.vertexBuffer;
      data.vertexBuffer = NULL;
   }

   if (data.normalBuffer)
   {
      delete [] data.normalBuffer;
      data.normalBuffer = NULL;
   }

   if (data.indexBuffer)
   {
      delete [] data.indexBuffer;
      data.indexBuffer = NULL;
   }

   data.vertexCount = 0;
   data.quadCount = 0;
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::generateGeometry(VisualizerObjectData& data)
{
   if (!data.object)
   {
      return;
   }

   freeObjectBuffers(data);

   // Before we can allocate memory for our vertex buffers, we first need to
   // determine exactly how many vertices we need.
   int skipCount = mPrefs.layerSkipSize + 1;
   double lastE = 0.0;
   int levelCount = data.object->getLevelCount();
   for (int levelIndex = 0; levelIndex < levelCount; ++levelIndex)
   {
      const std::vector<GCodeCommand>& levelData = data.object->getLevel(levelIndex).codes;

      // Skip layers if necessary.
      if (mPrefs.layerSkipSize > 0)
      {
         skipCount++;

         if (skipCount <= mPrefs.layerSkipSize)
         {
            continue;
         }

         skipCount = 0;
      }

      int commandCount = (int)levelData.size();
      for (int commandIndex = 0; commandIndex < commandCount; ++commandIndex)
      {
         const GCodeCommand& command = levelData[commandIndex];

         if (command.hasAxis)
         {
            // We only draw a line segment if we are extruding filament on this line.
            if (command.axisValue[E] != 0.0 && lastE + command.axisValue[E] > 0.0)
            {
               switch (mPrefs.drawQuality)
               {
               case DRAW_QUALITY_LOW:
                  {
                     // The line segment will consist of a single line between two points.
                     data.vertexCount += 2;
                  }
                  break;
               case DRAW_QUALITY_MED:
                  {
                     // The line segment will consist of 8 points that
                     // form together to make two flat quads.
                     data.vertexCount += 8;
                     data.quadCount += 4;
                  }
                  break;
               case DRAW_QUALITY_HIGH:
                  {
                     // The line segment will consist of 8 points that
                     // form together to make a rectangular box shape.
                     data.vertexCount += 8;
                     data.quadCount += 4;
                  }
                  break;
               }
            }

            lastE += command.axisValue[E];
            if (lastE > 0.0)
            {
               lastE = 0.0;
            }
         }
      }
   }

   if (data.vertexCount > 0)
   {
      skipCount = mPrefs.layerSkipSize + 1;

      data.vertexBuffer = new double[data.vertexCount * 3];
      if (mPrefs.drawQuality != DRAW_QUALITY_LOW)
      {
         data.normalBuffer = new double[data.vertexCount * 3];
         data.indexBuffer = new unsigned int[data.quadCount * 4];
      }

      double radius = data.object->getAverageLayerHeight() * 0.5;
      QVector3D up = QVector3D(0.0, 0.0, 1.0);

      double lastPos[AXIS_NUM] = {0.0,};

      // Now fill in our newly allocated buffer space.
      int pointIndex = 0;
      int normalIndex = 0;
      int quadIndex = 0;

      for (int levelIndex = 0; levelIndex < levelCount; ++levelIndex)
      {
         const std::vector<GCodeCommand>& levelData = data.object->getLevel(levelIndex).codes;

         // Skip layers if necessary.
         if (mPrefs.layerSkipSize > 0)
         {
            skipCount++;

            if (skipCount <= mPrefs.layerSkipSize)
            {
               continue;
            }

            skipCount = 0;
         }

         int commandCount = (int)levelData.size();
         for (int commandIndex = 0; commandIndex < commandCount; ++commandIndex)
         {
            const GCodeCommand& command = levelData[commandIndex];

            if (command.hasAxis)
            {
               // We only draw a line segment if we are extruding filament on this line.
               if (command.axisValue[E] != 0.0 && lastPos[E] + command.axisValue[E] > 0.0)
               {
                  QVector3D p1 = QVector3D(lastPos[X], lastPos[Y], lastPos[Z]);
                  QVector3D p2 = QVector3D(command.axisValue[X], command.axisValue[Y], command.axisValue[Z]);
                  
                  switch (mPrefs.drawQuality)
                  {
                  case DRAW_QUALITY_LOW:
                     {
                        addGeometryPoint(&data.vertexBuffer[pointIndex], pointIndex, p1);
                        addGeometryPoint(&data.vertexBuffer[pointIndex], pointIndex, p2);
                     }
                     break;

                  case DRAW_QUALITY_MED:
                  case DRAW_QUALITY_HIGH:
                     {
                        // Set up a rotation matrix
                        QMatrix4x4 rot;
                        rot.lookAt(p2 - p1, QVector3D(0.0, 0.0, 0.0), up);

                        QVector3D right;
                        right.setX(1.0);
                        right = right * rot;

                        QVector3D vec = (p2 - p1).normalized();
                        QVector3D norm;

                        int vertexIndex = pointIndex / 3;

                        // Generate our quads.
                        // Left
                        data.indexBuffer[quadIndex + 0] = vertexIndex + POINT_FIRST_TOP_LEFT;
                        data.indexBuffer[quadIndex + 1] = vertexIndex + POINT_FIRST_BOT_LEFT;
                        data.indexBuffer[quadIndex + 2] = vertexIndex + POINT_SECOND_BOT_LEFT;
                        data.indexBuffer[quadIndex + 3] = vertexIndex + POINT_SECOND_TOP_LEFT;
                        quadIndex += 4;

                        // Top
                        data.indexBuffer[quadIndex + 0] = vertexIndex + POINT_FIRST_TOP_RIGHT;
                        data.indexBuffer[quadIndex + 1] = vertexIndex + POINT_FIRST_TOP_LEFT;
                        data.indexBuffer[quadIndex + 2] = vertexIndex + POINT_SECOND_TOP_LEFT;
                        data.indexBuffer[quadIndex + 3] = vertexIndex + POINT_SECOND_TOP_RIGHT;
                        quadIndex += 4;

                        // Right
                        data.indexBuffer[quadIndex + 0] = vertexIndex + POINT_FIRST_BOT_RIGHT;
                        data.indexBuffer[quadIndex + 1] = vertexIndex + POINT_FIRST_TOP_RIGHT;
                        data.indexBuffer[quadIndex + 2] = vertexIndex + POINT_SECOND_TOP_RIGHT;
                        data.indexBuffer[quadIndex + 3] = vertexIndex + POINT_SECOND_BOT_RIGHT;
                        quadIndex += 4;

                        // Bottom
                        data.indexBuffer[quadIndex + 0] = vertexIndex + POINT_FIRST_BOT_LEFT;
                        data.indexBuffer[quadIndex + 1] = vertexIndex + POINT_FIRST_BOT_RIGHT;
                        data.indexBuffer[quadIndex + 2] = vertexIndex + POINT_SECOND_BOT_RIGHT;
                        data.indexBuffer[quadIndex + 3] = vertexIndex + POINT_SECOND_BOT_LEFT;
                        quadIndex += 4;

                        //// First cap
                        //data.indexBuffer[quadIndex + 0] = vertexIndex + POINT_FIRST_TOP_LEFT;
                        //data.indexBuffer[quadIndex + 1] = vertexIndex + POINT_FIRST_TOP_RIGHT;
                        //data.indexBuffer[quadIndex + 2] = vertexIndex + POINT_FIRST_BOT_RIGHT;
                        //data.indexBuffer[quadIndex + 3] = vertexIndex + POINT_FIRST_BOT_LEFT;
                        //quadIndex += 4;

                        //// Second cap
                        //data.indexBuffer[quadIndex + 0] = vertexIndex + POINT_SECOND_TOP_RIGHT;
                        //data.indexBuffer[quadIndex + 1] = vertexIndex + POINT_SECOND_TOP_LEFT;
                        //data.indexBuffer[quadIndex + 2] = vertexIndex + POINT_SECOND_BOT_LEFT;
                        //data.indexBuffer[quadIndex + 3] = vertexIndex + POINT_SECOND_BOT_RIGHT;
                        //quadIndex += 4;

                        // Generate our 8 vertices for this rectangle segment.
                        addGeometryPoint(&data.vertexBuffer[pointIndex], pointIndex, p1 + up * radius - right * radius - vec * radius);
                        addGeometryPoint(&data.vertexBuffer[pointIndex], pointIndex, p1 + up * radius + right * radius - vec * radius);
                        addGeometryPoint(&data.vertexBuffer[pointIndex], pointIndex, p1 - up * radius - right * radius - vec * radius);
                        addGeometryPoint(&data.vertexBuffer[pointIndex], pointIndex, p1 - up * radius + right * radius - vec * radius);

                        addGeometryPoint(&data.vertexBuffer[pointIndex], pointIndex, p2 + up * radius * 0.95 - right * radius + vec * radius);
                        addGeometryPoint(&data.vertexBuffer[pointIndex], pointIndex, p2 + up * radius * 0.95 + right * radius + vec * radius);
                        addGeometryPoint(&data.vertexBuffer[pointIndex], pointIndex, p2 - up * radius * 0.95 - right * radius + vec * radius);
                        addGeometryPoint(&data.vertexBuffer[pointIndex], pointIndex, p2 - up * radius * 0.95 + right * radius + vec * radius);

                        // Generate our 8 vertex normal values.
                        addGeometryPoint(&data.normalBuffer[normalIndex], normalIndex, (up - right - vec));
                        addGeometryPoint(&data.normalBuffer[normalIndex], normalIndex, (up + right - vec));
                        addGeometryPoint(&data.normalBuffer[normalIndex], normalIndex, (-up - right - vec));
                        addGeometryPoint(&data.normalBuffer[normalIndex], normalIndex, (-up + right - vec));

                        addGeometryPoint(&data.normalBuffer[normalIndex], normalIndex, (up - right + vec));
                        addGeometryPoint(&data.normalBuffer[normalIndex], normalIndex, (up + right + vec));
                        addGeometryPoint(&data.normalBuffer[normalIndex], normalIndex, (-up - right + vec));
                        addGeometryPoint(&data.normalBuffer[normalIndex], normalIndex, (-up + right + vec));
                     }
                     break;
                  }
               }

               for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
               {
                  lastPos[axis] = command.axisValue[axis];
               }

               lastPos[E] += command.axisValue[E];
               if (lastPos[E] > 0.0)
               {
                  lastPos[E] = 0.0;
               }
            }
         }
      }

      // Just a simple check to make sure we actually used the proper number of vertices.
      assert(data.vertexCount * 3 == pointIndex);
      assert(data.vertexCount * 3 == normalIndex);
      assert(data.quadCount * 4 == quadIndex);
   }
}

////////////////////////////////////////////////////////////////////////////////
void VisualizerView::addGeometryPoint(double* buffer, int& index, const QVector3D& point)
{
   if (!buffer)
   {
      return;
   }

   buffer[0] = point.x();
   buffer[1] = point.y();
   buffer[2] = point.z();

   index += 3;
}

////////////////////////////////////////////////////////////////////////////////
