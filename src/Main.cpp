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

#include <QApplication>
#include <QDesktopWidget>

#include <MainWindow.h>

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   MainWindow window;
   window.resize(800, 600);
   int desktopArea = QApplication::desktop()->width() *
                     QApplication::desktop()->height();
   int widgetArea = window.width() * window.height();
   if (((float)widgetArea / (float)desktopArea) < 0.75f)
      window.show();
   else
      window.showMaximized();
   return app.exec();
}
