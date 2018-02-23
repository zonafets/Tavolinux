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

#include "tr.hpp"

#include "tr_srvcmd.hpp"
#include "tr_common.hpp"
#include "tr_config.hpp"

extern tr_config config;

// =========================================================

QString tr::tipo_articolo::descrizione() {
  QString l;
  if ( stato&GRUPPO_ARTICOLO_PURO ) l+="|puro";
  if ( stato&GRUPPO_ARTICOLO_PIUMENO ) l+="|pi.meno";
  if ( stato&GRUPPO_ARTICOLO_PORZIONATO ) l+="|porzionato";
  if ( stato&GRUPPO_ARTICOLO_NASCOSTO ) l+="|nascosto";
  if ( stato&GRUPPO_ARTICOLO_SELEZIONE ) l+="|selezione";
  if ( stato&GRUPPO_ARTICOLO_VEROFALSO ) l+="|vero.falso";
  if ( stato&GRUPPO_ARTICOLO_CONSOLIDANTE ) l+="|consolidante";
  if ( stato&GRUPPO_ARTICOLO_VARIANTE ) l+="|variante";
  if ( stato&GRUPPO_ARTICOLO_COMMENTO ) l+="|commento";
  if ( stato&GRUPPO_VARIANTE_TUTTE ) l+="|variante.tutte";
  if ( stato&GRUPPO_ARTICOLO_COPERTO ) l+="|coperto";
  if ( stato&GRUPPO_ARTICOLO_FINALE ) l+="|finale";
  if ( stato&GRUPPO_ARTICOLO_ISOLATO ) l+="|isolato";
  if ( stato&GRUPPO_ARTICOLO_UNICO ) l+="|unico";
  if ( stato&GRUPPO_ARTICOLO_INIZIALE ) l+="|iniziale";
  if ( stato&GRUPPO_DESCRIZIONE_VARIABILE ) l+="|desc.var";
  l+="|";
  return l;
}

// =========================================================

bool tr::menu::carica( int id_listino ) {
  elenco.clear();

  srv->nolog=true;

  if ( !::cmd( tok_elenco_menu_listino, QSN( id_listino ) ) ) return false;

  if ( config.verbose<2 ) srv->nolog=true;

  elenco=srv->rcv_list();
  srv->eop();

  srv->nolog=false;

  return true;
}

// =========================================================

tr::menu::voce::voce( const tr::gruppi::gruppo&g ):tipo_articolo( 0 ), colore() {
  id_listino=0;
  locazione=loc_indefinita;
  disposizione=-1;
  geometria=-1;
  //id_varianti=0;
  is=g.is;
  is_padre=g.id_padre;
  tipo_articolo.stato=g.tipo_articolo.stato;
  descrizione_originale=g.descrizione;
  descrizione=descrizione_originale;
  icona="";
  pixmap="";
}

// =========================================================

bool tr::menu::voce::salva() {
  if ( icona.isNull() ) icona="";
  if ( pixmap.isNull() ) pixmap="";
  bool b=::cmd( tok_modifica_menu, QSN( id_listino ), QSN( id_istanza ), QSN( locazione ), QSN( disposizione ), QSN( geometria ),
     descrizione, colore.name(), icona, pixmap );

  if ( b ) {
    QString rcv=srv->rcv();
    parse( rcv );
    srv->eop();
    return true;
  }
  return false;
}

// =========================================================

bool tr::menu::voce::varianti() {
  if ( is_padre==IS_VARIANTI ) return false;
  if ( ::cmd( tok_ha_varianti, QSN( is ) ) ) {
    QString st=srv->rcv();
    srv->eop();
    if ( st.section( '|', 0, 0 )!="0" ) return true;
  }
  return false;
}

// =========================================================

