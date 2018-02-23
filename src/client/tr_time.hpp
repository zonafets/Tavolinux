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

#ifndef TR_TIME_HPP
  #define TR_TIME_HPP

  #include <qwidget.h>
  #include <qtable.h>
  #include <qlayout.h>
  #include <qcstring.h>
  #include "tr_label.hpp"

class tr_table_intervalli:public QTable {
  Q_OBJECT;



public:
  tr_table_intervalli( QWidget*parent=0, const char*name=0 );

  void paintFocus( QPainter*, const QRect& ) {
  };

  void paintCell( QPainter*p, int row, int col, const QRect&cr, bool selected, const QColorGroup&cg );

  void evidenzia( int row, int col );
  //bool event ( QEvent * e );

};



class tr_time:public QWidget {
  Q_OBJECT;



public:
  tr_time( QWidget*parent, const char*name );
  ~tr_time();

  tr_label*su;
  tr_label*sp1;
  tr_label*sx;
  tr_label*sp2;
  tr_label*dx;
  tr_label*sp3;
  tr_label*giu;
  QGridLayout*layout;

  tr_table_intervalli*qnibbles;

  static const int dim_intervalli=36;
  QByteArray intervalli();
  void intervalli( QByteArray valori );

  void evidenzia( int row, int col ) {
    qnibbles->evidenzia( row, col );
  }

  public slots:void dir_clicked();
  void qnibbles_currentChanged( int row, int col );
  void qnibbles_click_o_press( int row, int col, int button, const QPoint&mousePos );

private:
  bool ccc;
};


#endif
