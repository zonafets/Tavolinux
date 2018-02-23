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

#ifndef TR_TAB_WIDGET_HPP
#define TR_TAB_WIDGET_HPP

#include <qwidgetstack.h>
// #include <qtabbar.h>

class tr_tab_widget:public QWidgetStack {
    Q_OBJECT;
public:
    tr_tab_widget(QWidget*parent=0,const char* name=0):QWidgetStack(parent,name)
    { // tabBar()->hide();
      setFrameStyle( QFrame::NoFrame | QFrame::Plain );
    }
};

#endif
