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

#ifndef TR_COMANDA_HPP
  #define TR_COMANDA_HPP

  #include <qlineedit.h>
  #include <qlabel.h>
  #include <qevent.h>

  #include "tr.hpp"
  #include "tr_table.hpp"
  #include "tr_srvcmd.hpp"
  #include "tr_widget.hpp"
  #include "tr_label.hpp"

class Q_EXPORT tr_comanda:public tr_widget, public tr::enumeratori {
  Q_OBJECT;

public:

  class tr_selezione {
  public:
    tr_selezione() : stato(0) {}
    int riga;
    int colonna;
    QString qta;
    QString descrizione;
    QString tot_riga;
    int livello;
    int id_riga;
    union {
      int id_istanza;
      int is;
    };
    int id_articolo;
    int id_varianti;
    money prezzo;
    tr::tipo_articolo stato;

    bool portata;
    bool offerto;

  };

  class tr_riga_modificata {
	  public:
		  tr_riga_modificata() : token_op ( tok_endoftokens ) {}
		  QString quantita;
		  QString descrizione;
		  QString prezzo;
		  TRTokens token_op;

		  void aggiorna ( TRTokens t,
			  QString q = QString::null,
			  QString d = QString::null,
			  QString p = QString::null)
			  {
				  quantita = q;
				  descrizione = d;
				  prezzo = p;
				  token_op = t;
			  }

		  }; // tr_riga_modificata

  // indica in che modalit�di funzionamento si trova la comanda

  enum t_modalita { m_comanda, m_conto, m_separata, m_pagamento, m_conti, m_ffm };

  tr_comanda( QWidget*parent=0, const char*name=0 );

  // widgets

  tr_table*tabella;

  tr_label*su;
  tr_widget*spazio;
  tr_label*liv0; // antipasti
  tr_label*liv1; // primi
  tr_widget*spazio_1;
  tr_label*piu;
  tr_label*meno;
  tr_widget*spazio_2;
  tr_label*liv2; // secondi
  tr_label*liv3; // contorni
  tr_widget*spazio_3;
  tr_label*giu;

  tr_label*tot;
  tr_label*totale;

  double totale_comanda();
  double residuo();

  void totale_comanda( double t );
  void totale_comanda( QString t );

  int riga_da_id_riga( int id_riga ); // serve per cose tipo:  seleziona( riga_da_id_riga( 4 ) )
  void seleziona( int row, int col );
  void seleziona( int row );
  void seleziona(); // deseleziona eventualmente la riga o le righe corrente/i
  tr_selezione*selezionato();

  bool prima_riga_portate() {
    if ( selezionato() )
      if ( selezionato()->id_riga==_id_prima_riga_portate ) return true; return false;
  }

  void modalita( t_modalita m );

  t_modalita modalita() { return _modalita; };

  tr_selezione selezione( int row );

  tr_selezione ultimo() { return selezione( tabella->numRows()-1 ); };

  tr_selezione ultimo_articolo(); // restituisce i dati dell'ultima riga in tabella che sia un articolo

  bool modificato() { return _modificato; };

  void modificato( bool m ) { _modificato=m; };

  bool acquisisci_portata( int id_riga, double qta=1 );
  bool inserisci_articolo( int is );
  bool inserisci_articolo( int is, int is_padre,
    tr::tipo_articolo stato,
    bool var_metti=true,
    double* qta=NULL,
    QString prz=LEAVE,
    QString descrizione=LEAVE );
  bool inserisci_articolo(QString barcode, double qta);
  bool inserisci_pagamento( int id_istanza, QString prz=LEAVE );
  bool inserisci_conto( double prezzo );
  bool apri_conto( conti tipo=cnt_globale );
  bool chiudi_conto( conti_chiusi tipo=cnt_definitivo );
  bool annulla_conto();
  bool cancella();
  bool offri();
  bool annulla(); // per ora �abilitato solo come annulla conto
  bool modifica_descrizione( QString nuova );
  bool registrata() 	{return _registrata;}

  void scroll_su();
  void scroll_giu();

  void visualizza_ultimo() { tabella->ensureCellVisible( righe()-1, 0 ); };

  bool aggiorna(bool tieni_selezione=true); // legge o rilegge la comanda dal server e carica la tabella interna

  bool registra(); // registra la comanda, se modificato() �true
  bool stampa();
  bool nuova();

  void show_levels(); // solo se config.multi_comanda e' abilitato

  void adatta() { tabella->adatta(); }

  int righe() { return tabella->numRows(); };

  int righe_comanda();
  int righe_portate();

  bool portata() { return _portata; }; // indica se nell'elenco c'�almeno una portata nuova

  bool comanda() { return _comanda; }; // indica se nell'elenco c'�almeno una portata di comanda

  bool conto_aperto() { return _conto_aperto; };

  void cambia_sfondo( const QColor&colore );
  void cambia_sfondo( const QColor&colore, const QColor&colore_tabella_totale );

  bool cambia_prz( int row );
  bool cambia_qta( int row );
  bool cambia_qta( double qta );

  bool event( QEvent* );

  bool darkable() { return _darkable; }

  void darkable( bool d ); // imposta il proprio e quello dei ctrls contenuti

  void setMinMax( int mw, int mh, int MW=-1, int MH=-1 );

  tr::ordine dati;

  tr_riga_modificata ultimoDato() { return _ultimo_dato; }

  enum tr_tipo_col{tcol_qta,tcol_desc,tcol_prz,tcol_iva,t_col_data};
  tr_tipo_col tipo_colonna(int n_col);

signals:
  void articolo_selezionato( int row );
  void qta_clicked( int row );
  void prz_clicked( int row );
  void qta_changed( int row );
  void prz_changed( int row );
  void totale_clicked();


public slots:
  //enum evento { press, clic, dclic};
  //void 	tabella_toccata( evento e, int row, int col, int button, const QPoint &);
  void tabella_pressed( int row, int col, int button, const QPoint& );
  void tabella_valueChanged( int row, int col );
  void meno_clicked();
  void piu_clicked();
  void su_clicked();
  void giu_clicked();
  void liv_clicked();
  //void 	tabella_clicked( int row, int col, int button, const QPoint &);
  //void 	tabella_doubleClicked( int row, int col, int button, const QPoint &);
  //void 	tabella_currentChanged ( int row, int col );
  //void tabella_selectionChanged ()    { cout << "selectionChanged\n";};

private:

  tr_selezione _selezionato;
  bool _portata;
  bool _comanda;
  bool _modificato;
  bool _registrata;
  t_modalita _modalita; // per fault �comanda
  int _id_prima_riga_portate;
  bool _darkable;
  static bool _conto_aperto;
  tr_riga_modificata _ultimo_dato;

};

#endif
