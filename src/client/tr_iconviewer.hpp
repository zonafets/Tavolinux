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

#ifndef TR_ICONVIEWER_HPP
  #define TR_ICONVIEWER_HPP

  #include "tr_label.hpp"

  #include <qmemarray.h>

class tr_iconviewer:public tr_widget {
  Q_OBJECT;



public:

  tr_iconviewer( QWidget*parent, const char*name, QStringList icone, int nx=3, int ny=3, int dx=-1, int dy=-1, int bordo=1 );

  void select( QString icona );
  QString selected();
  void visualizza( QString icona );

  protected slots:void icona_clicked();

private:

  void visualizza_icone( int spostamento=0 );

  tr_label*prev;
  tr_label*next;
  QMemArray<tr_label*>icone;
  int page_base;
  uint nx, ny;
  uint dx, dy;
  int selezionata;

  QStringList elenco_icone;

};

#endif
