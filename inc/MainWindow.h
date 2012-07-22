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

#include <Constants.h>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QListView;
class QSplitter;
class QPushButton;
class QTableWidget;
class QDoubleSpinBox;
QT_END_NAMESPACE

class VisualizerView;
class GCodeObject;


class MainWindow : public QWidget
{
   Q_OBJECT

public:
   MainWindow();

protected:
   void keyPressEvent(QKeyEvent *event);

public slots:
   void onObjectSelectionChanged();
   void onAddPressed();
   void onRemovePressed();
   void onPlaterXPosChanged(double pos);
   void onPlaterYPosChanged(double pos);
   void onSplicePressed();
#ifdef BUILD_DEBUG_CONTROLS
   void onDebugExportLayerDataPressed();
#endif

   /**
    * Event handler when the extruder index has changed on an object item.
    */
   void onExtruderIndexChanged(int index);

private:
   void setupUI();
   void setupConnections();

   PreferenceData    mPrefs;

   QSplitter*        mMainSplitter;
   VisualizerView*   mVisualizerView;
   QTableWidget*     mObjectListWidget;
   QPushButton*      mAddFileButton;
   QPushButton*      mRemoveFileButton;
   QDoubleSpinBox*   mPlaterXPosSpin;
   QDoubleSpinBox*   mPlaterYPosSpin;
   QPushButton*      mSpliceButton;
#ifdef BUILD_DEBUG_CONTROLS
   QPushButton*      mDebugExportLayerButton;
#endif

   std::vector<GCodeObject*> mObjectList;
};

#endif // WINDOW_H
