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

#include "frm_scelta.hpp"
#include "frm_clienti.hpp"
#include "tr_dmacro2.hpp"
#include "tr_tools.hpp"
extern tr_tools tools;

/* HISTORY: 05-12-01  lo scelta tavoli con bis per lo sposta e' stato ridotto ai tavoli semplici
per evitare che si spostasse il contenuto di un tavolo sul bis del bar o asporto, cosa non consentita */


frm_scelta::frm_scelta( QWidget*parent, const char*name, bool modal, WFlags fl ):frm_base( parent, name, modal, fl ) {
  titolo->setText( tr( "scelta" ) );

  uint i;

  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  setMinMax( 0, 0, int( config.finestra_larghezza*0.95 ), int( config.finestra_altezza*0.95 ) );

  base_alza();
  nascondi_titolo();
  nascondi_pulsantiera();

  barra_stato->setPaletteForegroundColor( QColor( 255, 255, 255 ) );

  NEW( elenco, tr_listview( area, "elenco" ) );
  elenco->setMinimumSize( QSize( 440, 360 ) );
  connect( elenco, SIGNAL( close() ), this, SLOT( exit_clicked() ) );
  // elenco->setMaximumSize( QSize( 32767, 32767 ) );  // gi�nel costruttore

  NEW( elenco1, tr_listview( area, "elenco1" ) );
  elenco->setMinimumSize( QSize( 440, 360 ) );
  connect( elenco1, SIGNAL( close() ), this, SLOT( exit_clicked() ) );

  NEW( elenco2, tr_listview( area, "elenco2" ) );
  elenco->setMinimumSize( QSize( 440, 360 ) );
  connect( elenco2, SIGNAL( close() ), this, SLOT( exit_clicked() ) );

  QWidget*wp[]= {elenco, elenco1, elenco2 };
  TROWI( wp ) {
    QPalette p=wp[i]->palette().copy();
    QColorGroup cg( p.active() );
    cg.setColor( QColorGroup::Highlight, config.colore_selezione );
    cg.setColor( QColorGroup::HighlightedText, QColor( 0, 0, 127 ) );
    p.setActive( cg );
    wp[i]->setPalette( p );
  }

  connect( elenco, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( elenco_clicked( QListViewItem* ) ) );

  connect( elenco1, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( elenco1_clicked( QListViewItem* ) ) );

  /* connect( elenco2, SIGNAL(clicked( QListViewItem * )), this, SLOT( elenco2_clicked( QListViewItem * ))); */

  nascondi( elenco1, elenco2 );

  NEW( tasti_rapidi, QFrame( area, "tasti_rapidi" ) );
  tasti_rapidi->setBackgroundOrigin( QFrame::ParentOrigin );
  tasti_rapidi->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  tasti_rapidi->setMinimumSize( QSize( 0, config.altezza_riga ) );
  tasti_rapidi->setMaximumSize( QSize( maximumWidth(), config.altezza_riga ) );
  tasti_rapidi->setFrameShape( QFrame::NoFrame );
  tasti_rapidi->setFrameShadow( QFrame::Plain );
  tasti_rapidi->setPaletteBackgroundColor( QColor( 249, 67, 67 ) );
  QGridLayout*rapido_layout;
  NEW( rapido_layout, QGridLayout( tasti_rapidi, 1, 5, 0, 0, "rapido_layout" ) );

  for ( i=0; i<config.clienti_rapidi_size; i++ ) { // inserisce solo 5 tasti di scelta rapida cliente
    NEW( rapido[i], tr_label( tasti_rapidi, QString::number( i ) ) );
    rapido[i]->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
    rapido[i]->setMinimumSize( QSize( 100, config.altezza_riga ) );
    rapido[i]->setMaximumSize( QSize( 140, config.altezza_riga ) );
    rapido[i]->setBackgroundOrigin( QLabel::ParentOrigin );
    rapido[i]->setPaletteBackgroundColor( QColor( 249, 67, 67 ) );
    //rapido[i]->setAlignment( int( QLabel::AlignCenter ) );
    rapido[i]->setPaletteForegroundColor( WHITE );
    rapido[i]->setText( config.clienti_rapidi_rs[i], 12, true );
    rapido[i]->aspetto_bottone();
    //rapido[i]->setText("tasto rapido<br>libero");
    rapido_layout->addWidget( rapido[i], 0, i );
    connect( rapido[i], SIGNAL( clicked() ), this, SLOT( rapido_clicked() ) );
  }

  QWidget*ws[]= {
        elenco, elenco1, elenco2, 0,
        tasti_rapidi, tasti_rapidi, tasti_rapidi, 0,0 };
  build( ws, 0, 0 );

  modificato( false );

  tr_label**ps[]= { &imposta, &aggiungi, &sottogrp, &modifica, &cancella, &exit };
  QString nomi=TRFORM( ( "imposta#imposta|aggiungi#aggiungi|sottogrp#sottogrp|modifica#modifica|"
     "cancella#cancella|uscita#uscita" ) );
  pulsanti( nomi, false, ps, NULL );
  nascondi( imposta, modifica, cancella, sottogrp );

  CONNECT( imposta );
  CONNECT( aggiungi );
  CONNECT( sottogrp );
  CONNECT( modifica );
  CONNECT( cancella );
  CONNECT( exit );

  init();

}

