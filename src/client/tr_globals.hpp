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

#ifndef GLOBALS_H
  #define GLOBALS_H

  #include <iostream>
using namespace std;



  #include <cstdlib>

  #include <qwidget.h>
  #include <qregexp.h>
  #include <qlistview.h>

  #define QT_NO_CAST_ASCII true
  #define CMDLANG_SEPARATOR "|"

  #define enu_tessera enum {\
     p_sn, p_rftag,p_famiglia, p_tipo,p_id_p0,p_id_p1,p_id_t0,p_id_t1,p_id_t2,p_pagamenti,\
     p_minimo,p_valore,p_massimo,\
     p_pmin,p_pact,p_pmax,\
     p_scadenza,p_riferimento,p_ragione_sociale,p_sconto_percentuale}

  #define enu_tipi_tessere enum {\
     tt_tavolo=TRPayCard::tessera_tavolo_reale,\
     tt_prepagata=TRPayCard::tessera_prepagata,\
     tt_convenzione=TRPayCard::tessera_prepagata|TRPayCard::tessera_cliente,\
     tt_postpagato=TRPayCard::tessera_tavolo_fittizio,\
     tt_cameriere=TRPayCard::tessera_cameriere,\
     tt_punti=TRPayCard::tessera_punti,\
     tt_cliente=TRPayCard::tessera_cliente\
     }

class tr_globals {
public:
  tr_globals();

  QWidget*menu_popup;
  bool modo_bar;

  QStringList pixmaps;

  QString flags; // flags da login
  void init_flags();

  QString moduli;
  void init_modules();

  QString ragione_sociale;
  QString piva;

  QString lang;

  QStringList cache_menu_listino;

  QStringList voti;

  // stato post-memorizzato per comandi imposta_listino, login, sala e cliente
  enum {
    p_listino_id, p_listino_descrizione, // ogni form ha il suo
    p_utente_id, p_utente_descrizione, p_sala_id, p_sala_descrizione, p_turno_id, p_turno_descrizione, p_stato_last
  };

  QStringList stato;



  class tr_intestazione {
  public:
    tr_intestazione() {
      init();
    }

    void init() {
      id=-1;
      ragione_sociale="";
      aiva=0;
    }

    bool intestato() {
      return ( id!=-1?true:false );
    }

    int id;
    QString ragione_sociale;
    double aiva; // alicuota iva
  }
  intestazione;

};


#endif
