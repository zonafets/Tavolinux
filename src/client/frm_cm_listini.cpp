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

#include <iostream>
#include <qdatetime.h>
#include <qapplication.h>
#include <qregexp.h>
#include <qfile.h>
#include <qdatastream.h>

#include "cmdcodicigruppi.hpp"
#include "frm_scelta.hpp"
#include "tr_menu.hpp"
#include "frm_cm.hpp"
#include "tr_tools.hpp"
#include "tr_globals.hpp"

#include "tr_enumeratori.hpp"

#include <qurl.h>

#define USA_PRN_PADRE tr("Usa stampanti del padre")

extern tr_tools tools;
extern tr_globals glb;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77
//        FRM_CM_LISTINI
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

frm_cm_listini::frm_cm_listini( frm_cm*parent, const char*name, bool modal, WFlags fl ):frm_scelta( parent, name, modal, fl ) {

  titolo->setText( tr( "gestione listini" ) );

  form_cm=parent;

  base_alza();
  barra_titolo->hide();

  scopri_pulsantiera(); // x default frm_scelta la nasconde

  connect( elenco, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( elenco_clicked( QListViewItem* ) ) );
  connect( aggiungi, SIGNAL(clicked()), this, SLOT( add_cop_mod_clicked() ));
  connect( modifica, SIGNAL(clicked()), this, SLOT( add_cop_mod_clicked() ));
  CONNECT( cancella );

  init();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77
frm_cm_listini::~frm_cm_listini() {
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm_listini::init() {
  id_listino=form_cm->id_listino;
  tipo( tr_listview::listini_modifica, QSN(id_listino) );

  resize( minimumSizeHint() );
  stato_pulsanti();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm_listini::stato_pulsanti() {
  bool b=elenco->selezionato();
  if ( !b ) {
    id_listino=-1; nascondi( cancella, modifica, imposta );
  }
  else
    scopri( cancella, modifica, imposta );

  if ( !elenco->selezionato().isEmpty()) aggiungi->cambia_testo(tr("copia"));
  else aggiungi->cambia_testo(tr("aggiungi"));

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm_listini::elenco_clicked( QListViewItem*it ) {
  stato_pulsanti();
  if ( it ) id_listino=it->text( 1 ).toInt();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm_listini::add_cop_mod_clicked() {

  enum {
    id, descrizione, da_data, a_data, giorni, intervalli
      };

  bool add=false;
  bool mod=false;
  bool cop=false;

  if (elenco->selezionato().isEmpty() && sender()==aggiungi) add=true;
  if (!elenco->selezionato().isEmpty() && sender()==aggiungi) cop=true;
  if (!elenco->selezionato().isEmpty() && sender()==modifica) mod=true;

  frm_cm_listini_inserisci f( this, "frm_cm_listini_inserisci" );

  if (!elenco->selezionato().isEmpty()) {
    tr::mse listini(tok_elenco_listini_esteso);
    listini.carica( frm_scelta::id() );

    if ( listini.size() ) {
      tr::mse::riga l(listini[0]);

      if (cop) f.barra_stato->setText( tr("Copia Listino ")+l[p_descrizione]);
      if (mod) {
        f.barra_stato->setText( tr("Modifica Listino ")+l[p_descrizione]);
        f.codice->setText( l[p_codice] );
        f.descrizione->setText( l[p_descrizione] );
      }

      if (!add) {
        f.da_data->setText( tools.conforma_data(l[p_da_data]));
        f.a_data->setText( tools.conforma_data(l[p_a_data]));

        QByteArray b=tools.ba(l[p_intervalli]);

        f.intervalli->intervalli( b );
        f.giorni( l[p_giorni].toInt() );
      } // if mod
    }
    else {
      tools.msgbox("Risultato in'atteso. Listino non trovato");
    }
  }


  if ( f.accepted() ) {

    QByteArray b=f.intervalli->intervalli();

    tr::enumeratori::operazione_listino op;
    int id_copia=0;
    if (elenco->selezionato().isEmpty()) op = tr::enumeratori::listino_vuoto;
    else {
      op = tr::enumeratori::listino_copiato;
      id_copia = frm_scelta::id().toInt();
    }
    QString cod=NZ(f.codice->text());
    QString desc=NZ(f.descrizione->text());
    QString da_data=tools.data_dataiso(f.da_data->text());
    QString a_data=tools.data_dataiso(f.a_data->text());
    QString bi = tools.stringa(b);
    if (!mod) {
      if (bi.isEmpty()) bi="NULL";
      CMD( tok_inserisci_listino, QSN(op),QSN(id_copia),"0",
           cod, desc, da_data, a_data, bi, QSN(f.giorni() ) );
    }
    else
    {
      if (bi.isEmpty()) bi="NULL";
      if (da_data.isEmpty()) da_data="NULL";
      if (a_data.isEmpty()) a_data="NULL";
      CMD(tok_modifica_listino,frm_scelta::id(), cod, desc,
          da_data,a_data,bi,QSN(f.giorni()) );
    }

    elenco->carica( QSN(id_listino) );
  }

  stato_pulsanti();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm_listini::cancella_clicked() {
  elenco->carica( QSN(id_listino) );
  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77
//        FRM_CM_LISTINI::INSERISCI
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77
frm_cm_listini_inserisci::frm_cm_listini_inserisci( frm_cm_listini*parent, const char*name, bool modal, WFlags fl ):
    frm_ok_exit( parent, name, modal, fl ) {
  titolo->setText( tr( "inserimento/modifica/copia listino" ) );

  form_listini=parent;

  // elenco etichette

  QString validatore=tools.validatore( "listino.id" );
  QString falso=tools.segna_posto( validatore );
  NEW( codice, tr_input( area, "codice", tr( "Codice" ), falso, validatore ) );

  NEW(  descrizione, tr_input( area, "descrizione", tr( "Descrizione" ),
        QString(), tools.validatore("listino.descrizione")) );

  NEW( da_data, tr_input( area, "da_data", tr( "Da Data" ),
        QString(), "date"));
  NEW( a_data, tr_input( area, "a_data", tr( "A Data" ),
        QString(), "date"));

  NEW( intervalli, tr_time( area, "intervalli" ) );
  intervalli->evidenzia( 0, 0 ); // inizialmente evidenzia tutto

  // elenco giorni

  QString giorni[]= {
    tr( "LUN" ), tr( "MAR" ), tr( "MER" ), tr( "GIO" ), tr( "VEN" ), tr( "SAB" ), tr( "DOM" )
      };
  tr_label**chks[]= {
    &lun, &mar, &mer, &gio, &ven, &sab, &dom, NULL
      };

  // inizializza i check box per i giorni

  for ( int i=0; chks[i]!=NULL; i++ ) {
    tr_label*p;
    NEW( p, tr_label( area, giorni[i] ) );
    p->setText( giorni[i] );
    p->aspetto_check( true );
    p->setMinimumWidth( 100 );
    *chks[i]=p;
  }


  QWidget*ws[]= {
    codice, codice, descrizione, descrizione, descrizione, descrizione, descrizione, 0, da_data, da_data, a_data, a_data, vuoto(), vuoto(), vuoto(),
    0, intervalli, intervalli, intervalli, intervalli, intervalli, intervalli, intervalli, 0, lun, mar, mer,
    gio, ven, sab, dom, 0, 0
  };

  build( ws );

  // codice.isEmpty -> ok.hide, descrizione.disabled, etcc
  QWidget*w[]= {
    descrizione, da_data, a_data, intervalli, lun, mar, mer, gio, ven, sab, dom, NULL
      };
  link( codice, w );

  // sostituisce ok_clicked()
  disconnetti_ok();
  CONNECT( ok );

  init();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

frm_cm_listini_inserisci::~frm_cm_listini_inserisci() {
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm_listini_inserisci::init() {
  barra_stato->setText( tr( "Inserimento Listino" ) );
  barra_stato->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm_listini_inserisci::ok_clicked() {
  if ( !codice->text().isEmpty() ) {
    QString err;

    QDate dd;
    QDate ad;
    QTime dor;
    QTime aor;

    if ( !tools.data_valida( da_data->text(), &dd ) ) err+=tr( "campo \"da data\" non valido\n" );
    if ( !tools.data_valida( a_data->text(), &ad ) ) err+=tr( "campo \"a data\" non valido\n" );

    if ( dd.isNull()&&!ad.isNull() ) err+=tr( "specificare anche il campo \"dalla data\"\n" );
    if ( !dd.isNull()&&ad.isNull() ) err+=tr( "specificare anche il campo \"alla data\"\n" );
    if ( dor.isNull()&&!aor.isNull() ) err+=tr( "specificare anche il campo \"da ora\"\n" );
    if ( !dor.isNull()&&aor.isNull() ) err+=tr( "specificare anche il campo \"a ora\"\n" );
    if ( !dd.isNull()&&!ad.isNull()&&dd>ad ) err+=tr( "il campo \"da data\" posteriore a \"alla data\"\n" );

    if ( !err.isEmpty() ) {
      tools.msgbox( err ); return;
    }

    accept();


  }
  else
    tools.msgbox( tr( "Immettere almeno il codice" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

int frm_cm_listini_inserisci::giorni() {
  // prepara la variabile per i giorni
  int giorni=0;
  tr_label*chks[]= {
    lun, mar, mer, gio, ven, sab, dom, NULL
      };
  int i=0;
  while ( chks[i]!=NULL ) {
    if ( chks[i]->isOn() ) giorni|=( 1<<i ); i++;
  }
  return giorni;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm_listini_inserisci::giorni( int g ) {
  tr_label*chks[]= {
    lun, mar, mer, gio, ven, sab, dom, NULL
      };
  int i=0;
  while ( chks[i]!=NULL ) {
    if ( g&( 1<<i ) ) chks[i]->stato( tr_label::on );
    else
      chks[i]->stato( tr_label::off );
    i++;
  }
}

