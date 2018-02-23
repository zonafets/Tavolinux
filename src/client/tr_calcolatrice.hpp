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

#ifndef TR_CALCOLATRICE_HPP
  #define TR_CALCOLATRICE_HPP

  #include "tr_widget.hpp"
  #include <qvalidator.h>
  #include <qlcdnumber.h>
  #include <qlineedit.h>

  #include "frm_base.hpp"
  #include "tr_lineedit.hpp"
  #include "frm_base.hpp"

class tr_calcolatrice:public tr_widget {
  Q_OBJECT;



public:
  tr_calcolatrice( QWidget*parent, const char*name );
  QLCDNumber*pannello;

  void display( double num );

  enum modo {
    decimale_fisso, decimale_mobile
  };

  void impostazioni( QString valore, QString validatore, modo _modalita=decimale_fisso );

  void wide();

  bool accepted() {
    return accettato;
  }

  bool rejected() {
    return rigettato;
  }

  double value() {
    return pannello->value();
  }

  QString text();

signals:
  void clicked();

private:
  bool accettato;
  bool rigettato;
  tr_validator limiti;
  bool primo_tasto;
  QString snum;
  modo modalita;
  uint num_decimali;

  tr_label*tasto_decimale_zerozero;
  static const int ts_000=-1;
  static const int ts_back_space=-2;
  static const int ts_ok=-3;
  static const int ts_segno=-4;
  static const int ts_exit=-5;
  static const int ts_cambia_segno=-6;
  static const int ts_00=-7;

  QFrame*fattori;
  QVBoxLayout*fattori_layout;


  private slots:void tasto_clicked();
  void fattori_clicked();

};

#endif
