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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <qcanvas.h>

#include "frm_sale.hpp"
#include "frm_scelta.hpp"
#include "tr_globals.hpp"
#include "tr_tools.hpp"
#include "tr_forms.hpp"


extern tr_globals glb;
extern tr_tools tools;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_sale::frm_sale( frm_main*parent, const char*name, bool modal, WFlags fl ):frm_base( parent, name, modal, fl ) {
  form_main=parent;
  titolo->setText( tr( "inserimento/modifica sala" ) );

  NEW( tavoli, tr_tavoli_view( area, "tavoli" ) );

  iniz_area( 1, 1 );
  area_layout->addWidget( tavoli, 0, 0 );

  QCanvas*canvas;
  NEW( canvas, QCanvas( tavoli, "pavimento" ) );
  canvas->setDoubleBuffering( true );

  tavoli->setCanvas( canvas );

  tr_label**ps[]= {&imposta_sala,NULL,NULL,&tavolo,&tipo,NULL,&cancella, &exit };
  QString pulsanti_sala=TRFORM( ( "imposta#imposta|||proprieta#proprieta|tipo#tipo||cancella#cancella|uscita#uscita" ) );
  pulsanti( pulsanti_sala, false, ps, NULL );

  connect( tavoli, SIGNAL( toccato( QMouseEvent* ) ), this, SLOT( tavoli_clicked( QMouseEvent* ) ) );
  CONNECT( imposta_sala );
  CONNECT( tavolo );
  CONNECT( tipo );
  CONNECT( cancella );
  CONNECT( exit );

  connect( barra_stato, SIGNAL( sala_clicked() ), this, SLOT( scelta_sala_clicked() ) );

  init();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
frm_sale::~frm_sale() {
  srv->tavoli( NULL );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void frm_sale::init() {

  barra_stato->tipo( tr_barra_stato::stato );
  barra_stato->setCameriere( glb.stato[tr_globals::p_utente_descrizione] );
  barra_stato->setListino( glb.stato[tr_globals::p_listino_descrizione] );

  nascondi( tavolo, cancella );

  tavoli->inserimento( true );
  carica_sala( config.sala_ultima_id, config.sala_ultima_vista_a_griglia );

  srv->tavoli( tavoli );

  const int pos=tr::pixmap::p_tipo;
  TROWSLI( glb.pixmaps ) {
    int tipo=glb.pixmaps[i].section( '|', pos, pos ).toInt();
    if ( itm_tavolo::genere( tipo )==itm_tavolo::tavolo||itm_tavolo::bar( tipo )||
       itm_tavolo::genere( tipo )==itm_tavolo::arredo||itm_tavolo::genere( tipo )==itm_tavolo::dinamico )
         pixmap_tavoli<<glb.pixmaps[i];
  }
  n_pixmap=0;

  while ( pixmap_tavoli[n_pixmap].section( '|', 0, 0 )!="tavolo_q.png" ) n_pixmap++;

  tipo->cambia_icona( pixmap_tavoli[n_pixmap].section( '|', 0, 0 ) );

}


//--------------------------------------------------------------------------------------- carica_sala

void frm_sale::carica_sala( int id, bool vista_a_griglia ) {
  tavoli->seleziona( NULL, NULL );
  if ( tavoli->carica_tavoli( id, false ) ) {
    tavoli->tavoli_a_griglia( vista_a_griglia );
  }
  _id_prec_selezionato=-1;
  barra_stato->setTavolo( tr( "(toccare un tavolo o zona vuota per aggiugerlo)" ) );
  barra_stato->setSala( tavoli->sala().nome );
}

void frm_sale::carica_sala() {
  if ( tavoli->carica_tavoli( sala().id, true ) ) {
  }
  _id_prec_selezionato=-1;
  barra_stato->setTavolo( tr( "(toccare un tavolo o zona vuota per aggiugerlo)" ) );
  barra_stato->setSala( tavoli->sala().nome );
}

//--------------------------------------------------------------------------------------- resize_event

void frm_sale::resizeEvent( QResizeEvent*ev ) {
  tavoli->canvas()->resize( ev->size().width(), ev->size().height() );
  if ( tavoli->vista_a_griglia() ) tavoli->tavoli_a_griglia( true );
  tavoli->canvas()->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void frm_sale::tipo_clicked() {
  n_pixmap++;
  if ( ( uint )n_pixmap>=pixmap_tavoli.count() ) n_pixmap=0;
  tipo->cambia_icona( pixmap_tavoli[n_pixmap].section( '|', 0, 0 ) );
}

// --------------------------------------------------------------------------------------------- vista_clicked

void frm_sale::imposta_sala_clicked() {

  nascondi( exit );
  FORM( f, frm_sale_imposta );
  f->aggiorna( tavoli->sala().id );
  bool r=f->accepted();
  /* if (r || f->modificato) { if (r && f->selezionata()) carica_sala( f->sala()[p_is].toInt(), tavoli->vista_a_griglia() );
  else carica_sala(); } else carica_sala();	// ricarica quella corrente se esiste ancora */
  xFORM( f, frm_sale_imposta );
  scopri( exit );

  // tavoli->carica_tavoli( scelta.id_scelta() );

}

// ------------------------------------------------------------------------------------------ tavolo_clicked

void frm_sale::tavolo_clicked() {
  nascondi( exit );
  FORM( f, frm_sale_imposta_tavoli );
  f->nome->setText( selezionato()->numero() );
  f->exec();
  /* if (f->accepted()) { carica_sala( tavoli->sala().id, tavoli->vista_a_griglia() ); } */
  xFORM( f, frm_sale_imposta_tavoli );
  scopri( exit );

}


// --------------------------------------------------------------------------------------------- cancella_clicked

void frm_sale::cancella_clicked() {
  tavoli->cancella();
  _id_prec_selezionato=-1;
  stato_pulsanti();
}

// --------------------------------------------------------------------------------------------- exit_clicked

void frm_sale::stato_pulsanti() {
  if ( !selezionato() ) {
    nascondi( tavolo, cancella );
    barra_stato->setTavolo( "" );
  }
}

// --------------------------------------------------------------------------------------------- exit_clicked

void frm_sale::exit_clicked() {
  CMD( tok_riorganizza_database, QString( "tavoli" ) );
  CMD( tok_riorganizza_database, QString( "sale" ) );
  reject();
}

// --------------------------------------------------------------------------------------------- tavoli_clicked

void frm_sale::tavoli_clicked( QMouseEvent*e ) {

  if ( sala().id==-1 ) {
    imposta_sala_clicked();
    if ( sala().id==-1 ) return;
  }

  QString st;
  QPoint p( e->pos() );

  itm_tavolo*s=selezionato();
  if ( s!=NULL ) { //  && _id_prec_selezionato == -1)  {
    if ( _id_prec_selezionato==s->id() ) {
      nascondi( cancella, tavolo );
      tavoli->seleziona( ( itm_tavolo* ) NULL );
      _id_prec_selezionato=-1;
      barra_stato->setTavolo( tr( "(toccare un tavolo o zona vuota per aggiugerlo)" ) );
      barra_stato->setSala( tavoli->sala().nome );

      return;
    }

    scopri( cancella );
    /* if ( s->genere()==itm_tavolo::tavolo&&!s->tway() ) scopri( tavolo ); else nascondi( tavolo ); */
    if ( s->genere()==itm_tavolo::tavolo ) scopri( tavolo );
    else
      nascondi( tavolo );
    _id_prec_selezionato=s->id();
    if ( s->genere()==itm_tavolo::arredo||s->genere()==itm_tavolo::dinamico ) {
      barra_stato->setTavolo( tr( "ogg.arredo" ) );
      barra_stato->setSala( tavoli->sala().nome );
      // barra_stato->setCameriere("");
      barra_stato->setListino( tr( "(toccare stesso tavolo o zona vuota per spostare)" ) );
    }
    else {
      barra_stato->setTavolo( s->numero() );
      barra_stato->setSala( tavoli->sala().nome );
      // barra_stato->setCameriere("");
      barra_stato->setListino( tr( "(toccare stesso tavolo o zona vuota per spostare)" ) );
    }
    return;
  }

  if ( ( _id_prec_selezionato!=-1 )&&( s==NULL ) ) {
    tavoli->seleziona( _id_prec_selezionato );
    if ( tavoli->vista_a_griglia() ) BEEPER;
    tavoli->sposta( p );
    return;
  }

  if ( _id_prec_selezionato==-1&&s==NULL ) {
    if ( tavoli->vista_a_griglia() ) BEEPER;
    tavoli->aggiungi( p, pixmap_tavoli[n_pixmap].section( '|', 0, 0 ) );
    return;
  }
}


void frm_sale::scelta_sala_clicked() {

  QString id=QSN( tavoli->sala().id );
  QString descrizione;

  tools.scelta( tr_listview::sale, id, descrizione );
  if ( id!=QSN( tavoli->sala().id ) ) {
    carica_sala( id.toInt(), tavoli->vista_a_griglia() );
  }

  stato_pulsanti();

}

frm_sale_imposta::frm_sale_imposta( frm_sale*parent, const char*name, bool modal, WFlags fl ):
   frm_base( parent, name, modal, fl ) {

     form_sale=parent;

     titolo->setText( tr( "impostazione sala" ) );

     base_alza();

     nascondi_titolo();

     iniz_area( 1, 1 );

     NEW( elenco, tr_listview( area, "elenco_sale" ) );
     elenco->setMinMax( 400, 360, SMAX );
     elenco->tipo( tr_listview::sale );
     elenco->unselectable( false );
     area_layout->addWidget( elenco, 0, 0 );

     tr_label**ppuls[]= { &tavoli, &rinomina, &aggiungi, &cancella,  &exit };
     QString nomi=TRFORM( ( "tavoli/pav.#tavoli/pav.|rinomina#rinomina|aggiungi#aggiungi|"
        "cancella#cancella|uscita#uscita" ) );
     pulsanti( nomi, false, ppuls, NULL );

     CONNECT( tavoli );
     CONNECT( rinomina );
     CONNECT( aggiungi );
     CONNECT( cancella );
     CONNECT( exit );
     connect( elenco, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( elenco_clicked( QListViewItem* ) ) );

     init();
}

frm_sale_imposta::~frm_sale_imposta() {
}

void frm_sale_imposta::show() {
  frm_base::show();
  elenco->adatta();
}

void frm_sale_imposta::init() {

  barra_stato->setText( tr( "Scelta/Modifica/Aggiunta Sale" ) );

  nascondi( rinomina, cancella );

  modificato=false;
}

void frm_sale_imposta::stato_pulsanti( QListViewItem*it ) {
  bool b=( it||!elenco->selezionato().isEmpty() );
  b?scopri( tavoli, rinomina, cancella ):nascondi( tavoli, rinomina, cancella );
}

void frm_sale_imposta::aggiorna( int _id_sala ) {
  id_sala=_id_sala;
  elenco->carica( QSN( id_sala ) );
  stato_pulsanti( NULL );
  // elenco->adatta();
}

void frm_sale_imposta::elenco_clicked( QListViewItem*it ) {
  stato_pulsanti( it );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_sale_imposta::entra_clicked() {
  modificato=true;
  accept();
}

void frm_sale_imposta::tavoli_clicked() {
  nascondi( exit );
  FORM( f, frm_sale_imposta_tavoli );
  if ( f->accepted() ) modificato=true;
  xFORM( f, frm_sale_imposta_tavoli );
  scopri( exit );
  accept();
}

void frm_sale_imposta::rinomina_clicked() {
  QString st=tools.tastiera( ( QWidget* ) parent(), tr( "Rinomina Sala" ), sala() [p_nome] );
  if ( st!="" &&st!=sala() [p_nome] ) {
    QString id=sala() [p_is];
    if ( !CMD( tok_modifica_sala, id, st, LEAVE ) ) {
      tools.msgbox( this, tr( "Problemi di rinomina sala.\n\nUsare un altro nome." ) );
    }
    else {
      aggiorna( id.toInt() );
      modificato=true;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_sale_imposta::aggiungi_clicked() {
  QString st=tools.tastiera( ( QWidget* ) parent(), "", tr( "NOME SALA" ) );
  if ( st!="" ) {
    QString id="-1";
    if ( selezionata() ) id=sala() [p_is];
    if ( !CMD( tok_inserisci_sala, st, "" ) ) {
      tools.msgbox( this, tr( "Problemi di inserimento sala.\n\nUsare un altro nome." ) );
    }
    else {
      modificato=true;
      aggiorna( id.toInt() );
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_sale_imposta::cancella_clicked() {
  if ( tools.msgbox( tr( "Si e' sicuri di voler cancellare\nla sala e tutti i suoi tavoli?" ), tr( "Cancellazione sala" ), SINO )==SI ) {
    if ( tools.msgbox( tr( "Seconda conferma di cancellazione.\nSi e' veramente sicuri?" ), tr( "Cancellazione sala" ), SINO )==SI ) {
      QString id=sala() [p_is];
      if ( CMD( tok_cancella_sala, id ) ) {
        QString id_corrente=QSN( form_sale->sala().id );
        if ( id_corrente==id ) {
          form_sale->carica_sala( -1, false );
        }

        aggiorna( id_corrente.toInt() );
        modificato=true;
        stato_pulsanti();
      }
    }
  }
}

void frm_sale_imposta::exit_clicked() {
  reject();
}

frm_sale_imposta_tavoli::frm_sale_imposta_tavoli( frm_sale_imposta*parent, const char*name, bool modal, WFlags fl ):
   frm_ok_exit( parent, name, modal, fl ) {
     form_sale_imposta=parent;
     form_sale=parent->form_sale;
     stato->setText( tr( "Impostazione di tutti i tavoli della sala" ) );
     costruttore_comune( tp_sala );
}


frm_sale_imposta_tavoli::frm_sale_imposta_tavoli( frm_sale*parent, const char*name, bool modal, WFlags fl ):
   frm_ok_exit( parent, name, modal, fl ) {
     form_sale=parent;
     stato->setText( tr( "Impostazione dei tavoli selezionati" ) );
     if ( form_sale->multi_selezione() ) costruttore_comune( tp_tavoli );
     else
       costruttore_comune( tp_tavolo );
}


void frm_sale_imposta_tavoli::costruttore_comune( frm_sale_imposta_tavoli::tr_tipo tipo ) {

  _tipo=tipo;

  setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

  base_alza();

  nascondi_titolo();

  QString validatore=tools.validatore( "tavoli.nome_tavolo" );
  QString valore;
  if ( tipo==tp_tavolo ) valore=form_sale->selezionato()->nome();
  else
    valore="";

  NEW( nome, tr_input( area, "nome", tr( "Nome:" ), valore, validatore, true ) );
  // palette_dark(nome);

  NEW( tavoli, tr_listview( area, "tavoli" ) );
  tavoli->setMinMax( 300, 132, SMAX, 66*3 );
  // palette_dark(tavoli);

  NEW( pavimenti, tr_listview( area, "pavimenti" ) );
  pavimenti->setMinMax( 300, 132, SMAX, 66*3 );
  pavimenti->tipo( tr_listview::tipo_pavimenti );
  // palette_dark(pavimenti);

  NEW( stampanti, tr_listview( area, "stampanti" ) );
  stampanti->setMinMax( 100, config.elenchi_altezza_riga*2, SMAX, config.elenchi_altezza_riga*4 );
  stampanti->tipo( tr_listview::stampanti );
  // palette_dark(stampanti);


  QWidget*w[]= {     nome, nome, nome, nome, 0,
			 tavoli, tavoli, pavimenti,pavimenti, 0,
			 stampanti, stampanti,stampanti,stampanti,0, 0 };
  build( w );

  if ( tipo!=tp_tavolo ) {
    nascondi( nome );
    disabilita( nome );
  }
  if ( tipo!=tp_sala ) disabilita( pavimenti );
  disabilita( stampanti );

  /* ?????????????????????????????????????????????? da verificare perch�non agisce su tavoli */
  if ( tipo==tp_tavolo ) {
    disconnetti_annulla();

    connect( annulla, SIGNAL( clicked() ), this, SLOT( extra_clicked() ) );
    itm_tavolo*t=form_sale->tavoli->selezionato();
    if ( t&&!t->tway() ) {
      annulla->show();
      annulla->setText( "az_conto.png", tr( "c.rapido" ) );
      disabilita( pavimenti, stampanti, NULL );
    }
    else
      disabilita( tavoli, pavimenti, stampanti, NULL );
    tavoli->tipo( tr_listview::tipo_tavoli );
    connect( nome, SIGNAL( textChanged( const QString& ) ), this, SLOT( nome_textChanged( const QString& ) ) );
  }
  else {
    tavoli->tipo( tr_listview::tipo_tavoli );
    tavoli->escludi( "tavolo_bar.png" );
    tavoli->escludi( "tavolo_as.png" );
  }

  //QWidget *w1[] = { pavimenti, stampanti, NULL };
  //link( tavoli, w1 );

  tavoli->carica();
  pavimenti->carica();
  stampanti->carica();

  stato_pulsanti();

  disconnetti_ok();
  CONNECT( ok );

#define SC(x) connect(x, SIGNAL( clicked( QListViewItem*)), this, SLOT( x##_clicked( QListViewItem*)))
  SC( tavoli );
  SC( pavimenti );
#undef SC

  stato_pulsanti();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_sale_imposta_tavoli::~frm_sale_imposta_tavoli() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_sale_imposta_tavoli::show() {
  frm_ok_exit::show();
  if ( tavoli ) tavoli->adatta();
  if ( pavimenti ) pavimenti->adatta();
  if ( stampanti ) stampanti->adatta();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_sale_imposta_tavoli::nome_textChanged( const QString&t ) {
  tr_widget*ok=( tr_widget* ) child( "ok" );
  if ( t.isEmpty() ) {
    nascondi( ok );
    // disabilita( tavoli );
  }
  else {
    scopri( ok );
    // abilita( tavoli );
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_sale_imposta_tavoli::ok_clicked() {
  bool ok=false;
  int n;

  if ( _tipo==tp_sala&&!tavoli->selezionato().isEmpty() ) {

    if ( !CMD( tok_inizio_modifiche_sala ) ) return;

    QCanvasItemList list;
    QCanvasItemList::Iterator it;
    itm_tavolo*t=NULL;
    n=form_sale->tavoli->canvas()->allItems().size();
    list=form_sale->tavoli->canvas()->allItems();
    it=list.begin();
    t=( itm_tavolo* )*it;

    while ( t&&n ) {

      if ( t->genere()==itm_tavolo::tavolo&&!t->bar() ) {

        QString pixmap=t->pixmap();
        QString nnome=LEAVE;
        if ( nome->text()!=t->numero() ) nnome=nome->text();
        if ( !tavoli->selezionato().isEmpty() ) {
          pixmap=tavoli->id();
        }

        int id_tavolo=t->id();
        ok=CMD( tok_modifica_tavolo, QSN( id_tavolo ), nnome, LEAVE, LEAVE, LEAVE, pixmap );
        // if (!ok) break;
      }

      if ( _tipo==tp_tavolo ) t=NULL;
      if ( _tipo==tp_sala ) {
        if ( it!=list.end() ) {
          ++it;
          t=( itm_tavolo* )*it;
          n--;
        }
        else
          t=NULL;
      }
    }

    if ( !CMD( tok_fine_modifiche_sala, QSN( form_sale->sala().id ) ) ) return;

  }

  if ( _tipo==tp_tavolo ) {
    itm_tavolo*t=form_sale->tavoli->selezionato();
    QString pixmap=t->pixmap();
    QString nnome=LEAVE;
    if ( nome->text()!=t->numero() ) nnome=nome->text();
    if ( !tavoli->selezionato().isEmpty() ) {
      pixmap=tavoli->id();
    }
    int id_tavolo=t->id();
    CMD( tok_modifica_tavolo, QSN( id_tavolo ), nnome, LEAVE, LEAVE, LEAVE, pixmap );
    accept();
  }

  if ( !pavimenti->selezionato().isEmpty() ) {
    QString st=pavimenti->id();
    ok=CMD( tok_modifica_sala, form_sale_imposta->sala() [p_is], LEAVE, st );
  }

  if ( ok ) accept();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void frm_sale_imposta_tavoli::stato_pulsanti() {
  if ( !nome->isEnabled() ) {
    bool a=tavoli->selezionato().isEmpty();
    bool b=pavimenti->selezionato().isEmpty();
    if ( a||b ) scopri( ok );
    if ( !a&&!b ) nascondi( ok );
  }
}

void frm_sale_imposta_tavoli::tavoli_clicked( QListViewItem*it ) {
  if ( it&&it->text( 0 ).left( 3 ).upper()==config.bar )
    nome->setText( it->text( 0 ) );
  stato_pulsanti();
}

void frm_sale_imposta_tavoli::pavimenti_clicked( QListViewItem* ) {
  stato_pulsanti();
}

void frm_sale_imposta_tavoli::extra_clicked() {
  QString id, result;
  itm_tavolo*t=form_sale->tavoli->selezionato();
  QString on_off="off";
  if ( t->tipi_conto().find( "-1" )!=-1 ) on_off="on";
  QString on_off1="off";
  if ( t->tipi_conto().find( "-2" )!=-1 ) on_off1="on";

  QString frm_tipi_conto=TRFORM( "status=Scelta tipi conto per tavolo %nome%\n""listview=(tok:%tok%,tutti,noexit,onclick)\n"
     "label=Conto normale#cnt(chk:%on_off%);Conto<br>con resto#cntr(chk:%on_off1%)\n""buttons=ok#ok|uscita#exit\n" );
  // "links=listview|cntr" );
  FRM_PARAMS;
  FRM_PARAM( "%nome%", t->nome() );
  FRM_PARAM( "%tok%", QSN( tok_elenco_tipi_conto ) );
  FRM_PARAM( "%on_off%", on_off );
  FRM_PARAM( "%on_off1%", on_off1 );


  bFORM( scelta, frm_tipi_conto, this );
  tr_widget*ok=scelta->pointer( "ok" );
  tr_widget*uscita=scelta->pointer( "exit" );
  tr_listview*lv=scelta->lvs( "listview" );
  lv->seleziona( QStringList::split( ',', t->tipi_conto(), true ) );
  tr_label*normale=scelta->lbs( "cnt" );
  tr_label*cresto=scelta->lbs( "cntr" );
  if ( !lv->selezionati().count() ) cresto->setEnabled( false );

  bool chiudi=false;
  while ( scelta->interazione()!=uscita ) {
    if ( scelta->iterato==lv ) {
      if ( !lv->selezionati().count() ) cresto->setEnabled( false );
      else
        cresto->setEnabled( true );
    }
    if ( scelta->iterato==ok ) {
      if ( lv->selezionati().count() )
        id=lv->selezionati().join( "," );
      else
        id="";
      if ( !id.isEmpty()&&normale->isOn() ) id+=",-1";
      if ( id.isEmpty() ) id="-1";
      if ( !id.isEmpty()&&cresto->isOn() ) id+=",-2";
      CMD( tok_modifica_tavolo, QSN( t->id() ), LEAVE, LEAVE, LEAVE, LEAVE, LEAVE, id );
      chiudi=true;
      break;
    }
  }

  xFORM( scelta, frm_tipi_conto );
  if ( chiudi ) accept();

}
