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

#ifndef TR_COLORI_HPP
  #define TR_COLORI_HPP

  #include "tr_label.hpp"

  #define COLROWS 2
  #define COLCOLS 6

class tr_colori:public tr_widget {
  Q_OBJECT;



public:

  tr_colori( QWidget*parent, const char*name );

  void select( QColor colore );
  QColor selected();

  protected slots:void colore_clicked();

private:

  tr_label*selezionato;
  tr_label*label;
  tr_label*colori[COLROWS] [COLCOLS];

};

#endif
