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

#ifndef FRM_TASTIERA_H
  #define FRM_TASTIERA_H



/** il corpo �definito in tools.cpp */


  #include "frm_base.hpp"
  #include "tr_lineedit.hpp"
  #include "tr_label.hpp"

  #define  LARGHEZZA_PULSANTE_TASTIERA 54
  #define  ALTEZZA_PULSANTE_TASTIERA 54
  #define COLORE_PAD QColor(233,235,183)
  #define COLORE_TASTIERA QColor(253,255,203)

class frm_tastiera:public frm_base {
  Q_OBJECT

  public:frm_tastiera( QWidget*parent=0, const char*name=0, bool modal=FALSE, WFlags fl=MYFLAGS );
  ~frm_tastiera();

  tr_input*pannello;
  QFrame*tastiera;
  QFrame*pad;
  tr_label**tasti;

  static const int t_bs=7;
  static const int t_spazio=34;
  static const int t_ok=11;
  static const int t_canc=0;

  tr_label*maiuscole;
  tr_label*minuscole;
  tr_label*simboli;
  tr_label*exit;
  tr_label*ok;
  tr_label*sinistra;
  tr_label*destra;
  tr_label*cancella;

  tr_label*canc;
  tr_label*bs;
  tr_label*spazio;
  tr_label*libero;

  void imposta_maiuscole();
  void imposta_minuscole();
  void imposta_simboli();

  public slots:void tasto_clicked();
  void ok_clicked();
  void exit_clicked();
  void canc_clicked();
  void cancella_clicked();
  void bs_clicked();
  void spazio_clicked();
  void maiuscole_clicked();
  void minuscole_clicked();
  void simboli_clicked();
  void sx_clicked();
  void dx_clicked();

protected:
  void init();

private:
  tr_input::tr_elenco _elenco;

};

#endif
