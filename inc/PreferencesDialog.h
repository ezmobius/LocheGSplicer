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


#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <Constants.h>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QTabWidget;
class QComboBox;
class QSpinBox;
QT_END_NAMESPACE


class PreferencesDialog : public QDialog
{
   Q_OBJECT

public:
   PreferencesDialog(PreferenceData& prefs);

   PreferenceData getPreferences() const;

protected:
   void keyPressEvent(QKeyEvent *event);

signals:
   void emitBackgroundColorChanged(const QColor& color);

public slots:
   void onSaveConfigPressed();
   void onLoadConfigPressed();
   void onDrawQualityChanged(int value);
   void onLayerSkipChanged(int value);
   void onBackgroundColorPressed();
   void onDefaultPressed();

private:
   void setupUI();
   void setupConnections();

   void setBackgroundColor(const QColor& color);

   PreferenceData    mPrefs;

   QTabWidget*       mTabWidget;

   QPushButton*      mSaveConfigurationButton;
   QPushButton*      mLoadConfigurationButton;
   QComboBox*        mDrawQualityCombo;
   QSpinBox*         mLayerSkipSpin;
   QPushButton*      mBackgroundColorButton;

   QPushButton*      mOkButton;
   QPushButton*      mCancelButton;
   QPushButton*      mDefaultButton;
};

#endif // PREFERENCES_DIALOG_H
