/*
TAVOLINUX touch screen tools for Linux
Copyright (C) 2002-2005  Stefano Zaglio

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details(LICENSE.GPL).

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

Stefano Zaglio (stefanozaglio AT yahoo DOT it)

*/

#ifndef TR_DMACRO2_HPP
#define TR_DMACRO2_HPP

#ifndef TR_CONFIG_CPP
#include "tr_config.hpp"
extern tr_config config;
#endif

// #define PTEXT(icon, text)   QString("<img name=\"img\" src=\"") + QString(icon) + QString("\" width=\"32\" height=\"32\"><br><b>")+ text + QString("</b>")
inline QString PTEXT(QString icon, QString text) {
  return QString("<img name=\"img\" src=\"") + icon + "\" width=\""+QString::number(config.icon_size)+"\" height=\""+QString::number(config.icon_size)+"\"><br><b>"+ text + QString("</b>");
}
// #define PICOTEXT(icon, text)   QString("<img name=\"img\" src=\"") + QString(icon) + QString("\" width=\"32\" height=\"32\"><b>")+ tr(text.latin1()) + QString("</b>")
inline QString PICOTEXT(QString icon, QString text) {
  QString("<img name=\"img\" src=\"") + QString(icon) + "\" width=\""+QString::number(config.icon_size)+"\" height=\""+QString::number(config.icon_size)+"\"><b>"+ text + QString("</b>");
}
inline QString P32ICON(QString icon) {
  return (QString("<img name=\"img\" src=\"") + QString(icon) + "\" width=\""+QString::number(config.icon_size)+"\" height=\""+QString::number(config.icon_size)+"\">");
}
inline QString PICON(QString icon) {
  return (QString("<img name=\"img\" src=\"") + QString(icon) + QString("\">"));
}

#endif
