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

#include "frm_bis.hpp"
#include "frm_sale.hpp"
#include "frm_comanda.hpp"
#include "frm_conto.hpp"
#include "tr_tavoli_view.hpp"
#include "tr_tools.hpp"
#include "tr_globals.hpp"
#include "tr_dmacro2.hpp"

extern tr_tools tools;
extern tr_globals glb;

#include <qrect.h>
#include <qsize.h>

#define TQ "tavolo_q.png"

frm_bis::frm_bis( frm_main*parent, const char*name, bool modal, WFlags fl ):frm_base( parent, name, modal, fl ) {
  form_main=parent;

  titolo->setText( tr( "scelta tavolo bis" ) );

  nascondi_titolo();
  base_alza();

  _selezionato=NULL;

  uint i;
  const uint max_bis=10;

  QColor sfondo;

  tr_label**puls[]= { &conto, &comanda, &incassa, &sposta };
  QString etichette=TRFORM( ( "conto#conto|comanda#comanda|incassa#incassa|sposta#sposta" ) );
  for ( i=0; i<COUNT( puls ); i++ ) {
    tr_label*p;
    QString etichetta=etichette.section( '|', i, i );
    NEW( p, tr_label( area, etichetta.section( '#', 1, 1 ) ) );
    sfondo=p->aspetto_bottone( config.pulsante_menu );
    p->setTextPixelSize( 12 );
    QString icona=( "az_" +etichetta.section( '#', 1, 1 )+".png" );
    p->setText( PTEXT( icona, etichetta.section( '#', 0, 0 ) ) );
    p->setMinMax( 88, 60 );
    *puls[i]=p;
  }

  setPaletteBackgroundColor( sfondo );

  tr_label*numero[max_bis];
  for ( i=0; i<max_bis; i++ ) {
    QString n=QString::number( i );
    NEW( numero[i], tr_label( area, n ) );
    numero[i]->setMinMax( 68, 68 );

    QString prefix;
    QString postfix;
    int stato=form_main->tavoli->selezionato()->biss[i];

    bool selezionato=false;
    if ( !i ) {
      selezionato=true;
      _selezionato=numero[i];
    }
    numero[i]->setPixmap( itm_tavolo::componi_tavolo( TQ, n, sfondo, selezionato, stato ) );

    connect( numero[i], SIGNAL( clicked() ), this, SLOT( numero_clicked() ) );
  }

  primo=numero[0];

  int dx=88;
  QWidget*ws[]= {
      conto, numero[0], numero[1], numero[2], numero[3], numero[4], incassa, 0,
      comanda, numero[5], numero[6], numero[7], numero[8], numero[9], sposta ,0,
      vuoto(88,1), vuoto(dx,1), vuoto(dx,1),vuoto(dx,1),vuoto(dx,1),vuoto(88,1),0,0};

  build( ws, 8, 6, true );

  CONNECT( conto );
  CONNECT( comanda );
  CONNECT( incassa );
  CONNECT( sposta );

  tr_label**ps[]= { &svuota, &sblocca, &exit };
  QString nomi=TRFORM( ( "svuota#svuota|sblocca#sblocca|uscita#uscita" ) );
  pulsanti( nomi, false, ps, NULL );

  CONNECT( sblocca );
  CONNECT( svuota );
  CONNECT( exit );

  barra_stato->setText( tr( "Scegliere BIS" ) );

  id=-1;

  init();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_bis::~frm_bis() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_bis::init() {


  show();
  numero_clicked();

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_bis::numero_clicked() {
  tr_label*n=( tr_label* ) sender();
  if ( !n ) n=primo;

  if ( _selezionato ) {
    bis=QString( _selezionato->name() ).toInt();
    stato=form_main->tavoli->selezionato()->biss[bis];
    _selezionato->setPixmap( itm_tavolo::componi_tavolo( TQ, QString::number( bis ), paletteBackgroundColor(), false, stato ) );
  }
  _selezionato=n;
  bis=QString( n->name() ).toInt();
  id=form_main->tavoli->selezionato()->id()+bis;
  stato=form_main->tavoli->selezionato()->biss[bis];
  _selezionato->setPixmap( itm_tavolo::componi_tavolo( TQ, QString( n->name() ), paletteBackgroundColor(), true, stato ) );

  //barra_stato->testo->label->setPaletteBackgroundColor( QColor( 100,100,100));
  barra_stato->setText( tr( "Tavolo " )+"|" +form_main->tavoli->selezionato()->numero()+"|" +tr( " bis " )+"|" +
     QString::number( bis )+"|" );
  barra_stato->testo->adjustSize();

  stato_pulsanti();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_bis::sblocca_clicked() {
  accept();

  CMD( tok_sblocca_tavolo, QSN( id ) );

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_bis::svuota_clicked() {
  accept();

  QString fai;
  if ( config.chiedi_su_svuota )
    fai=tools.msgbox( tr( "Si e' sicuri di voler SVUOTARE il tavolo?" ), SINO );
  if ( fai==SI )
    CMD( tok_svuota_tavolo, QSN( id ) );

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_bis::sposta_clicked() {
  accept();

  int id_da=id;
  QString id_a;
  QString descrizione;
  if ( tools.scelta( tr_listview::tavoli, id_a, descrizione ) ) {
    CMD( tok_sposta_tavolo, QSN( id_da ), id_a );
  }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_bis::conto_clicked() {
  accept();

  FORM( f, frm_conto );

  if ( f->apri_tavolo( form_main->tavoli->sala(), *form_main->tavoli->selezionato(), bis ) ) f->exec();
  f->chiudi_tavolo();

  xFORM( f, frm_conto );

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_bis::comanda_clicked() {
  accept();

  FORM( f, frm_comanda );

  if ( f->apri_tavolo( form_main->tavoli->sala(), *form_main->tavoli->selezionato(), bis ) ) f->exec();
  f->chiudi_tavolo();
  bool scambia=f->scambia;

  xFORM( f, frm_comanda );

  if ( scambia ) conto_clicked();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_bis::incassa_clicked() {
  accept();
  CMD( tok_incassa_tavolo, QSN( id ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_bis::exit_clicked() {
  reject();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_bis::stato_pulsanti() {
  enum {
    p_comanda, p_conto, p_incassa, p_sposta, p_svuota, p_sblocca
  };
  bool vis[]= {true,true,true,true,true,true};
  tr_label*ps[]= {comanda,conto,incassa,sposta,svuota,sblocca};

  if ( !( stato&( 1<<TAVOLO_OCCUPATO ) ) ) {
    vis[p_conto]=false;
    vis[p_sposta]=false;
    vis[p_svuota]=false;
  }
  if ( !( stato&( 1<<TAVOLO_CONTO ) ) ) vis[p_incassa]=false;
  if ( stato&( 1<<TAVOLO_CONTO_DIVISO ) ) vis[p_comanda]=false;

  if ( !FLAGS( p_main_sposta ) ) vis[p_sposta]=false;
  if ( !FLAGS( p_main_conto ) ) vis[p_conto]=false;
  if ( !FLAGS( p_main_comanda ) ) vis[p_comanda]=false;
  if ( !FLAGS( p_main_gestione_svuota ) ) vis[p_svuota]=false;
  if ( !FLAGS( p_main_gestione_sblocca ) ) vis[p_sblocca]=false;

  TROWI( vis )( vis[i]?scopri( ps[i] ):nascondi( ps[i] ) );

}

#undef TQ
