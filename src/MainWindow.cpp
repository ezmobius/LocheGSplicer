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
#include <GCodeSplicer.h>
#include <MainWindow.h>

#include <QtGui>
#include <QVariant>


////////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow()
{
   setupUI();
   setupConnections();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::keyPressEvent(QKeyEvent *e)
{
   if (e->key() == Qt::Key_Escape)
      close();
   else
      QWidget::keyPressEvent(e);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::onObjectSelectionChanged()
{
   int firstSelectedIndex = -1;
   int selectionCount = 0;
   int rowCount = (int)mObjectListWidget->rowCount();
   for (int rowIndex = 0; rowIndex < rowCount; ++rowIndex)
   {
      // Remove all items that are selected.
      if (mObjectListWidget->isItemSelected(mObjectListWidget->item(rowIndex, 0)))
      {
         selectionCount++;
         if (firstSelectedIndex == -1)
         {
            firstSelectedIndex = rowIndex;
         }
      }
   }

   bool anySelected = selectionCount > 0? true: false;
   bool singleObject = selectionCount == 1? true: false;

   // Plater controls can only be edited on a single object.
   mPlaterXPosSpin->setEnabled(singleObject);
   mPlaterYPosSpin->setEnabled(singleObject);
   if (singleObject && firstSelectedIndex > -1 && firstSelectedIndex < (int)mObjectList.size())
   {
      // If we are showing our plater controls, update their current values with
      // our currently selected object.
      GCodeObject* object = mObjectList[firstSelectedIndex];
      if (object)
      {
         mPlaterXPosSpin->setValue(object->getOffsetPos()[X] + object->getCenter()[X]);
         mPlaterYPosSpin->setValue(object->getOffsetPos()[Y] + object->getCenter()[Y]);
      }
   }

   // Remove button can only be used if there are any selected.
   mRemoveFileButton->setEnabled(anySelected);

#ifdef BUILD_DEBUG_CONTROLS
   // The debug export layer data button can only be used if there is a single item
   // selected.
   mDebugExportLayerButton->setEnabled(singleObject);
#endif
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::onAddPressed()
{
   // Check for any previously used directories.
   QSettings settings(COMPANY_NAME, APPLICATION_NAME);
   QString lastDir = settings.value(LAST_IMPORT_FOLDER, "").toString();

   QFileDialog dlg;
   QString fileName = dlg.getOpenFileName(this, "Open GCode File", lastDir, "GCODE (*.gcode);; All Files (*.*)");
   if (!fileName.isEmpty())
   {
      QFileInfo fileInfo = fileName;

      // Remember this directory.
      lastDir = fileInfo.absolutePath();
      settings.setValue(LAST_IMPORT_FOLDER, lastDir);

      // Attempt to load our given file.
      GCodeObject* newObject = new GCodeObject(mPrefs);

      if (!newObject->loadFile(fileName))
      {
         // Failed to load the file.
         QString errorStr = "Failed to load file \'" + fileName + "\' with error: " + newObject->getError();
         QMessageBox::critical(this, "Failure!", errorStr, QMessageBox::Ok, QMessageBox::NoButton);
         delete newObject;
         return;
      }

      // First attempt to find the next extruder index to use.
      int extruderIndex = 0;
      int count = (int)mObjectList.size();
      for (int index = 0; index < count; ++index)
      {
         GCodeObject* object = mObjectList[index];
         if (object)
         {
            if (object->getExtruder() >= extruderIndex)
            {
               extruderIndex = object->getExtruder() + 1;
            }
         }
      }

      if (extruderIndex >= (int)mPrefs.extruderList.size())
      {
         extruderIndex = 0;
      }

      newObject->setExtruder(extruderIndex);
      mVisualizerView->addObject(newObject);
      mObjectList.push_back(newObject);

      int rowIndex = mObjectListWidget->rowCount();
      mObjectListWidget->insertRow(rowIndex);

      QTableWidgetItem* fileItem = new QTableWidgetItem(fileInfo.fileName());
      fileItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

      QSpinBox* extruderSpin = new QSpinBox();
      extruderSpin->setMinimum(0);
      extruderSpin->setMaximum((int)mPrefs.extruderList.size() - 1);
      extruderSpin->setValue(extruderIndex);
      connect(extruderSpin, SIGNAL(valueChanged(int)), this, SLOT(onExtruderIndexChanged(int)));

      mObjectListWidget->setItem(rowIndex, 0, fileItem);
      mObjectListWidget->setCellWidget(rowIndex, 1, extruderSpin);
      mObjectListWidget->resizeColumnsToContents();

      mSpliceButton->setEnabled(true);
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::onRemovePressed()
{
   int rowCount = (int)mObjectListWidget->rowCount();
   for (int rowIndex = 0; rowIndex < rowCount; ++rowIndex)
   {
      // Remove all items that are selected.
      if (mObjectListWidget->isItemSelected(mObjectListWidget->item(rowIndex, 0)))
      {
         if (rowIndex >= (int)mObjectList.size())
         {
            break;
         }

         GCodeObject* object = mObjectList[rowIndex];
         mObjectList.erase(mObjectList.begin() + rowIndex);
         mVisualizerView->removeObject(object);
         mObjectListWidget->removeRow(rowIndex);
         delete object;
         rowIndex--;
         rowCount--;
      }
   }

   if (mObjectListWidget->rowCount() == 0)
   {
      mSpliceButton->setEnabled(false);
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::onPlaterXPosChanged(double pos)
{
   int rowCount = (int)mObjectListWidget->rowCount();
   for (int rowIndex = 0; rowIndex < rowCount; ++rowIndex)
   {
      // Remove all items that are selected.
      if (mObjectListWidget->isItemSelected(mObjectListWidget->item(rowIndex, 0)))
      {
         if (rowIndex >= (int)mObjectList.size())
         {
            break;
         }

         mObjectList[rowIndex]->setOffsetPos(
            pos - mObjectList[rowIndex]->getCenter()[X],
            mObjectList[rowIndex]->getOffsetPos()[Y],
            mObjectList[rowIndex]->getOffsetPos()[Z]);

         mVisualizerView->updateGL();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::onPlaterYPosChanged(double pos)
{
   int rowCount = (int)mObjectListWidget->rowCount();
   for (int rowIndex = 0; rowIndex < rowCount; ++rowIndex)
   {
      // Remove all items that are selected.
      if (mObjectListWidget->isItemSelected(mObjectListWidget->item(rowIndex, 0)))
      {
         if (rowIndex >= (int)mObjectList.size())
         {
            break;
         }

         mObjectList[rowIndex]->setOffsetPos(
            mObjectList[rowIndex]->getOffsetPos()[X],
            pos - mObjectList[rowIndex]->getCenter()[Y],
            mObjectList[rowIndex]->getOffsetPos()[Z]);

         mVisualizerView->updateGL();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::onSplicePressed()
{
   QSettings settings(COMPANY_NAME, APPLICATION_NAME);
   QString lastDir = settings.value(LAST_EXPORT_FOLDER, "").toString();
   lastDir += "\\Spliced";

   QFileDialog dlg;
   QString fileName = dlg.getSaveFileName(this, "Export GCode File", lastDir, "GCODE (*.gcode);; All Files (*.*)");
   if (!fileName.isEmpty())
   {
      QFileInfo fileInfo = fileName;

      // Remember this directory.
      lastDir = fileInfo.absolutePath();
      settings.setValue(LAST_EXPORT_FOLDER, lastDir);

      GCodeSplicer builder(mPrefs);
      int count = (int)mObjectList.size();
      for (int index = 0; index < count; ++index)
      {
         builder.addObject(mObjectList[index]);
      }

      if (!builder.build(fileName))
      {
         // Failed to load the file.
         QString errorStr = "Failed to splice file \'" + fileName + "\' with error: " + builder.getError();
         QMessageBox::critical(this, "Failure!", errorStr, QMessageBox::Ok, QMessageBox::NoButton);
      }
      else
      {
         QMessageBox::information(this, "Success!", "GCode spliced and exported!", QMessageBox::Ok);
      }
   }
}

#ifdef BUILD_DEBUG_CONTROLS
////////////////////////////////////////////////////////////////////////////////
void MainWindow::onDebugExportLayerDataPressed()
{
   QSettings settings(COMPANY_NAME, APPLICATION_NAME);
   QString lastDir = settings.value(LAST_EXPORT_FOLDER, "").toString();
   lastDir += "\\Spliced";

   QFileDialog dlg;
   QString fileName = dlg.getSaveFileName(this, "Export GCode File", lastDir, "GCODE (*.gcode);; All Files (*.*)");
   if (!fileName.isEmpty())
   {
      QFileInfo fileInfo = fileName;

      // Remember this directory.
      lastDir = fileInfo.absolutePath();
      settings.setValue(LAST_EXPORT_FOLDER, lastDir);

      GCodeSplicer builder(mPrefs);
      int count = mObjectListWidget->rowCount();
      for (int index = 0; index < count; ++index)
      {
         if (mObjectListWidget->isItemSelected(mObjectListWidget->item(index, 0)))
         {
            builder.addObject(mObjectList[index]);
            break;
         }
      }

      if (!builder.debugBuildLayerData(fileName))
      {
         // Failed to load the file.
         QString errorStr = "Failed to export file \'" + fileName + "\' with error: " + builder.getError();
         QMessageBox::critical(this, "Failure!", errorStr, QMessageBox::Ok, QMessageBox::NoButton);
      }
      else
      {
         QMessageBox::information(this, "Success!", "Debug gcode layer data exported!", QMessageBox::Ok);
      }
   }
}
#endif

////////////////////////////////////////////////////////////////////////////////
void MainWindow::onExtruderIndexChanged(int index)
{
   QSpinBox* extruderSpin = dynamic_cast<QSpinBox*>(sender());
   if (extruderSpin)
   {
      // Find the object index that is changing.
      int rowCount = mObjectListWidget->rowCount();
      for (int rowIndex = 0; rowIndex < rowCount; ++rowIndex)
      {
         if (mObjectListWidget->cellWidget(rowIndex, 1) == extruderSpin)
         {
            mObjectList[rowIndex]->setExtruder(extruderSpin->value());
            mVisualizerView->updateGL();
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::setupUI()
{
   setWindowTitle(tr("LocheGSplicer"));

   QHBoxLayout* mainLayout = new QHBoxLayout();
   mainLayout->setMargin(0);

   mMainSplitter = new QSplitter(Qt::Horizontal);
   mainLayout->addWidget(mMainSplitter);

   ////////////////////////////////////////////////////////////////////////////////
   // Main visualizer window goes on the left side.
   mVisualizerView = new VisualizerView(mPrefs);
   mMainSplitter->addWidget(mVisualizerView);

   ////////////////////////////////////////////////////////////////////////////////
   // GCode list and other plating properties go on the right side.
   QWidget* rightWidget = new QWidget();
   mMainSplitter->addWidget(rightWidget);
   QVBoxLayout* rightLayout = new QVBoxLayout();
   rightWidget->setLayout(rightLayout);

   QGroupBox* objectGroup = new QGroupBox("Objects");
   rightLayout->addWidget(objectGroup);

   QVBoxLayout* objectLayout = new QVBoxLayout();
   objectGroup->setLayout(objectLayout);

   // Object list box.
   mObjectListWidget = new QTableWidget(0, 2);
   mObjectListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
   mObjectListWidget->setToolTip("The list of currently imported gcode files to be spliced.");
   objectLayout->addWidget(mObjectListWidget);

   QStringList headers;
   headers.push_back("File");
   headers.push_back("Extruder #");
   mObjectListWidget->setHorizontalHeaderLabels(headers);

   // Import and remove buttons for the object list.
   QHBoxLayout* buttonLayout = new QHBoxLayout();
   objectLayout->addLayout(buttonLayout);
   mAddFileButton = new QPushButton("Import");
   mRemoveFileButton = new QPushButton("Remove");
   mAddFileButton->setToolTip("Import and add a gcode file to the list.");
   mRemoveFileButton->setToolTip("Remove all selected gcode items from the list.");
   mRemoveFileButton->setEnabled(false);
   buttonLayout->addWidget(mAddFileButton);
   buttonLayout->addWidget(mRemoveFileButton);

   // Below the object list are the plating controls for
   // positioning each gcode file offset.
   QGroupBox* platerGroup = new QGroupBox("Plater");
   rightLayout->addWidget(platerGroup);

   QGridLayout* platerLayout = new QGridLayout();
   platerGroup->setLayout(platerLayout);

   // Add plater controls.
   QLabel* platerXLabel = new QLabel("X: ");
   QLabel* platerYLabel = new QLabel("Y: ");
   mPlaterXPosSpin = new QDoubleSpinBox();
   mPlaterYPosSpin = new QDoubleSpinBox();
   mPlaterXPosSpin->setMinimum(-50.0);
   mPlaterYPosSpin->setMinimum(-50.0);
   mPlaterXPosSpin->setMaximum(mPrefs.platformWidth + 50.0);
   mPlaterYPosSpin->setMaximum(mPrefs.platformHeight + 50.0);
   mPlaterXPosSpin->setSingleStep(mPrefs.platerIncrementSize);
   mPlaterYPosSpin->setSingleStep(mPrefs.platerIncrementSize);
   platerLayout->addWidget(platerXLabel, 0, 0, 1, 1);
   platerLayout->addWidget(mPlaterXPosSpin, 0, 1, 1, 1);
   platerLayout->addWidget(platerYLabel, 0, 2, 1, 1);
   platerLayout->addWidget(mPlaterYPosSpin, 0, 3, 1, 1);
   platerLayout->setColumnStretch(0, 0);
   platerLayout->setColumnStretch(1, 1);
   platerLayout->setColumnStretch(2, 0);
   platerLayout->setColumnStretch(3, 1);
   mPlaterXPosSpin->setEnabled(false);
   mPlaterYPosSpin->setEnabled(false);

   // Below the plater controls are the final builder controls.
   QGroupBox* buildGroup = new QGroupBox("Splice");
   rightLayout->addWidget(buildGroup);

   QVBoxLayout* buildLayout = new QVBoxLayout();
   buildGroup->setLayout(buildLayout);

   mSpliceButton = new QPushButton("Splice");
   mSpliceButton->setToolTip("Splices and exports the final gcode file.");
   mSpliceButton->setEnabled(false);
   buildLayout->addWidget(mSpliceButton);

#ifdef BUILD_DEBUG_CONTROLS
   mDebugExportLayerButton = new QPushButton("DEBUG: Export Layer Markings");
   buildLayout->addWidget(mDebugExportLayerButton);
   mDebugExportLayerButton->setToolTip("This will export a gcode file that contains comments to mark where this\napplication thinks each layer begins (Only does this on the first selected object).");
   mDebugExportLayerButton->setEnabled(false);
#endif

   QList<int> sizes;
   sizes.push_back((width() / 3) * 2);
   sizes.push_back(width() / 3);
   mMainSplitter->setSizes(sizes);
   setLayout(mainLayout);
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::setupConnections()
{
   connect(mObjectListWidget,       SIGNAL(itemSelectionChanged()),  this, SLOT(onObjectSelectionChanged()));
   connect(mAddFileButton,          SIGNAL(pressed()),               this, SLOT(onAddPressed()));
   connect(mRemoveFileButton,       SIGNAL(pressed()),               this, SLOT(onRemovePressed()));
   connect(mPlaterXPosSpin,         SIGNAL(valueChanged(double)),    this, SLOT(onPlaterXPosChanged(double)));
   connect(mPlaterYPosSpin,         SIGNAL(valueChanged(double)),    this, SLOT(onPlaterYPosChanged(double)));
   connect(mSpliceButton,           SIGNAL(pressed()),               this, SLOT(onSplicePressed()));
#ifdef BUILD_DEBUG_CONTROLS
   connect(mDebugExportLayerButton, SIGNAL(pressed()),               this, SLOT(onDebugExportLayerDataPressed()));
#endif
}

////////////////////////////////////////////////////////////////////////////////