frm_scelta::~frm_scelta() {
}

void frm_scelta::init() {
  stato_pulsanti();

}

void frm_scelta::show() {
  frm_base::show();
  int w=sizeHint().width();
  elenco->adatta();
  elenco1->adatta();
  elenco2->adatta();
  //frm_base::hide();
  qApp->sendPostedEvents();
  int ww=sizeHint().width();
  if ( ww!=w ) {
    center();
  }
  //frm_base::show();
}

void frm_scelta::exit_clicked() {
  reject();
}

void frm_scelta::imposta_clicked() {
  accept();
}

void frm_scelta::rapido_clicked() {
  tr_label*l=( tr_label* ) sender();
  if ( elenco->tipo()==tr_listview::clienti_rapidi ) {

    if ( !elenco->selezionato().isEmpty() ) {
      for ( uint i=0; i<config.clienti_rapidi_size; i++ )
        if ( config.clienti_rapidi_id[i].toInt()==id() ) BEEPER;
    };

    uint n=QString( l->name() ).toInt();
    config.clienti_rapidi_id[n]=id();
    config.clienti_rapidi_rs[n]=descrizione();
    config.salva_clienti_rapidi();
    l->setText( config.clienti_rapidi_rs[n] );

    modificato( true );
  }
  else {
    QString st=l->name();
    int n=st.toInt();
    st=config.clienti_rapidi_id[n];
    if ( elenco->seleziona( st ) ) accept();
    else
      BEEP;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_scelta::tipo( tr_listview::tr_tipo __tipo, QString id_selezione, QString id_gruppo ) {
  _tipo=__tipo;
  elenco->tipo( _tipo );

  QString st=tr( "Scegliere " );

  tasti_rapidi->hide();
  elenco->nascondi( tr_listview::e_abc );
  elenco->scopri( tr_listview::e_exit );

  switch ( _tipo ) {

    case tr_listview::gruppi:
      st+=( tr( "gruppo" ) );
    break;

    case tr_listview::sale:
      st+=( tr( "sala" ) );
      elenco->unselectable( false );
    break;

    case tr_listview::listini_modifica:
      st=( tr( "Inserisci/Modifica listini" ) );
    break;

    case tr_listview::listini:
      st+=( tr( "listino" ) );
    break;

    case tr_listview::clienti_esteso:
      elenco->abc_clic_effect( false );

    case tr_listview::selezione_clienti:
    case tr_listview::clienti_rapidi:
    case tr_listview::clienti:
      elenco->scopri( tr_listview::e_abc );
      if ( _tipo==tr_listview::clienti ) st+=tr( "cliente" );
      else
        st+=tr( "cliente -> metterlo su -> tasto rapido" );
      if ( _tipo==tr_listview::clienti_rapidi||config.clienti_rapidi_size>0 )
        tasti_rapidi->show();
      sottogrp->setText( PTEXT( "az_sotto_cli_.png", "sotto.cli." ) );
      stato_pulsanti();
      scopri_pulsantiera();
    break;

    case tr_listview::camerieri:
      st+=( tr( "cameriere" ) );
    break;

    case tr_listview::turni:
      st+=( tr( "turno" ) );
      elenco->nascondi( tr_listview::e_exit );
    break;

    case tr_listview::prenotazioni:
      st+=( tr( "prenotazioni" ) );
    break;

    case tr_listview::tavoli: {
        nascondi( cancella, modifica, aggiungi );

        tr_listview*l[]= {elenco, elenco1, elenco2};
        TROWI( l ) {
          l[i]->setMinimumSize( QSize( 220, 360 ) );
          l[i]->nascondi( tr_listview::e_abc );
          l[i]->nascondi( tr_listview::e_exit );
        }
        elenco->tipo( tr_listview::sale );
        elenco1->tipo( tr_listview::tavoli_semplici );
        elenco2->tipo( tr_listview::bis );
        elenco2->carica();
        st+=( tr( "tavolo di arrivo" ) );
        scopri( elenco1, elenco2 );
        stato_pulsanti();
        scopri_pulsantiera();
      }
    break;

    case tr_listview::tavoli_semplici: {
        nascondi( cancella, modifica, aggiungi );

        tr_listview*l[]= {elenco, elenco1, elenco2};
        TROWI( l ) {
          l[i]->setMinimumSize( QSize( 220, 360 ) );
          l[i]->nascondi( tr_listview::e_abc );
          l[i]->nascondi( tr_listview::e_exit );
        }
        elenco->tipo( tr_listview::sale );
        elenco1->tipo( tr_listview::tavoli_semplici );
        st+=( tr( "tavolo" ) );
        scopri( elenco1 );
        stato_pulsanti();
        scopri_pulsantiera();
      }
    break;

    case tr_listview::stampanti:
      st+=( tr( "stampante" ) );
    break;

    case tr_listview::messaggi:
      st+=( tr( "messaggio" ) );
    break;

    case tr_listview::conti_ieri:
      setMinMax( int( config.finestra_larghezza*0.95 ), int( config.finestra_altezza*0.70 ) );
      st+=( tr( "conto (ieri)" ) );
    break;
    case tr_listview::conti_ultimi:
      setMinMax( int( config.finestra_larghezza*0.95 ), int( config.finestra_altezza*0.70 ) );
      st+=( tr( "conto" ) );
    break;

    case tr_listview::log_com: {
        st=( tr( "Log comunicazione server" ) );
        QFont f( "Courier", 16 );
        elenco->setFont( f );
        elenco->altezza_riga( 20 );
        elenco->setMinMax( 700, 500 );
      }
    break;

    case tr_listview::tipo_doc_e_pagamenti: {
        nascondi( cancella, modifica, aggiungi );

        tr_listview*l[]= {elenco, elenco1};
        TROWI( l ) {
          l[i]->setMinimumSize( QSize( 320, 360 ) );
          l[i]->nascondi( tr_listview::e_abc );
          l[i]->nascondi( tr_listview::e_exit );
        }
        elenco->tipo( tr_listview::tipo_doc );
        elenco1->tipo( tr_listview::selezione_pagamenti );
        elenco1->carica();
        st+=( tr( "stampa diretta su tasto CONTO di questo tavolo" ) );
        scopri( elenco1 );
        stato_pulsanti();
        scopri_pulsantiera();
      }
    break;

    default:
      return;
  }

  barra_stato->setText( st );
  elenco->carica( id_selezione, id_gruppo );

  if ( _tipo==tr_listview::tavoli ) {
    elenco_clicked( elenco->chiave( id_selezione ) );
  }

  if ( _tipo==tr_listview::tavoli_semplici ) {
    elenco_clicked( elenco->chiave( id_selezione ) );
  }

  if ( elenco->righe()==1&&_tipo==tr_listview::sale ) {
    elenco->seleziona( tr_listview::e_first );
    accept();
  }

  if ( elenco->righe()>1&&elenco->righe()<3&&_tipo==tr_listview::sale ) {
    elenco->seleziona( tr_listview::e_next );
    accept();
  }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString frm_scelta::id() {
  QString key;

  switch ( _tipo ) {
    case tr_listview::tavoli:
      if ( elenco1->selezionato().isEmpty() ) return tr_listview::e_none;
      else
        key=elenco1->id();
      if ( !elenco2->selezionato().isEmpty() ) key=QSN( key.toInt()+elenco2->id().toInt() );
    break;

    case tr_listview::tavoli_semplici:
      if ( elenco1->selezionato().isEmpty() ) return tr_listview::e_none;
      else
        key=elenco1->id();
    break;

    default:
      if ( elenco->selezionato().isEmpty() ) return tr_listview::e_none;
      key=elenco->id();
  }
  return key;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString frm_scelta::descrizione() {
  if ( elenco->selezionato().isEmpty() ) return "";
  if ( _tipo==tr_listview::tavoli||_tipo==tr_listview::tavoli_semplici )
    return elenco1->descrizione();
  return elenco->descrizione();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_scelta::elenco_clicked( QListViewItem*it ) {
  if ( it )

    switch ( _tipo ) {

      case tr_listview::clienti_rapidi:
      case tr_listview::listini_modifica:
      break;

      case tr_listview::tavoli:
      case tr_listview::tavoli_semplici:
        elenco1->escludi( QString::null );
        elenco1->escludi( "|!{[\.]}|||!" +elenco->sala() [tr::enumeratori::sala::p_nome] );
        elenco1->carica();
      break;

      default:
        accept();
    } // switch

  stato_pulsanti();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_scelta::elenco1_clicked( QListViewItem*it ) {

  if ( it ) stato_pulsanti();

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_scelta::aggiungi_clicked() {
  switch ( elenco->tipo() ) {
    case tr_listview::sale:
    case tr_listview::listini:
    case tr_listview::clienti:
    case tr_listview::clienti_rapidi:
    case tr_listview::clienti_esteso: {
        bool b=false;
        FORM( f, frm_clienti );
        if ( f->accepted()&&f->modificato() ) b=true;
        xFORM( f, frm_clienti );
        if ( b ) aggiorna();
      }
    break;

    case tr_listview::camerieri:
    case tr_listview::stampanti:
    case tr_listview::listini_modifica:
    default:
      return;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_scelta::modifica_clicked() {
  switch ( elenco->tipo() ) {
    case tr_listview::sale:
    case tr_listview::listini:
    case tr_listview::clienti:
    case tr_listview::clienti_rapidi: {
        FORM( f, frm_clienti );
        if ( f->modifica( id() )&&f->accepted()&&f->modificato() )
          aggiorna();
        xFORM( f, frm_clienti );
      }
    break;

    case tr_listview::camerieri:
    case tr_listview::stampanti:
    case tr_listview::listini_modifica:
    default:
      return;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_scelta::cancella_clicked() {
  bool canc=false;
  QString id_cli;

  QString msg=tr( "Si e' veramente sicuri di voler cancellare " );
  switch ( elenco->tipo() ) {
    case tr_listview::sale:
    case tr_listview::listini:
    case tr_listview::listini_modifica:
      msg=tr( "!!! ATTENZIONE !!! La cancellazione del listino, provochera' la cancellazione di tutti i suoi articoli e anche del relativo menu.\nSi e' veramente sicuri di voler cancellare " );
    break;
    case tr_listview::clienti:
    case tr_listview::clienti_rapidi:
    break;

    case tr_listview::camerieri:
    case tr_listview::stampanti:
    default:
      return;
  }

  msg+=descrizione();
  if ( tools.msgbox( msg, SINO )==SI ) canc=true;
  if ( canc&&config.beginner )
    if ( tools.msgbox( tr( "Seconda conferma alla cancellazione.<br>Si e' veramente sicuri?" ), SINO )!=SI )
      canc=false;

  if ( canc ) {
    switch ( elenco->tipo() ) {
      case tr_listview::sale:
      case tr_listview::listini:
      case tr_listview::listini_modifica:
        CMD( tok_rimuovi_listino, id() );
      break;

      case tr_listview::clienti:
      case tr_listview::clienti_rapidi: {
          QString id_cli=elenco->id();
          CMD( tok_cancella_cliente, id() );

          // elimina il tasto rapido relativo
          QString s="";
          for ( uint i=0; i<config.clienti_rapidi_size; i++ )
            if ( config.clienti_rapidi_id[i]==id_cli ) {
              config.clienti_rapidi_id[i]=s;
              config.clienti_rapidi_rs[i]=s;
              rapido[i]->setText( s );
              config.salva_clienti_rapidi();
            break;
            }
        }
      break;

      case tr_listview::camerieri:
      case tr_listview::stampanti:
      default:
        return;
    }

    aggiorna();
  }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_scelta::stato_pulsanti() {
  switch ( _tipo ) {
    case tr_listview::clienti:
    break;

    case tr_listview::clienti_rapidi:
      if ( elenco->selezionato().isEmpty() ) {
        nascondi( cancella, modifica, sottogrp );
        if ( !mostra_imposta ) nascondi( imposta );
      }
      else {
        scopri( cancella, modifica, sottogrp );
        if ( mostra_imposta ) scopri( imposta );
      }
    break;

    case tr_listview::selezione_clienti:
      nascondi( aggiungi );
    break;

    case tr_listview::tavoli_semplici:
    case tr_listview::tavoli:
      if ( elenco->selezionato().isEmpty() ) {
        disabilita( elenco1 );
      }
      else {
        abilita( elenco1 );
      }
      if ( elenco1->selezionato().isEmpty() ) {
        disabilita( elenco2 );
        if ( !mostra_imposta ) nascondi( imposta );
      }
      else {
        abilita( elenco2 );
        if ( mostra_imposta ) scopri( imposta );
      }
    break;


    default:
      if ( elenco->selezionato().isEmpty() ) nascondi( cancella, modifica );
      else
        scopri( cancella, modifica );
  } // switch
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool frm_scelta::aggiorna() {
  bool b=elenco->carica( elenco->id() );
  elenco->adatta();
  stato_pulsanti();
  return b;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_scelta::sottogrp_clicked() {
  QString frm_sottocli=TRFORM(
     ( "palette=3\n""status=Gestione |Sotto Clienti\n""listview=#list(tok:%tok%,minr:8,abc)\n"
     "buttons=aggiungi#add|modifica#modify|cancella#erase|uscita#exit\n""links=list|modify|erase\n" ) );

  denu_elenco_sottoclienti;
  FRM_PARAMS;
  FRM_PARAM( "%tok%", QSN( tok_elenco_sottoclienti )+"|" +id() );
  bFORM( sottocli, frm_sottocli, this );
  CTRL_FRM( sottocli );
  CTRL( list );
  CTRL( add );
  CTRL( modify );
  CTRL( erase );
  CTRL( exit );

  tr_widget*iterato=sottocli->interazione();

  while ( iterato!=NULL ) {

    if ( iterato==erase ) {
      if ( tools.msgbox( tr( "Si e' sicuri di voler cancellare il sotto cliente?" ), SINO )==SI )
        CMD( tok_cancella_sottocliente, list() );
    }

    if ( iterato==add||iterato==modify ) {

      QString frm_scli=TRFORM(
         ( "status=Gestione Sotto Cliente %subcli%\n""validatore=anagrafica_sottoclienti\n"
         "input=Nome#name(.nome);Cognome#lname(.cognome)\n""input=Matricola#pnumber(.matricola);Codice Fiscale#cf(.cf)\n"
         "input=Data di nascita#birtday(.data_nascita);hole=\n""buttons=ok#ok|uscita#exit\n"
         "links=name|lname|pnumber|cf|birtday|ok" ) );

      FRM_PARAMS;
      if ( iterato==modify ) FRM_PARAM( "%subcli%", list() );
      else
        FRM_PARAM( "%subcli%", "" );

      bFORM( scli, frm_scli, sottocli );
      CTRL_FRM( scli );
      CTRL( name );
      CTRL( lname );
      CTRL( pnumber );
      CTRL( cf );
      CTRL( birtday );
      CTRL( ok );
      CTRL( exit );

      if ( iterato==modify ) {
        QString data=list.listview->record();
        scli->sets( "||name|lname|pnumber|cf|", data );
        birtday.value( tools.dataiso_data( data.section( '|', ( enu_elenco_sottoclienti )p_data_nascita ) ) );
      }

      tr_widget*iterato1=scli->interazione();

      while ( iterato1!=NULL ) {

        if ( iterato1==ok ) {
          if ( iterato==add )
            if ( CMD( tok_inserisci_sottocliente, id(), name(), lname(), pnumber(), cf(),
               tools.data_dataiso( birtday(), true ) ) )
                 iterato1=NULL;
          if ( iterato==modify )
            if ( CMD( tok_modifica_sottocliente, list(), name(), lname(), pnumber(), cf(),
               tools.data_dataiso( birtday(), true ) ) )
                 iterato1=NULL;
        }

        if ( iterato1==exit ) iterato1=NULL;
        if ( iterato1!=NULL ) iterato1=sottocli->interazione();
      }

      xFORM( scli, frm_scli );

    }

    list.listview->aggiorna();

    if ( iterato==exit ) iterato=NULL;
    if ( iterato!=NULL ) iterato=sottocli->interazione();

  }

  xFORM( sottocli, frm_sottocli );

}
