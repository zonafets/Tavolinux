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

/* HISTORY: 07/09/04 aggiunta ? su tutta la qta nel separa comanda (separa_clicked)
22/12/04 sul cambiamento prz dell'articolo separato, ora aggiorna lo stato dei pagamenti

*/
#include <iostream>

#include "frm_conto.hpp"
#include "cmdcodicigruppi.hpp"
#include "frm_scelta.hpp"
#include "tr_menu.hpp"
#include "tr_tools.hpp"
#include "tr_globals.hpp"
#include "tr_dmacro.hpp"
#include "tr_dmacro2.hpp"
#include "tr_display.hpp"
#include "msgs.hpp"

extern tr_tools tools;
extern tr_globals glb;
extern tr_display display_lcd;

frm_conto::frm_conto( QWidget*parent, const char*name, bool modal, WFlags fl ):frm_base( parent, name, modal, fl ) {
  titolo->setText( tr( "conto" ) );

  //const uint dy =config.finestra_altezza_titolo+ config.finestra_altezza_stato;
  // iniz_area(2,3);

  NEW( comanda, tr_comanda( area, "comanda" ) );
  comanda->setMinMax( 320, config.finestra_altezza_centrale, SMAX, config.finestra_altezza_centrale );
  comanda->darkable( true );
  // area_layout->addMultiCellWidget( comanda,0,1,0,0 );

  // Modifica di Luca per impedire modifiche di quantita
  // ad utenti non abilitati

  int half=config.finestra_altezza_centrale/2;

  NEW( separata, tr_comanda( area, "separata" ) );
  separata->setMinMax( 290, half, SMAX, half );
  separata->cambia_sfondo( paletteBackgroundColor(), QColor( 184, 255, 213 ) );
  separata->darkable( true );
  // area_layout->addWidget( separata, 0,1 );

  NEW( pagamento, tr_comanda( area, "pagamento" ) );
  pagamento->setMinMax( 290, half, SMAX, half );
  pagamento->cambia_sfondo( paletteBackgroundColor(), QColor( 254, 227, 208 ) );
  pagamento->darkable( true );
  // area_layout->addWidget( pagamento, 1,1 );

  NEW( forme_pagamenti, tr_listview( area, "pagamenti" ) );
  forme_pagamenti->nascondi( tr_listview::e_abc );
  if ( !config.pagamenti_con_barra ) forme_pagamenti->nascondi( tr_listview::e_barra );
  forme_pagamenti->setMinMax( 190, config.finestra_altezza_centrale, SMAX, SMAX );
  forme_pagamenti->darkable( true );
  forme_pagamenti->tipo( tr_listview::pagamenti );
  forme_pagamenti->carica();
  // area_layout->addMultiCellWidget( forme_pagamenti, 0 ,1, 2,2 );

  /* aggiunta pulsanti */
  tr_label**ps[]= { &dividi, &annulla, &altro,NULL,NULL,&stampa,&a_comanda,&cancella, &exit };
  QString nomi=TRFORM( ( "dividi#dividi|annulla#annulla|altro#altro|||stampa#stampa|comanda#comanda|"
     "cancella#cancella|uscita#uscita" ) );
  pulsanti( nomi, false, ps, NULL, true );

  NEW( puls_panel, QFrame( area, "puls_panel" ) );
  QHBoxLayout*puls_panel_layout;
  puls_panel->setMaximumHeight( 60 );
  puls_panel->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
  puls_panel_layout=new QHBoxLayout( puls_panel, 0, -1, "puls_panel_layout" );

  NEW( puls_dx, tr_label( puls_panel, "puls_dx" ) );
  NEW( puls_sx, tr_label( puls_panel, "puls_sx" ) );

  QPixmap bp=QPixmap( PIXMAP( config.pulsante_barra ) );
  uint pulsanti_dx=bp.width();
  uint pulsanti_dy=bp.height();
  tr_label*puls[]= {puls_dx,puls_sx};
  TROWI( puls ) {
    tr_label*p=puls[i];
    connect( p, SIGNAL( clicked() ), this, SLOT( puls_clicked() ) );
    p->setMinimumSize( pulsanti_dx, pulsanti_dy /* 32+24 */ );
    //p->setMaximumSize( 32767, 32767 );
    p->setMaximumSize( pulsanti_dx, pulsanti_dy /* 32+	24 */ );
    //p->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferredrred ));
    p->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
    p->setPaletteForegroundColor( QColor( 255, 237, 155 ) );
    p->setBackgroundOrigin( QLabel::WindowOrigin );
    p->setTextPixelSize( config.pulsanti_font_size );
    p->pulsante_animato();
    // p->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );
    QString pulsante="pulsante_menu.jpg";
    QColor c=p->aspetto_bottone( pulsante );

    QString etichetta;
    QString icona;
    if ( i==0 ) {
      etichetta=tr( "sconto %" );
      icona="az_sconto_perc.png";
    }
    if ( i==1 ) {
      etichetta=tr( "sc.valore" );
      icona="az_sconto_valore.png";
    }

    p->setText( PTEXT( icona, etichetta ) );

    puls_panel_layout->addWidget( p, Qt::AlignCenter );
    puls_panel->setPaletteBackgroundColor( c );

  } // troi puls

  // signals and slots connections
  CONNECT( dividi );
  CONNECT( annulla );
  CONNECT( altro );
  CONNECT( stampa );
  CONNECT( a_comanda );
  CONNECT( cancella );
  CONNECT( exit );

  connect( comanda, SIGNAL( articolo_selezionato( int ) ), this, SLOT( comanda_clicked( int ) ) );

  connect( comanda, SIGNAL( totale_clicked() ), this, SLOT( comanda_totale_clicked() ) );
  connect( separata, SIGNAL( totale_clicked() ), this, SLOT( comanda_totale_clicked() ) );
  connect( pagamento, SIGNAL( totale_clicked() ), this, SLOT( comanda_totale_clicked() ) );

  connect( comanda, SIGNAL( qta_clicked( int ) ), this, SLOT( comanda_qta_clicked( int ) ) );
  connect( comanda, SIGNAL( prz_clicked( int ) ), this, SLOT( comanda_prz_clicked( int ) ) );
  connect( comanda, SIGNAL( qta_changed( int ) ), this, SLOT( comanda_qta_changed( int ) ) );
  connect( comanda, SIGNAL( prz_changed( int ) ), this, SLOT( comanda_prz_changed( int ) ) );

  connect( separata, SIGNAL( articolo_selezionato( int ) ), this, SLOT( separata_clicked( int ) ) );
  // connect ( separata, SIGNAL( qta_clicked(int) ), this, SLOT( separata_qta_clicked(int) ) );
  connect( separata, SIGNAL( prz_clicked( int ) ), this, SLOT( separata_prz_clicked( int ) ) );

  connect( pagamento, SIGNAL( articolo_selezionato( int ) ), this, SLOT( pagamento_clicked( int ) ) );
  connect( pagamento, SIGNAL( qta_clicked( int ) ), this, SLOT( pagamento_qta_clicked( int ) ) );
  connect( pagamento, SIGNAL( prz_clicked( int ) ), this, SLOT( pagamento_prz_clicked( int ) ) );

  connect( forme_pagamenti, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( forme_pagamenti_clicked( QListViewItem* ) ) );

  QWidget*ws[]= {
    comanda, separata,  forme_pagamenti, forme_pagamenti,  0,
    comanda, pagamento, forme_pagamenti, forme_pagamenti,  0,
    comanda, pagamento, puls_panel,      puls_panel,       0,
    0 };

  build( ws, 0, 0 );

  if ( !config.sconto_interattivo ) puls_panel->hide();

  barra_stato->tipo( tr_barra_stato::stato );
  barra_stato->setCameriere( glb.stato[tr_globals::p_utente_descrizione] );
  barra_stato->setListino( glb.stato[tr_globals::p_listino_descrizione] );

  glb.intestazione.init();
  scambia=false;
  init();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* *  Destroys the object and frees any allocated resources */
frm_conto::~frm_conto() {
  // no need to DELETE child widgets, Qt does it all for us
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_conto::init() {

  comanda->modalita( tr_comanda::m_conto );
  separata->modalita( tr_comanda::m_separata );
  pagamento->modalita( tr_comanda::m_pagamento );

  comanda->adjustSize();
  resize( minimumSizeHint() );
  show();
  /* esegue il calcolo delle dimensioni dei widget. Questo permette un corretto calcolo delle
  // dimensioni delle colonne delle tabelle */

  _divisione_fittizia=false;
  devnul=false;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77
void frm_conto::show() {
  frm_base::show();
  forme_pagamenti->adatta();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::contanti() {
  if ( forme_pagamenti->isEnabled()&&forme_pagamenti->selezionati().count()<1 )
    forme_pagamenti->seleziona( QSN( config.pg_contanti ) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::aggiorna( t_comanda controllo, bool solo_stato ) {
  tr_comanda*c;
  if ( controllo==c_comanda ) c=comanda;
  if ( controllo==c_separata ) c=separata;
  if ( controllo==c_pagamento ) c=pagamento;
  if ( !solo_stato /* && c->isEnabled() */ ) {
    if ( c==comanda ) {
      // l'aggiornamento di questo provoca la deselezione di riga
      // quindi vengono disabilitati i relativi pulsanti
    }
    c->aggiorna();
  }

}

void frm_conto::aggiorna( bool solo_stato ) {
  aggiorna( c_separata, solo_stato );
  aggiorna( c_comanda, solo_stato );
  aggiorna( c_pagamento, solo_stato );
  stato_pulsanti();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::dividi_clicked() {
  devnul=false;
  nascondi( exit );

  QString valore="2";
  valore=tools.calcolatrice( this, tr( "Dividi conto X" ), valore, "int" );

  scopri( exit );
  if ( valore.isNull() ) return;
  int n_conti=valore.toInt();
  if ( !n_conti ) return;


  // divisione
  money totale=comanda->totale_comanda();
  money divisione=totale/n_conti;
  money aggiungi_a_ultimo=0;
  if ( config.arrotonda( divisione )*n_conti!=config.arrotonda( totale ) ) {
    switch ( config.arrotonda_conto_diviso ) {
      case config.arr_alternato:
        divisione=config.arrotonda( divisione, 2 );
        totale=config.arrotonda( divisione*n_conti );
      break;

      case config.arr_difetto:
        divisione=config.arrotonda_x_difetto( divisione, 2 );
        totale=config.arrotonda( divisione*n_conti );
      break;

      case config.arr_eccesso:
        divisione=config.arrotonda_x_eccesso( divisione, 2 );
        totale=config.arrotonda( divisione*n_conti );
      break;

      default:
        divisione=config.arrotonda_x_difetto( totale/n_conti, 2 );
        aggiungi_a_ultimo=totale-divisione*n_conti;
    }
  }

  int i;
  comanda->modalita( tr_comanda::m_conti );
  QStringList elenco;
  const QString conto_diviso=tr( "0|0|0|0|1|CONTO DIVISO|0|" );
  for ( i=1; i<n_conti; i++ ) {
    elenco<<conto_diviso+config.formatta( divisione, 2 );
  }

  elenco<<conto_diviso+config.formatta( divisione+aggiungi_a_ultimo, 3 );
  comanda->tabella->aggiorna( elenco );

  comanda->totale_comanda( QSN( totale ) );

  _divisione_fittizia=true;

  stato_pulsanti();

  // Aggiorna anche il display se serve

  if ( display_lcd.inizializzato() )
    display_lcd.aggiorna( tr_display::tr_dividi_conto, QSN( n_conti ) );

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::annulla_clicked() {
  devnul=false;

  barra_stato->setCliente( "" );
  if ( glb.intestazione.intestato() ) {
    glb.intestazione.id=-1;
    glb.intestazione.ragione_sociale="";

    if ( conto_aperto() ) CMD( tok_intesta_conto, QString( "" ) );
  }

  if ( separata->conto_aperto() ) {
    separata->annulla();
  }
  if ( pagamento->conto_aperto() ) pagamento->annulla();

  if ( _divisione_fittizia&&comanda->modalita()==tr_comanda::m_conti ) {
    comanda->modalita( tr_comanda::m_conto );
    _divisione_fittizia=false;
  }
  aggiorna();

  // aggiorna il display se necessario

  if ( display_lcd.inizializzato() )
    display_lcd.aggiorna( tr_display::tr_annulla_conto );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::altro_clicked() {
  devnul=false;
  enum {
    cambia
  };

  QString etichette=TRFORM( ( "camb.desc.#chgdesc|tessera#tessere" ) );

  TRDiritti::TRDirittiUtente diritti[]= { p_conto_cambia_descrizione,p_none };

  if ( comanda->selezionato()&&!comanda->selezionato()->stato.gdv() )
    diritti[0]=TRDiritti::p_null;

  QString n=tools.hvmenu( altro, etichette, diritti );
  if ( n.isEmpty() ) return;

  if ( n=="chgdesc" ) {
    QString old_desc=comanda->selezionato()->descrizione;
    QString st=tools.input_descrizione( this, tr( "Cambio descrizione articolo conto" ), old_desc );
    if ( st!=old_desc ) {
      if ( comanda->modifica_descrizione( st ) ) {
        aggiorna( frm_conto::c_comanda );
      }
    }
  } // if camv.desc.

  if ( n=="tessere" ) {
    QString tag=tools.input_descrizione( this, tr( "Immettere codice tessera" ) );
    if ( !tag.isEmpty() ) srv->msg( "TAG|" +tag+"|" +config.sn );
  } // tessera

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::stampa_clicked() {
  devnul=false;
  denu_apri_tavolo;

  if ( stato_tavolo[p_articoli_p_nullo]=="1" &&config.chiedi_su_articoli_p_nullo ) {
    if ( tools.msgbox( MSG_1, SINO )!=SI ) {
      return;
    }
  }


  TRDiritti::TRDirittiUtente diritti[]= {
    p_conto_stampa_fattura, p_conto_stampa_ricevuta, p_conto_stampa_scontrino, p_conto_stampa_proforma,
         p_conto_stampa_concordato, p_conto_stampa_intestazione
  };

  QString etichette;
  etichette=TRFORM( ( "fattura#fattura|ricevuta#ricevuta|scontrino#scontrino|proforma#proforma|"
     "conto#conto|intestazione#intestazione" ) );

  if ( pagamento->dati.documenti.count() ) {
    int docs[]= {
      tr_comanda::doc_fattura, tr_comanda::doc_fattura_divisa, tr_comanda::doc_ricevuta, tr_comanda::doc_ricevuta_divisa,
           tr_comanda::doc_scontrino, tr_comanda::doc_scontrino_divisa, tr_comanda::doc_proforma,
           tr_comanda::doc_proforma_divisa, tr_comanda::doc_concordato, tr_comanda::doc_concordato_divisa
    };
    TROWI( docs )
    if ( pagamento->dati.documenti.findIndex( QSN( docs[i] ) )==-1 )
      diritti[i/2]=TRDiritti::p_null;
  }


  QString n=tools.hvmenu( stampa, etichette, diritti );
  if ( n.isEmpty() ) return;

  if ( n=="fattura" ) print( td_fattura );
  if ( n=="ricevuta" ) print( td_ricevuta );
  if ( n=="scontrino" ) print( td_scontrino );
  if ( n=="proforma" ) print( td_proforma );
  if ( n=="conto" ) print( td_concordato );
  if ( n=="intestazione" ) intestazione_clicked();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::intestazione_clicked( bool rientro ) {
  if ( rientro&&glb.intestazione.intestato() ) return;

  QString id;
  QString descrizione;
  QString record="fill-it";
  if ( tools.scelta( tr_listview::clienti_esteso, id, descrizione, record ) ) {

    tr::mse::riga cliente_esteso( record );
    QString pagamenti=cliente_esteso[tr::cliente_esteso::p_pagamenti];
    double aiva=cliente_esteso[tr::cliente_esteso::p_aiva].toDouble();
    if ( !pagamenti.isEmpty()&&!pagamento->conto_aperto()&&!separata->conto_aperto() ) {
      bool r;
      if ( comanda->modalita()==tr_comanda::m_conti ) r=pagamento->apri_conto( tr_comanda::cnt_diviso );
      else
        r=pagamento->apri_conto( tr_comanda::cnt_globale );
    }
    this->intestazione( id.toInt(), descrizione, aiva );
    aggiorna( frm_conto::c_pagamento );
    //}
  }
  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77
void frm_conto::a_comanda_clicked() {
  if ( conto_aperto() ) annulla_clicked();
  scambia=true;
  accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::cancella_clicked() {

  if ( devnul ) {
    print( td_devnul );
    accept();
    return;
  }

  if ( comanda->selezionato() ) {
    comanda->cancella();
  }

  if ( pagamento->selezionato() ) {
    pagamento->cancella();
    if ( !pagamento->righe()&&!separata->righe() ) pagamento->annulla_conto();
  }

  stato_pulsanti();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::exit_clicked() {
  devnul=false;
  if ( comanda->modificato() ) {
    if ( tools.msgbox( this, tr( "Sono state apportate delle modifiche\n Si desidera salvare?" ), tr( "Attenzione" ), SINO )==SI ) {
      if ( separata->righe() ) annulla_clicked();
      comanda->registra();
      accept();
    }
    else {
      annulla_clicked();
      reject();
    }
  }
  else {
    annulla_clicked();
    reject();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::comanda_qta_clicked( int row ) {
  devnul=false;
  if ( _divisione_fittizia==true ) dividi_conto();
  if ( FLAGS( p_conto_modifica_qta )&&!pagamento->righe() ) {
    if ( comanda->modalita()!=tr_comanda::m_conti ) {
      comanda->cambia_qta( row );
      stato_pulsanti();
      if ( display_lcd.inizializzato() ) {
        display_lcd.aggiorna( comanda->ultimoDato().token_op==tok_storna?tr_display::tr_storna_articolo:
           tr_display::tr_modifica_quantita, comanda->ultimoDato().quantita, comanda->ultimoDato().descrizione,
           comanda->ultimoDato().prezzo );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::comanda_prz_clicked( int row ) {
  devnul=false;
  if ( _divisione_fittizia==true ) dividi_conto();
  if ( FLAGS( p_conto_modifica_prz )&&!pagamento->righe() ) {
    comanda->cambia_prz( row );
    stato_pulsanti();
    if ( display_lcd.inizializzato() ) {
      display_lcd.aggiorna( tr_display::tr_modifica_prezzo, comanda->ultimoDato().quantita, comanda->ultimoDato().descrizione,
         comanda->ultimoDato().prezzo );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::comanda_qta_changed( int ) {
  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::comanda_prz_changed( int ) {
  stato_pulsanti();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::comanda_clicked( int ) {
  devnul=false;

  pagamento->seleziona();

  if ( separata->selezionato() ) {
    int id_riga=separata->selezionato()->id_riga;
    separata->seleziona();
    if ( comanda->acquisisci_portata( id_riga ) ) {
      aggiorna();
      if ( separata->righe()==0 ) {
        separata->annulla_conto();
        aggiorna();

        // Aggiorna il display

        if ( display_lcd.inizializzato() )
          display_lcd.aggiorna( tr_display::tr_annulla_conto );
      }
      else {
        // Aggiorna il display

        if ( display_lcd.inizializzato() )
          display_lcd.aggiorna( tr_display::tr_totale_conto, QString::null, QString::null,
             QSNP( separata->totale_comanda() ) );
      }
    }
  }

  stato_pulsanti();
}

void frm_conto::separata_prz_clicked( int row ) {
  devnul=false;
  if ( FLAGS( p_conto_modifica_prz ) ) {
    if ( separata->cambia_prz( row ) )
      if ( pagamento->righe() ) aggiorna( c_pagamento );
  }
}


void frm_conto::separata_clicked( int ) {
  devnul=false;

  if ( pagamento->righe() ) return;

  if ( comanda->selezionato() ) {
    int id_riga=comanda->selezionato()->id_riga;
    int riga=comanda->selezionato()->riga;
    double qta=comanda->selezionato()->qta.toDouble();
    comanda->seleziona();
    if ( comanda->modalita()==tr_comanda::m_conti ) {
      if ( _divisione_fittizia ) {
        dividi_conto();
        comanda->seleziona( riga );
        id_riga=comanda->selezionato()->id_riga;
      }

      if ( !separata->conto_aperto() ) separata->apri_conto( tr_comanda::cnt_diviso );

    }
    else {
      if ( !separata->conto_aperto() ) separata->apri_conto( tr_comanda::cnt_separato );
    }

    bool r;
    if ( qta>3 ) {
      if ( tools.msgbox( tr( "Si vuole spostare tutta la riga?" ), SINO )==SI )
        r=separata->acquisisci_portata( id_riga, qta );
      else
        r=separata->acquisisci_portata( id_riga );
    }
    else
      r=separata->acquisisci_portata( id_riga );
    if ( r ) {
      aggiorna();
      separata->visualizza_ultimo();

      // Aggiorna il display

      if ( display_lcd.inizializzato() )
        display_lcd.aggiorna( tr_display::tr_totale_conto, QString::null, QString::null, QSNP( separata->totale_comanda() ) );

    }
  }
  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::pagamento_qta_clicked( int row ) {
  devnul=false;
  if ( FLAGS( p_conto_modifica_qta_pagamento ) )
    if ( pagamento->cambia_qta( row ) ) stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::pagamento_prz_clicked( int row ) {
  devnul=false;
  if ( FLAGS( p_conto_modifica_prz_pagamento ) )
    if ( pagamento->cambia_prz( row ) ) stato_pulsanti();
}

void frm_conto::pagamento_clicked( int ) {
  devnul=false;
  comanda->seleziona();
  separata->seleziona();
  stato_pulsanti();
}


void frm_conto::forme_pagamenti_clicked( QListViewItem*it ) {
  devnul=false;

  tr::gruppi::gruppo p;

  if ( !it ) {
    contanti();
    return;
  }
  else {
    forme_pagamenti->seleziona( it );
    p=forme_pagamenti->pagamento();
    if ( p.tipo_articolo.gas() ) return;
  }

  if ( comanda->modalita()==tr_comanda::m_conti ) {
    if ( _divisione_fittizia ) dividi_conto();
  }

  if ( !pagamento->conto_aperto()&&!separata->conto_aperto() ) {
    bool r;
    if ( comanda->modalita()==tr_comanda::m_conti ) {
      r=pagamento->apri_conto( tr_comanda::cnt_diviso );
      for ( int i=0; i<comanda->righe(); i++ ) {
        tr_comanda::tr_selezione s=comanda->selezione( i );
        separata->acquisisci_portata( s.id_riga );
      }
      aggiorna();
    }
    else
      r=pagamento->apri_conto( tr_comanda::cnt_globale );
    if ( !r ) return;
  }

  pagamento->inserisci_pagamento( p.is );
  pagamento->seleziona();

  stato_pulsanti();

  // Aggiorna il display

  if ( display_lcd.inizializzato() )
    display_lcd.aggiorna( tr_display::tr_inserisci_pagamento, pagamento->ultimoDato().quantita,
       pagamento->ultimoDato().descrizione, pagamento->ultimoDato().prezzo );


}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::dividi_conto() {
  bool r;
  r=CMD( tok_dividi_conto );
  int i;
  for ( i=0; i<comanda->righe(); i++ ) {
    double prz=config.num_interno( comanda->tabella->text( i, 2 ) ).toDouble();
    comanda->inserisci_conto( prz );
  }
  r=CMD( tok_registra_comanda );
  r=CMD( tok_nuova_comanda );
  if ( r ) {
    comanda->aggiorna();
    _divisione_fittizia=false;
  }
}


bool frm_conto::apri_tavolo( const itm_sala&s, const itm_tavolo&t, int bis ) {
  tavolo=t;
  int id=tavolo.id()+bis;
  tavolo.set( tr::enumeratori::tavolo_esteso::p_id, QSN( id ) );
  sala=s;
  return apri_tavolo();
}


bool frm_conto::apri_tavolo( const itm_sala&s, const itm_tavolo&t ) {
  tavolo=t;
  sala=s;
  return apri_tavolo();
}


bool frm_conto::apri_tavolo() {

  denu_apri_tavolo;

  stato_tavolo=tavolo.apri();
  if ( !stato_tavolo.count() ) return false;

  if ( stato_tavolo[p_articoli_p_nullo]=="1" &&config.chiedi_su_articoli_p_nullo ) {
    if ( tools.msgbox( MSG_1, SINO )!=SI ) return false;

  }

  if ( stato_tavolo[p_coperti_presenti]=="0" &&config.chiedi_su_coperti ) {
    if ( tools.msgbox( MSG_2, SINO )!=SI ) return false;
  }


  CMD( tok_imposta_listino, stato_tavolo[p_cod_listino] );
  barra_stato->setListino( stato_tavolo[p_descr_listino] );

  if ( tavolo.stato()&itm_tavolo::conto_diviso ) {
    comanda->modalita( tr_comanda::m_conti );
  }

  separata->totale_comanda( "0.00" );
  pagamento->totale_comanda( "0.00" );

  contanti();

  aggiorna( c_comanda, false );

  if ( comanda->righe()==0 ) {
    tools.msgbox( ( QWidget* ) parent(), tr( "Il tavolo e' vuoto" ) );
    reject();
    return false;
  }

  barra_stato->setTavolo( tavolo.nome() );
  barra_stato->setSala( sala.nome );

  stato_pulsanti();

  // Aggiorna anche il display

  if ( display_lcd.inizializzato() )
    display_lcd.aggiorna( tr_display::tr_totale_conto, QString::null, QString::null, QSNP( comanda->totale_comanda() ) );

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::chiudi_tavolo() {

  if ( tavolo.card.count()&&!comanda->righe() ) tavolo.chiudi( true );
  else
    tavolo.chiudi();
  int stato=tavolo.tipo();
  if ( ( stato&T_TAVOLO_TAKEAWAY )==T_TAVOLO_TAKEAWAY&&!comanda->righe() ) {
    CMD( tok_cancella_tavolo, QSN( tavolo.id() ) );
  }
}


void frm_conto::comanda_totale_clicked() {
  if ( FLAGS( p_conto_stampa_devnull ) ) {
    comanda->seleziona();
    scopri( cancella );
    devnul=true;
  }
  else
    devnul=false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

bool frm_conto::conto_aperto() {
  return ( comanda->conto_aperto()||separata->conto_aperto()||pagamento->conto_aperto() )?true:false;
}


bool frm_conto::apri_conto( tr_comanda::conti c ) {
  return comanda->apri_conto( c );
}

bool frm_conto::apri_conto() {
  if ( !conto_aperto() ) {
    if ( !multi_conto() ) return comanda->apri_conto();
    else
      return separata->apri_conto( tr_comanda::cnt_diviso );
  }
  else
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

bool frm_conto::chiudi_conto( tr_comanda::conti_chiusi tipo ) {
  if ( comanda->conto_aperto() ) return comanda->chiudi_conto( tipo );
  if ( separata->conto_aperto() ) return separata->chiudi_conto( tipo );
  if ( pagamento->conto_aperto() ) return pagamento->chiudi_conto( tipo );
  return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::stato_pulsanti() {

  enum {
    p_dividi, p_annulla, p_altro, p_stampa, p_cancella, p_exit, p_piu, p_meno, p_a_comanda
  };

  enum {
    p_comanda, p_separata, p_pagamento, p_forme_pagamenti
  };

  bool stato[]= { true, true, true, true, true, true, true, true, true };
  tr_label*ps[]= { dividi, annulla, altro, stampa, cancella, exit, comanda->piu, comanda->meno, a_comanda };

  bool abili[]= { true, true, true, true };
  tr_widget*pa[]= { comanda, separata, pagamento, forme_pagamenti };

  if ( tavolo.card.isEmpty() )
    barra_stato->ico_tavolo->setPixmap( PIXMAP( "stato_tavolo.png" ) );
  else
    barra_stato->ico_tavolo->setPixmap( PIXMAP( "tessera.png" ) );


  if ( glb.intestazione.intestato()&&glb.intestazione.aiva!=0 )
    abili[p_forme_pagamenti]=false;

  if ( !comanda->selezionato()&&!pagamento->selezionato() ) stato[p_cancella]=false;

  if ( comanda->selezionato() ) {
    if ( multi_conto()&&!pagamento->selezionato() ) stato[p_cancella]=false;
    if ( comanda->selezionato()->stato.gav() ) abili[p_separata]=false;
    if ( comanda->selezionato()->stato.gavf()||comanda->selezionato()->stato.gpeso() ) {
      stato[p_piu]=false;
      stato[p_meno]=false;
    }

    if ( !comanda->selezionato()->portata ) {
      stato[p_piu]=false;
    }

  }

  else {

    stato[p_piu]=false;
    stato[p_meno]=false;

  }

  if ( comanda->righe()==0 ) abili[p_comanda]=false;

  if ( comanda->righe()==0&&separata->righe()==0 ) {
    abili[p_forme_pagamenti]=false;
    stato[p_dividi]=false;
    stato[p_stampa]=false;
  }

  if ( multi_conto() ) {
    stato[p_dividi]=false;
    stato[p_a_comanda]=false;
  }
  if ( separata->righe()!=0 ) stato[p_dividi]=false;
  if ( pagamento->righe()!=0 ) stato[p_dividi]=false;

  if ( separata->righe()==0&&!comanda->selezionato() )
    abili[p_separata]=false;

  if ( separata->righe()==0&&pagamento->righe()==0&&!_divisione_fittizia&&!glb.intestazione.intestato() )
    stato[p_annulla]=false;

  if ( pagamento->righe()==0 ) {
    puls_panel->setEnabled( true );
    abili[p_pagamento]=false;
  }
  else {
    abili[p_comanda]=false;
    abili[p_separata]=false;
    puls_panel->setEnabled( false );
    forme_pagamenti->seleziona( tr_listview::e_none );
    if ( !pagamento->selezionato() ) stato[p_cancella]=false;
    if ( pagamento->residuo()!=0 ) stato[p_stampa]=false;
  }

  if ( forme_pagamenti->elenco->selectionMode()==QListView::Multi )
    abili[p_forme_pagamenti]=false;

  if ( !comanda->selezionato()&&!separata->selezionato()&&!pagamento->selezionato() ) stato[p_cancella]=false;

  if ( !FLAGS( p_conto_dividi ) ) stato[p_dividi]=false;

  if ( !FLAGS( p_conto_separa ) ) {
    stato[p_dividi]=false;
    abili[p_separata]=false;
  }

  if ( !FLAGS( p_conto_pagamenti ) )
    abili[p_forme_pagamenti]=false;
  if ( !FLAGS( p_conto_exit ) ) stato[p_exit]=false;
  if ( !FLAGS( p_conto_stampa_cancella ) ) stato[p_cancella]=false;
  if ( !FLAGS( p_conto_modifica_qta ) ) {
    stato[p_piu]=false;
    stato[p_meno]=false;
  }

  TROWI( stato )( stato[i]?scopri( ps[i] ):nascondi( ps[i] ) );
  TROWI( abili )( abili[i]?abilita( pa[i] ):disabilita( pa[i] ) );

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::intestazione( int id, QString ragione_sociale, double aiva ) {
  glb.intestazione.id=id;
  glb.intestazione.ragione_sociale=ragione_sociale;
  glb.intestazione.aiva=aiva;
  barra_stato->setCliente( ragione_sociale );

  if ( conto_aperto() ) CMD( tok_intesta_conto, QSN( id ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

bool frm_conto::print( tipo_doc tipo ) {
  tr_comanda::docs doc;
  QString stampante;
  bool intestare=false;

  const int num_docs=6;

  tipo_doc tipi[num_docs]= { td_ricevuta, td_fattura, td_scontrino, td_proforma, td_concordato, td_devnul };

  QString stampanti[num_docs]= { "RICEVUTA", "FATTURA", "SCONTRINO", "PROFORMA", "CONCORDATO", "/dev/nul" };

  tr_comanda::docs tipi_doc[num_docs]= {
    tr_comanda::doc_ricevuta, tr_comanda::doc_fattura, tr_comanda::doc_scontrino, tr_comanda::doc_proforma,
         tr_comanda::doc_concordato, tr_comanda::doc_proforma
  };

  tr_comanda::docs tipi_doc_divisi[num_docs]= {
    tr_comanda::doc_ricevuta_divisa, tr_comanda::doc_fattura_divisa, tr_comanda::doc_scontrino_divisa,
         tr_comanda::doc_proforma_divisa, tr_comanda::doc_concordato_divisa, tr_comanda::doc_proforma_divisa
  };

  bool intestabile[num_docs]= { true, true, false, false, false, false };

  TROWI( tipi ) {
    if ( tipi[i]==tipo ) {
      stampante=stampanti[i];
      if ( comanda->modalita()==tr_comanda::m_conti ) doc=tipi_doc_divisi[i];
      else
        doc=tipi_doc[i];
      if ( intestabile[i]&&glb.intestazione.intestato() ) intestare=true;;
    }
  }

  bool r=true;

  int qcs=1;

  if ( multi_conto() ) {
    if ( _divisione_fittizia==true ) dividi_conto();
    if ( separata->righe()==0 ) qcs=comanda->righe();
  }

  frm_wait*w=NULL;
  QTime t;
  t.start();
  if ( qcs>1 ) w=tools.wait( this, tr( "Stampa in corso..." ) );
  else if ( !conto_aperto() ) apri_conto();
  for ( int i=0; i<qcs&&r; i++ ) {

    if ( !w&&t.elapsed()>2000 ) w=tools.wait( this, tr( "Stampa in corso..." ) );

    if ( multi_conto()&&!separato() ) {
      tr_comanda::tr_selezione s=comanda->selezione( 0 );
      bool r=conto_aperto();
      if ( !r ) r=separata->apri_conto( tr_comanda::cnt_diviso );
      if ( !r ) {
        tools.msgbox( tr( "Conto non aperto in stampa" ) );
        return false;
      }

      separata->acquisisci_portata( s.id_riga );
    }

    money residuo=pagamento->residuo();
    if ( pagamento->righe()==0 ) {
      QStringList p;
      abilita( forme_pagamenti );
      if ( tipo==td_devnul ) p<<QSN( config.pg_contanti );
      else if ( forme_pagamenti->isEnabled() ) p=forme_pagamenti->selezionati();

      if ( !p.count() ) p<<QSN( config.pg_contanti );

      bool ok=true;
      TROWSLI( p )
      if ( ok ) ok=CMD( tok_inserisci_articolo_comanda, "1", p[i], LEAVE, LEAVE, LEAVE, LEAVE );
      if ( !ok ) {
        if ( separata->conto_aperto() ) separata->annulla();
        if ( pagamento->conto_aperto() ) pagamento->annulla();
        if ( conto_aperto() ) chiudi_conto();
        break;
      }
      residuo=0;
    }

    if ( tipo==td_devnul ) {
      if ( residuo!=0 ) CMD( tok_inserisci_articolo_comanda, "1", QSN( config.pg_contanti ), LEAVE, LEAVE, LEAVE, LEAVE );
      chiudi_conto( tr_comanda::cnt_devnul );
      CMD( tok_nuova_comanda );
    }
    else {

      if ( intestare ) CMD( tok_intesta_conto, QSN( glb.intestazione.id ) );

      if ( tipo==td_proforma ) chiudi_conto( tr_comanda::cnt_fittizio );
      else
        chiudi_conto();

      r=CMD( tok_stampa_conto, QSN( doc ), "0", stampante );
      if ( w ) w->avanza();
      r=CMD( tok_nuova_comanda );
    }
  } // for

  if ( w ) DELETE( w );

  if ( tipo==td_devnul ) {
    cout<<"\n --- per un buco interno il conto e' stato perso -- \n\n";
    return r;
  }

  aggiorna();

  if ( comanda->righe()==0||( config.proforma_conclusiva&&tipo==td_proforma ) ) {

    accept();
  }
  else {
    separata->totale_comanda( "0.00" );
    pagamento->totale_comanda( "0.00" );

    contanti();
    //reject();
  }

  stato_pulsanti();
  return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_conto::card_readed() {
  if ( !card.count() ) {
    frm_base::card_readed();
    return;
  }
  enu_tessera;

  int tipo=card[p_famiglia].toInt();

  switch ( tipo ) {
    case TRPayCard::famiglia_prepagata:
    case TRPayCard::famiglia_convenzione:
    case TRPayCard::famiglia_cliente: {
        bool fai=false;
        if ( !conto_aperto() ) {
          if ( apri_conto() ) fai=true;
          else {
            annulla_clicked();
          }
        }
        else
          fai=true;
        if ( fai ) {
          enu_tipi_tessere;
          if ( CMD( tok_pagamento_tag, card[p_rftag] ) ) {
            aggiorna( frm_conto::c_pagamento );
            if ( ( card[p_tipo].toInt()&tt_cliente )&&( card[p_id_p0].toInt() ) ) {
              tr::mse elenco( tok_elenco_clienti_esteso, card[p_id_p0] );
              barra_stato->setCliente( elenco.descrizione() );
            }
          }
        }
      }
    break;

    case TRPayCard::famiglia_postpagato:
      if ( card[p_id_t0].toInt()==tavolo.id()||tavolo.bar() ) {
        tavolo.chiudi();
        stato_tavolo=tavolo.apri();
        if ( stato_tavolo.count() ) tavolo.card=card;
        else
          reject();
      }
    break;

    default:
      frm_base::card_readed();
  }

  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77
void frm_conto::puls_clicked() {
  tr_label*p=( tr_label* ) sender();
  QString prz;
  QString sc;
  if ( qstrcmp( p->name(), "puls_sx" ) ) {
    sc=tr( "Sconto % (Da 0 a 99)" );
    do prz=tools.calcolatrice( this, sc, prz, "int" );
    while ( prz.toInt()>99||prz.toInt()<0 );
  }
  if ( qstrcmp( p->name(), "puls_dx" ) ) {
    sc=tr( "Sconto Valore" );
    prz=tools.calcolatrice( this, sc, prz, "real2" );
  }
  if ( !prz.isNull() ) {
    if ( qstrcmp( p->name(), "puls_sx" ) ) {
      if ( separata->conto_aperto() )
        prz=QSN( separata->totale_comanda()*prz.toDouble()/100.00 );
      else
        prz=QSN( comanda->totale_comanda()*prz.toDouble()/100.00 );
    }

    if ( comanda->modalita()==tr_comanda::m_conti ) {
      if ( _divisione_fittizia ) dividi_conto();
    }
    if ( !pagamento->conto_aperto()&&!separata->conto_aperto() ) {
      bool r;
      if ( comanda->modalita()==tr_comanda::m_conti ) {
        r=pagamento->apri_conto( tr_comanda::cnt_diviso );
        for ( int i=0; i<comanda->righe(); i++ ) {
          tr_comanda::tr_selezione s=comanda->selezione( i );
          separata->acquisisci_portata( s.id_riga );
        }
        aggiorna();
      }
      else
        r=pagamento->apri_conto( tr_comanda::cnt_globale );
      if ( !r ) return;
    }

    pagamento->inserisci_pagamento( 301, prz );
    stato_pulsanti();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77
