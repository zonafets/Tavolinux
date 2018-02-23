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

/* HISTORY: 28-09-04 prenotazioni: correzione bachi visualizzazione e stampa
01-03-05 corretto bug flags non re-inizializzati al ri-login 09-12-05 estesi controlli input e label con radio choice e color
e font cycling 10-12-05 aggiunta maschera parametri generali configurazione
11-12-05 esteso terminale ad accettare segni di punteggiatura e comandi esterni */

#include <qaccel.h>
#include <cmath>

/*#define bFORM( variabile, struttura, padre)\
mem_check_begin(); \
frm_base *variabile; \
NEW(variabile, frm_base(padre,#struttura)); \
variabile->build(struttura)
*/

#include "frm_base.hpp"
#include "frm_main.hpp"
#include "tr_enumeratori.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "tr_globals.hpp"
#include "tr_tools.hpp"
#include "frm_scelta.hpp"
#include "frm_conto.hpp"
#include "frm_comanda.hpp"
#include "frm_bis.hpp"
#include "tavolo_bits.hpp"
#include "frm_sale.hpp"
#include "frm_cm.hpp"

#include "cmddiritti.hpp"
#include "cmdpaycard.hpp"
#include "tr_menu_dim.hpp"

extern void*devnul;
extern tr_globals glb;
extern tr_tools tools;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_main::frm_main( QWidget*parent, const char*name, bool modal, WFlags fl ):
   frm_base( parent, name, modal, fl|WType_TopLevel|WStyle_NormalBorder ) {
     titolo->setText( tr( "gestione sale" ) );

     setMinimumSize( QSize( config.finestra_larghezza, config.finestra_altezza ) );

     NEW( tavoli, tr_tavoli_view( area, "tavoli" ) );

     iniz_area( 1, 1 );
     area_layout->addWidget( tavoli, 0, 0 );

     QCanvas*canvas;
     NEW( canvas, QCanvas( tavoli, "pavimento" ) );
     canvas->setDoubleBuffering( true );

     tavoli->setCanvas( canvas );
     connect( tavoli, SIGNAL( toccato( QMouseEvent* ) ), this, SLOT( tavoli_clicked( QMouseEvent* ) ) );

     QString pulsanti_main=
        TRFORM( "ristampa#ristampa|sposta#sposta|tavolata#tavolata|conto#conto|bis#az_bis|"
        "comanda#comanda|incassa#incassa|gestione#gestione|uscita#uscita" );
     tr_label**ws[]= { &ristampa, &sposta, &tavolata, &conto, &bis, &comanda, &incassa, &gestione, &exit };

     TRDiritti::TRDirittiUtente diritti[]= {
         p_main_ristampa, p_main_sposta, p_main_tavolata, p_main_conto, p_main_bis, p_main_comanda, p_none, p_none, p_none
       };

     pulsanti( pulsanti_main, false, ws, diritti );

     CONNECT( ristampa );
     CONNECT( sposta );
     CONNECT( tavolata );
     CONNECT( conto );
     CONNECT( bis );
     CONNECT( comanda );
     CONNECT( incassa );
     CONNECT( gestione );
     CONNECT( exit );

     init();
     connect( barra_stato, SIGNAL( ico_tavolo_clicked() ), this, SLOT( ico_tavolo_clicked() ) );
     connect( barra_stato, SIGNAL( ico_sala_clicked() ), this, SLOT( ico_sala_clicked() ) );
     connect( barra_stato, SIGNAL( sala_clicked() ), this, SLOT( scelta_sala_clicked() ) );
     connect( barra_stato, SIGNAL( ico_cameriere_clicked() ), this, SLOT( ico_cameriere_clicked() ) );
     connect( barra_stato, SIGNAL( cameriere_clicked() ), this, SLOT( cameriere_clicked() ) );

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
frm_main::~frm_main() {
  srv->tavoli( NULL );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_main::init() {

  barra_stato->tipo( tr_barra_stato::stato );

  tavolata->pulsante_animato( false );
  gestione->pulsante_animato( false );

  carica_sala( config.sala_ultima_id, config.sala_ultima_vista_a_griglia );

  srv->tavoli( tavoli );

  id_utente=tr_listview::e_none;

  devnul=NULL;

  stato_pulsanti();
}

//--------------------------------------------------------------------------------------- resize_event

void frm_main::resizeEvent( QResizeEvent*ev ) {
  tavoli->canvas()->resize( ev->size().width(), ev->size().height() );
  if ( tavoli->vista_a_griglia() )
    tavoli->tavoli_a_griglia( true );
  tavoli->canvas()->update();
}

/////////////////////////////////////////////////////////////////////////

void frm_main::ristampa_clicked() {
  if ( !first_login() )
    return;

  itm_tavolo*selezionato=( tavoli->selezionato() );

  int id_tavolo;
  if ( selezionato )
    id_tavolo=selezionato->id();

  enum {
    m_ultimi, m_conto, m_comanda, m_ultimi_ieri, m_ultimi_10
  };

  TRDirittiUtente diritti[]= {
    p_main_ristampa_ultimi, p_main_ristampa, p_main_ristampa_comanda, p_main_ristampa_ultimi, p_main_ristampa_ultimi
  };

  if ( selezionato->stato()&itm_tavolo::apparecchiato ) {
    diritti[m_ultimi]=p_null;
    diritti[m_ultimi_ieri]=p_null;
    diritti[m_ultimi_10]=p_null;
  }

  QString menu=TRFORM( "ultimi#ultimi|conto#conto|comanda#comanda;""ultimi\\ieri#yesterday|10...#last_ten" );

  QString voce=tools.hvmenu( ristampa, menu, diritti );
  if ( voce.isEmpty() )
    return;

  QString msg=tr( "Attenzione!<br>Quest'operazione altera lo stato del magazzino.Si desidera proseguire?" );

  if ( voce=="conto" ) {
    if ( CMD( tok_ristampa_conto ) )
      tools.msgbox( tr( "Il conto e' stato ristampato" ) );
  }

  if ( voce=="comanda" ) {
    if ( CMD( tok_ristampa_comanda, QSN( id_tavolo ) ) )
      tools.msgbox( tr( "La comanda e' stata ristampata" ) );
  }

  if ( voce=="yesterday" ) {
    QString id_conto;
    QString descrizione;
    if ( tools.scelta( tr_listview::conti_ieri, id_conto, descrizione ) ) {
      QString riapri=SI;
      if ( config.riapri_conto_nsq==0 ) riapri="";
      if ( config.riapri_conto_nsq==2 )
        riapri=tools.msgbox( msg, SINO );
      if ( riapri==SI ) {
        CMD( tok_riapri_conto, id_conto, QSN( id_tavolo ) );
      }
    } // if
  }

  if ( voce=="last_ten" ) {
    QString id_conto;
    QString descrizione;
    if ( tools.scelta( tr_listview::conti_ultimi, id_conto, descrizione ) ) {
      QString riapri=SI;
      if ( config.riapri_conto_nsq==0 ) riapri="";
      if ( config.riapri_conto_nsq==2 )
        riapri=tools.msgbox( msg, SINO );
      if ( riapri==SI ) {
        CMD( tok_riapri_conto, id_conto, QSN( id_tavolo ) );
      }
    } // if
  }

}

/////////////////////////////////////////////////////////////////////////

void frm_main::sposta_clicked() {
  if ( !first_login() )
    return;
  QString id=QSN( tavoli->sala().id );
  QString descrizione;
  if ( tools.scelta( tr_listview::tavoli, id, descrizione ) ) {
    if ( CMD( tok_sposta_tavolo, QSN( tavoli->selezionato()->id() ), id ) ) {
      chk_tavolo( tavoli->selezionato()->id() );
    }
  }
}

//--------------------------------------------------------------------------------------- carica_sala

void frm_main::carica_sala( int id, bool /* vista */ ) {
  tavoli->carica_tavoli( id, false );
  if ( !tavoli->selezionato() ) {
    barra_stato->setTavolo( tr( "(toccare un tavolo)" ) );
    barra_stato->setSala( tavoli->sala().nome );
  }
}

enum {
  bw_w=1, bw_b=2, bw_bw=3
};

/////////////////////////////////////////////////////////////////////////
int frm_main::bw() {
  int i=bw_w;
  if ( devnul==( void* ) this ) i=bw_b;
  if ( devnul==( void* )-1 ) i=bw_bw;
  return i;
}

/*================================================================
=  Author: Stefano Zaglio                       Date: 2005-12-02 =
=  Scope:  show statistics of current room                       =
================================================================*/

void frm_main::ico_sala_clicked() {
  if ( !first_login() ) return;

  int i_bw=bw();
  denu_statistiche_tavoli_sala;
  if ( ::cmd( tok_statistiche_tavoli_sala, QSN( i_bw ), QSN( tavoli->sala().id ) ) ) {
    QString riga=srv->rcv();
    QStringList elenco=srv->rcv_list();
    srv->eop();

    QString frm_stat_sala=TRFORM( "margine=3\n""readonly=true\n""status=Statistiche sala |%nome_sala%\n"
       "label=TOTALI(bg:light);DA ULT.CH;ATTUALI;SOMMA;hole=(50)\n"
       "label=SALA;#tot_odierno(right,%col%,cc);#tot_attuale_ordinato(right,%col%,cc);#tot_in_cassa(right,%col%,cc);hole=(50)\n"
       "label=COPERTI;#tot_cop_odierno(right,%col%,cc);#tot_cop_attuale_ordinato(right,%col%,cc);#tot_coperti(right,%col%,cc);hole=(50)\n"
       "label=COP.MEDIO(dim:160|1);#cop_med_odierno(right,%col%,cc);#cop_med_attuale_ordinato(right,%col%,cc);#cop_med_globale(right,%col%,cc);hole=(50)\n"
       "listview=#tavoli_sala(small,dim:|300||300,sortable,noexit,dxsx);#tavoli_sala;#tavoli_sala;#tavoli_sala;#tavoli_sala\n"
       "buttons=colonne#cols|exit#exit" );
    FRM_PARAMS;
    FRM_PARAM( "%nome_sala%", tavoli->sala().nome );
    FRM_PARAM( "%col%", "bw" );
    bFORM( fss, frm_stat_sala, this );
    CTRL_FRM( fss );
    CTRL( tavoli_sala );
    CTRL( cols );
    CTRL( exit );

#define CAV(x) CTRL(x);x.value(riga.section('|',p_##x,p_##x))

    CAV( tot_odierno );
    CAV( tot_attuale_ordinato );
    CAV( tot_in_cassa );
    CAV( tot_cop_odierno );
    CAV( tot_cop_attuale_ordinato );
    CAV( tot_coperti );
    CAV( cop_med_odierno );
    CAV( cop_med_attuale_ordinato );
    CAV( cop_med_globale );

    QString tots="tot_odierno|tot_attuale_ordinato|tot_in_cassa|tot_cop_odierno|tot_cop_attuale_ordinato|tot_coperti|cop_med_odierno|cop_med_attuale_ordinato|cop_med_globale";
    QStringList tots_list=QStringList::split( '|', tots );
    if ( !config.stat_room_colors.isEmpty() )
      TROWSLI( tots_list ) {
        tr_label*l=fss->lbs( tots_list[i] );
        if ( l ) l->setPaletteBackgroundColor( config.stat_room_colors.section( '|', i, i ) );
      }

    QString testi=TR( "tavolo|aperto da|non servito da|cameriere|voto|numero coperti|"
       "totale tavolo|totale ultimo conto|totale ultimo conto(nf)|totale odierno|totale odierno(nf)|totale coperti|totale coperti(nf)" );
    QString testi_header=TR(
       "TAVOLO|APERTO DA|NON SERV.|CAMERIERE|VOTO|N.COP|TOT.TAV.|TOT.UC.|T.U.(NF)|TOT.OGI|T.O.(NF)|TOT.COP|T.C.(NF)" );

    QString cols_list_str="12|%12|%h12|#tcpnf|85|s\n""11|%11|%h11|#tcp|85|n\n""10|%10|%h10|#tognf|85|s\n""9|%9|%h9|#tog|85|n\n"
       "8|%8|%h8|#tucnf|85|s\n""7|%7|%h7|#tuc|85|n\n""6|%6|%h6|#tta|85|a\n""5|%5|%h5|#cop|70|a\n""4|%4|%h4|vot|110|a\n"
       "3|%3|%h3|cam|110|a\n""2|%2|%h2|#nsd|110|a\n""1|%1|%h1|#apd|120|a\n""0|%0|%h0|$tav|100|a";
    QStringList cols_list=QStringList::split( '\n', cols_list_str );
    TROWSLI( cols_list ) {
      int invi=cols_list.count()-i-1;
      QString r1="%" +QSN( invi );
      QString rp=testi.section( '|', invi, invi );
      cols_list[i]=cols_list[i].replace( r1, rp );
      r1="%h" +QSN( invi );
      rp=testi_header.section( '|', invi, invi );
      cols_list[i]=cols_list[i].replace( r1, rp );
    } // trwsli

    if ( config.stat_colonne_sala.left( 2 )!="0|" ) config.stat_colonne_sala.prepend( "0|" );

    QString header;
    QString struttura;
#define GENERA_HS \
   for (int i=cols_list.count()-1;i>=0;i--) {\
   int invi=cols_list[i].section('|',0,0).toInt();\
   if (!struttura.isEmpty()) struttura+='|';\
   bool trovato=false;\
   if (cols_list[i].section('|',5,5)=="s" && (bw()==bw_b || bw()==bw_bw)) {\
   invi--;\
   }\
   for (int j=0;j<=config.stat_colonne_sala.contains('|');j++)\
   if (config.stat_colonne_sala.section('|',j,j)==QSN(invi)) {\
   if (cols_list[i].section('|',5,5)=="a") {\
   trovato=true;\
   }\
   if (cols_list[i].section('|',5,5)=="s" && (bw()==bw_b || bw()==bw_bw)) {\
   trovato=true;\
   }\
   if (cols_list[i].section('|',5,5)=="n" && (bw()==bw_w || bw()==bw_bw)) {\
   trovato=true;\
   }\
   if (trovato) {\
   if (!header.isEmpty()) header+='|';\
   header+=cols_list[i].section('|',2,2);\
   header+='!';\
   header+=cols_list[i].section('|',4,4);\
   struttura+=cols_list[i].section('|',3,3);\
   }\
   }\
   if (!trovato)\
   {\
   struttura+='-';\
   struttura+=cols_list[i].section('|',3,3);\
   }\
   } // for i

    GENERA_HS;

    tavoli_sala.listview->header( header );
    tavoli_sala.listview->tipo_fittizio( struttura, elenco );

    while ( fss->interazione()!=exit ) {
      if ( fss->iterato==cols ) {
        QString str_cols=TRFORM( "palette=3\n""status=Scelta colonne da visualizzare\n"
           "listview=#columns(header:Colonna!300,nessuno,minr:10)\n" );
        FRM_PARAMS;
        bFORM( cols, str_cols, fss );
        CTRL_FRM( cols );
        CTRL( columns );

        columns.listview->escludi( "|||||s" );
        columns.listview->tipo_fittizio( "@id|$desc|-hd|-tipo|-dim|-bw", cols_list );
        QStringList l=QStringList::split( '|', config.stat_colonne_sala );
        columns.value( l );

        QString header;

        cols->interazione();
        config.stat_colonne_sala=columns( "|" );
        if ( config.stat_colonne_sala.left( 2 )!="0|" ) config.stat_colonne_sala.prepend( "0|" );
        config.qs.writeEntry( "/stat_colonne_sala", config.stat_colonne_sala );

        header="";
        struttura="";
        GENERA_HS;
#undef GENERA_HS

        tavoli_sala.listview->header( header );
        tavoli_sala.listview->tipo_fittizio( struttura, elenco );

        xFORM( cols, frm_stat_sale );
      } // cols

    } // while

    config.stat_room_colors="";
    TROWSLI( tots_list ) {
      tr_label*l=fss->lbs( tots_list[i] );
      if ( l ) {
        if ( !config.stat_room_colors.isEmpty() ) config.stat_room_colors+="|";
        config.stat_room_colors+=l->paletteBackgroundColor().name();
      }
    } // TROWSLI
    CFG( stat_room_colors, config.stat_room_colors );

    xFORM( fss, frm_stat_sale );
  } // if cmd stat tavoli
}

/////////////////////////////////////////////////////////////////////////

void frm_main::ico_tavolo_clicked() {
  if ( !first_login() )
    return;
}



void frm_main::scelta_sala_clicked() {
  if ( !first_login() )
    return;

  QString id=QSN( tavoli->sala().id );
  QString descrizione;

  if ( tools.scelta( tr_listview::sale, id, descrizione ) ) {
    if ( id.toInt()!=config.sala_ultima_id ) {
      CFG( sala_ultima_id, id.toInt() );
      carica_sala( config.sala_ultima_id, config.sala_ultima_vista_a_griglia );
    }

    stato_pulsanti();
  }

}


void frm_main::ico_cameriere_clicked() {
  if ( FLAGS( p_conto_stampa_devnull ) ) {
    if ( FLAGS( p_fnt ) ) {
      if ( !devnul ) devnul=this;
      else if ( devnul==( void* ) this ) devnul=( void* )-1;
      else if ( devnul==( void* )-1 ) devnul=NULL;
    }
    else {
      if ( devnul ) {
        devnul=NULL;
      }
      else {
        devnul=( void* ) this;
        tavoli->seleziona();
      }
    }
    stato_pulsanti();
  }
}

void frm_main::cameriere_clicked() {
  QString id, descrizione;
  if ( tools.scelta( tr_listview::camerieri, id, descrizione ) ) {
    logout();
    first_login( id, descrizione );
  }
}

// ------------------------------------------------------------------------------------------ tavolata_clicked

void frm_main::tavolata_clicked() {
  if ( !first_login() )
    return;

  tavolata->not_stato();

  bool annulla=false;

  if ( !tavoli->tavolata() ) {
    if ( tavoli->selezionato()->tavolata() ) {

      QString p=tools.msgbox( this, tr( "Si desidera 'sciogliere' la tavolata o 'aggregare' un tavolo?" ), tr( "Opzioni tavolata" ),
         TRFORM( "sciogliere#melt|aggregare#group|annulla#cancel" ) );

      if ( p=="melt" ) {
        tavoli->sciogli_tavolata( tavoli->selezionato()->id() );
        tavoli_clicked( NULL );
        tavolata->not_stato();
        stato_pulsanti();
        return;
      }

      if ( p=="group" )
        annulla=false;

      if ( p=="cancel" ) {
        tavolata->not_stato();
        stato_pulsanti();
        return;
      }
    }

    if ( !annulla ) {
      tavoli->tavolata( true );

      if ( tavoli->tavoli_aggregati().isEmpty() ) {
        barra_stato->setTavolo( tr( "(toccare tavolo da aggregare)" ) );
        barra_stato->setSala( tavoli->sala().nome );
      }
      else {
        barra_stato->setTavolo( tavoli->tavoli_aggregati() );
        barra_stato->setSala( tavoli->sala().nome );
      }
    }
  }

  else {
    if ( tavoli->n_tavoli_aggregati()==1 ) {
      tavoli->sciogli_tavolata( tavoli->selezionato()->id() );
    }
    annulla=true;
  }

  if ( annulla ) {
    tavoli->tavolata( false );

    tavoli_clicked( NULL );
  }

  stato_pulsanti();
}

// -----------------------------------------------------------------------

void frm_main::apri_conto() {
  itm_tavolo*t=tavoli->selezionato();

  if ( t->tway() ) {
    prenotazioni();
  }
  else {
    FORM( f, frm_conto );

    t->aggiorna();

    if ( f->apri_tavolo( tavoli->sala(), *t ) ) {
      f->exec();
    }
    f->chiudi_tavolo();

    bool scambia=f->scambia;
    xFORM( f, frm_conto );
    if ( scambia )
      comanda_clicked();
    else
      t->card.clear();
  }

  stato_pulsanti();

}

// ----------------------------------------------------------------------------


void frm_main::conto_clicked() {

  if ( !first_login() ) return;

  itm_tavolo*t=tavoli->selezionato();

  if ( tools.conto_clicked( conto, t, QStringList(), -1 ) )
    apri_conto();

  stato_pulsanti();
}

void frm_main::bis_clicked() {
  if ( !first_login() )
    return;

  nascondi( exit );

  goFORM( frm_bis );

  stato_pulsanti();
}

void frm_main::comanda_clicked() {
  if ( !first_login() )
    return;

  itm_tavolo*t=tavoli->selezionato();

  QString prenotato=LEAVE;

  if ( t->tway() ) {
    if ( ::cmd( tok_inserisci_tavolo_fittizio ) ) {
      QString id=srv->rcv();
      srv->eop();
      t->set( tr::tavolo_esteso::p_id, id );
      int iid=t->id();
      prenotato=prenota( this, t, QString::null, true );
    }
  }

  if ( !prenotato.isNull() ) {
    FORM( f, frm_comanda );

    if ( f->apri_tavolo( tavoli->sala(), *t ) ) {
      f->exec();
      cout << __FUNCTION__<<":"<<__LINE__<<endl;
    }
    cout << __FUNCTION__<<":"<<__LINE__<<endl;
    t->aggiorna();
    cout << __FUNCTION__<<":"<<__LINE__<<endl;
    f->chiudi_tavolo();
    cout << __FUNCTION__<<":"<<__LINE__<<endl;
    bool scambia=f->scambia;
    if ( f->comanda->registrata()&&( prenotato!=LEAVE ) ) {
      prenotato.prepend( '1' ); // take-away
      CMD( tok_prenota, prenotato );
    }
    xFORM( f, frm_comanda );
    if ( scambia )
      apri_conto();
    else
      t->card.clear();

    stato_pulsanti();
  }

}

// ------------------------------------------------------------------------------------ incassa_clicked

void frm_main::incassa_clicked() {

  if ( !first_login() )
    return;

  if ( ::cmd( tok_incassa_tavolo, QSN( tavoli->selezionato()->id() ) ) ) {
    QString st=srv->rcv();
    srv->eop();
    tavoli->selezionato()->set( tr::tavolo_esteso::p_stato, st );
    tavoli->selezionato()->aggiorna();
    nascondi( incassa );
  }

  stato_pulsanti();

}

// --------------------------------------------------------------------------------------------- gestuibe_clicked

void frm_main::gestione_clicked() {

  if ( !first_login() )
    return;

  bool selezionato=( tavoli->selezionato() );

  int id_tavolo;
  if ( selezionato )
    id_tavolo=tavoli->selezionato()->id();

  enum {
    m_prenotazioni=3
  };

  QString etichette=TRFORM( "gestione#gestione|extra#extra|sblocca#sblocca|"
     "prenotazioni#prenotazioni|chiusura#chiusura|totali#totals|svuota#svuota;FFM#ffm;"
     "totals\\totali#totali|stato sale#stato_sale|voti#voti|turni#turni;"
     "gestione\\tessere#tessere|fornit.#fornit_|magaz.#magaz_|sala#sala|menu#menu|clienti#clienti;"
     "extra\\terminale#terminale|ricarica#ricarica|log.com#log.com|azz.cont.#azz_cont""|test#test" );

  TRDiritti::TRDirittiUtente diritti[]= {
   p_none, p_none,  p_main_gestione_sblocca,
	 p_main_gestione_prenotazioni, p_main_gestione_chiusura, p_main_gestione_totali, p_main_gestione_svuota,p_main_gestione_ffm,
	 p_main_gestione_totali,p_main_gestione_sale,p_none,p_none,
	 p_main_gestione_tessere, p_main_gestione_fornitori, p_main_gestione_magazzino, p_main_gestione_sala, p_main_gestione_menu, p_main_gestione_clienti,
	 p_main_gestione_terminale, p_none , p_main_gestione_log_com, p_none,
         p_develop // comto
  };

  if ( !tavoli->selezionato() ) {
    TROWI( diritti ) {
      if ( ( diritti[i]==p_main_gestione_svuota&&!devnul )||diritti[i]==p_main_gestione_sblocca )
        diritti[i]=p_null;
    }
  } // if !tavoli->sel


  QString voce=tools.hvmenu( gestione, etichette, diritti );
  if ( voce.isEmpty() )
    return;

  if ( voce=="sala" ) {
    goFORM( frm_sale );
    selezionato=( tavoli->selezionato() );
  }

  if ( voce=="voti" ) frm_voti();
  if ( voce=="turni" ) frm_turni();

  if ( voce=="stato_sale" ) {
    stato_sale();
  }

  if ( voce=="menu" ) {
    goFORM( frm_cm );
    if ( glb.cache_menu_listino.count() )
      glb.cache_menu_listino.clear();

  }

  if ( voce=="fornit_" )
    frm_fornitori();

  if ( voce=="magaz_" )
    frm_fornitori( true );

  if ( voce=="tessere" )
    frm_tessere();

  if ( voce=="clienti" ) {
    QString id;
    QString descrizione;
    QString niente="";
    tools.scelta( tr_listview::clienti_rapidi, id, descrizione, niente, QString::null, false );
  }

  if ( voce=="svuota" ) {
    if ( devnul ) {
      bool fai=false;
      if ( config.pulisci_nsq==1 ) fai=true;
      if ( config.pulisci_nsq==2 )
        if ( tools.msgbox( tr( "Si e' sicuri di voler VUOTARE il tavolo?" ), SINO )==SI )
          fai=true;
      if ( fai ) {
        CMD( tok_pulisci );
        devnul=NULL;
      }
    }
    else {
      QString fai="";
      if ( config.chiedi_su_svuota )
        fai=tools.msgbox( tr( "Si e' sicuri di voler SVUOTARE il tavolo?" ), SINO );
      if ( fai==SI ) {
        CMD( tok_svuota_tavolo, QSN( tavoli->selezionato()->id() ) );
        tavoli->selezionato()->aggiorna();
      }
    }
  }

  if ( voce=="sblocca" ) {
    CMD( tok_sblocca_tavolo, QSN( tavoli->selezionato()->id() ) );
  }

  if ( voce=="prenotazioni" ) {
    prenotazioni();
  }

  if ( voce=="chiusura" ) {
    if ( tools.msgbox( tr( "Si vuole eseguire la " )+config.chiusura_testo+tr( " ?" ), tr( "Chiusura" ), SINO )==SI ) {
      srv->silent=true; // disabilita la gestione d'errore integrata
      if ( ::cmd( tok_chiusura_fiscale, QString( "0" ) ) ) {
        srv->silent=false; // riabilita la gestione d'errore

        QString msg=srv->last_rcv().mid( 3 );

        if ( srv->last_rcv().left( 3 )!=config.cmd_ok.left( 3 ) ) {
          if ( tools.msgbox( msg+tr( "\n\nSi vuole forzare la chiusura fiscale?" ), tr( "Chiusura fiscale" ), SINO )==SI ) {
            CMD( tok_chiusura_fiscale, QString( "1" ) );
          }
          else
            srv->eop();
        }
        else
          srv->eop();
      }
      else {
        srv->silent=false;
        srv->eop();
      }
    }
  }

  if ( voce=="terminale" ) {
    QString st=tools.tastiera( this, tr( "Comando da mandare al server comande" ), "" );
    if ( st!="" ) {
      cout<<"----- inizio sezione comando in manuale -------------------------------\n";
      srv->cmd( st );
      tools.wait( 1500 );
      srv->eop();
      cout<<"----- fine sezione comando in manuale -------------------------------\n";
    }
  }

  if ( voce=="totali" ) {
    QProcess p( this );
    QString modulo=config.exe_path+"../bin/chiusure";
    cout<<tr( "Eseguo il modulo:" )<<modulo<<endl;
    p.addArgument( modulo );
    if ( p.start() ) {
      hide();
      while ( p.isRunning() );
      show();
    }

    else {
      tools.msgbox( tr( "Attenzione. Il modulo CHIUSURE non e' stato ""avviato per uno dei seguenti motivi:<br>"
         "* modulo assente<br>""* errore interno<br>""Avvertire assistenza" ) );

    } // else
  }

  if ( voce=="ricarica" ) {
    if ( tools.msgbox( tr( "Si e' sicuri di voler ricaricare i terminali?" ), SINO )==SI )
      CMD( tok_ricarica_terminali );
  }

  if ( voce=="log.com" ) {
    QString id;
    QString desc;
    id=tr_listview::e_last;
    tools.scelta( tr_listview::log_com, id, desc );
  }

  if ( voce=="test" ) tests();

  if ( voce=="ffm" ) frm_ffm();

  if ( voce=="azz_cont" ) {
    if ( tools.msgbox( TR( "Si desidera veramente azzerare i contatori per ricevute, fatture, etc.?" ), SINO )==SI ) {
      if ( tools.msgbox( TR( "Si desidera mantenere i contatori?" ), TR( "Conferma invertita" ), SINO )==NO )
        CMD( tok_azzera_contatori );
    }
  }

  stato_pulsanti();

}



// --------------------------------------------------------------------------------------------- exit_clicked

void frm_main::exit_clicked() {
  if ( !first_login() )
    return;

  if ( solo_logout )
    logout();
  else {
    QString p=tools.msgbox( this, tr( "Si e' sicuri di voler uscire dal sistema?" ), tr( "Avvertenza" ),
       TRFORM( "SI#yes|CAMBIA<br>CAMERIERE#change|NO#no" ) );


    if ( p=="yes" )
      close();

    if ( p=="change" ) {
      logout();
      first_login();
    }

  } // else

}



// --------------------------------------------------------------------------------------------- chk_tavolo


int frm_main::chk_tavolo( int id ) {
  int r=0;

  if ( ::cmd( tok_controlla_tavolo, QSN( id ) ) ) {
    itm_tavolo*t=tavoli->selezionato();
    if ( t->tway() ) {
      t->set( tr::tavolo_esteso::p_stato, "0" );
      t->aggiorna();
      barra_stato->setCliente( "" );
      return r;
    }

    QString st=srv->rcv();
    QStringList e=QStringList::split( '|', st, true );

    if ( !config.format_table_state.isEmpty() ) {
      if ( e[1].find( '=' )!=-1 ) {
        QString format=config.format_table_state;
        if ( ( t->stato()&tr_tavolo::apparecchiato )!=tr_tavolo::apparecchiato )
          format.replace( "%tot_tav%", "(%tot_ult_f%)" );
        barra_stato->cliente->setSplit( format, e[1] );
      }
      else
        barra_stato->setCliente( e[1] );
    }
    else
      barra_stato->setCliente( e[1] );

    r=e[0].toInt();
    if ( r!=t->stato() ) {
      t->set( tr::tavolo_esteso::p_stato, e[0] );
      t->aggiorna();
    }

    int old=t->bis;
    t->bis=1;
    t->azzera_biss();
    t->biss[0]=r;
    QStringList biss=srv->rcv_list();
    srv->eop();

    TROWSLI( biss ) {
      QString st=biss[i];
      int id=st.section( config.cmd_sep, 0, 0 ).toInt();
      int bis=id-int( id/10.0 )*10;
      t->bis|=( 1<<bis );
      int stato=st.section( config.cmd_sep, 1, 1 ).toInt();
      t->biss[bis]=stato;
    }
    if ( old!=t->bis ) {
      if ( t->bis>1 )
        t->set( tr_tavolo::p_stato, QSN( t->stato()|itm_tavolo::con_bis ) );
      t->aggiorna();
    }
  }

  return r;
}



// --------------------------------------------------------------------------------------------- tavoli_clicked

void frm_main::tavoli_clicked( QMouseEvent* ) {
  if ( first_login() ) {
    stato_pulsanti();
    card.clear();
  }
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void frm_main::logout() {

  if ( tools.actWin()&&!qstrcmp( tools.actWin()->name(), "tr_hvmenu" ) ) {
    tools.actWin()->close();
    QTimer::singleShot( 50, this, SLOT( logout() ) );

  }
  if ( ( frm_main* ) tools.actWin()!=this )
    return;

  id_utente=tr_listview::e_none;
  ::cmd( tok_logout );
  if ( config.turns_ask_NoStartLogin==2 ) {
    glb.stato[tr_globals::p_turno_id]="";
    glb.stato[tr_globals::p_turno_descrizione]="";
    stato->cameriere->setSplit( "%c%;%t%", "c=;t=" );
  }
  else
    stato->setCameriere( "" );

  glb.stato[tr_globals::p_utente_id]="";
  glb.stato[tr_globals::p_utente_descrizione]="";
  glb.init_flags();
  stato_pulsanti();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool frm_main::first_login( QString id, QString _descrizione, bool da_tessera ) {
  if ( id_utente!=tr_listview::e_none ) {
    return true;
  }

  QString descrizione="";
  if ( glb.stato[tr_globals::p_turno_id].isEmpty()&&config.turns_ask_NoStartLogin==2 ) {
    if ( !tools.scelta( tr_listview::turni, glb.stato[tr_globals::p_turno_id], glb.stato[tr_globals::p_turno_descrizione] ) ) {
      glb.stato[tr_globals::p_turno_id]="";
      glb.stato[tr_globals::p_turno_descrizione]="";
      return false;
    }
  }

  bool r=false;
  if ( id.isNull() ) {
    if ( !tools.scelta( tr_listview::camerieri, id_utente, descrizione ) ) {
      if ( config.turns_ask_NoStartLogin==2 ) {
        glb.stato[tr_globals::p_turno_id]="";
        glb.stato[tr_globals::p_turno_descrizione]="";
      }
      return false;
    }
  }
  else {
    id_utente=id;
    descrizione=_descrizione;
  }

  glb.init_flags();

  QString pwd=LEAVE;
  if ( !da_tessera&&( config.tipo_protocollo=="0" ||config.tipo_protocollo=="3" ) ) {
    pwd=tools.input_descrizione( this, tr( "IMMETTERE PAROLA DI ACCESSO" ), "", true );
    if ( !pwd.isEmpty() )
      r=::cmd( tok_login, config.tipo_terminale, config.tipo_protocollo, id_utente, pwd );
    else
      id_utente=tr_listview::e_none;
  }
  else {
    if ( da_tessera )
      r=::cmd( tok_login, config.tipo_terminale, "1", id_utente, pwd );
    else
      r=::cmd( tok_login, config.tipo_terminale, config.tipo_protocollo, id_utente, pwd );
  }

  if ( r ) {
    QString st=srv->rcv();
    glb.flags=st.section( '|', 0, 0 );
    if ( descrizione.isNull() )
      descrizione=st.section( '|', 1, 1 );

    if ( !MODULO( modulo_prenotazioni ) )
      glb.flags[p_main_gestione_prenotazioni-1]='0';
    if ( !MODULO( modulo_barcode ) )
      glb.flags[p_comanda_codice_barre-1]='0';

    glb.stato[tr_globals::p_utente_id]=id_utente;
    glb.stato[tr_globals::p_utente_descrizione]=descrizione;
    if ( glb.stato[tr_globals::p_turno_id].isEmpty() )
      stato->setCameriere( descrizione );
    else
      stato->cameriere->setSplit( "%c%;%t%", "c=" +descrizione+";t=" +glb.stato[tr_globals::p_turno_descrizione] );

    srv->eop();

  }
  else // r
       id_utente=tr_listview::e_none;

  if ( id_utente!=tr_listview::e_none ) {

    r=true;

    if ( FLAGS( p_solo_bar ) ) {
      glb.modo_bar=true;

      int id_bar=-1;

      QCanvasItemList list=tavoli->canvas()->allItems();
      QCanvasItemList::Iterator it=list.begin();
      for ( ; it!=list.end(); ++it ) {
        itm_tavolo*t=( itm_tavolo* ) ( *it );
        if ( t->bar()&&!t->bs()&&( t->numero()==config.bar_stazione||config.bar_stazione.isEmpty() ) ) {
          id_bar=t->id();
          break;
        } // if
      } // for

      if ( id_bar!=-1 ) {

        tavoli->seleziona( id_bar );

        r=true;
        while ( r ) {

          FORM( f, frm_comanda );
          itm_tavolo*t=tavoli->selezionato();
          if ( !f->apri_tavolo( tavoli->sala(), *t ) ) {
            r=false;
          }
          else if ( !f->accepted() )
            r=false;

          f->chiudi_tavolo();
          bool scambia=f->scambia;

          xFORM( f, frm_comanda );

          if ( r&&scambia ) {
            FORM( f, frm_conto );
            if ( f->apri_tavolo( tavoli->sala(), *tavoli->selezionato() ) )
              f->exec();
            f->chiudi_tavolo();
            xFORM( f, frm_conto );
          }

        }

      }
      else {
        tools.msgbox( tr( "Abilitato solo ad uso bar ma nessun bar presente in sala" ) );
      }

      logout();
      glb.modo_bar=false;
      r=false;

    }
  }

  if ( r )
    stato_pulsanti();
  else {
    if ( config.turns_ask_NoStartLogin==2 ) {
      glb.stato[tr_globals::p_turno_id]="";
      glb.stato[tr_globals::p_turno_descrizione]="";
    }
  }
  return r;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_main::stato_pulsanti() {

  enum {
    p_ristampa, p_sposta, p_tavolata, p_conto, p_bis, p_comanda, p_incassa, p_gestione, p_exit
  };

  bool stato[]= { true, true, true, true, true, true, true, true, true };

  tr_label*ps[]= { ristampa, sposta, tavolata, conto, bis, comanda, incassa, gestione, exit };

  if ( tavoli->tavolata() ) {
    barra_stato->setTavolo( tavoli->tavoli_aggregati() );
    barra_stato->setSala( tavoli->sala().nome );
    stato[p_ristampa]=false;
    stato[p_sposta]=false;
    stato[p_conto]=false;
    stato[p_bis]=false;
    stato[p_exit]=false;
    stato[p_gestione]=false;
    stato[p_comanda]=false;
    stato[p_incassa]=false;
  }

  if ( !devnul )
    barra_stato->ico_cameriere->setPixmap( PIXMAP( "stato_cameriere.png" ) );
  if ( devnul==this )
    barra_stato->ico_cameriere->setPixmap( PIXMAP( "stato_cameriere_black.png" ) );
  if ( devnul==( void* )-1 )
    barra_stato->ico_cameriere->setPixmap( PIXMAP( "stato_cameriere_red.png" ) );

  itm_tavolo*t=tavoli->selezionato();
  if ( t ) {
    int r=chk_tavolo( t->id() );
    if ( t->tavolata() ) {
      barra_stato->setTavolo( tavoli->tavoli_aggregati() );
      barra_stato->setSala( tavoli->sala().nome );
    }
    else {
      barra_stato->setTavolo( t->numero() );
      barra_stato->setSala( tavoli->sala().nome );
    }
    if ( t->bar() ) {
      stato[p_tavolata]=false;
      stato[p_bis]=false;
    }

    if ( !( t->tipo()&T_TAVOLO_CON_BIS ) )
      stato[p_bis]=false;

    if ( t->tavolata()&&( r&( 1<<TAVOLO_CONTO_DIVISO ) ) )
      stato[p_sposta]=false;

    if ( !( r&( 1<<TAVOLO_OCCUPATO ) ) ) {
      stato[p_sposta]=false;
      if ( !t||!t->tway() )
        stato[p_conto]=false;
    }
    if ( !( r&( 1<<TAVOLO_CONTO ) ) )
      stato[p_incassa]=false;
    if ( r&( 1<<TAVOLO_CONTO_DIVISO ) )
      stato[p_comanda]=false;

    if ( t->bis>1 )
      stato[p_tavolata]=false;

  }
  else {
    barra_stato->setTavolo( tr( "(toccare un tavolo)" ) );
    barra_stato->setSala( tavoli->sala().nome );
    barra_stato->setCliente( "" );
  }

  if ( id_utente==tr_listview::e_none||!t ) {
    stato[p_ristampa]=false;
    stato[p_sposta]=false;
    stato[p_tavolata]=false;
    stato[p_conto]=false;
    stato[p_comanda]=false;
    stato[p_incassa]=false;
    stato[p_bis]=false;
  }

  if ( !FLAGS( p_exit ) ) { // solo logout
    solo_logout=true;
  }
  else {
    solo_logout=false;
    exit->cambia_testo( tr( "uscita" ) );
  }

  if ( !FLAGS( p_main_ristampa )&&!FLAGS( p_main_ristampa_ultimi ) )
    stato[p_ristampa]=false;
  if ( !FLAGS( p_main_sposta ) )
    stato[p_sposta]=false;
  if ( !FLAGS( p_main_tavolata ) )
    stato[p_tavolata]=false;
  if ( !FLAGS( p_main_conto ) )
    stato[p_conto]=false;
  if ( !FLAGS( p_main_bis ) )
    stato[p_bis]=false;
  if ( !FLAGS( p_main_comanda ) )
    stato[p_comanda]=false;

  if ( !FLAGS( p_main_gestione_svuota )&&!FLAGS( p_main_gestione_totali )&&!FLAGS( p_main_gestione_chiusura )&&
     !FLAGS( p_main_gestione_prenotazioni )&&!FLAGS( p_main_gestione_sblocca )&&!FLAGS( p_main_gestione_terminale )&&
     !FLAGS( p_main_gestione_sala )&&!FLAGS( p_main_gestione_menu )&&!FLAGS( p_main_gestione_clienti ) )
       stato[p_gestione]=false;

  TROWI( stato )( stato[i]?scopri( ps[i] ):nascondi( ps[i] ) );

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool frm_main::eventFilter( QObject*o, QEvent*e ) {

  if ( e->type()==QEvent::MouseButtonPress ) {
    QString st=o->name();
    while ( o->parent()!=NULL ) {
      o=o->parent();
      st.prepend( "." );
      st.prepend( o->name() );
    }

    tools.log_event_buffer<<st;
    if ( tools.log_event_buffer.count()>tools.log_event_buffer_maxlines )
      tools.log_event_buffer.pop_front();
  }
  // return frm_base::eventFilter(o,e);  // NO BUONO
  return false;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void frm_main::prenotazioni() {

  denu_elenco_prenotazioni;

  itm_tavolo*t=tavoli->selezionato();

  bool fittizi=false;
  if ( t&&t->tway() ) {
    t=NULL;
    fittizi=true;
  }

  QString frm_prenotazioni=
     TRFORM( "minmax=%95|%90\n""margine=0\n""status=Prenotazioni%xtav%\n"
     "listview=(header:Nome|Data/Ora|Note|#,noexit,onclick,small,hl:""%stato% eq 5?#550000,tok:%tok%%tokp%)\n"
     "buttons=aggiungi#aggiungi|dettaglio#dettaglio|comanda#comanda|conto#conto|"
     "stampa#stampa|cancella#cancella|uscita#uscita\n""links=listview|dettaglio|cancella|comanda|conto" );

  FRM_PARAMS;
  FRM_PARAM( "%stato%", QSN( ( enu_elenco_prenotazioni )p_stato ) );
  FRM_PARAM( "%tok%", QSN( tok_elenco_prenotazioni ) );

  QString tokp;
  if ( fittizi )
    tokp="|1";
  else
    tokp="|2";
  tokp+="|||";
  if ( t ) {
    tokp+=QSN( t->id() );
    QString xtav=tr( " per tavolo |" )+t->numero();
    frm_prenotazioni.replace( "%xtav%", xtav );
  }
  else {
    frm_prenotazioni.remove( "%xtav%" );
  }

  frm_prenotazioni.replace( "%tokp%", tokp );

  bFORM( f, frm_prenotazioni, this );

  tr_listview*elenco=f->lvs( "listview" );
  tr_label*aggiungi=f->lbs( "aggiungi" );
  tr_label*dettaglio=f->lbs( "dettaglio" );
  tr_label*comanda=f->lbs( "comanda" );
  tr_label*conto=f->lbs( "conto" );
  tr_label*stampa=f->lbs( "stampa" );
  tr_label*cancella=f->lbs( "cancella" );
  tr_label*pulisci=NULL;
  tr_label*exit=f->lbs( "uscita" );


  if ( fittizi )
    f->nascondi( aggiungi );
  f->nascondi( comanda, conto );

  tr_widget*iterato=f->interazione();

  while ( iterato!=NULL ) {

    if ( iterato==elenco ) {
      f->nascondi( comanda, conto );

      enum {
        p_comanda, p_conto
      };

      bool stati[]= { false, false };
      if ( !elenco->selezionato().isEmpty() ) {
        QString riga=elenco->record();
        QStringList dati=QStringList::split( '|', riga, true );
        if ( dati[p_id_tavolo]!="0" &&dati[p_id_tavolo]!="" )
          stati[p_conto]=true;
        if ( dati[p_tipo]!="2" ) {
          stati[p_conto]=true;
          if ( dati[( enu_elenco_prenotazioni )p_stato]!="5" )
            stati[p_comanda]=true;
        }
      }
      if ( stati[p_comanda] )
        f->scopri( comanda );
      if ( stati[p_conto] )
        f->scopri( conto );
    }

    if ( iterato==dettaglio /* || iterato==modifica */ ) {
      QString prenotato=prenota( f, t?t:NULL, elenco->record() );
    }

    if ( iterato==aggiungi /* || iterato==modifica */ ) {
      QString prenotato=prenota( f, t?t:NULL );
      if ( !prenotato.isNull() ) {
        prenotato.prepend( '2' ); // tavolo
        if ( CMD( tok_prenota, prenotato ) )
          elenco->carica();
        // elenco->aggiorna();
      }
    }

    if ( iterato==cancella ) {
      if ( tools.msgbox( tr( "Si e' veramente sicuri di voler cancellare la prenotazione?" ), SINO )==SI ) {
        CMD( tok_cancella_prenotazione, elenco->id() );
        elenco->aggiorna();
      }
      if ( elenco->selezionato().isEmpty() )
        f->nascondi( comanda, conto );
    }

    if ( iterato==comanda||iterato==conto ) {
      QString riga=elenco->record();
      QStringList dati=QStringList::split( '|', riga, true );

      itm_tavolo tway;
      itm_tavolo*tt;
      if ( !t ) {
        tway.set( tr::tavolo_esteso::p_nome, "TWAY" );
        tway.set( tr::tavolo_esteso::p_id, dati[p_id_tavolo] );
        tway.set( tr::tavolo_esteso::p_tipo, QSN( T_TAVOLO_TAKEAWAY ) );
        tt=&tway;
      }
      else
        tt=t;

      bool scambia=false;
      if ( iterato==comanda ) {
        FORM3( f, fcmd, frm_comanda );
        if ( fcmd->apri_tavolo( tavoli->sala(), *tt ) )
          fcmd->exec();
        fcmd->chiudi_tavolo();
        scambia=fcmd->scambia;
        xFORM( fcmd, frm_comanda );
      }
      if ( iterato==conto||scambia ) {
        FORM3( f, fcnt, frm_conto );
        if ( fcnt->apri_tavolo( tavoli->sala(), *tt ) )
          fcnt->exec();
        fcnt->chiudi_tavolo();
        xFORM( fcnt, frm_conto );
      }

      elenco->aggiorna();
    }


    TRTokens tok=tok_error;
    if ( iterato==stampa ) {
      if ( !elenco->id().isEmpty() ) {
        CMD( tok_stampa_prenotazione, elenco->id() );
      }
      else {

        QString frm_prenotazioni_data_a_data=
           TRFORM( "status=Seleziona intervallo date\n""input=Da data#da_data(ggmmaaaa);A data#a_data(ggmmaaaa)\n"
           "buttons=ok#ok|uscita#uscita\n""links=da_data|ok" );

        QString p;

        bFORM( fpd, frm_prenotazioni_data_a_data, f );

        CTRL_FRM( fpd );
        CTRL( da_data );
        CTRL( a_data );
        CTRL( ok );
        CTRL( uscita );

        tr_widget*iterato=fpd->interazione();

        while ( iterato!=NULL ) {

          if ( iterato==ok ) {

            p=QString( "%1|%2|%3" ).arg( fittizi?"1" :
               "2" ).arg( tools.data_dataiso( da_data() ) ).arg( tools.data_dataiso( a_data() ) );

            iterato=NULL;
          }

          if ( iterato==uscita )
            iterato=NULL;

          if ( iterato )
            iterato=fpd->interazione();
        }
        if ( !p.isEmpty() )
          CMD( tok_stampa_prenotazioni, p );

        xFORM( fpd, frm_prenotazioni_data_a_data );

      }
    }


    if ( iterato==exit )
      iterato=NULL;

    if ( iterato )
      iterato=f->interazione();

  } // while

  xFORM( f, frm_prenotazioni );


}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString frm_main::prenota( frm_base*f, itm_tavolo*tavolo, QString dettaglio, bool per_asporto ) {

  denu_elenco_prenotazioni;

  QString risult=QSNull;
  QString frm_prenotazioni_aggiungi=
     TRFORM( "palette=3\n""?asp:status=Dati per Asporto\n""!asp:status.notr=%atav%\n"
     "input=Nome#nome(prenotazioni.nome_cliente,alato)\n""#=input=Nome#nome(alato)\n"
     "input=Data#data(ggmmaaaa,today)(gg/mm/aaaa);Ora#ora(hhmm)(hh:mm);hole=(50)\n"
     "?asp:input=Cartoni#coperti(prenotazioni.coperti);Preavviso#preavviso(prenotazioni.preavviso)""(min.);hole=(50)\n"
     "!asp:input=Coperti#coperti(prenotazioni.coperti);Preavviso#preavviso(prenotazioni.preavviso)"
     "(min.);Libera dopo#libera dopo(chk:off,prenotazioni.libera_dopo)(min.)\n""input=Note#note(prenotazioni.note)\n"
     "buttons=ok#ok|uscita#uscita\n""links=nome|coperti|data|ora|preavviso|libera dopo|note|ok" );

  FRM_PARAMS;
  if ( per_asporto ) frm_prenotazioni_aggiungi.prepend( "conds=asp\n" );

  QString atav;
  if ( tavolo )
    atav=tr( " al tavolo |" )+tavolo->numero();
  else
    atav="";

  if ( !dettaglio.isEmpty() ) {
    frm_prenotazioni_aggiungi.prepend( "readonly=true\n" );
    atav.prepend( tr( "Dettaglio Prenotazione" ) );
  }
  else
    atav.prepend( tr( "Aggiungi Prenotazione" ) );

  FRM_PARAM( "%atav%", atav );

  bFORM( fpa, frm_prenotazioni_aggiungi, f );
  QString campi="nome|data|ora|coperti|preavviso|libera dopo|note";

  if ( !dettaglio.isEmpty() ) {
    QStringList e=QStringList::split( '|', dettaglio, true );
    QString valori=e[p_nome_cliente]+'|' +e[p_data_ora].section( ' ', 0, 0 )+'|' +e[p_data_ora].section( ' ', 1, 1 )+'|' +
       e[p_coperti]+'|' +e[p_preavviso]+'|' +e[p_libera_dopo]+'|' +e[p_note];
    fpa->sets( campi, valori );
  }

  tr_widget*iterato=fpa->interazione();

  while ( iterato ) {
    if ( iterato==fpa->lbs( "ok" ) ) {
      enum {
        p_nome, p_data, p_ora, p_coperti, p_preavviso, p_libera_dopo, p_note
      };
      QString risults=fpa->gets( campi );
      QStringList rs=QStringList::split( '|', risults, true );
      if ( rs[p_ora].isEmpty()&&!per_asporto )
        tools.msgbox( tr( "L'ora non e' stata specificata" ) );
      if ( !rs[p_ora].isEmpty()||per_asporto ) {
        QString ps="|";
        if ( tavolo )
          ps+=QSN( tavolo->id() );
        ps+='|';
        ps+=rs[p_nome];
        ps+='|';
        ps+=tools.data_dataiso( rs[p_data]+"T" +rs[p_ora], true );
        ps+='|';
        ps+=rs[p_preavviso];
        ps+='|';
        ps+=rs[p_note];
        ps+='|';
        ps+=rs[p_coperti];
        ps+='|';
        ps+=rs[p_libera_dopo];
        risult=ps;
        iterato=NULL;
      }
    }
    else
      iterato=NULL;
    if ( iterato )
      iterato=fpa->interazione();
  } // while iterato

  xFORM( fpa, frm_prenotazioni_aggiungi );

  return risult;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_main::frm_fornitori( bool con_magazzino ) {

  QString str_suppliers=TRFORM(
     ( "minmax=80%|90%\n""status=Gestione |Fornitori\n""listview=#supplier(tok:%tok%,abc)\n"
     "buttons=aggiungi#aggiungi|modifica#modifica|cancella#cancella|""carico#load|giacenze#lying|""uscita#exit\n"
     "links=supplier|modifica|cancella" ) );

  FRM_PARAMS;
  FRM_PARAM( "%tok%", QSN( tok_elenco_fornitori ) );


  bFORM( suppliers, str_suppliers, this );
  CTRL_FRM( suppliers );
  CTRL( supplier );
  CTRL( aggiungi );
  CTRL( modifica );
  CTRL( cancella );
  CTRL( load );
  CTRL( lying );
  CTRL( exit );

  tr_widget*iterato=suppliers->interazione();

  while ( iterato!=NULL ) {

    bool modificato=false;

    // -------------------------------------------------------------
    if ( iterato==aggiungi||iterato==modifica ) {

      QString id="";
      if ( iterato==modifica )
        id=supplier();

      QString str_supplier=TRFORM(
         ( "margine=6;spazio=3\n""minmax=95%|95%\n""status=%title%\n"
         "input=Codice#code(anagrafica_fornitori.id_fornitore);Ragione Sociale#name(anagrafica_fornitori.ragione_sociale);name;"
         "listview=#pagam(tp:%tp%,dim:190,nobar)\n"
         "input=P.IVA#piva(anagrafica_fornitori.p_iva);Codice Fiscale#cfis(anagrafica_fornitori.c_f);cfis;listview=#pagam\n"
         "input=Via#via(anagrafica_fornitori.via);via;via;listview=#pagam\n"
         "input=CAP#cap(anagrafica_fornitori.cap);Citta'#citta(anagrafica_fornitori.citta);citta;listview=#pagam\n"
         "input=Telefono#tel(anagrafica_fornitori.telefono);FAX#fax(anagrafica_fornitori.fax);hole=0;listview=#pagam\n"
         "input=Data ult.fatt.#datauf(readonly);hole=100;hole=100;listview=#pagam\n""buttons=ok#ok|uscita#exit\n"
         "links=code|name|pagam|piva|cfis|via|cap|citta|tel|fax|datauf" ) );

      FRM_PARAMS;
      FRM_PARAM( "%title%", tr( "Aggiunta/Modifica Fornitore" ) );
      FRM_PARAM( "%tp%", QSN( tr_listview::selezione_pagamenti ) );

      bFORM( supplier, str_supplier, suppliers );
      CTRL_FRM( supplier );
      CTRL( code );
      CTRL( name );
      CTRL( piva );
      CTRL( cfis );
      CTRL( via );
      CTRL( cap );
      CTRL( citta );
      CTRL( tel );
      CTRL( fax );
      CTRL( datauf );
      CTRL( pagam );
      CTRL( ok );
      CTRL( exit );

      if ( iterato==modifica ) {
        ::cmd( tok_elenco_fornitori_esteso, id );
        QString valori=srv->rcv();
        srv->eop();
        supplier->sets( "|code|name|via|citta|cap|tel|fax|piva|cfis|||datauf", valori );
        QStringList pagamenti=QStringList::split( ',', valori.section( '|', 13, 13 ) );
        pagam.values( pagamenti );
      }

      tr_widget*iterato=supplier->interazione();

      while ( iterato!=NULL ) {

        if ( iterato==ok ) {
          QString valori=supplier->gets( "code|name|via|citta|cap|tel|fax|piva|cfis|" );
          valori+=pagam( "," );
          if ( !id.isEmpty() ) {
            valori=id+"|" +valori;
            modificato=CMD( tok_modifica_fornitore, valori );
          }
          else
            modificato=CMD( tok_inserisci_fornitore, valori );
          iterato=NULL;
        }

        if ( iterato==exit )
          iterato=NULL;

        if ( iterato )
          iterato=supplier->interazione();

      }

      xFORM( supplier, str_supplier );

    }

    // -------------------------------------------------------------
    if ( iterato==cancella )
      modificato=CMD( tok_cancella_fornitore, supplier() );

    // -------------------------------------------------------------
    if ( iterato==load ) {
      QString str_docs=TRFORM(
         ( "minmax=95%|95%\n""status=Elenco documenti fornitore |%fornitore%\n"
         "listview=#docs(header:CAUSALE|DATA|TIPO|TOT,tok:%tok%|%id%)\n"
         "buttons=nuovo#nuovo|modifica#modifica|stampa#stampa|uscita#exit\n""links=docs|modifica|stampa" ) );

      FRM_PARAMS;
      FRM_PARAM( "%fornitore%", supplier.listview->descrizione() );
      FRM_PARAM( "%tok%", QSN( tok_elenco_doc_fornitore ) );
      FRM_PARAM( "%id%", supplier.listview->id() );

      bFORM( frm_docs, str_docs, suppliers );
      CTRL_FRM( frm_docs );
      CTRL( docs );
      CTRL( nuovo );
      CTRL( modifica );
      CTRL( stampa );
      CTRL( exit );

      while ( frm_docs->interazione()!=exit ) {
        if ( frm_docs->iterato==nuovo ) tools.msgbox( "Nuovo documento" );

      }

      xFORM( frm_docs, str_docs );

    }

    // -------------------------------------------------------------
    if ( iterato==lying ) {
    }

    if ( modificato )
      supplier.listview->aggiorna();

    // -------------------------------------------------------------
    if ( iterato==exit )
      iterato=NULL;

    if ( iterato )
      iterato=suppliers->interazione();
  }

  xFORM( suppliers, str_suppliers );

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_main::frm_tessere() {

  enu_tessera;

  QString frm_cards=TRFORM(
     ( "minmax=100%|95%\n""margine=4\n""events=custom\n""status=Gestione |Tessere\n"
     "listview=lista#list(header:CODICE!220|SCADENZA!140|RAG.SOC.!380,""tok:%tok%,minr:8,onclick,noexit)\n"
     "buttons=codice#codice|modifica#modifica|associa#associa|conto#conto|"
     "comanda#comanda|ricarica#ric_tes|stampa#stampa|cancella#cancella|uscita#exit\n"
     "links=list|associa|conto|comanda|ric_tes|cancella|stampa|modifica" ) );

  FRM_PARAMS;
  FRM_PARAM( "%tok%", QSN( tok_elenco_tag ) );
  bFORM( cards, frm_cards, this );
  CTRL_FRM( cards );
  CTRL( list );
  CTRL( codice );
  CTRL( associa );
  CTRL( conto );
  CTRL( comanda );
  CTRL( ric_tes );
  CTRL( modifica );
  CTRL( stampa );
  CTRL( cancella );
  CTRL( exit );

  QStringList tessera;

  cards->interazione();
  while ( cards->iterato!=NULL ) {

    bool aggiorna=false;

    QString tag;

    if ( !list.listview->selezionato().isEmpty()&&cards->iterato!=( tr_widget* ) cards ) {
      tag=list.listview->descrizione();
      tessera=QStringList::split( '|', list.listview->record(), true );
    }
    else {
      tessera=cards->card;
      tag=tessera[p_rftag];
      if ( cards->iterato==( tr_widget* ) cards&&tag.isEmpty() ) {
        card=cards->card;
        card_readed();
        cards->interazione();
        continue;
      }
    }
    if ( cards->iterato==list&&!tag.isEmpty() ) {

      int tipo=tessera[p_famiglia].toInt();

      if ( tessera[p_id_t0]=="0" ) tessera[p_id_t0]="";
      if ( tessera[p_id_t1]=="0" ) tessera[p_id_t1]="";
      if ( tessera[p_id_t2]=="0" ) tessera[p_id_t2]="";

      if ( tipo==TRPayCard::famiglia_punti||tipo==TRPayCard::famiglia_prepagata||tipo==TRPayCard::famiglia_convenzione||
         tipo==TRPayCard::famiglia_postpagato ) cards->scopri( ric_tes );
      else
        cards->nascondi( ric_tes );

      if ( tipo==TRPayCard::famiglia_tavolo ) cards->scopri( associa );
      else
        cards->nascondi( associa );
      if ( tipo==TRPayCard::famiglia_tavolo||!tessera[p_id_t0].isEmpty()||!tessera[p_id_t0].isEmpty() ) {
        cards->scopri( conto, comanda );
      }
      else
        cards->nascondi( conto, comanda );
    }
    if ( cards->iterato==associa ) {
      QString id_associato="";
      QString desc_associato;
      if ( tools.scelta( tr_listview::tavoli, id_associato, desc_associato ) ) {
        tessera[p_id_t1]=id_associato;
        bool punti=tessera[p_famiglia].toInt()==TRPayCard::famiglia_punti;
        QStringList params;
        params<<tessera[p_rftag];
        params<<tessera[p_famiglia];
        params<<tessera[p_tipo];
        params<<tessera[p_id_p0];
        params<<tessera[p_id_p1];
        params<<tessera[p_id_t0];
        params<<tessera[p_id_t1];
        params<<tessera[p_id_t2];
        params<<tessera[p_pagamenti];
        params<<( punti?LEAVE:tessera[p_minimo] );
        params<<( punti?LEAVE:tessera[p_valore] );
        params<<( punti?LEAVE:tessera[p_massimo] );
        params<<( punti?tessera[p_pmin]:QString( LEAVE ) );
        params<<( punti?tessera[p_pact]:QString( LEAVE ) );
        params<<( punti?tessera[p_pmax]:QString( LEAVE ) );
        params<<tessera[p_scadenza];
        params<<tessera[p_riferimento];
        params<<tessera[p_sconto_percentuale];
        CMD( tok_modifica_tag, params );
        aggiorna=true;
      } // if scelta
    } // if associa

    if ( cards->iterato==cancella ) {
      if ( tools.msgbox( tr( "Si e' sicuri di voler cancellare la tessera " )+tag, SINO )==SI )
        if ( CMD( tok_cancella_tag, tag ) ) aggiorna=true;
    }
    if ( cards->iterato==ric_tes ) {
      int tipo=tessera[p_famiglia].toInt();
      int i_p_act=-1, i_p_max=-1;
      if ( tipo==TRPayCard::famiglia_punti ) {
        i_p_act=p_pact;
        i_p_max=p_pmax;
      }
      if ( tipo==TRPayCard::famiglia_prepagata||tipo==TRPayCard::famiglia_convenzione||tipo==TRPayCard::famiglia_postpagato ) {
        i_p_act=p_valore;
        i_p_max=p_massimo;
      }
      if ( i_p_act!=-1&&i_p_max!=-1 ) {
        QString val=tools.calcolatrice( cards, tr( "Valore per ricarica" ), tessera[i_p_max] );
        QString estremi;
        if ( !val.isEmpty() ) {
          if ( config.ricarica_tessere_incrementale )
            val=QSN( val.toDouble()+tessera[i_p_act].toDouble() );
          if ( config.chiedi_estremi_ricarica_tessera )
            estremi=tools.input_descrizione( cards, "Estremi doc. ricarica" );
          if ( tools.msgbox( tr( "Si e' sicuri di voler ricaricare la tessera " )+tag+tr( " a " )+val, SINO )==SI ) {
            tessera[i_p_act]=val;
            if ( !estremi.isEmpty() )
              tessera[p_riferimento]=estremi;
            bool punti=tessera[p_famiglia].toInt()==TRPayCard::famiglia_punti;
            QStringList params;
            params<<tessera[p_rftag];
            params<<tessera[p_famiglia];
            params<<tessera[p_tipo];
            params<<tessera[p_id_p0];
            params<<tessera[p_id_p1];
            params<<tessera[p_id_t0];
            params<<tessera[p_id_t1];
            params<<tessera[p_id_t2];
            params<<tessera[p_pagamenti];
            params<<( punti?LEAVE:tessera[p_minimo] );
            params<<( punti?LEAVE:tessera[p_valore] );
            params<<( punti?LEAVE:tessera[p_massimo] );
            params<<( punti?tessera[p_pmin]:QString( LEAVE ) );
            params<<( punti?tessera[p_pact]:QString( LEAVE ) );
            params<<( punti?tessera[p_pmax]:QString( LEAVE ) );
            params<<tessera[p_scadenza];
            params<<tessera[p_riferimento];
            params<<tessera[p_sconto_percentuale];
            if ( CMD( tok_modifica_tag, params ) ) {
              CMD( tok_stampa_tag, tag );
              aggiorna=true;
            }
          }
        }
      }
    }
    if ( cards->iterato==stampa ) CMD( tok_stampa_tag, tag );
    if ( cards->iterato==( tr_widget* ) cards||cards->iterato==codice||cards->iterato==modifica ) {

      int famiglia;

      if ( cards->iterato==codice ) {
        tag=tools.input_descrizione( cards, tr( "Immettere codice tessera" ) );
        if ( tag.isEmpty() ) {
          cards->interazione();
          continue;
        }
      } // se codice

      if ( ::cmd( tok_identifica_tag, tag ) ) {
        QString st=srv->rcv();
        tessera.clear();
        tessera=QStringList::split( '|', st, true );
        cards->iterato=modifica;
      }
      else {
        tessera.clear();
      }
      srv->eop();

      QString str_card=TRFORM( (
         //"minmax=        100%|90%\n"
         "margine=       6;spazio=4\n""events=        custom\n""events.ignore= cards\n""palette=       3\n"
         "status=        Gestione tessera |%tag%\n"
         "listview=      #type(onclick,noexit);                                      #type;                                          #payments(tp:%tp_type%,onclick,noexit); #payments;                          #payments\n"
         "input=         Importo min.#amount_min(val:real2,oneline);                 Imp. Attuale#amount_actual(val:real2,oneline);  Imp. Max#amount_max(val:real2);         Ric.Rel.#ricrel(val:real2,onupdate);Sc.%#sc(val:real2)\n"
         "input=         Scadenza#expiration(ggmmaaaa);                              #expiration;                                    input=Score#score_min(val:int);         attuale#score_actual(val:int);      max#score_max(val:int)\n"
         "input=         Cliente#customer(alato,tp:%tp_cli%:0,onclick);              #customer;                                      Sotto.Cli#sub_customer(alato,oneline);  #sub_customer;                      #sub_customer\n"
         "label=         Intestazione Automatica#intesta(chk:off);                   #intesta;                                       input=Tavolo#table(alato,readonly);     hole=;                              hole=\n"
         "input=         Riferimento#reference(alato,tags.nota_riferimento,oneline); #reference;                                     #reference;                             hole=;                              hole=\n"
         "buttons=       ok#ok|associa#associa|uscita#exit\n""!mod:links=    type|payments|amount_min|amount_actual|amount_max|conto|"
         "score_min|score_actual|score_max|customer|sub_customer|expiration|intesta|""table|reference|ricrel|sc" ) );

      FRM_PARAMS;
      FRM_PARAM( "%tag%", tag );
      FRM_PARAM( "%tp_type%", QSN( tr_listview::selezione_pagamenti ) );
      FRM_PARAM( "%tp_cli%", QSN( tr_listview::clienti_rapidi ) );
      if ( cards->iterato==modifica ) str_card.prepend( "conds=mod\n" );
      bFORM( card, str_card, cards );
      CTRL_FRM( card );
      CTRL( type );
      CTRL( conto );
      CTRL( payments );
      CTRL( amount_min );
      CTRL( amount_actual );
      CTRL( ricrel );
      CTRL( amount_max );
      CTRL( score_min );
      CTRL( score_actual );
      CTRL( score_max );
      CTRL( customer );
      CTRL( sub_customer );
      CTRL( expiration );
      CTRL( intesta );
      CTRL( table );
      CTRL( reference );
      CTRL( sc );
      CTRL( ok );
      CTRL( associa );
      CTRL( exit );

      enu_tipi_tessere;
      denu_identifica_tag;
#define spec_tessera (  TRPayCard::tessera_apri_conto|\
   TRPayCard::tessera_chiudi_conto|\
   TRPayCard::tessera_intesta_conto|\
   TRPayCard::tessera_pagamento|\
   TRPayCard::tessera_cliente|\
   TRPayCard::tessera_sconto_percentuale )

      int tipi[10];
      tipi[TRPayCard::famiglia_tavolo]=tt_tavolo;
      tipi[TRPayCard::famiglia_prepagata]=tt_prepagata;
      tipi[TRPayCard::famiglia_convenzione]=tt_convenzione;
      tipi[TRPayCard::famiglia_postpagato]=tt_postpagato;
      tipi[TRPayCard::famiglia_cameriere]=tt_cameriere;
      tipi[TRPayCard::famiglia_punti]=tt_punti;
      tipi[TRPayCard::famiglia_cliente]=tt_cliente;

      QString elenco=tr( "%1|Tavolo\n""%2|Prepagata\n""%3|Convenzione\n""%4|Punti\n""%5|Postpagato\n"
         "%6|Cameriere\n""%7|Cliente\n" );
      elenco.replace( "%1", QSN( TRPayCard::famiglia_tavolo ) );
      elenco.replace( "%2", QSN( TRPayCard::famiglia_prepagata ) );
      elenco.replace( "%3", QSN( TRPayCard::famiglia_convenzione ) );
      elenco.replace( "%4", QSN( TRPayCard::famiglia_punti ) );
      elenco.replace( "%5", QSN( TRPayCard::famiglia_postpagato ) );
      elenco.replace( "%6", QSN( TRPayCard::famiglia_cameriere ) );
      elenco.replace( "%7", QSN( TRPayCard::famiglia_cliente ) );
      type.values( QStringList::split( '\n', elenco ) );
      type.listview->tipo_fittizio( "@id|$desc" );
      card->nascondi( associa );

      QString id_associato="";
      QString desc_associato;

      TRTokens tok=tok_inserisci_tag;

      if ( cards->iterato==modifica ) {

        qApp->processEvents();

        int tipo=tessera[p_tipo].toInt();
        if ( tipo&TRPayCard::tessera_apri_conto )
          conto.value( 0 );
        if ( tipo&TRPayCard::tessera_chiudi_conto )
          conto.value( 1 );
        ( uint )tipo&=( ~( uint )spec_tessera );

        QStringList e=tessera;

        payments.value( e[p_pagamenti], ',' );
        amount_min.value( e[p_minimo] );
        amount_actual.value( e[p_valore] );
        amount_max.value( e[p_massimo] );
        score_min.value( e[p_pmin] );
        score_actual.value( e[p_pact] );
        score_max.value( e[p_pmax] );
        sc.value( e[p_sconto_percentuale] );
        famiglia=e[p_famiglia].toInt();
        if ( e[p_id_p0]!="0" ) {
          if ( ( famiglia==TRPayCard::famiglia_cameriere ) ) {
            customer.value( e[p_id_p0], tok_elenco_camerieri_esteso );
            customer.input->id( e[p_id_p0] );
          }
          else {
            customer.value( e[p_id_p0], tok_elenco_clienti_esteso );
            customer.input->id( e[p_id_p0] );
            sub_customer.value( e[p_id_p1], tok_elenco_sottoclienti_esteso );
            sub_customer.input->id( e[p_id_p1] );
          }
        }
        expiration.value( tools.conforma_data( e[p_scadenza] ) );
        bool intesta_conto=( e[p_tipo].toInt()&TRPayCard::tessera_intesta_conto );
        intesta.value( intesta_conto );
        if ( e[p_id_t1]!="0" ) id_associato=e[p_id_t1];
        table.value( id_associato, tok_elenco_tavolo_esteso );
        tok=tok_modifica_tag;
        type.value( e[p_famiglia] );
        reference.value( e[p_riferimento] );

        card->disabilita( type );
        card->iterato=type;
      }
      else
        card->interazione();

      QString pagamenti="";
      while ( card->iterato!=NULL ) {

        if ( card->iterato==type ) {

          if ( !type().isEmpty() ) famiglia=type().toInt();

          enu_tipi_tessere tipo;
          ( int )tipo=tipi[famiglia-1];
          pagamenti="";
          if ( cards->iterato!=modifica ) payments.listview->seleziona( tr_listview::e_none );

          card->nascondi( associa );
          customer->input->setLabel( TR( "Cliente" ) );
          customer->input->tp( tr_listview::clienti_rapidi );
          tr_widget*tutto[]= {
            amount_min, amount_actual, amount_max, score_min, score_actual,
            score_max, customer, payments, conto, intesta, ricrel,
                 table,  reference, sc, NULL
          };
          card->abilita( tutto );
          card->disabilita( sub_customer, intesta );
          payments.listview->elenco->setSelectionMode( QListView::NoSelection );
          switch ( famiglia ) {

            case TRPayCard::famiglia_tavolo:
              card->disabilita( tutto );
              card->abilita( table );
              card->scopri( associa );
            break;
            case TRPayCard::famiglia_prepagata:
              card->disabilita( score_min, score_actual, score_max, payments, table );
              pagamenti=QSN( config.pg_tessere );
            break;
            case TRPayCard::famiglia_cliente:
              card->disabilita( amount_min, amount_actual, amount_max, ricrel, score_min, score_actual, score_max,
                 table, reference, sc );
            break;
            case TRPayCard::famiglia_convenzione:
              payments.listview->elenco->setSelectionMode( QListView::Single );
              card->disabilita( score_min, score_actual, score_max, table, sc );
            break;
            case TRPayCard::famiglia_punti:
              card->disabilita( tutto );
              card->abilita( score_min, score_actual, score_max );
            break;
            case TRPayCard::famiglia_postpagato:
              card->disabilita( tutto );
              card->abilita( conto, amount_min, amount_actual, amount_max, ricrel );
            break;
            case TRPayCard::famiglia_cameriere:
              card->disabilita( tutto );
              card->abilita( customer );
              customer->input->tp( tr_listview::camerieri );
              customer->input->setLabel( TR( "Camer." ) );
            break;

            default:
              tools.msgbox( tr( "Tipo tessera non riconosciuto" ) );
              card->iterato=NULL;
          }

          if ( tessera.count()>3||iterato==modifica ) card->disabilita( type );

          if ( type().toInt()!=TRPayCard::famiglia_cameriere&&!customer.input->id().isEmpty() ) {
            card->abilita( sub_customer, intesta );
            sub_customer.input->tok( tok_elenco_sottoclienti, customer.input->id(), 0 );
          }

        } // if iterato=type

        // associa
        if ( card->iterato==associa ) {
          tools.scelta( tr_listview::tavoli_semplici, id_associato, desc_associato );
          table.value( desc_associato );
        }

        if ( card->iterato==customer ) {
          if ( type().toInt()!=TRPayCard::famiglia_cameriere&&!customer.input->id().isEmpty() ) {
            card->abilita( sub_customer, intesta );
            sub_customer.input->tok( tok_elenco_sottoclienti, customer.input->id(), 0 );
          }
          if ( customer.input->id().isEmpty() ) {
            sub_customer.input->id( "" );
            card->disabilita( sub_customer, intesta );
          }
        }

        if ( card->iterato==ricrel ) {
          amount_actual.value( amount_actual().toDouble()+ricrel().toDouble() );
        }

        // ok
        if ( card->iterato==ok ) {

          bool salta=false;

          int tipo=tipi[type().toInt()];
          if ( pagamenti.isEmpty() ) pagamenti=payments( "," );
          if ( !pagamenti.isEmpty() ) {
            tipo|=TRPayCard::tessera_pagamento;
          }
          else {
            if ( type()==TRPayCard::famiglia_convenzione ) {
              tools.msgbox( tr( "La tessera convenzione deve avere almeno un pagamento" ) );
              salta=true;
            }
          }
          if ( !salta&&tipo&TRPayCard::tessera_prepagata ) {
            if ( !amount_actual().toDouble()||!amount_max().toDouble() ) {
              tools.msgbox( tr( "I valori degli importi devono essere diversi da zero" ) );
              salta=true;
            }
            if ( !salta&&( !config.ricarica_tessere_incrementale&&cards->iterato!=modifica )&&
               ( !( amount_min().toDouble()<amount_actual().toDouble() )||
               !( amount_actual().toDouble()<=amount_max().toDouble() ) ) ) {
                 tools.msgbox( tr( "Gli importi devono essere crescenti" ) );
                 salta=true;
            }
          }
          if ( famiglia==TRPayCard::famiglia_cliente ) {
            if ( customer.input->id().isEmpty() ) {
              tools.msgbox( tr( "Scegliere un cliente dall'elenco" ) );
              salta=true;
            }
          }
          if ( famiglia==TRPayCard::famiglia_cameriere ) {
            if ( customer.input->id().isEmpty() ) {
              tools.msgbox( tr( "Scegliere un cameriere dall'elenco" ) );
              salta=true;
            }
          }
          if ( famiglia==TRPayCard::famiglia_punti ) {
            if ( !score_actual().toDouble()||!score_max().toDouble() ) {
              tools.msgbox( tr( "I punti tessera devono essere diversi da zero" ) );
              salta=true;
            }
            if ( !salta&&( !config.ricarica_tessere_incrementale&&cards->iterato!=modifica )&&
               ( !( score_min().toDouble()<score_actual().toDouble() )||
               !( score_actual().toDouble()<=score_max().toDouble() ) ) ) {
                 tools.msgbox( tr( "I punti devono essere crescenti" ) );
                 salta=true;
            }
          }
          if ( famiglia==TRPayCard::famiglia_tavolo&&!id_associato.toInt() ) {
            tools.msgbox( tr( "Specificare il tavolo associato" ) );
            salta=true;
          }

          if ( salta ) {
            card->interazione();
            continue;
          }

          if ( conto.value()=="0" ) ( uint )tipo|=TRPayCard::tessera_apri_conto;
          if ( conto.value()=="1" ) ( uint )tipo|=TRPayCard::tessera_chiudi_conto;
          if ( intesta.isOn() ) ( uint )tipo|=TRPayCard::tessera_intesta_conto;
          if ( !customer.input->id().isEmpty()&&famiglia!=TRPayCard::famiglia_cameriere )
            ( uint )tipo|=TRPayCard::tessera_cliente;
          if ( sc.value().toDouble()>0.001 )
            ( uint )tipo|=TRPayCard::tessera_sconto_percentuale;
          if ( CMD( tok, tag, type(), QSN( tipo ), customer.input->id(), sub_customer.input->id(), LEAVE, id_associato, LEAVE, // tavoli
          pagamenti, amount_min(), amount_actual(), amount_max(), score_min(), score_actual(), score_max(), expiration.dataiso( true ), reference(), sc() ) ) {
            card->iterato=NULL;
            aggiorna=true;
            CMD( tok_stampa_tag, tag );
          }
        }

        if ( card->iterato==exit ) card->iterato=NULL;
        if ( card->iterato!=NULL ) card->interazione();
      }

      xFORM( card, str_card );
    }

    if ( cards->iterato==exit )
      cards->iterato=NULL;
    if ( aggiorna )
      list.listview->aggiorna();
    if ( cards->iterato )
      cards->iterato=cards->interazione();
  }

  xFORM( cards, frm_cards );
#undef spec_tessera
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_main::card_readed() {
  if ( !card.count() ) {
    frm_base::card_readed();
    return;
  }

  enu_tessera;
  int tipo=card[p_famiglia].toInt();

  if ( tipo==TRPayCard::famiglia_cameriere ) {
    logout();
    if ( first_login( card[p_id_p0], QString::null, true ) ) {
      QTimer::singleShot( config.auto_logout, this, SLOT( logout() ) );
    }
  } // cameriere

  if ( tipo==TRPayCard::famiglia_tavolo ) {
    if ( first_login() ) {
      int id=card[p_id_t1].toInt();
      tavoli->seleziona( NULL, &id );
    }
  } // tavolo

  if ( tipo==TRPayCard::famiglia_postpagato ) {
    itm_tavolo*t=tavoli->selezionato();
    bool procedi=true;
    if ( !t ) {
      if ( card[p_id_t0].toInt()!=0 ) {
        int id=card[p_id_t0].toInt();
        tavoli->seleziona( NULL, &id );
        t=tavoli->selezionato();
        if ( !t )
          tools.msgbox( tr( "Tessera non associata a nessun tavolo<br>""di questa sala. Sceglierne uno e ripassare<br>"
             "la card al lettore" ) );
        else {
          t->card=card;
          t->aggiorna();
        }
      }
      else {
        tools.msgbox( tr( "Questa card postpagata non e' associata a nessun tavolo.<br>"
           "Scegliere un tavolo e ripassare la card al lettore" ) );
      }
      procedi=false;
    }
    if ( procedi&&card[p_id_t0]==QSN( t->id() ) ) {
      t->card=card;
      t->aggiorna();
      procedi=false;
    }
    if ( t&&t->bar() ) {
      t->card=card;
      t->aggiorna();
      procedi=false;
    }

    if ( procedi&&!card[p_id_t0].isEmpty()&&card[p_id_t0].toInt()!=0&&card[p_id_t0].toInt()!=t->id() ) {
      if ( tools.msgbox( tr( "Tessere gia' precedentemente assegnata a un altro tavolo<br>""Si vuol assegnarla a questo tavolo?" ), SINO )!=SI ) {
        procedi=false;
        int id=card[p_id_t0].toInt();
        t->card=card;
        tavoli->seleziona( NULL, &id );
      }
    }
    if ( procedi ) {
      card[p_id_t0]=QSN( t->id() );
      t->card=card;
      card.pop_front();
      if ( CMD( tok_modifica_tag, card ) ) {
        t->aggiorna();
      }
    }
  }

  stato_pulsanti();

}



void frm_main::stato_sale() {

  denu_statistiche_sale;

  QString frm_statosala=TRFORM(
     ( "minmax=%95|%90\n""margine=3\n""status=%titolo%\n""input=N. cop#num_cop(readonly,alato);C. med#cop_medio(readonly,alato);hole=\n"
     "label=Stato tavoli per sala#stato_tavoli(oneline)\n""listview=#tab_open(header:Sala!350|Tav.ap.!150|Tav.tot!150,noexit/*,small*/)\n"
     "label=Venduto articoli per gruppo di stampa#ven_grp_lab(oneline)\n"
     "listview=#ven_grp(header:Gruppo!350|Qta!150|Totale!150,noexit,small,""hl:%nome% eq TOTALE?#ff0000)\n"
     "buttons=dettaglio#detail|uscita#exit\n""links=tab_open|detail" ) );
  QString p=( devnul?"3" :"1" );
  if ( !::cmd( tok_statistiche_sale, p ) ) {
    return;
  }

  QString r=srv->rcv();
  QStringList fld=QStringList::split( CMDLANG_SEPARATOR, r, true );
  FRM_PARAMS;
  FRM_PARAM( "%titolo%", fld[p_intestazione] );
  FRM_PARAM( "%nome%", QSN( ( enu_statistiche_sale )p_voce+1 ) );

  bFORM( fs, frm_statosala, this );
  CTRL_FRM( fs );
  CTRL( cop_medio );
  CTRL( num_cop );
  CTRL( tab_open );
  CTRL( ven_grp );
  CTRL( exit );
  CTRL( detail );

  cop_medio->value( fld[p_coperto_medio]+" EU" );
  num_cop->value( fld[p_coperti_ultima_chiusura] );
  QStringList wrk;
  {
    QStringList t;
    t=QStringList::split( ',', fld[p_tavoli_aperti_per_sala], true );

    for ( int i=0; i<( int )t.count(); i+=4 ) {
      wrk<<t[i]+CMDLANG_SEPARATOR+t[i+1]+CMDLANG_SEPARATOR+t[i+2]+CMDLANG_SEPARATOR+t[i+3];
    }

    tab_open.listview->carica( wrk, "@id|$sala|#tavap|#tavtot" );

  }
  wrk.clear();
  while ( ( r=srv->rcv() )!=config.cmd_eod ) wrk<<( QString::number( wrk.count() )+CMDLANG_SEPARATOR+r );
  ven_grp.listview->carica( wrk, "@id|$gruppo|#qta|#prz" );
  tab_open.listview->setMinMax( 0, 0, 32767, config.elenchi_altezza_riga*4 );
  ven_grp.listview->setMinMax( 0, 0, 32767, config.elenchi_altezza_riga*4 );
  while ( fs->interazione() ) {
    if ( fs->iterato==detail ) {
      denu_statistiche_sale_tavoli;
      QString frm_dettagliosala=
         TRFORM( "minmax=%95|%90\n""margine=3\n""status=Statistiche sala |%nome_sala%\n"
         "listview=(header:Tavolo!80|Tot.Ord!120|Tot.Vend!140|Tot.Cop!80|N.Conti!80|Aperto.Da!200"",small,minr:14,tok:%tok%)\n" );
      FRM_PARAMS;
      FRM_PARAM( "%nome_sala%", tab_open.listview->descrizione() );
      FRM_PARAM( "%tok%", QSN( tok_statistiche_sale_tavoli )+"|" +p+"|" +tab_open.listview->id() );
      bFORM( fds, frm_dettagliosala, fs );
      fds->interazione();
      xFORM( fds, frm_dettagliosala );
    }
    if ( fs->iterato==exit ) break;
  }

  xFORM( fs, frm_statosala );

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_main::tests() {
  QString str_comto=TRFORM(
     ( "minmax=screen\n""margine=0\n""status=Comanda/Conto\n""extra=comanda;menu(sp:1,bsize:60|54,rc:9|7)\n""buttons=ok|exit" ) );
  FRM_PARAMS;
  bFORM( comto, str_comto, this );
  tr_menu_dim*menu=( tr_menu_dim* ) comto->pointer( "menu" );
  menu->carica( "1" );
  comto->interazione();
  xFORM( comto, str_comto );
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// form comulativa e sostitutiva per COManda/conTO e bar

void frm_main::frm_ffm() {

  if ( glb.lang!=config.exe_path+"polski" ) {
    QProcess p( this );
    QString modulo=config.exe_path+"../bin/fatmese";
    cout<<tr( "Eseguo il modulo:" )<<modulo<<endl;
    p.addArgument( modulo );
    if ( p.start() ) {
      hide();
      while ( p.isRunning() );
      show();
    }

    else {
      tools.msgbox( tr( "Attenzione. Il modulo FATMESE non e' stato ""avviato per uno dei seguenti motivi:<br>"
         "* modulo assente<br>""* errore interno<br>""Avvertire assistenza" ) );

    }
    return;
  }

  QString str_ffm=TRFORM( "margine=0\n""status=Fattura Fine Mese\n""extra=comanda#comanda(ffm)\n"
     "!pys:input=Cliente#customer(readonly,alato)\n""?pys:input=Cliente#customer(readonly,alato);Pagamento#paym(readonly,alato)\n"
     "buttons=ok#ok|cliente#clienti|?pys:pagamento#pagamento|exit#exit" );

  QStringList payments=QStringList::split( '|', config.ffm_payments );
  if ( payments.count()>1 ) str_ffm.prepend( "conds=pys\n" );

  FRM_PARAMS;
  bFORM( ffm, str_ffm, this );
  CTRL_FRM( ffm );
  CTRL( ok );
  CTRL( clienti );
  CTRL( customer );
  CTRL( pagamento );
  CTRL( paym );
  CTRL( exit );
  tr_comanda*comanda=( tr_comanda* ) ffm->pointer( "comanda" );
  QString id_cli;
  QString descrizione_cli;
  QString id_pay;
  paym.value( TR( "TUTTI" ) );
  while ( ffm->interazione()!=exit ) {

    if ( ffm->iterato==clienti ) {
      tools.scelta( tr_listview::clienti_rapidi, id_cli, descrizione_cli );
      if ( !id_cli.isEmpty() ) customer.value( descrizione_cli );
    }

    if ( ffm->iterato==pagamento ) {
      QStringList elenco;

      TROWSLI( payments ) {
        denu_elenco_gruppo;
        QString st=srv->rcv( tok_elenco_gruppo, payments[i] );
        st=st.section( '|', p_descrizione, p_descrizione );
        st.prepend( '|' );
        st.prepend( payments[i] );
        elenco<<st;
      }
      tr_tools::tr_elenco item;
      item=tools.elenco( elenco, TR( "PAGAMENTO" ), "@id|$desc" );
      if ( !item.id.isEmpty() ) {
        id_pay=item.id;
        paym.value( item.descrizione );
      }
      else {
        id_pay="";
        paym.value( TR( "TUTTI" ) );
      }
    }

    if ( ffm->iterato==ok ) {
      tr_table*tbl=comanda->tabella;
      if ( tbl->text( 0, 0 ).isEmpty() ) {
        tools.msgbox( TR( "Immettere i dati in tabella prima di confermare" ) );
        continue;
      }


      QStringList vats=QStringList::split( ';', config.ffm_vats );

      if ( config.ffm_chk_befor_print=="all" ) {
        bool salta=false;
        for ( int row=0; row<tbl->numRows(); row++ ) {
          if ( tbl->text( row, 0 ).isEmpty() ) break;
          QString data=tbl->text( row, 0 );
          QDate dd;
          if ( !tools.data_valida( data, &dd ) ) {
            tbl->setCurrentCell( row, 0 );
            tools.msgbox( TR( "Data non valida" ) );
            salta=true;
            break;
          }
          QString vat=tbl->text( row, 2 );
          bool trovato=false;
          TROWSLJ( vats )
          if ( vats[j].section( '|', 1, 1 )==vat ) {
            trovato=true;
            break;
          }
          if ( !trovato ) {
            tbl->setCurrentCell( row, 2 );
            tools.msgbox( TR( "IVA non valida" ) );
            salta=true;
            break;
          }
        }
        if ( salta ) continue;
      }

      if ( ::cmd( tok_inserisci_tavolo_fittizio ) ) {
        QString id=srv->rcv();
        srv->eop();
        CMD( tok_apri_tavolo, id );
        comanda->nuova();

        bool annullato=false;
        for ( int row=0; row<tbl->numRows(); row++ ) {
          if ( tbl->text( row, 0 ).isEmpty() ) break;
          QString is;
          QString prz=tbl->text( row, 3 );
          QString vat=tbl->text( row, 2 );
          QString ref=tbl->text( row, 1 );
          QString descrizione=config.ffm_description;
          if ( descrizione.isEmpty() ) descrizione=tbl->text( row, 0 );
          else {
            descrizione.replace( "%date%", tbl->text( row, 0 ) );
            descrizione.replace( "%ref%", ref );
            descrizione.replace( "%vat%", vat );
            descrizione.replace( "%sum_row%", prz );
          }
          TROWSLJ( vats )
          if ( vats[j].section( '|', 1, 1 )==tbl->text( row, 2 ) ) {
            is=vats[j].section( '|', 0, 0 );
            break;
          }
          if ( !CMD( tok_inserisci_articolo_comanda, "1", is, descrizione, prz, LEAVE, LEAVE ) ) {
            annullato=true;
            break;
          }
        }
        if ( !annullato ) {
          CMD( tok_registra_comanda );
          comanda->nuova();
          comanda->apri_conto();
          if ( !id_cli.isEmpty() ) CMD( tok_intesta_conto, id_cli );
          if ( payments.count() ) {
            if ( !id_pay.isEmpty() ) {
              annullato=comanda->inserisci_pagamento( id_pay.toInt() );
            }

            else
              TROWSLI( payments ) {
                annullato=comanda->inserisci_pagamento( payments[i].toInt() );
                if ( !annullato ) break;
              }
          }
          else
            annullato=comanda->inserisci_pagamento( config.pg_contanti );
          if ( annullato ) {
            comanda->chiudi_conto();
            CMD( tok_stampa_conto, QSN( tr_comanda::doc_fattura ), "0", "FATTURA" );
          }
          else
            comanda->annulla_conto();
        }
        CMD( tok_chiudi_tavolo );
        CMD( tok_cancella_tavolo, id );
        break;
      }
    }
  }
  xFORM( ffm, str_ffm );
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void frm_main::frm_voti() {
  denu_elenco_voti;

  QString frm_voti=TRFORM( "margine=3\n""status=Gestione Voti\n"
     "listview=#voti(header:Descrizione!200|Soglia!80,onclick,tok:%tok%,noexit)\n"
     "input=Descrizione#description(alato,voti.descrizione);Soglia#limit(alato,voti.soglia)\n"
     "buttons=aggiungi#add|modifica#modify|cancella#erase|uscita#exit\n""links=voti|modify|erase\n" );
  FRM_PARAMS;
  FRM_PARAM( "%tok%", QSN( tok_elenco_voti ) );
  bFORM( fev, frm_voti, this );
  CTRL_FRM( fev );
  CTRL( voti );
  CTRL( description );
  CTRL( limit );
  CTRL( add );
  CTRL( modify );
  CTRL( erase );
  CTRL( exit );
  do {
    fev->interazione();
    if ( fev->iterato==voti ) {
      description.value( voti.listview->descrizione() );
      limit.value( voti.listview->record().section( '|', 2, 2 ) );
    }
    if ( fev->iterato==modify ) {
      if ( CMD( tok_modifica_voto, voti(), description(), limit(), LEAVE ) )
        voti.listview->aggiorna();
    }
    if ( fev->iterato==add ) {
      if ( CMD( tok_modifica_voto, LEAVE, description(), limit(), LEAVE ) )
        voti.listview->aggiorna();
    }
    if ( fev->iterato==erase ) {
      if ( CMD( tok_modifica_voto, voti(), LEAVE ) )
        voti.listview->aggiorna();
    }
  }
  while ( fev->iterato!=exit );
  xFORM( fev, frm_fev );
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void frm_main::frm_turni() {
  denu_elenco_turni;

  QString frm_turni=TRFORM( "margine=3\n""status=Gestione Turni\n""listview=#turni(onclick,tok:%tok%,noexit)\n"
     "input=Descrizione#description(alato,turni.descrizione)\n""buttons=aggiungi#add|modifica#modify|cancella#erase|uscita#exit\n"
     "links=turni|modify|erase\n" );
  FRM_PARAMS;
  FRM_PARAM( "%tok%", QSN( tok_elenco_turni ) );
  bFORM( fet, frm_turni, this );
  CTRL_FRM( fet );
  CTRL( turni );
  CTRL( description );
  CTRL( add );
  CTRL( modify );
  CTRL( erase );
  CTRL( exit );
  do {
    fet->interazione();
    if ( fet->iterato==turni ) description.value( turni.listview->descrizione() );
    if ( fet->iterato==modify ) {
      if ( CMD( tok_modifica_turno, turni(), description() ) ) turni.listview->aggiorna();
    }
    if ( fet->iterato==add ) {
      if ( CMD( tok_modifica_turno, LEAVE, description() ) ) turni.listview->aggiorna();
    }
    if ( fet->iterato==erase ) {
      if ( CMD( tok_modifica_turno, turni(), LEAVE ) ) turni.listview->aggiorna();
    }
  }
  while ( fet->iterato!=exit );
  xFORM( fet, frm_fet );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

