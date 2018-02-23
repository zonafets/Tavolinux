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

#ifndef FRM_CONTO_H
  #define FRM_CONTO_H

/* form per gestione conto */
  #include "frm_base.hpp"
  #include "tr_menu.hpp"
  #include "tr_label.hpp"
  #include "tr_comanda.hpp"
  #include "tr_listview.hpp"
  #include "tr_tavoli_view.hpp"
  #include "frm_comanda.hpp"
  #include "tr_enumeratori.hpp"

class frm_conto:public frm_base {
  Q_OBJECT;



public:
  frm_conto( QWidget*parent=0, const char*name=0, bool modal=true, WFlags fl=MYFLAGS );
  ~frm_conto();

  tr_label*dividi;
  tr_label*annulla;
  tr_label*altro;
  tr_label*libero1;
  tr_label*stampa;
  tr_label*libero2;
  tr_label*a_comanda;
  tr_label*cancella;
  tr_label*exit;


  QFrame*puls_panel;
  tr_label*puls_sx;
  tr_label*puls_dx;

  tr_comanda*comanda;
  tr_comanda*separata;
  tr_comanda*pagamento;
  tr_listview*forme_pagamenti;

  bool apri_tavolo();
  bool apri_tavolo( const itm_sala&sata, const itm_tavolo&t, int bis );
  bool apri_tavolo( const itm_sala&sata, const itm_tavolo&t );
  void chiudi_tavolo();
  void intestazione( int id, QString ragione_sociale, double aiva );

  bool scambia;
  void contanti();

  bool conto_aperto();
  bool apri_conto();
  bool apri_conto( tr_comanda::conti );
  bool chiudi_conto( tr_comanda::conti_chiusi tipo=tr_comanda::cnt_definitivo );

  void stato_pulsanti();

  enum tipo_doc {
    td_ricevuta, td_scontrino, td_fattura, td_proforma, td_concordato, td_devnul
  };

  bool print( tipo_doc tipo );

  public slots:void dividi_clicked();
  void annulla_clicked();
  void altro_clicked();
  void stampa_clicked();
  void a_comanda_clicked();
  void cancella_clicked();
  void exit_clicked();

  void card_readed();

  void intestazione_clicked( bool rientro=false );

  void comanda_clicked( int );
  void comanda_qta_clicked( int );
  void comanda_prz_clicked( int );
  void comanda_totale_clicked();
  void comanda_qta_changed( int );
  void comanda_prz_changed( int );

  void separata_clicked( int );
  void separata_prz_clicked( int );

  void pagamento_clicked( int );
  void pagamento_qta_clicked( int );
  void pagamento_prz_clicked( int );

  void forme_pagamenti_clicked( QListViewItem* );
  void show();

  void puls_clicked();

protected:
  void init();

  void dividi_conto();

  tr_tavolo tavolo;
  itm_sala sala;

private:
  bool devnul;
  QStringList stato_tavolo;

  enum t_comanda {
    c_comanda, c_separata, c_pagamento
  };

  void aggiorna( t_comanda controllo, bool solo_stato=false );
  void aggiorna( bool solo_stato=false );
  bool _divisione_fittizia;

  bool multi_conto() {
    return comanda->modalita()==tr_comanda::m_conti?true:false;
  }

  bool separato() {
    return separata->righe();
  } // indica se c'�un conto separato in corso

};

#endif
