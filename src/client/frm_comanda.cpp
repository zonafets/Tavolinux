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

/* HISTORY: 30-09-04  aggiunta gestione codici a barre: keyPressEvent e altro_clicked */

#include "frm_comanda.hpp"
#include "frm_conto.hpp"
#include <qapplication.h>
#include <qregexp.h>
#include <qfile.h>
#include <qdatastream.h>
#include "cmdcodicigruppi.hpp"
#include "frm_scelta.hpp"
#include "tr_menu.hpp"
#include "tr_globals.hpp"
#include "tr_tools.hpp"
#include "tr_display.hpp"
#include "msgs.hpp"
#include "tr_dmacro2.hpp"
#include <iostream>

#define MSG_METTI_VAR "metti var."
#define MSG_TOGLI_VAR "togli var."

extern tr_globals glb;
extern tr_tools tools;
extern tr_display display_lcd;


frm_comanda::frm_comanda( QWidget*parent, const char*name, bool modal, WFlags fl ):frm_base( parent, name, modal, fl ) {

  form_main=( frm_main* ) parent;

  titolo->setText( tr( "comanda" ) );

  iniz_area( 2, 2 );

  NEW( menu, tr_menu( area, "menu" ) );
  menu->uso();

  CONNECT( menu );

  NEW( comanda, tr_comanda( area, "comanda" ) );
  comanda->show_levels();
  connect( comanda, SIGNAL( totale_clicked() ), this, SLOT( comanda_totale_clicked() ) );
  connect( comanda, SIGNAL( articolo_selezionato( int ) ), this, SLOT( comanda_clicked( int ) ) );
  connect( comanda, SIGNAL( qta_clicked( int ) ), this, SLOT( qta_clicked( int ) ) );
  connect( comanda, SIGNAL( prz_clicked( int ) ), this, SLOT( prz_clicked( int ) ) );
  connect( comanda, SIGNAL( qta_changed( int ) ), this, SLOT( qta_changed( int ) ) );
  connect( comanda, SIGNAL( prz_changed( int ) ), this, SLOT( prz_changed( int ) ) );

  // Modifica di Luca per impedire aggiramenti di modifiche quantita

  /* int cfr=1; if (config.calcolatrice) cfr=0;

  if (FLAGS(p_utente_mancino)) { area_layout->addMultiCellWidget( menu, 0,1,0, 0, Qt::AlignCenter );
  area_layout->addMultiCellWidget( comanda, 0,cfr,1, 1, Qt::AlignCenter ); } else {
  area_layout->addMultiCellWidget( comanda, 0,cfr,0, 0, Qt::AlignCenter );
  area_layout->addMultiCellWidget( menu, 0,1,1, 1, Qt::AlignCenter ); }

  if (config.calcolatrice) { NEW( calcolatrice, tr_calcolatrice(area,"calcolatrice"));
  area_layout->addMultiCellWidget( calcolatrice, 1,1,0, 0, Qt::AlignCenter );
  int dy=config.finestra_altezza_centrale-calcolatrice->minimumSizeHint().height();
  comanda->setMinMax( 800-menu->larghezza(), dy, 800-menu->larghezza(), dy ); dy=calcolatrice->minimumSizeHint().height();
  calcolatrice->setMinMax( 800-menu->larghezza(), dy, 800-menu->larghezza(), dy );

  } else calcolatrice=NULL;

  */

  if ( FLAGS( p_utente_mancino ) ) {
    area_layout->addMultiCellWidget( menu, 0, 1, 0, 0, Qt::AlignCenter );
    area_layout->addMultiCellWidget( comanda, 0, 0, 1, 1, Qt::AlignCenter );
  }
  else {
    area_layout->addMultiCellWidget( comanda, 0, 0, 0, 0, Qt::AlignCenter );
    area_layout->addMultiCellWidget( menu, 0, 1, 1, 1, Qt::AlignCenter );
  }

  NEW( calcolatrice, tr_calcolatrice( area, "calcolatrice" ) );
  calcolatrice->wide();

  if ( FLAGS( p_utente_mancino ) )
    area_layout->addMultiCellWidget( calcolatrice, 1, 1, 1, 1, Qt::AlignCenter );
  else
    area_layout->addMultiCellWidget( calcolatrice, 1, 1, 0, 0, Qt::AlignCenter );

  calcolatrice->display( 1 );

  CONNECT( calcolatrice );

  if ( !config.calcolatrice ) {
    int dy=config.finestra_altezza_centrale;
    comanda->setMinMax( config.finestra_larghezza-menu->larghezza(), dy );
    calcolatrice->hide();
  }
  else {
    int dy=config.finestra_altezza_centrale-calcolatrice->minimumSizeHint().height();
    comanda->setMinMax( config.finestra_larghezza-menu->larghezza(), dy );
  }

  tr_label**ps[]= {
    &altro, &calc, &trasmetti, &varianti, &segue, &insieme_a,
    &conto, &cancella, &exit };

  TRDiritti::TRDirittiUtente diritti[]= {
    p_none,p_none,p_none,p_none,p_comanda_segue,p_comanda_insieme_a,
    p_none,p_none,p_none
  };

  QString nomi=TRFORM( "altro#altro|calc#calc|registra#registra|varianti#varianti|segue#segue|"
     "insieme a#insieme a|conto#conto|cancella#cancella|uscita#uscita" );
  pulsanti( nomi, false, ps, NULL, true );

  CONNECT( altro );
  CONNECT( calc );
  CONNECT( trasmetti );
  CONNECT( varianti );
  CONNECT( segue );
  CONNECT( insieme_a );
  CONNECT( conto );
  CONNECT( cancella );
  CONNECT( exit );

  var_metti=metti;
  scambia=false;
  parcheggiata=false;

  // visualizza prima prech�cos�adatta le dimensioni della tabella
  barra_stato->tipo( tr_barra_stato::stato );
  barra_stato->setTavolo( "" ); // ...
  barra_stato->setSala( "" ); // ...
  barra_stato->setCameriere( glb.stato[tr_globals::p_utente_descrizione] );
  barra_stato->setListino( glb.stato[tr_globals::p_listino_descrizione] );

  show();

  grabKeyboard();

  glb.intestazione.init();

  devnul=false;

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* *  Destroys the object and frees any allocated resources */
frm_comanda::~frm_comanda() {
  releaseKeyboard();
  // no need to DELETE child widgets, Qt does it all for us
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_comanda::show() {
  frm_base::show();
  comanda->adatta();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::stato_pulsanti() {

  enum {
    p_altro, p_calc, p_trasmetti, p_varianti, p_segue, p_insieme_a, p_conto, p_cancella, p_exit, p_piu, p_meno, p_calcolatrice
  };

  enum {
    p_comanda, p_menu
  };

  bool stato[]= {
	true, true, true, true, true, true, true, true, true, true, true, true
	  };

  bool abili[]= { true, true };

  tr_widget*ps[]= {
	altro, calc, trasmetti, varianti, segue, insieme_a,
	conto, cancella, exit,
	comanda->piu, comanda->meno, calcolatrice
  };

  tr_widget*pa[]= {comanda, menu};

  if ( tavolo.card.isEmpty() )
    barra_stato->ico_tavolo->setPixmap( PIXMAP( "stato_tavolo.png" ) );
  else
    barra_stato->ico_tavolo->setPixmap( PIXMAP( "tessera.png" ) );

  tr_comanda::tr_selezione*s=comanda->selezionato();

  tr_menu::tr_key*cd=menu->cd();

  if ( comanda->righe()==0 ) {
    abili[p_comanda]=false;
    stato[p_varianti]=false;
    bool tc=!tavolo.tipi_conto().isEmpty();
    if ( tc ) stato[p_conto]=false; // solo se conto rapido
  }

  if ( comanda->righe_portate()==0 ) {
    stato[p_insieme_a]=false;
    stato[p_segue]=false;
    stato[p_varianti]=false;
  }
  else {
    if ( comanda->ultimo().id_istanza==tr::gruppi::insieme_a||comanda->ultimo().id_istanza==tr::gruppi::segue ) {
      stato[p_segue]=false;
      stato[p_insieme_a]=false;
    }
  }

  if ( !comanda->modificato() ) stato[p_trasmetti]=false;

  if ( s ) {

    if ( s->id_istanza<tr::gruppi::articolo_base||!s->portata ) {
      abili[p_menu]=false;
      stato[p_varianti]=false;
      stato[p_segue]=false;
      stato[p_insieme_a]=false;
    }
    else {
      tr_comanda::tr_selezione sp=comanda->selezione( s->riga-1 );
      if ( sp.id_istanza==tr::gruppi::segue||sp.id_istanza==tr::gruppi::insieme_a||comanda->prima_riga_portate() ) {
        stato[p_segue]=false;
        stato[p_insieme_a]=false;
      }

      if ( s->stato.gav()||s->stato.gam() ) {
        abili[p_menu]=false;
        stato[p_segue]=false;
        stato[p_insieme_a]=false;
      }

      if ( s->stato.gam() ) stato[p_varianti]=false;

    }

    if ( s->stato.gavf()||s->stato.gpeso()||s->stato.gam() ) {
      stato[p_piu]=false;
      stato[p_meno]=false;
    }

    if ( !s->portata ) {
      stato[p_piu]=false;
    }

  }
  else {
    if ( comanda->righe_portate()>0&&( comanda->ultimo().stato.gam()||comanda->ultimo().stato.gav() ) )
      stato[p_varianti]=false;
    stato[p_cancella]=false;
    stato[p_piu]=false;
    stato[p_meno]=false;
  }

  if ( var_metti==senza ) stato[p_varianti]=false;

  if ( !menu->voci() ) abili[p_menu]=false;

  if ( !menu->voci()||!comanda->righe() ) stato[p_conto]=false;

  if ( !config.calcolatrice ) {
    int dy=config.finestra_altezza_centrale;
    comanda->setMinMax( config.finestra_larghezza-menu->larghezza(), dy );
    stato[p_calcolatrice]=false;
  }
  else {
    int dy=config.finestra_altezza_centrale-calcolatrice->minimumSizeHint().height();
    comanda->setMinMax( config.finestra_larghezza-menu->larghezza(), dy );
    calcolatrice->setMinMax( comanda->width(), calcolatrice->minimumSizeHint().height() );
  }

  if ( !FLAGS( p_exit )&&glb.modo_bar ) { // solo logout
    exit->cambia_testo( tr( LOGOUT ) );
  }
  else
    exit->cambia_testo( tr( "uscita" ) );

  if ( !FLAGS( p_comanda_segue ) ) stato[p_segue]=false;
  if ( !FLAGS( p_comanda_insieme_a ) ) stato[p_insieme_a]=false;
  if ( !FLAGS( p_comanda_cancella ) ) stato[p_cancella]=false;
  if ( !FLAGS( p_comanda_inserisci ) ) abili[p_menu]=false;
  if ( !FLAGS( p_comanda_conto ) ) stato[p_conto]=false;
  if ( !FLAGS( p_comanda_trasmetti ) ) stato[p_trasmetti]=false;
  if ( !FLAGS( p_comanda_exit ) ) stato[p_exit]=false;
  if ( !FLAGS( p_comanda_modifica_qta ) ) {
    stato[p_piu]=false;
    stato[p_meno]=false;
  }

  TROWI( stato )( stato[i]?scopri( ps[i] ):nascondi( ps[i] ) );
  TROWI( abili )( abili[i]?abilita( pa[i] ):disabilita( pa[i] ) );

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::aggiorna_comanda( bool solo_stato ) {
  if ( !solo_stato ) comanda->aggiorna();

  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::menu_clicked() {

  tr::menu::voce*v=menu->selezionata();
  if ( v ) { // potrebbe essere un CD

    bool b=false;

    QString prz="";

    if ( v->tipo_articolo.gapr() ) {
      prz=tools.calcolatrice( this, tr( "Prezzo " )+v->descrizione_originale, prz, "real2", config.calcolatrice_dec_fisso );
      if ( prz.isNull() ) return;
    }
    else
      prz=LEAVE;

    if ( calc->isOn()&&!v->tipo_articolo.gam()&&!v->tipo_articolo.gac()&&!v->tipo_articolo.gav() ) {
      QString qta="1";
      QString maschera=QString::null;

      qta=tools.calcolatrice( this, tr( "Qta " )+v->descrizione_originale, qta, maschera, v->tipo_articolo.gpeso() );


      if ( !qta.isNull() ) {
        double _qta=qta.toDouble();
        b=inserisci_articolo( &_qta, prz );
      }
    }
    else {
      if ( config.calcolatrice&&calcolatrice->pannello->value()>0.0&&!v->tipo_articolo.gam()&&!v->tipo_articolo.gac()&&
         !v->tipo_articolo.gav() ) {
           double _qta=calcolatrice->pannello->value();
           b=inserisci_articolo( &_qta, prz );
           calcolatrice->display( 1 );
      }
      else
        b=inserisci_articolo( NULL, prz );
    }

    if ( b ) {
      aggiorna_comanda( true );
      controllo_tessera();
    }
    else
      stato_pulsanti();

  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77
bool frm_comanda::inserisci_articolo( double*qta, QString prz ) {
  tr::menu::voce*v=menu->selezionata();
  bool r=comanda->inserisci_articolo( v->is, v->is_padre, v->tipo_articolo(), var_metti==metti?true:false, qta, prz );
  if ( r&&display_lcd.inizializzato() ) {
    display_lcd.aggiorna( tr_display::tr_inserisci_articolo, comanda->ultimoDato().quantita, comanda->ultimoDato().descrizione,
       comanda->ultimoDato().prezzo );
  }
  return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::altro_clicked() {
  enum {
    cod_barre, listino, messaggio, offri
  };
  uint nascosti=0;
  QString etichette=TRFORM( ( "cod.barre#cod.barre|listino#listino|messaggio#messaggio|offri#offri" ) );

  TRDirittiUtente diritti[]= {
    p_comanda_codice_barre,p_comanda_listino, p_comanda_messaggio,
    p_none
  };

  diritti[offri]=p_null;

  if ( comanda->selezionato()&&!comanda->selezionato()->portata&&!comanda->selezionato()->stato.gav() )
    diritti[offri]=p_comanda_offri;

  QString n=tools.hvmenu( altro, etichette, diritti );
  if ( n.isEmpty() ) return;

  if ( n=="cod.barre" ) {
    releaseKeyboard();
    QString barcode=tools.input_descrizione( this, "IMMETTERE CODICE A BARRE", "", false );
    if ( !barcode.isEmpty()&&comanda->inserisci_articolo( barcode, 1 ) ) aggiorna_comanda();
    grabKeyboard();
  }

  if ( n=="listino" ) {
    QString id=id_listino;
    QString descr;
    if ( tools.scelta( tr_listview::listini, id, descr ) ) {
      if ( id_listino!=id&&CMD( tok_imposta_listino, id ) ) {
        id_listino=id;
        barra_stato->setListino( descr );
        menu->cambia( id, true, false );
      }
    }
  }


  if ( n=="messaggio" ) {
    QString id;
    QString descr;
    if ( tools.scelta( id_listino, tr_listview::messaggi, id, descr ) ) {
      comanda->inserisci_articolo( id.toInt(), 0, tr::tipo_articolo( GRUPPO_ARTICOLO_MESSAGGIO ) );
      aggiorna_comanda();
    }
  }

  if ( n=="offri" ) {
    comanda->offri();
  }

  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77
bool frm_comanda::stampa() {
  if ( comanda->righe_portate()!=0&&!tavolo.tway() ) {
    if ( config.stampa_comanda_nsq==1 ) comanda->stampa();
    else if ( config.stampa_comanda_nsq==2 ) {
      if ( tools.msgbox( this, tr( "Vuoi stampare la comanda?" ), tr( "Stampa comanda registrata" ), SINO )==SI ) {
        return comanda->stampa();
      }
    } // if
  } // if righe
  return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

bool frm_comanda::controllo_tessera() {
  if ( tavolo.card.count() ) {
    enu_tessera;
    if ( tavolo.card[p_famiglia].toInt()==TRPayCard::famiglia_postpagato ) {
      if ( comanda->totale_comanda()>tavolo.card[p_massimo].toDouble() ) {
        tools.msgbox( tr( "Il totale comanda(" )+comanda->totale->text()+tr( ") supera il massimale della scheda postpagata" )+
           tavolo.card[p_massimo]+")" );
        return false;
      }
    }
  } // se card
  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

bool frm_comanda::trasmetti_comanda() {
  if ( !controllo_tessera() ) return false;

  if ( comanda->registra() ) {
    parcheggiata=false;

    // aggiorna il display

    if ( display_lcd.inizializzato() )
      display_lcd.aggiorna( tr_display::tr_totale_conto, QString::null, QString::null, QSNP( comanda->totale_comanda() ) );

    if ( !stampa() ) comanda->nuova();
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::trasmetti_clicked() {
  if ( trasmetti_comanda() ) {
    aggiorna_comanda();
    if ( !glb.modo_bar ) accept();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::varianti_clicked() {
  tr_comanda::tr_selezione*s=comanda->selezionato();
  if ( !s&&comanda->righe_portate()>0&&!comanda->ultimo().stato.gam()&&!comanda->ultimo().stato.gav()&&menu->cd()&&
     menu->cd()->is!=IS_VARIANTI ) {
       s=&comanda->ultimo();
       if ( !s ) BEEPER /* &&s->ha_varianti() */
         else
           var_metti=togli;
  }

  if ( menu->cd()&&menu->cd()->is!=IS_VARIANTI ) {
    if ( !menu->isEnabled() ) menu->setEnabled( true );
    menu->cd( tr_menu::varianti, s->is );
  }
  if ( var_metti==togli ) {
    var_metti=metti;
    varianti->setText( PTEXT( "az_variante.png", tr( MSG_METTI_VAR ) ) );
  }
  else {
    var_metti=togli;
    varianti->setText( PTEXT( "az_variante_meno.png", tr( MSG_TOGLI_VAR ) ) );
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::segue_clicked() {
  comanda->inserisci_articolo( tr::gruppi::segue );
  aggiorna_comanda();
  /* nascondi( segue, insieme_a ); if (comanda->modificato() ) scopri(trasmetti); else nascondi(trasmetti); abilita( menu ); */
  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::insieme_a_clicked() {
  comanda->inserisci_articolo( tr::gruppi::insieme_a );
  aggiorna_comanda();
  /* nascondi( segue, insieme_a ); if (comanda->modificato()) scopri( trasmetti); else nascondi(trasmetti); abilita( menu ); */
  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::apri_conto() {
  scambia=true;
  trasmetti_comanda();
  accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::conto_clicked() {
  bool r=true;
  if ( comanda->modificato() ) {
    r=comanda->registra();
    // if (!stampa()) comanda->nuova();
    stampa();
    comanda->nuova();
  }

  if ( r ) {

    setEnabled( false );
    if ( display_lcd.inizializzato() )
      display_lcd.aggiorna( tr_display::tr_totale_conto, QString::null, QString::null, QSNP( comanda->totale_comanda() ) );

    if ( tools.conto_clicked( conto, &tavolo, stato_tavolo, comanda->totale_comanda() ) )
      apri_conto();

    aggiorna_comanda();

    denu_apri_tavolo;
    stato_tavolo[p_coperti_presenti]=comanda->dati.coperti;
    stato_tavolo[p_articoli_p_nullo]=comanda->dati.non_prezzati;

    if ( comanda->righe()==0&&!glb.modo_bar&&( config.uscita_conto_rapido_nsc==1||!tavolo.bar() ) ) {
      accept();
    }

    if ( comanda->righe()==0&&tavolo.bar()&&!config.id_menu_bar.isEmpty() )
      menu->cd( config.id_menu_bar.toInt() );

    stato_pulsanti();
    setEnabled( true );

  } // if (r)

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::cancella_clicked() {
  if ( devnul ) {
    print( td_devnul );
    if ( ( config.cancella_comanda_non_conclusivo&&tavolo.bar() )||glb.modo_bar ) {
      aggiorna_comanda();
      devnul=false;
    }
    else
      accept();
    return;
  }

  if ( comanda->selezionato() ) comanda->cancella();
  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::exit_clicked() {
  bool tc=!tavolo.tipi_conto().isEmpty();
  if ( comanda->modificato() ) {
    bool trasmetti=false;
    if ( tc&&config.salva_in_uscita_se_conto_rapido_nsq==1 ) trasmetti=true;
    else if ( !tc||config.salva_in_uscita_se_conto_rapido_nsq==2 ) {
      QString r=tools.msgbox( this, tr( "Sono state apportate delle modifiche<br> Si desidera trasmettere?<br>" ), tr( "Attenzione" ), SIANNULLANO );
      if ( r==SI ) trasmetti=true;
      if ( r==ANNULLA ) return;
      if ( r==NO ) reject();
    }
    if ( trasmetti ) {
      trasmetti_comanda();
      reject(); // questo forza l'accept del trasmetti
      cout << __FUNCTION__<<":"<<__LINE__<<endl;
    }
  }
  else
    reject();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::qta_clicked( int row ) {
  if ( FLAGS( p_comanda_modifica_qta ) ) {
    comanda->cambia_qta( row );
    devnul=false;
    stato_pulsanti();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::prz_clicked( int row ) {
  if ( FLAGS( p_comanda_modifica_prz ) ) {
    comanda->cambia_prz( row );
    devnul=false;
    stato_pulsanti();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::qta_changed( int ) {
  stato_pulsanti();
  if ( display_lcd.inizializzato() ) {
    display_lcd.aggiorna( comanda->ultimoDato().token_op==tok_storna?tr_display::tr_storna_articolo:
       tr_display::tr_modifica_quantita, comanda->ultimoDato().quantita, comanda->ultimoDato().descrizione,
       comanda->ultimoDato().prezzo );
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::prz_changed( int ) {
  stato_pulsanti();
  if ( display_lcd.inizializzato() ) {
    display_lcd.aggiorna( tr_display::tr_modifica_prezzo, comanda->ultimoDato().quantita, comanda->ultimoDato().descrizione,
       comanda->ultimoDato().prezzo );
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::comanda_clicked( int ) {

  tr_comanda::tr_selezione*s=comanda->selezionato();

  bool prec=false;
  if ( s&&s->portata&&!s->stato.gam() ) {
    tr::menu::voce*a=menu->voce( s->is );

    if ( a&&a->varianti() ) {
      menu->cd( tr_menu::varianti, a->is );
      var_metti=metti;
      varianti->setText( PTEXT( "az_varianti.png", tr( "varianti" ) ) );
    }
    else {
      prec=true;
      var_metti=senza;
    }

  }
  else
    prec=true;

  if ( prec&&menu->cd()&&menu->cd()->is==IS_VARIANTI )
    menu->cd( tr_menu::precedente );

  devnul=false;
  stato_pulsanti();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

bool frm_comanda::apri_tavolo( const itm_sala&s, const itm_tavolo&t, int bis ) {
  tavolo=t;
  int id=tavolo.id()+bis;
  tavolo.set( tr::enumeratori::tavolo_esteso::p_id, QSN( id ) );
  sala=s;
  return apri_tavolo();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

bool frm_comanda::apri_tavolo( const itm_sala&s, const itm_tavolo&t ) {
  tavolo=t;
  sala=s;
  return apri_tavolo();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77
bool frm_comanda::apri_tavolo() {
  denu_apri_tavolo;

  stato_tavolo=tavolo.apri();
  if ( !stato_tavolo.count() ) return false;

  if ( stato_tavolo[p_comanda_parcheggiata]=="1" ) {
    parcheggiata=true;
    tools.msgbox( tr( "Attenzione<br><br>Comanda parcheggiata<br>presente in questo tavolo" ) );
  }

  if ( tavolo.prenotato() ) {
    if ( tools.msgbox( tr( "Tavolo prenotato. E' il Sig. " )+stato_tavolo[p_nome_prenotatore]+" ?", SINO )==SI ) {
      CMD( tok_soddisfa_prenotazione );
    }
  }

  id_listino="-1";
  if ( CMD( tok_imposta_listino, stato_tavolo[p_cod_listino] ) ) {
    id_listino=stato_tavolo[p_cod_listino];
    barra_stato->setListino( stato_tavolo[p_descr_listino] );
    menu->cambia( stato_tavolo[p_cod_listino] );
  }
  else
    disabilita( menu );

  comanda->aggiorna();

  aggiorna_comanda( true );

  enu_tessera;
  if ( card.count()>=p_id_t1&&tavolo.id()==card[p_id_t0].toInt() ) {
    barra_stato->ico_tavolo->setText( PICON( "tessera.png" ) );
  }

  barra_stato->setTavolo( tavolo.nome() );
  barra_stato->setSala( sala.nome );

  stato_pulsanti();

  if ( tavolo.bar()&&!config.id_menu_bar.isEmpty() )
    menu->cd( config.id_menu_bar.toInt() );

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::chiudi_tavolo() {

  if ( parcheggiata&&!comanda->modificato() ) {
    CMD( tok_parcheggia_comanda );
  }

  tavolo.chiudi();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::calcolatrice_clicked() {
  if ( calcolatrice->rejected() ) {
    config.calcolatrice=false;
    stato_pulsanti();
  }
  if ( calcolatrice->accepted() ) {
    tr_comanda::tr_selezione*s=comanda->selezionato();
    if ( !s ) BEEPER;
    comanda->cambia_qta( calcolatrice->value() );
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_comanda::calc_clicked() {
  config.calcolatrice=!config.calcolatrice;
  calc->stato( tr_label::off );
  stato_pulsanti();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_comanda::keyPressEvent( QKeyEvent*e ) {
  if ( MODULO( modulo_barcode ) ) {
    QString barcode=QString::null;
    if ( e ) barcode=tools.barcode( e->text() );
    if ( !barcode.isNull() ) {
      double qta=1;
      if ( config.calcolatrice&&calcolatrice->pannello->value()>0.0 )
        qta=calcolatrice->pannello->value();
      if ( comanda->inserisci_articolo( barcode, qta ) ) {
        if ( display_lcd.inizializzato() ) {
          display_lcd.aggiorna( tr_display::tr_inserisci_articolo, comanda->ultimoDato().quantita,
             comanda->ultimoDato().descrizione, comanda->ultimoDato().prezzo );
        }
        aggiorna_comanda();
      }
    }

    e->ignore();
  }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_comanda::comanda_totale_clicked() {
  if ( FLAGS( p_comanda_stampa_devnull ) ) {
    comanda->seleziona();
    scopri( cancella );
    devnul=true;
  }
  else
    devnul=false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

bool frm_comanda::print( tipo_doc tipo ) {
  trasmetti_comanda();
  if ( comanda->apri_conto() ) {
    if ( tipo==td_devnul ) {
      CMD( tok_inserisci_articolo_comanda, "1", QSN( config.pg_contanti ), LEAVE, LEAVE, LEAVE, LEAVE );
      comanda->chiudi_conto( tr_comanda::cnt_devnul );
      CMD( tok_nuova_comanda );
    }
  }

  if ( tipo==td_devnul ) {
    cout<<"\n --- per un buco interno il conto e' stato perso -- \n\n";
    return true;
  }

  aggiorna_comanda();

  if ( ( comanda->righe()==0||( config.proforma_conclusiva&&tipo==td_proforma ) )&&
     ( !glb.modo_bar||config.uscita_conto_rapido_nsc==1 ) ) {
       accept();
  }

  stato_pulsanti();
  return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_comanda::card_readed() {
  if ( !card.count() ) {
    frm_base::card_readed();
    return;
  }
  enu_tessera;
  int tipo=card[p_famiglia].toInt();
  switch ( tipo ) {
    case TRPayCard::famiglia_postpagato:
      if ( card[p_id_t0].toInt()==tavolo.id()||tavolo.bar() ) {
        tavolo.chiudi();
        tavolo.card=card;
        stato_tavolo=tavolo.apri();
        if ( !stato_tavolo.count() ) reject();
        else
          aggiorna_comanda();
      }
    break;

    default:
      frm_base::card_readed();
  }
  stato_pulsanti();
}
