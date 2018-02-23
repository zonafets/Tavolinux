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

#ifndef TR_CANVAS_HPP
  #define TR_CANVAS_HPP

  #include "tr_widget.hpp"
  #include <qimage.h>
  #include <qpicture.h>
  #include <qpixmap.h>

class tr_canvas:public tr_widget {
  Q_OBJECT public:tr_canvas( tr_widget*parent, const char*name );

  QString data();
  void data( QString );

  void clear();

  enum tr_mode {
    m_draw, m_erase
  };

  tr_mode _mode;

  void draw() {
    _mode=m_draw;
  }

  void erase() {
    _mode=m_erase;
  }

  tr_mode mode() {
    return _mode;
  }

protected:
  void mousePressEvent( QMouseEvent*e );
  void mouseMoveEvent( QMouseEvent*e );
  void paintEvent( QPaintEvent*e );

  void leaveEvent( QEvent* );
  void enterEvent( QEvent* );

  QPixmap pict;

  QPoint last_pos;


};

#endif
