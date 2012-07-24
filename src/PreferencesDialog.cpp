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

   QWidget* editorTab = new QWidget();
   mTabWidget->addTab(editorTab, "Editor");

   QGridLayout* editorLayout = new QGridLayout();
   editorTab->setLayout(editorLayout);

   mSaveConfigurationButton = new QPushButton("Save Config...");
   editorLayout->addWidget(mSaveConfigurationButton, 0, 0, 1, 1);

   mLoadConfigurationButton = new QPushButton("Load Config...");
   editorLayout->addWidget(mLoadConfigurationButton, 0, 1, 1, 1);
   editorLayout->setRowStretch(0, 0);

   QLabel* backgroundColorLabel = new QLabel("Background Color: ");
   backgroundColorLabel->setAlignment(Qt::AlignRight);
   editorLayout->addWidget(backgroundColorLabel, 1, 0, 1, 1);

   mBackgroundColorButton = new QPushButton();
   mBackgroundColorButton->setFlat(true);
   editorLayout->addWidget(mBackgroundColorButton, 1, 1, 1, 1);
   setBackgroundColor(mPrefs.backgroundColor);
   editorLayout->setRowStretch(1, 0);
   editorLayout->setRowStretch(2, 1);

   mDefaultButton = new QPushButton("Reset to Defaults");
   editorLayout->addWidget(mDefaultButton, 3, 0, 1, 2);
   editorLayout->setRowStretch(3, 0);

   setLayout(mainLayout);
}

////////////////////////////////////////////////////////////////////////////////
void PreferencesDialog::setupConnections()
{
   connect(mSaveConfigurationButton,  SIGNAL(pressed()),    this, SLOT(onSaveConfigPressed()));
   connect(mLoadConfigurationButton,  SIGNAL(pressed()),    this, SLOT(onLoadConfigPressed()));
   connect(mBackgroundColorButton,    SIGNAL(pressed()),    this, SLOT(onBackgroundColorPressed()));
   connect(mDefaultButton,            SIGNAL(pressed()),    this, SLOT(onDefaultPressed()));
}

////////////////////////////////////////////////////////////////////////////////
void PreferencesDialog::setBackgroundColor(const QColor& color)
{
   mPrefs.backgroundColor = color;
   QPixmap pix = QPixmap(QSize(16, 16));
   pix.fill(mPrefs.backgroundColor);
   mBackgroundColorButton->setIcon(QIcon(pix));

   emit onBackgroundColorChanged();
}

////////////////////////////////////////////////////////////////////////////////
