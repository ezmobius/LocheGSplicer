/*
 * LocheGSplicer
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
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
 *
 * Author: Lochemage
 */

#include <QApplication>
#include <QDesktopWidget>

#include <Window.h>

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   Window window;
   window.resize(window.sizeHint());
   int desktopArea = QApplication::desktop()->width() *
                     QApplication::desktop()->height();
   int widgetArea = window.width() * window.height();
   if (((float)widgetArea / (float)desktopArea) < 0.75f)
      window.show();
   else
      window.showMaximized();
   return app.exec();
}