bool tr::menu::voce::parse( const QString s ) {

  QStringList e=QStringList::split( config.cmd_sep, s, true );
  if ( e.size()<p_last ) return false;

  /* non so perchè ma sballa this e fa casini int* vi[]={ &id_listino, &id_istanza, &id_istanza_padre, &tipo_articolo.stato,
  &locazione, &disposizione, &geometria, &id_varianti_globali, &id_varianti_gruppo, &id_varianti_articolo};
  tr::menu::voce::tr_posizioni pvi[] = { p_id_listino, p_id_istanza, p_id_istanza_padre, p_tipo_articolo,
  p_locazione, p_disposizione, p_geometria, p_id_varianti_globali, p_id_varianti_gruppo, p_id_varianti_articolo };

  TROWI(vi) *vi[pvi[i]] = e[pvi[i]].toInt(); */

  id_listino=e[p_id_listino].toInt();
  id_istanza=e[p_id_istanza].toInt();
  id_istanza_padre=e[p_id_istanza_padre].toInt();
  tipo_articolo.stato=e[p_tipo_articolo].toInt();
  locazione=( tr_locazione )e[p_locazione].toInt();
  disposizione=e[p_disposizione].toInt();
  geometria=e[p_geometria].toInt();
  // id_varianti=e[p_id_varianti_articolo].toInt();

  if ( e[p_colore]!="#000000" ) colore=QColor( e[p_colore] );

  /* non funziona QString* vs[] = { &descrizione, &descrizione_originale, &icona, &pixmap };
  tr::menu::voce::tr_posizioni pvs[] = {p_descrizione, p_descrizione_originale, p_icona, p_pixmap };
  TROWI(vs)  *vs[pvs[i]] = e[pvs[i]]; */

  descrizione=e[p_descrizione];
  descrizione_originale=e[p_descrizione_originale];
  icona=e[p_icona];
  pixmap=e[p_pixmap];

  return true;

}


// =========================================================

/* tr::menu::voce& tr::menu::voce::operator=(tr::menu::voce &c) { id_listino=c.id_listino; id_istanza=c.id_istanza;
id_istanza_padre = c.id_istanza_padre; ha_figli=c.ha_figli; x=c.x; y =c.y;
pagina = c.pagina; stato=c.stato; stato_extra=c.stato_extra; colore=c.colore;
descrizione=c.descrizione; descrizione_originale=c.descrizione_originale; elementi() = c.elementi(); return *this; }; */

// =========================================================
//            SALE e SALA
// =========================================================
/* bool tr::sale::carica() {

elenco.clear();

CMD c = cmd_srv->elenco_sale(); if (!cmd_srv->cmd(c)) return false;

QString st; while ((st=cmd_srv->rcv())!=config.cmd_eod) elenco<<st; return true;

} */
// =========================================================
/* bool tr::sale::sala::parse(const QString &s) {

QStringList e = QStringList::split(config.cmd_sep, s, true); if (e.size()<p_last) return false;

id = e[p_id].toInt(); nome = e[p_nome]; return true;

} */
// =========================================================
//            LISTINI e LISTINO
// =========================================================
/* bool tr::listini::carica() {

elenco.clear();

CMD c = cmd_srv->elenco_listini(); if (!cmd_srv->cmd(c)) return false;

QString st; while ((st=cmd_srv->rcv())!=config.cmd_eod) elenco<<st; return true;

} */
// =========================================================
/* bool tr::listini::listino::parse(const QString &s) {

QStringList e = QStringList::split(config.cmd_sep, s, true); if (e.size()<p_last) return false;

id = e[p_id].toInt(); codice = e[p_codice]; descrizione = e[p_descrizione]; return true;

} */
// =========================================================
//            ORDINE e RIGA
// =========================================================

