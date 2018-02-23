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

#ifndef FRM_COMANDA_H
  #define FRM_COMANDA_H

/* form per inserimento comande */

  #include "frm_base.hpp"
  #include "frm_main.hpp"
  #include "tr_menu.hpp"
  #include "tr_label.hpp"
  #include "tr_comanda.hpp"
  #include "tr_tavoli_view.hpp"
  #include "tr_calcolatrice.hpp"
  #include "tr_enumeratori.hpp"

  #include <qvariant.h>
  #include <qpixmap.h>
  #include <qdialog.h>
  #include <qdict.h>

class frm_comanda:public frm_base {
  Q_OBJECT;



public:
  frm_comanda( QWidget*parent=0, const char*name=0, bool modal=true, WFlags fl=MYFLAGS );
  ~frm_comanda();

  tr_label*altro;
  tr_label*calc;
  tr_label*trasmetti;
  tr_label*varianti;
  tr_label*segue;
  tr_label*insieme_a;
  tr_label*conto;
  tr_label*cancella;
  tr_label*exit;
  tr_menu*menu;
  tr_comanda*comanda;
  tr_calcolatrice*calcolatrice;

  bool scambia;
  bool parcheggiata;

  bool apri_tavolo();
  bool apri_tavolo( const itm_sala&sata, const itm_tavolo&t, int bis );
  bool apri_tavolo( const itm_sala&sata, const itm_tavolo&t );
  void chiudi_tavolo();

  bool stampa();
  bool trasmetti_comanda();
  bool controllo_tessera();

  enum tipo_doc {
    td_ricevuta, td_scontrino, td_fattura, td_proforma, td_concordato, td_devnul
  };

  bool print( tipo_doc tipo );

  public slots:void altro_clicked();
  void calc_clicked();
  void trasmetti_clicked();
  void varianti_clicked();
  void segue_clicked();
  void insieme_a_clicked();
  void conto_clicked();
  void cancella_clicked();
  void exit_clicked();

  void calcolatrice_clicked();

  void comanda_totale_clicked();

  void menu_clicked();
  void comanda_clicked( int );
  void qta_clicked( int );
  void prz_clicked( int );
  void qta_changed( int );
  void prz_changed( int );

  void show();

  void stato_pulsanti();

  void card_readed();

protected:

  itm_tavolo tavolo;
  itm_sala sala;

  void scopri_varianti();
  void apri_conto();

  protected slots:void keyPressEvent( QKeyEvent*e );

private:
  bool devnul;

  QStringList stato_tavolo;

  void aggiorna_comanda( bool solo_stato=false );

  enum {
    metti, togli, senza
  }
  var_metti;

  bool inserisci_articolo( double*qta=NULL, QString prz=LEAVE );

  QString id_listino;

  frm_main*form_main;

};

#endif
