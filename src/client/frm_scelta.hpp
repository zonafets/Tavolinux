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

#ifndef FRM_SCELTA_H
  #define FRM_SCELTA_H

  #include "tr_label.hpp"
  #include "frm_base.hpp"
  #include "tr_listview.hpp"
  #include "tr_enumeratori.hpp"

  #include <qlayout.h>
  #include <qlistview.h>

class frm_scelta:public frm_base {
  Q_OBJECT

  public:frm_scelta( QWidget*parent=0, const char*name=0, bool modal=FALSE, WFlags fl=MYFLAGS );
  ~frm_scelta();

  tr_listview*elenco;
  tr_listview*elenco1;
  tr_listview*elenco2;
  QFrame*tasti_rapidi;

  tr_label*imposta;
  tr_label*aggiungi;
  tr_label*sottogrp;
  tr_label*modifica;
  tr_label*cancella;
  tr_label*exit;

  void tipo( tr_listview::tr_tipo __tipo, QString id_selezione=QString::null, QString id_gruppo=QString::null );

  QString descrizione();
  QString id();

  QString record() {
    return elenco->record();
  }

  bool aggiorna();

  void stato_pulsanti();

  bool mostra_imposta;

private:
  bool _modificato;
  tr_listview::tr_tipo _tipo;

public:
  bool modificato() {
    return _modificato;
  }

  void modificato( bool v ) {
    _modificato=v;
  }

  public slots:void show();

  void imposta_clicked();
  void aggiungi_clicked();
  void sottogrp_clicked();
  void modifica_clicked();
  void cancella_clicked();
  void exit_clicked();
  void rapido_clicked();
  void elenco_clicked( QListViewItem*it );
  void elenco1_clicked( QListViewItem*it );

protected:


  void init();

  // vedere config.max_tasti_rapidi
  tr_label*rapido[10]; // array di puntatori a tasti rapidi
};

#endif