bool tr::ordine::carica( tr::ordine::tipo cosa ) {

  elenco.clear();

  bool r=false;

  switch ( cosa ) {

    case t_comande:
    case t_conto:
    case t_conti:
      r=cmd( tok_elenco_comande );
    break;

    case t_portate:
    case t_pagamenti:
      r=cmd( tok_elenco_portate );
    break;

    case t_separata:
      r=cmd( tok_elenco_comande_separate );
    break;

    case t_ordine:
      r=cmd( tok_elenco_comande );
    break;

  }
  if ( !r ) return false;

  QString st;

  st=srv->rcv(); // legge totale

  totale=st.section( config.cmd_sep, 0, 0 );
  resto=st.section( config.cmd_sep, 2, 2 );
  non_prezzati=st.section( config.cmd_sep, 5, 5 );
  coperti=st.section( config.cmd_sep, 4, 4 );
  documenti=QStringList::split( ',', st.section( config.cmd_sep, 3, 3 ) );

  st=srv->rcv(); // legge specifiche comanda BO|BO|num art

  // pre-carica elenco con l'elenco delle portate e coamnde
  elenco=srv->rcv_list();

  riga_portate=( cosa!=t_portate?elenco.size():0 );

  if ( cosa==t_ordine ) {

    if ( !cmd( tok_elenco_portate ) ) return false;

    st=srv->rcv(); // legge totale

    totale=st.section( config.cmd_sep, 0, 0 );
    resto=st.section( config.cmd_sep, 2, 2 );
    documenti=QStringList::split( ',', st.section( config.cmd_sep, 3, 3 ) );

    st=srv->rcv(); // legge specifiche comanda BO|BO|num art

    // pre-carica elenco con l'elenco delle portate e coamnde
    // elenco << srv->rcv_list();   NON IMPELMENTATOI << QStringList
    while ( ( st=srv->rcv() )!=config.cmd_eod ) elenco<<st;
  }

  srv->eop();
  return true;
}

// =========================================================

bool tr::ordine::riga::parse( QString s ) {

  QStringList e=QStringList::split( config.cmd_sep, s, true );
  if ( e.size()<p_last ) return false;

  id_riga=e[p_id_riga].toInt();
  id_istanza=e[p_id_istanza].toInt();
  livello=e[p_livello].toInt();
  id_art=e[p_id_art];
  qta=e[p_qta].toDouble();
  descrizione=e[p_descrizione];
  prezzo=e[p_prezzo].toDouble();
  tot_riga=e[p_tot_riga].toDouble();
  tipo_articolo( e[p_stato].toInt() );
  offerto=e[p_offerto]=="1" ?true:false;
  id_listino=e[p_id_listino].toInt();
  portata=e[p_portata].toInt()==1?true:false;

  return true;

}


// =========================================================
//            TAVOLI e TAVOLO
// =========================================================

bool tr::tavoli::carica( tr::tavoli::tr_ordine ordine ) {

  elenco.clear();

  bool r=false;
  if ( ordine==ordine_nessuno ) r=cmd( tok_elenco_tavoli );
  else
    r=cmd( tok_elenco_tavoli_ordinati, QSN( ordine ) );

  if ( !r ) return false;

  elenco=srv->rcv_list();
  srv->eop();
  return true;

}

bool tr::tavoli::tavolo::parse( QString s ) {
  QStringList e=QStringList::split( config.cmd_sep, s, true );
  if ( e.size()<p_last ) return false;

  id=e[p_id].toInt();
  nome=e[p_nome];
  stato=e[p_stato].toInt();
  pixmap=e[p_pixmap];
  sala=e[p_sala];

  return true;
}

// =========================================================
//         GRUPPI e GRUPPO
// =========================================================

bool tr::gruppi::carica( int da_id, bool espandi, bool solo_articoli ) {
  elenco.clear();

  bool r;
  if ( !espandi ) r=cmd( tok_elenco_gruppi, QSN( da_id ) );
  else
    r=cmd( tok_elenco_gruppi_esteso, QSN( da_id ) );
  if ( !r ) return false;

  QString st;
  if ( !solo_articoli ) elenco=srv->rcv_list();
  else {
    while ( ( st=srv->rcv() )!=config.cmd_eod ) {
      if ( st.section( '|', gruppo::p_id_istanza, gruppo::p_id_istanza ).toInt()>=articolo_base )
        elenco<<st;
    } // while
  } // else

  srv->eop();
  return true;
}

// =========================================================

bool tr::gruppi::gruppo::parse( QString s ) {

  QStringList e=QStringList::split( config.cmd_sep, s, true );

  id_istanza=e[p_id_istanza].toInt();
  codice=e[p_codice];
  prezzo=e[p_prezzo].toDouble();
  descrizione=e[p_descrizione];
  ha_figli=( e[p_ha_figli]=="1" ?true:false );
  tipo_articolo( e[p_tipo_articolo].toInt() );
  stato_extra=e[p_stato_extra].toInt();
  id_articolo=e[p_id_articolo];
  id_padre=e[p_id_padre].toInt();
  id_listino=e[p_id_listino].toInt();
  iva=e[p_iva].toDouble();
  reparto=e[p_reparto].toInt();
  porzioni=e[p_porzioni].toInt();

  return true;

}

