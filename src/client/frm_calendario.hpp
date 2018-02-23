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

#ifndef FRM_CALENDARIO_H
  #define FRM_CALENDARIO_H
  #include "frm_base.hpp"
  #include "tr_config.hpp"

  #include <qdatetime.h>
  #include <qwidget.h>
  #include <qlineedit.h>

  #define COLORE_MESE 			QColor(255,237,155)
  #define COLORE_MESE_EVIDENZIATO 	QColor(255,255,127)
  #define COLORE_GIORNO 		QColor(174,228,255)
  #define COLORE_GIORNO_EVIDENZIATO 	QColor(160,255,255)

  #define LARGHEZZA_PULSANTE_CALENDARIO 	54
  #define ALTEZZA_PULSANTE_CALENDARIO 	54

extern bool calendario( QWidget*parent=0, QDate*data=NULL );
extern bool calendario( QLineEdit*le );



class frm_calendario:public frm_base {
  Q_OBJECT public:frm_calendario( QWidget*parent=0, const char*name=0, bool modal=TRUE, WFlags fl=MYFLAGS );
  ~frm_calendario();

  static const int fattore_illuminazione=120;
  QColor colore_domenica;

  tr_label*giorni[42];
  tr_label*mesi[12];

  QDate data() {
    return _data;
  };

  bool data( QDate d );

  bool cancellato() {
    return _cancellato;
  }

  tr_label*anno_succ;
  tr_label*anno_prec;
  tr_label*cancella;
  tr_label*exit;
  tr_label*libero;

  public slots:void exit_clicked();
  void cancella_clicked();
  void anno_succ_clicked();
  void anno_prec_clicked();
  void giorno_clicked();
  void mese_clicked();

protected:
  void init();

private:
  bool _cancellato;
  QDate _data;
  QDate _da, _a;
  QColor _colore;
  tr_label*mese_sel;
  tr_label*giorno_sel;
};

#endif
