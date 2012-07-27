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


#include <PreferencesDialog.h>

#include <QtGui>
#include <QVariant>


////////////////////////////////////////////////////////////////////////////////
PreferencesDialog::PreferencesDialog(PreferenceData& prefs)
   : mPrefs(prefs)
{
   setupUI();
   setupConnections();
}

////////////////////////////////////////////////////////////////////////////////
PreferenceData PreferencesDialog::getPreferences() const
{
   return mPrefs;
}

////////////////////////////////////////////////////////////////////////////////
void PreferencesDialog::keyPressEvent(QKeyEvent *e)
{
   if (e->key() == Qt::Key_Escape)
      close();
   else
      QWidget::keyPressEvent(e);
}

////////////////////////////////////////////////////////////////////////////////
void PreferencesDialog::onSaveConfigPressed()
{
   QMessageBox::information(this, "Save!", "Not implemented yet.", QMessageBox::Ok);
}

////////////////////////////////////////////////////////////////////////////////
void PreferencesDialog::onLoadConfigPressed()
{
   QMessageBox::information(this, "Load!", "Not implemented yet.", QMessageBox::Ok);
}

////////////////////////////////////////////////////////////////////////////////
void PreferencesDialog::onDrawQualityChanged(int value)
{
   mPrefs.drawQuality = (DrawQuality)value;
}

////////////////////////////////////////////////////////////////////////////////
void PreferencesDialog::onLayerSkipChanged(int value)
{
   mPrefs.layerSkipSize = value;
}

////////////////////////////////////////////////////////////////////////////////
void PreferencesDialog::onBackgroundColorPressed()
{
   QColor newColor = QColorDialog::getColor(mPrefs.backgroundColor, this, "Background Color");

   if (newColor.isValid())
   {
      setBackgroundColor(newColor);
   }
}

////////////////////////////////////////////////////////////////////////////////
void PreferencesDialog::onDefaultPressed()
{
   mPrefs = PreferenceData();

   setBackgroundColor(mPrefs.backgroundColor);
}

////////////////////////////////////////////////////////////////////////////////
void PreferencesDialog::setupUI()
{
   setWindowTitle(tr("Preferences"));
   resize(400, 300);

   QVBoxLayout* mainLayout = new QVBoxLayout();
   mainLayout->setMargin(0);

   mTabWidget = new QTabWidget();
   mainLayout->addWidget(mTabWidget);

   // Editor Tab.
   QWidget* editorTab = new QWidget();
   mTabWidget->addTab(editorTab, "Editor");

   QGridLayout* editorLayout = new QGridLayout();
   editorTab->setLayout(editorLayout);

   // File Group.
   QGroupBox* fileGroup = new QGroupBox("File");
   editorLayout->addWidget(fileGroup, 0, 0, 1, 1);

   QHBoxLayout* fileLayout = new QHBoxLayout();
   fileGroup->setLayout(fileLayout);

   mSaveConfigurationButton = new QPushButton("Save Config...");
   fileLayout->addWidget(mSaveConfigurationButton);

   mLoadConfigurationButton = new QPushButton("Load Config...");
   fileLayout->addWidget(mLoadConfigurationButton);
   editorLayout->setRowStretch(0, 0);

   // Rendering Group.
   QGroupBox* renderingGroup = new QGroupBox("Rendering");
   editorLayout->addWidget(renderingGroup, 1, 0, 1, 1);

   QGridLayout* renderingLayout = new QGridLayout();
   renderingGroup->setLayout(renderingLayout);

   QLabel* drawQualityLabel = new QLabel("Draw Quality: ");
   drawQualityLabel->setAlignment(Qt::AlignRight);
   renderingLayout->addWidget(drawQualityLabel, 0, 0, 1, 1);

   mDrawQualityCombo = new QComboBox();
   mDrawQualityCombo->addItem("Low");
   mDrawQualityCombo->addItem("Medium");
   mDrawQualityCombo->addItem("High");
   mDrawQualityCombo->setCurrentIndex(mPrefs.drawQuality);
   renderingLayout->addWidget(mDrawQualityCombo, 0, 1, 1, 2);

   QLabel* layerSkipLabel = new QLabel("Layer Skip: ");
   layerSkipLabel->setAlignment(Qt::AlignRight);
   renderingLayout->addWidget(layerSkipLabel, 1, 0, 1, 1);

   mLayerSkipSpin = new QSpinBox();
   mLayerSkipSpin->setValue(mPrefs.layerSkipSize);
   renderingLayout->addWidget(mLayerSkipSpin, 1, 1, 1, 2);

   mBackgroundColorButton = new QPushButton("Background Color");
   renderingLayout->addWidget(mBackgroundColorButton, 2, 0, 1, 3);
   setBackgroundColor(mPrefs.backgroundColor);
   renderingLayout->setRowStretch(1, 0);

   renderingLayout->setColumnStretch(0, 1);
   renderingLayout->setColumnStretch(1, 1);
   renderingLayout->setColumnStretch(2, 1);

   editorLayout->setRowStretch(2, 1);

   // Reset button.
   QHBoxLayout* buttonLayout = new QHBoxLayout();
   mainLayout->addLayout(buttonLayout);

   mOkButton = new QPushButton("Ok");
   buttonLayout->addWidget(mOkButton);

   mCancelButton = new QPushButton("Cancel");
   buttonLayout->addWidget(mCancelButton);

   mDefaultButton = new QPushButton("Reset to Defaults");
   buttonLayout->addWidget(mDefaultButton);

   setLayout(mainLayout);
}

////////////////////////////////////////////////////////////////////////////////
void PreferencesDialog::setupConnections()
{
   // File
   connect(mSaveConfigurationButton,   SIGNAL(pressed()),                  this, SLOT(onSaveConfigPressed()));
   connect(mLoadConfigurationButton,   SIGNAL(pressed()),                  this, SLOT(onLoadConfigPressed()));

   // Rendering
   connect(mDrawQualityCombo,          SIGNAL(currentIndexChanged(int)),   this, SLOT(onDrawQualityChanged(int)));
   connect(mLayerSkipSpin,             SIGNAL(valueChanged(int)),          this, SLOT(onLayerSkipChanged(int)));
   connect(mBackgroundColorButton,     SIGNAL(pressed()),                  this, SLOT(onBackgroundColorPressed()));

   // Dialog Buttons
   connect(mOkButton,                  SIGNAL(pressed()),                  this, SLOT(accept()));
   connect(mCancelButton,              SIGNAL(pressed()),                  this, SLOT(close()));
   connect(mDefaultButton,             SIGNAL(pressed()),                  this, SLOT(onDefaultPressed()));
}

////////////////////////////////////////////////////////////////////////////////
void PreferencesDialog::setBackgroundColor(const QColor& color)
{
   mPrefs.backgroundColor = color;
   QPixmap pix = QPixmap(QSize(16, 16));
   pix.fill(mPrefs.backgroundColor);
   mBackgroundColorButton->setIcon(QIcon(pix));

   emit emitBackgroundColorChanged(mPrefs.backgroundColor);
}

////////////////////////////////////////////////////////////////////////////////