// =========================================================

bool tr::gruppi::gruppo::carica( int is ) {

  if ( ::cmd( tok_elenco_gruppo, QSN( is ) ) ) {
    QString st=srv->rcv();
    srv->eop();
    return parse( st );
  }

  /* CMD c = cmd_srv->elenco_articolo(is); if (!cmd_srv->cmd(c)) return false;

  QString st=cmd_srv->rcv(); cmd_srv->sbuffer(); return parse(st); */
  return false;
}

// =========================================================
//         VARIANTI e GRUPPO
// =========================================================

bool tr::varianti::carica( int da_id ) {
  elenco.clear();

  if ( !cmd( tok_elenco_varianti, QSN( da_id ) ) ) return false;

  elenco=srv->rcv_list();
  srv->eop();
  return true;
}

extern void*devnul;

// =========================================================
//         MSE
// =========================================================

tr::mse::mse( TRTokens tid ):tid( tid ) {
  costruttore_comune();
}

tr::mse::mse( TRTokens tid, QString id, QString id1 ):tid( tid ) {
  costruttore_comune();
  carica( id, id1 );
}

void tr::mse::costruttore_comune() {
  struct {
    TRTokens token;
    QString struttura;
  }
  ids[]= {
      {tok_elenco_sale, "@id|$nome<|-n.tav."},
      {tok_elenco_clienti,"@id|cod|$nome<"},
      {tok_elenco_stampanti,"$nome<"},
      {tok_elenco_stampanti_gruppo, "$nome<"},
      {tok_elenco_tavoli, "+id|$nome|-id_sala|-pixmap|-nome_sala"},
      {tok_elenco_tavoli_esteso, "$nome_tavolo|-i_id_sala|-i_tav_x|-i_tav_y|-i_tipo|"
      "-i_n_posti|-i_stato|-i_ruotato|-i_ordine|@id_tavolo|-id_prima_comanda|"
      "-id_tavolata|-id_listino|-pixmap|-id_gruppo_listino|-elenco_tipi_conto|"
      "-i_id_prenotazione"},
      {tok_elenco_tavolo_esteso, "$nome_tavolo|-i_id_sala|-i_tav_x|-i_tav_y|-i_tipo|"
      "-i_n_posti|-i_stato|-i_ruotato|-i_ordine|@id_tavolo|-id_prima_comanda|"
      "-id_tavolata|-id_listino|-pixmap|-id_gruppo_listino|-elenco_tipi_conto|"
      "-i_id_prenotazione"},
      {tok_elenco_camerieri,"@id|$nome<"},
      {tok_elenco_camerieri_esteso,"@id|$nome<|-cod|-pass"},
      {tok_elenco_conti,"@id|$desc|tavolo|cameriere|data_immissione|#totale"},
      {tok_elenco_sale_esteso, "@id|$nome<|-pixmap"},
      {tok_elenco_listini,"@id|-cod|$nome<"},
      {tok_elenco_listini_esteso, "@id|-cod|$nome<"},
      {tok_elenco_messaggi, "@id|desc.grp<|$desc.art"},
      {tok_elenco_clienti_esteso,"@id|-cod|$ragione<|-via|-citta|-cap|-telefono|-fax|"
                                 "-iva|-cf|-data_i|-data_m|-data_f|-pagamenti|-aiva|-diva"},
      {tok_elenco_tipi_conto,"@id|$descr"},
      {tok_elenco_prenotazioni,"@id|-id_tavolo|-tipo|$nome|dataora|-preavviso|^note|#coperti|-stato|-libera_dopo"},
      {tok_egm,"@id|-cod|$descr"},
      {tok_elenco_ricetta,"qta|@id|$desc|-um|-fatt|-mm|-iva"},
      {tok_elenco_fornitori,"@i_id_fornitore|-s_cod_fornitore|$s_ragione_sociale"},
      {tok_elenco_fornitori_esteso,
        "@id|-s_cod_fornitore|$s_ragione_sociale<|-s_via|-s_citta|"
        "-s_cap|-s_telefono|-s_fax|-s_p_iva|-s_c_fiscale|-s_data_inserimento|"
        "-s_data_modifica|-s_data_ultima_fattura|-s_pagamenti_preferiti"},
      {tok_elenco_tag,"@sn|$rftag|-famiglia|-tipo|-idp1|-idp2|-idt1|-idt2|-idt3|-pag|"
                      "-vmin|-vact|-vmax|-pmin|-pact|-pmax|scadenza|-rif|ragsoc|-sc"},
      {tok_elenco_sottoclienti,"-id_padre|@id|$nome|cognome|-matricola|-cf|-nascita"},
      {tok_elenco_sottoclienti_esteso,"-id_padre|@id|$nome|cognome|-matricola|-cf|-nascita"},
      {tok_elenco_doc_fornitore,"-id_for|@id_doc|data_doc|-s_cod_doc|$s_causale_doc|-s_num_tipo_doc|s_desc_tipo_doc|-i_tipo_doc|-s_cameriere|#d_totale_doc"},
      {tok_elenco_gruppi_esteso,"+IS|-COD|-#PRZ|$DESCR<|-FIGLI|-TIPO|-RAGRP|-RIS|!PADRE|-LISTINO|-IVA|-REPARTO|GRPPRN|-PORZ"},
      {tok_elenco_gruppi_stampa,"@id|$descr|-ord"},
      {tok_elenco_associazioni_gruppi_stampa,"+IS|-COD|$DESCR<|-FIGLI|-TIPO|-IDART|!PADRE|-IDGRPPRN|GRPPRN"},
      {tok_statistiche_sale_tavoli,"$Tavolo|#TotOrd|#TotVend|#TotCop|#NConti|#ApertoDa"},
      {tok_elenco_voti,"@id|$descr|#soglia|-pixmap"},
      {tok_elenco_turni,"@id|$descr"},
      };

  TROWI( ids )
  if ( ids[i].token==tid ) {
    struttura=ids[i].struttura;
    break;
  }
  p_id=-1;
  p_desc=-1;
  maschera="";
  if ( !struttura.isEmpty() ) {
    QStringList e=QStringList::split( '|', struttura, true );
    bool sep=false;
    TROWSLI( e ) {
      QString dato=e[i];
      QChar ch=dato[0];
      if ( ch!='-' ) {
        if ( sep ) maschera+='|';
        sep=true;
        switch ( ch ) {
          case '@':
            p_id=i;
            sep=false;
          break;
          case '$':
            maschera+='*';
            if ( p_desc==-1 ) p_desc=i;
          break;
          case '0':
          case '#':
            maschera+='#';
          break;
          case '^':
            maschera+='^';
          break;
          default:
            maschera+="*";
        } // switch
      }
    }
  }
}

