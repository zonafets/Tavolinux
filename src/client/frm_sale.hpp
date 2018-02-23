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

#ifndef FRM_SALE_H
  #define FRM_SALE_H

/* from di gestione delle sale, inserimento/modifica tavoli */

  #include "frm_base.hpp"
  #include "frm_main.hpp"
  #include "frm_ok_exit.hpp"
  #include "tr_tavoli_view.hpp"
  #include "tr_label.hpp"
  #include "tr_listview.hpp"
  #include "tr.hpp"

  #include <qvariant.h>
  #include <qpixmap.h>
  #include <qdialog.h>
  #include <qlistbox.h>

class frm_sale:public virtual frm_base, public tr::enumeratori::sala {
  Q_OBJECT;



public:
  frm_sale( frm_main*parent=0, const char*name=0, bool modal=true, WFlags fl=MYFLAGS );
  ~frm_sale();

  tr_label*imposta_sala;
  tr_label*tavolo;
  tr_label*tipo;
  tr_label*cancella;
  tr_label*exit;
  tr_tavoli_view*tavoli;

  itm_tavolo*selezionato() {
    return tavoli->selezionato();
  };

  itm_sala&sala() {
    return tavoli->sala();
  };

  void carica_sala( int id, bool vista );
  void carica_sala();

  bool multi_selezione() {
    return false;
  }

  void stato_pulsanti();

  public slots:void imposta_sala_clicked();
  void tavolo_clicked();
  void tipo_clicked();
  void cancella_clicked();
  void exit_clicked();
  void tavoli_clicked( QMouseEvent* );
  void scelta_sala_clicked();


  protected slots:void init();
  void resizeEvent( QResizeEvent*ev );

protected:
  int _id_prec_selezionato;
  frm_main*form_main;

private:
  int n_pixmap;
  QStringList pixmap_tavoli;
};

class frm_sale_imposta_tavoli;



class frm_sale_imposta:public frm_base, public tr::enumeratori::sala {
  Q_OBJECT;



public:
  frm_sale_imposta( frm_sale*parent=0, const char*name=0, bool modal=true, WFlags fl=MYFLAGS );
  ~frm_sale_imposta();

  tr_listview*elenco;
  QListBox*tipo;

  tr_label*entra;
  tr_label*tavoli;
  tr_label*rinomina;
  tr_label*aggiungi;
  tr_label*cancella;
  tr_label*exit;

  void stato_pulsanti( QListViewItem*it=NULL );

  bool selezionata() {
    return elenco->selezionato()!=QString::null?true:false;
  }

  tr::mse::riga sala() {
    return elenco->sala();
  }

  void aggiorna( int id_sala );
  bool modificato;

  public slots:void elenco_clicked( QListViewItem*it );
  void entra_clicked();
  void tavoli_clicked();
  void rinomina_clicked();
  void aggiungi_clicked();
  void cancella_clicked();
  void exit_clicked();

  void show();

protected:
  void init();

private:
  int id_sala;
  frm_sale*form_sale;
  friend class frm_sale_imposta_tavoli;
};



class frm_sale_imposta_tavoli:public frm_ok_exit, public tr::enumeratori::sala {
  Q_OBJECT;



public:

  enum tr_tipo {
    tp_sala, tp_tavolo, tp_tavoli
  };

  frm_sale_imposta_tavoli( frm_sale_imposta*parent=0, const char*name=0, bool modal=true, WFlags fl=MYFLAGS );

  frm_sale_imposta_tavoli( frm_sale*parent=0, const char*name=0, bool modal=true, WFlags fl=MYFLAGS );

  ~frm_sale_imposta_tavoli();

  tr_input*nome;
  tr_listview*tavoli;
  tr_listview*pavimenti;
  tr_listview*stampanti;

  void stato_pulsanti();

  public slots:void nome_textChanged( const QString& );
  void ok_clicked();
  void extra_clicked();
  void tavoli_clicked( QListViewItem* );
  void pavimenti_clicked( QListViewItem* );
  void show();

private:
  void costruttore_comune( tr_tipo tipo );
  tr_tipo _tipo;

  frm_sale*form_sale;
  frm_sale_imposta*form_sale_imposta;

};

#endif
