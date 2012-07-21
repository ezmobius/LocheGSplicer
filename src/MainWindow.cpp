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
#include <GCodeBuilder.h>
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
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::onBuildPressed()
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

      GCodeBuilder builder(mPrefs);
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

      GCodeBuilder builder(mPrefs);
      int count = (int)mObjectList.size();
      for (int index = 0; index < count; ++index)
      {
         builder.addObject(mObjectList[index]);
      }

      if (!builder.debugBuildLayerData(fileName))
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
   mObjectListWidget->setSelectionMode(QAbstractItemView::MultiSelection);
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
   buttonLayout->addWidget(mAddFileButton);
   buttonLayout->addWidget(mRemoveFileButton);

   // Below the object list are the plating controls for
   // positioning each gcode file offset.
   QGroupBox* platerGroup = new QGroupBox("Plater");
   rightLayout->addWidget(platerGroup);

   QVBoxLayout* platerLayout = new QVBoxLayout();
   platerGroup->setLayout(platerLayout);

   // TODO: Add plater controls.

   // Below the plater controls are the final builder controls.
   QGroupBox* buildGroup = new QGroupBox("Splice");
   rightLayout->addWidget(buildGroup);

   QVBoxLayout* buildLayout = new QVBoxLayout();
   buildGroup->setLayout(buildLayout);

   mBuildButton = new QPushButton("Splice");
   buildLayout->addWidget(mBuildButton);

#ifdef BUILD_DEBUG_CONTROLS
   mDebugExportLayerButton = new QPushButton("DEBUG: Export Layer Breakdown");
   buildLayout->addWidget(mDebugExportLayerButton);
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
   connect(mAddFileButton,          SIGNAL(pressed()), this, SLOT(onAddPressed()));
   connect(mRemoveFileButton,       SIGNAL(pressed()), this, SLOT(onRemovePressed()));
   connect(mBuildButton,            SIGNAL(pressed()), this, SLOT(onBuildPressed()));
#ifdef BUILD_DEBUG_CONTROLS
   connect(mDebugExportLayerButton, SIGNAL(pressed()), this, SLOT(onDebugExportLayerDataPressed()));
#endif
}

////////////////////////////////////////////////////////////////////////////////