QString tr::mse::id() {
  if ( elenco.count()==1 )
    if ( p_id!=-1 ) return elenco[0].section( '|', p_id, p_id );
  return "";
}


QString tr::mse::descrizione() {
  if ( elenco.count()==1 ) {
    if ( p_desc!=-1 ) return elenco[0].section( '|', p_desc, p_desc );
    return elenco[0];
  }
  return "";
}

QString tr::mse::column( int n ) {
  if ( elenco.count()==1 ) return elenco[0].section( '|', n, n );
  return "";
}


bool tr::mse::carica( QString id1, QString id2 ) {

  elenco.clear();

  srv->nolog=true;

  bool r=false;
  if ( tid==tok_elenco_conti ) {
    if ( !devnul )
      r=cmd( tid, id1, QSN( tr::enumeratori::cnt_definitivo ) );
    else
      r=cmd( tid, id1, QSN( tr::enumeratori::cnt_devnul ) );
  }
  else {
    if ( tid==tok_elenco_tavoli_esteso||tid==tok_elenco_menu_listino ) {
      r=cmd( tid, id1, id2 );
      if ( config.verbose<2 ) srv->nolog=true;
    }
    else
      r=cmd( tid, id1, id2 );
  }

  if ( !r ) return false;

  elenco=srv->rcv_list();
  srv->eop();
  return true;

}
