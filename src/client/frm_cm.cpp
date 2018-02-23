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
// from composizione menu
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

frm_cm::frm_cm( QWidget*parent, const char*name, bool modal, WFlags fl ):frm_base( parent, name, modal, fl ) {

  ultimo_prezzo=0;
  ultima_percentuale=0;
  ultimo_modo=1;

  titolo->setText( tr( "composizione menu'" ) );

  //const uint dy =config.finestra_altezza_titolo+ config.finestra_altezza_stato;
  iniz_area( 1, 2 );

  NEW( menu, tr_menu( area, "menu" ) );
  menu->design(); // imposta il modo design
  // seconda connessione per gestione pulsanti locali
  CONNECT( menu );

  NEW( gruppi, tr_listview( area, "gruppi" ) );
  gruppi->setFixedSize(QSize( 800-menu->larghezza(), config.finestra_altezza_centrale ));
  gruppi->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ));
  QFont f=gruppi->font();
  f.setPixelSize( config.menu_elenco_font_size );
  gruppi->setFont(f);

  //gruppi->setMinimumSize( QSize( 800-menu->larghezza(), config.finestra_altezza_centrale ) );
  //gruppi->setMaximumSize( QSize( 800-menu->larghezza(), config.finestra_altezza_centrale ) );
  // con le seguenti non centra ma mette i 2 controlli in alto
  //gruppi->setMinimumSize( QSize( 800-LARGHEZZA_MENU, ALTEZZA_MENU ) );
  //gruppi->setMaximumSize( QSize( 800-LARGHEZZA_MENU, ALTEZZA_MENU ) );
  //gruppi->setGeometry(0, dy ,
  //800-LARGHEZZA_MENU, config.finestra_altezza_centrale );
  gruppi->tipo( tr_listview::articoli );
  precedente_grp_is=0;
  gruppi->carica(tr_listview::e_none, QSN(precedente_grp_is));
  gruppi->onoff(); // un clic seleziona un altro deseleziona

  denu_elenco_gruppi;
  gruppi->highlights<<QSN((enu_elenco_gruppi)p_tipo_gruppo)+" and " +
        QSN(GRUPPO_ARTICOLO_SELEZIONE)+"?##"; //+config.colore_gruppi.name();

  connect( gruppi, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( gruppi_clicked( QListViewItem* ) ) );

  if (FLAGS(p_utente_mancino)) {
    area_layout->addWidget( menu, 0, 0, Qt::AlignCenter );
    area_layout->addWidget( gruppi, 0, 1, Qt::AlignCenter );
  }
  else
  {
    area_layout->addWidget( gruppi, 0, 0, Qt::AlignCenter );
    area_layout->addWidget( menu, 0, 1, Qt::AlignCenter );
  }


  //resize( QSize(800, 600).expandedTo(minimumSizeHint()) );

  /* aggiunta pulsanti */
  tr_label**ps[]= {
    &listino, &annulla, &aggiungi, &varianti, &proprieta, &altro, &cancella, &exit
      };
  QString etichette=TRFORM(
    "listino#listino|annulla#annulla|aggiungi#aggiungi|varianti#varianti|proprieta#proprieta_menu|altro#altro|cancella#cancella|uscita#uscita");

  pulsanti( etichette, false, ps, NULL );

  nascondi( annulla, varianti, altro, proprieta );

  CONNECT( listino );
  CONNECT( aggiungi );
  CONNECT( annulla );
  CONNECT( varianti );
  CONNECT( altro );
  CONNECT( proprieta );
  CONNECT( cancella );
  CONNECT( exit );

  enable_barcode();

  init();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

frm_cm::~frm_cm() {
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm::init() {

  // inizializzazione variabili di classe
  id_listino=glb.stato[tr_globals::p_listino_id].toInt();
  if (id_listino<1) id_listino=1; // listino base

  menu->cambia( id_listino );

  barra_stato->tipo(tr_barra_stato::stato);
  barra_stato->setTavolo( "");
  barra_stato->setSala( "");

  stato_pulsanti();

}

void frm_cm::show() {
  frm_base::show();
  gruppi->adatta();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm::stato_pulsanti() {

  // tenere allineato con ordine pulsanti specificato nel costruttore
  enum {
    p_listino, p_annulla, p_aggiungi, p_varianti, p_proprieta, p_altro, p_cancella
      };

  bool stato[]= {
    true, true, true, true, true, true, true
  };

  // allinea i gruppi visualizzati con quelli del percorso di menu
  tr_menu::tr_key *k=menu->cd();
  int grp_is=0;
  if (k) grp_is=k->is; else grp_is=0;
  if (grp_is==IS_VARIANTI) { // ID_VARIANTI
    if (!gruppi->selezionato().isEmpty()) gruppi->deseleziona(gruppi->selezionato());
    disabilita(gruppi);
    stato[p_aggiungi]=false;
    stato[p_varianti]=false;
    stato[p_altro]=false;
    // nasconde proprieta' su selezionine di un pulsante di menu contenente una variante
    if (gruppi->selezionato().isEmpty()) stato[p_proprieta]=false;
  }
  else {
    abilita(gruppi);
    if (grp_is!=precedente_grp_is) aggiorna_gruppi=true;
    if (aggiorna_gruppi) {
      QString prec_sel;
      if (!gruppi->selezionato().isEmpty()) prec_sel=gruppi->selezionato();
      gruppi->carica(tr_listview::e_none, QSN(grp_is));
      precedente_grp_is=grp_is;
      aggiorna_gruppi=false;
      if (!prec_sel.isEmpty()) gruppi->seleziona(prec_sel);
    }
  }


  tr_label*ps[]= {
    listino, annulla, aggiungi, varianti, proprieta, altro, cancella
      };

  tr::menu::voce*s = menu->selezionata();
  int v = 0;
  if (menu->corrente()) {
    if (!gruppi->selezionato().isEmpty()) gruppi->deseleziona(gruppi->selezionato());
    v=menu->corrente()->is;
  }

  if (!v && gruppi->selezionato().isEmpty()) {
    stato[p_proprieta]=false;
    stato[p_cancella]=false;
  }

  if (!gruppi->selezionato().isEmpty()) {
    abilita(menu);
    tr::gruppi::gruppo grp;
    grp.parse(gruppi->record());
    if (grp.tipo_articolo() & GRUPPO_ARTICOLO_SPECIALE) stato[p_proprieta]=false;
    // if (grp.tipo_articolo() & GRUPPO_ARTICOLO_MESSAGGIO) stato[p_proprieta]=false;
  }

  if ((!s || !s->varianti()) && gruppi->is_grp_varianti().isEmpty()) stato[p_varianti]=false;

  // se il listino non �stato caricato o non esistono listini...
  if ( id_listino==-1 || !gruppi->righe() ) {
    disabilita( gruppi );
    if ( !s && !menu->voci() ) disabilita( menu );
    stato[p_altro]=false;
    stato[p_varianti]=false;
    stato[p_altro]=false;
    stato[p_proprieta]=false;
  }

  if ( id_listino==-1 ) {
    stato[p_aggiungi]=false;
  }

  if ( !annullato() ) stato[p_annulla]=false;

  TROWI( stato )( stato[i]?scopri( ps[i] ):nascondi( ps[i] ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm::exit_clicked() {
  accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77
void frm_cm::menu_clicked() {
  static tr::menu::voce*precedente=NULL;

  if (gruppi->isEnabled() && !gruppi->selezionato().isEmpty()  && menu->corrente() &&
      !menu->corrente()->is) {
    if (!menu->aggiungi(&gruppi->articolo())) BEEP;
  }

  tr::menu::voce*v=menu->selezionata();
  if (v) {
    /*
    if (gruppi->isEnabled() && !gruppi->selezionato().isEmpty() &&
        gruppi->articolo().is!=menu->corrente()->is ) {
          menu->inserisci(&gruppi->articolo());
    }
    */
    if (v->tipo_articolo.gas() &&
      v->locazione==tr::menu::voce::loc_elementi)
      stato->setSala(tr("tocca ancora x aprire"));
    else {
      stato->setSala("");
      if (v==precedente) {menu->deseleziona();v=NULL;}
    }
    precedente=v;
  }

  stato_pulsanti();

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm::altro_clicked() {

  QString etichette;

  tr_menu::tr_key *k=menu->cd();

  // se sei nel main non visualizzare altro all'infuori dei gruppi stampa

  if (k)
  	etichette = TRFORM(("geometria|disponi#dispose|disponi.ABC#dispose_abc|grp.stampa#grp_stampa|stampa;"
                       "geometria\\2x2#2x2|3x1#3x1|3x3#3x3|4x4#4x4|5x6#5x6;"
                       "stampa\\gruppi#gruppi|varianti#varianti"));
  else
  	etichette = TRFORM(("grp.stampa#grp_stampa|stampa;"
                        "stampa\\gruppi#gruppi|varianti#varianti"));

  QString n=tools.hvmenu( altro, etichette );
  if ( n.isNull() ) return;

  if (n=="dispose") menu->disponi( gruppi->elenco_dati() );
  if (n=="dispose_abc") menu->disponi( gruppi->elenco_dati(true) );
  if (n=="2x2") menu->geometria( 4 );
  if (n=="3x1") menu->geometria( 3 );
  if (n=="3x3") menu->geometria( 9 );
  if (n=="4x4") menu->geometria( 16 );
  if (n=="5x6") menu->geometria( 30 );
  if (n=="grp_stampa") frm_gruppi_stampa();
  if (n=="gruppi" || n=="varianti") frm_stampa(n);

  stato_pulsanti();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm::listino_clicked() {

  FORM( f, frm_cm_listini );

  bool b=f->accepted();
  QString id=f->id();
  QString desc=f->descrizione();

  xFORM( f, frm_cm_listini );

  if ( b&&id_listino!=id.toInt() ) {
    if (CMD(tok_imposta_listino, QSN(id.toInt()) )) {
      id_listino=id.toInt();
      barra_stato->setListino( desc );
      gruppi->carica();
      menu->cambia( id_listino,true,false );
      /*
    glb.stato[tr_globals::p_listino_id]=QSN(id_listino);
    glb.stato[tr_globals::p_listino_descrizione]=f->descrizione
    */
    }
    else id_listino=-1;

    stato_pulsanti();
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm::varianti_clicked() {
  menu->cd(tr_menu::varianti);

  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

bool frm_cm::annullato() {
  return menu->annullato();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm::annulla_clicked() {
  menu->annulla();
  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

// il cancella ha 3 funzioni:
// 1. cancellare tutto il menu se nessun articolo o voce è selezionato
// 2. cancellare il gruppo se è selezionato (questo cancella in automatico la voce)
// 3. cancellare la voce di menu se è selezionata
void frm_cm::cancella_clicked() {
  enum tr_azioni {clr_gruppo=1, clr_voce, clr_menu} az=(tr_azioni)0;

  tr_menu::tr_key *cd=menu->cd();
  tr::menu::voce *v=NULL;
  if (cd) v=menu->voce(cd->is);

  if ( !gruppi->selezionato().isEmpty() ) az=clr_gruppo;
  if ( menu->selezionata() ) az=clr_voce;
  if ( gruppi->selezionato().isEmpty() && !menu->selezionata()) az=clr_menu;
  if ( v && v->is==IS_VARIANTI && menu->selezionata() ) {
    az=clr_gruppo;
    v=menu->selezionata();
  }

  // preparazione msg di controllo
  QString msg;
  QString cosa;
  switch (az) {

  default:
    tools.msgbox(tr("Situazione ambigua nella cancellazione<br>"));
    return;

  case clr_gruppo:
    cosa=tr("gruppo/articolo");
    msg=tr("Attenzione\nCancellando il %cosa% <B>\"" );
    if (v && v->is_padre==IS_VARIANTI) msg+=v->descrizione_originale;
    else msg+=gruppi->articolo().descrizione;
    msg+=tr( "\"</B>  si cancellano tutti i suoi aggregati<br> e le relative voci di menu.<br><br>Si e' sicuri?");
    break;

  case clr_voce:
    cosa=tr("voce");
    msg=tr("Attenzione\nCancellando la %cosa% <B>\"" )+
        menu->selezionata()->descrizione_originale+
        tr( "\"</B>  si cancellano le eventuali sotto voci di menu<br>"
            "tranne quelle presenti in corolla<br>Si e' sicuri?");
    break;

  case clr_menu:
    cosa=tr("MENU");
    msg=tr("ATTENZIONE<br><b>Verra' <u>cancellato</u></b><br>TUTTO il %cosa%<br>Si e' sicuri?");
    break;

  }

  // conferme

  msg.replace("%cosa%",cosa);
  if (tools.msgbox( msg, SINO )!=SI) return;

  if (config.beginner && az!=clr_voce) {
    msg=tr( "Secondo controllo per cancellazione<br><br>Si vuole tenere il %cosa%?" );
    msg.replace("%cosa%",cosa);
    if (tools.msgbox( msg, SINO )==SI) return;
  }

  // if ( r==falso) return;

  // cancellazione


  switch (az) {

  case clr_gruppo: {
      int is=-1;
      if (v && v->is_padre==IS_VARIANTI) is=v->is;
      else is=gruppi->articolo().is;
      nascondi( annulla );
      if ( CMD( tok_rimuovi_gruppo,QSN(is), QSN(id_listino) )) {
        menu->cancella(is);
        aggiorna_gruppi=true;
      }
    }
    break;

  case clr_voce:
    if (menu->cancella()) {
      scopri( annulla );
      aggiorna_gruppi=true;
    }
    break;

  case clr_menu:
    menu->rimuovi();
    nascondi( annulla );
    menu->cambia( id_listino );

    break;

  }

  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm::aggiungi_clicked() {

  const int gas=GRUPPO_ARTICOLO_SELEZIONE;
  const int gap=GRUPPO_ARTICOLO_PURO;
  const int gac=GRUPPO_ARTICOLO_SELEZIONE|GRUPPO_ARTICOLO_PURO;
  const int gav=GRUPPO_ARTICOLO_VARIANTE;
  const int gapv=gap|gav;
  const int gasv=gas|gav;

  bool ricarica=false;

  TRDiritti::TRDirittiUtente diritti[]={
    TRDiritti::p_none,TRDiritti::p_none,TRDiritti::p_null,
    TRDiritti::p_null,TRDiritti::p_none};
  enum {p_gruppo,p_articolo,p_composto,p_affilia,p_variante} n;
  QString etichette=TRFORM("gruppo#gruppo|articolo#articolo|art.composto#art.composto|affilia#affilia|variante#variante");

  tr::menu::voce* padre = NULL;
  if (menu->cd()) padre = menu->voce(menu->cd()->is);

  // il sotto-gruppo di articolo o variante non e' ammesso
  if ( padre&&padre->tipo_articolo.gapv() )
    diritti[p_gruppo]=TRDiritti::p_null;

  // l'articolo di articolo non e' permesso
  if ( padre&&padre->tipo_articolo.gap() )
    diritti[p_articolo]=TRDiritti::p_null;

  // la variante di variante non e' ammessa
  if ( padre&&padre->tipo_articolo.gav() )
    diritti[p_variante]=TRDiritti::p_null;

  // la variante di variante non e' ammessa
  if ( !gruppi->righe() )
    diritti[p_variante]=TRDiritti::p_null;

  // listino vuoto o main menu
  if ( !padre )
    diritti[p_articolo]=TRDiritti::p_null;

  QString voce=tools.hvmenu(
      aggiungi, etichette, diritti );
  if ( voce.isEmpty() ) return;

  // variabili parametro per comando (in esatta sequenza)
  int ereditarieta=0;
  int id_padre=0;
  if ( padre ) {

    id_padre=padre->id_istanza;
  }

  QString codice=LEAVE;
  QString descrizione="";
  money prezzo;
  if ( !ultimo_modo ) prezzo=ultima_percentuale;
  else
    prezzo=ultimo_prezzo;
  money percentuale=0;
  money iva=0;
  int tipo=0;
  QString stampanti="";
  int id_listino_padre=id_listino;
  QString reparto="0";
  QString barcodes=QString::null;

  /*
  x TEST
  */

  if (voce=="gruppo") {
      tipo|=gas;
      n=p_gruppo;
  }

  if (voce=="articolo") {
      tipo|=gap;
      if ( menu->cd() ) { // se sara' un art. di un gruppo ...
        ereditarieta=255;
      }
      n=p_articolo;
  }

  if (voce=="art.composto") {
      tipo|=gac;
      if ( menu->cd() ) { // se sara' un art. di un gruppo ...
        ereditarieta=255;
      }
      n=p_composto;
  }

  if (voce=="affilia") {
    tools.msgbox( tr( "Funzione non ancora attivata" ) );
    return;
  }

  if (voce=="variante") {
    ereditarieta=255;
    tipo|=gapv;
    /*
    if ( !ultimo_modo ) {
      fv->prezzo_tipo->numero(0); //ultima_percentuale;
      fv->prezzo->setLabel( tr( "%" ) );
    }
    */
    n=p_variante;
  }

  tr::gruppi::gruppo grp(tipo);
  grp.prezzo=prezzo;
  modifica_gruppo(grp);
  stato_pulsanti();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

/* questa funzione riconosce se grp e' un nuovo gruppo o gia' esistente
quindi da modificare. Riconosce il tipo e crea la finestra pie' adatta.
Grazie alle nuove funzioni, questa funzione sostituisce le 5 classi:
frm_agg,frm_agg_art,frm_agg_var,frm_agg_grp, frm_cm_mod*/

void frm_cm::modifica_gruppo(tr::gruppi::gruppo& grp)
{

  denu_elenco_gruppi;

  const int gas=GRUPPO_ARTICOLO_SELEZIONE;
  const int gap=GRUPPO_ARTICOLO_PURO;
  const int gac=GRUPPO_ARTICOLO_SELEZIONE|GRUPPO_ARTICOLO_PURO;
  const int gav=GRUPPO_ARTICOLO_VARIANTE;
  const int gapv=gap|gav;
  const int gasv=gas|gav;

  bool ricarica=false;
  int ereditarieta=255;

  /*
  %specifica è grp.descrizione
  */
  enum {e_grp,e_art,e_var} elem;
  enum {o_ins,o_mod} op;

  QString risult=QSNull;
  QString barcodes;

  if (grp.isNull()) op=o_ins;
  else op=o_mod;
  if (grp.ta->gap()) elem=e_art;
  if (grp.ta->gav()) elem=e_var;
  if (grp.ta->gas()) elem=e_grp;

  QString s_elem;
  switch (elem) {
    case e_art: s_elem=tr("articolo");break;
    case e_grp: s_elem=tr("gruppo");break;
    case e_var: s_elem=tr("variante");
  }

  QString s_op;
  switch (op) {
    case o_ins: s_op=tr("Aggiunta a ");break;
    case o_mod: s_op=tr("Modifica di ");break;
  }

  tr::menu::voce* vcd;
  if (menu->cd()) vcd=menu->voce(menu->cd()->is); else vcd=NULL;

  // determinazione padre

  QString s_spec="";

  /*
  if (vcd && vcd->is!=-1) {
    if ( vcd ) {
      s_elem.append(vcd->descrizione_originale);
      if (grp.is_padre!=vcd->is) grp.is_padre=vcd->is;
    } else s_elem.append("MAIN");
  }
  else {
    if (grp.id_padre==-1) grp.id_padre=0;
    s_elem.append("MAIN");
  }
  */
  if (vcd && vcd->is!=-1 && (vcd->is==grp.is_padre || grp.is_padre==-1)) {
    if ( vcd ) {
      s_spec=(vcd->descrizione_originale);
      if (grp.is_padre!=vcd->is) grp.is_padre=vcd->is;
    } else s_spec=TR("MAIN");
  }
  else {
    s_spec=TR("MAIN");
    if (grp.id_padre==-1) grp.id_padre=0;
    else {
      denu_elenco_gruppo;
      QString rcv=::rcv(tok_elenco_gruppo,QSN(grp.id_padre));
      s_spec=rcv.section('|',p_descrizione,p_descrizione);
    }
  }

  QString str_fimgav=TRFORM((
  // "minmax=400|200|95%|90%\n"
  "status.notr=%s_op% %s_elem% |%s_spec%|\n"
  "input=Codice#code(articolo.codice);Nome o Descrizione#name(articolo.descrizione,oneline);#name;#name;#name\n"
  "listview=Stampa su:#printers(tok:%tok_elenco_stampanti%,tutti,noexit)\n"

  "?art,!modmsg:input=Prezzo#price(listino_detail.prezzo_base,alato,0);price;"
    "IVA%#vat(listino_detail.iva,alato,%config.iva%);"
    "Reparto#dep(listino_detail.reparto,alato,0)\n"

  "?var:input=Prezzo#price(listino_detail.prezzo_base,alato,0);price;"
    "label=Tipo#type(radio:perc.|*fisso,onclick);"
    "input=IVA%#vat(listino_detail.iva,alato,%config.iva%)\n"

  "!grp,ric,!modmsg:input=QTA#qta(val:real3);UM#um;fattore#factor(val:real3);MM#mm;IVA.Carico#load_vat(listino_detail.iva,oneline)\n"

  "?art,!mod,!modmsg:label=articolo a peso#weight(chk:off,onclick,dim:%w%,oneline);weight;messaggio#msg(chk:off,dim:%w%,oneline);chiedi prezzo#ask_prz(chk:off,dim:%w%,oneline);input=Porzioni#nporzioni(val:int,alato)\n"
  "?var,!mod:label=+/- [aggiungi/togli]#moreless(chk:off,dim:%w%);moreless;commento#comment(chk:off,dim:%w%);specializza#spec(chk:off,dim:%w%)\n"

  "!grp,!modmsg,?mod:input=Porzioni#nporzioni(val:int,alato,oneline);#nporzioni;hole=;hole=;hole=\n"

  "?grp:buttons=ok#ok|uscita#exit\n"
  "!grp:buttons=ok#ok|?mod:immagine#draw|?bcs:barcode#barcode|?ric:ricetta#recipe|"
    "?ric:magaz.#whouse|?mod:x stamp.#lang|uscita#exit\n"

  "?grp:links=name|code|printers\n"
  "?art:links=name|code|printers|price|vat|qta|um|factor|mm|dep|weight|msg|load_vat"
    "|ask_prz|nporzioni|ok|barcode|recipe|whouse|image\n"
  "?art:links=ask_prz|price\n"
  "?art:links=printers|msg\n"
  "?var:links=name|code|printers|price|type|vat|qta|um|factor|mm|load_vat"
    "|ok|barcode|recipe|whouse|moreless|comment|spec|image\n"
  "?var:links=comment|price|type|vat|moreless|load_vat|qta|um|factor|mm\n"
  "?art:links=msg|price(clear)|type|vat|moreless|dep|load_vat|weight|ask_prz|nporzioni|qta|um|factor|mm|barcode\n"
  "?var:links=spec(chk:off)|printers\n"
  ));

  FRM_PARAMS;
  FRM_PARAM("%w%","|"+QSN(config.altezza_riga)+"||"+QSN(config.altezza_riga));
  FRM_PARAM("%s_op%",s_op);
  FRM_PARAM("%s_elem%",s_elem);
  FRM_PARAM("%s_spec%",s_spec);
  FRM_PARAM("%config.iva%",QSN(config.iva));
  FRM_PARAM("%tok_egm%",QSN(tok_egm));
  FRM_PARAM("%tok_elenco_stampanti%",QSN(tok_elenco_stampanti));

// su grp deselez tutte stampanti
//

  QString conds;
  if (elem==e_art) {
    conds="art";
    grp.tipo_articolo(gap);
  }
  if (elem==e_grp) {
    conds="grp";
    grp.tipo_articolo(gas);
  }
  if (elem==e_var) {
    conds="var";
    grp.tipo_articolo(gapv);
  }
  if (MODULO(modulo_barcode)) conds+="|bcs";
  if (op==o_mod) conds+="|mod";
  if (MODULO(modulo_ricettario) && op!=o_ins) conds+="|ric";
  if (op==o_mod && (grp.tipo_articolo() & GRUPPO_ARTICOLO_MESSAGGIO)) {
    conds.replace("|bcs","");
    conds.replace("|ric","");
    conds+="|modmsg";
  }
  if (!conds.isEmpty()) str_fimgav.prepend("conds="+conds+"\n");

  bFORM(fimgav,str_fimgav,this);

  CTRL_FRM(fimgav);

  CTRL(code);
  CTRL(name);
  enum { tipo_percentuale, tipo_fisso };
  CTRL(price);
  CTRL(vat);
  CTRL(dep);
  CTRL(draw);
  CTRL(qta);
  CTRL(um);
  CTRL(factor);
  CTRL(mm);
  CTRL(load_vat);
  CTRL(printers);
  CTRL(weight);
  CTRL(msg);
  CTRL(moreless);
  CTRL(comment);
  CTRL(spec);
  CTRL(type);
  CTRL(recipe);
  CTRL(whouse);
  CTRL(ask_prz);
  CTRL(nporzioni);
  CTRL(ok);
  CTRL(barcode);
  CTRL(lang);
  CTRL(exit);

  /*
  tr_input *code,*name,*price,*vat,*dep,*qta,*um,*factor,*mm;
  tr_listview *printers;
  tr_label *weight,*msg,*moreless,*comment,*spec,*type,*recipe,*whouse;

  tr_ctrl ctrls[]={
    &code,&name,&price,&vat,&dep,&qta,&um,&factor,&mm,&printers,&weight,
    &msg,&moreless,&comment,&spec,&type,&recipe,&whouse};

  fimgav->pointers(
    "code|name|price|vat|dep|qta|um|factor|mm|printers|weight|msg|"
    "moreless|comment|spec|type|recipe|whouse",ctrls);
  */

  /*QStringList elenco=QStringList::split('|',"ANACONDA|SCARPA|SCREMARE");
  name->input->elenco(elenco);*/

  printers.listview->setMinMax( 380, config.elenchi_altezza_riga*4+config.elenchi_altezza_riga/2, 32767,
                    config.elenchi_altezza_riga*4+config.elenchi_altezza_riga/2 );

  QString stampanti_padre=printers(",");

  if (op==o_ins && elem!=e_grp) fimgav->sets("qta|um|factor|mm|load_vat","1|N|1|N|"+QSN(config.iva));

  if (op==o_ins) {
    if (grp.iva==-1) grp.iva=config.iva;
    vat.value(grp.iva);
  }

  if (elem!=e_grp) {
    if (op==o_mod) {
      tr::mse stampanti_gruppo(tok_elenco_stampanti_gruppo,QSN(grp.is),LEAVE);
      printers.value(stampanti_gruppo.elenco);
      stampanti_padre=printers(",");
    }
    else {
      tr::mse stampanti_gruppo(tok_elenco_stampanti_gruppo,QSN(grp.is_padre),LEAVE);
      printers.value(stampanti_gruppo.elenco);
      stampanti_padre=printers(",");
    }
  }
  else {
      ereditarieta = 0;
      if (grp.id_padre!=0 || grp.is!=-1)
      {
      tr::mse stampanti_gruppo(tok_elenco_stampanti_gruppo);
      stampanti_gruppo.carica((op==o_ins) ? QSN(grp.id_padre) : QSN(grp.is),LEAVE);
      printers.value(stampanti_gruppo.elenco);
      stampanti_padre=printers(",");
      }
      else printers.listview->seleziona(tr_listview::e_none);

  }


  bool disab_ric=false;
  bool disab_ric_pulsante=false;
  tr::mse ricetta(tok_elenco_ricetta);

  if (op==o_mod) {
    code.value(grp.codice);
    name.value(grp.descrizione);

    int dec;
    QString val;
    if (grp.ta->gpeso()) {
      dec=3;
      val="real3";
      weight.value(true);
    }
    else
    {
      dec=2;
      val="real2";
    }

    price.value( config.formatta(grp.prezzo, dec) );
    price.validatore(val);

    if (grp.iva==-1) grp.iva=config.iva;
    vat.value(grp.iva);
    dep.value(grp.reparto);

    if (MODULO(modulo_barcode) && ::cmd(tok_elenca_barcode,QSN(grp.is))) {
      QStringList bcs=srv->rcv_list();
      srv->eop();
      barcodes=bcs.join("|");
    }

  if (MODULO(modulo_ricettario)) {
    bool con_ricetta=grp.tipo_articolo() & GRUPPO_ARTICOLO_CON_RICETTA;
    if (con_ricetta) disab_ric=true;
    else {
      ricetta.carica(QSN(grp.is));
      denu_elenco_ricetta;
      tr::mse::riga r(ricetta.elenco[0]);
      if (r[p_id_istanza_base].toInt()==grp.is) {
        QString data=r[p_qta]+"|"+r[p_um]+"|"+r[p_fattore]+"|"+r[p_mm]+"|"+r[p_iva];
        fimgav->sets("qta|um|factor|mm|load_vat",data);
        disab_ric_pulsante=true;
      }
    } // else ricetta
  } // modulo ricetta
  else
    disab_ric=true;
  if (disab_ric) fimgav->disabilita(qta,um,factor,mm,load_vat);
  if (disab_ric_pulsante) fimgav->nascondi(recipe); else fimgav->scopri(recipe);

  } // if op=o_mod

  if (nporzioni) nporzioni.value(grp.porzioni);

  /************************
  * inizio interazione
  */

  tr_widget* iterato=fimgav->interazione();

  while (iterato) {

    // ==============
    if (iterato==draw) {
      denu_elenco_immagine_articolo;
      QString rcv=srv->rcv(tok_elenco_immagine_articolo,grp.id_articolo);

      QString str_img=TRFORM((
        // "minmax=480|200\n"
        "palette=3\n"
        "margine=0\n"
        "status=Immagine associata a |%name%\n"
        "extra=canvas\n"
        "buttons=ok#ok|pulisci#polish|disegna#draw|cancella#cancel|exit#exit" ));

      FRM_PARAMS;
      FRM_PARAM("%name%",name());
      bFORM( img, str_img, fimgav );
      CTRL_FRM ( img );
      CTRL( ok );
      CTRL( polish );
      CTRL( cancel );
      CTRL( draw );
      CTRL( exit );
      CTRL( canvas );
      img->nascondi(draw);

      canvas=rcv;

      while (img->interazione()!=NULL) {
        if (img->iterato==ok) {
          QString data=canvas();
          if (CMD(tok_associa_immagine_articolo,grp.id_articolo,data)) break;
        }
        if (img->iterato==polish) canvas->canvas->clear();
        if (img->iterato==cancel) {
          canvas->canvas->erase();
          img->nascondi(cancel);
          img->scopri(draw);
          }
        if (img->iterato==draw  ) {
          canvas->canvas->draw ();
          img->nascondi(draw);
          img->scopri(cancel);
          }
        if (img->iterato==exit) break;
      } // while
      xFORM( img, str_img );

    }

    // ==============
    if (iterato==type) {
      if ( type.label->number()==tipo_percentuale )
        price.caption( tr( "%" ) );
      if ( type.label->number()==tipo_fisso )
        price.caption( tr( "Prezzo" ) );
    }

    // ==============
    if (iterato==weight) {
      if (weight.isOn()) {
        price->validatore("real3");
        price->value( config.formatta(price->value(), 3));
      }
      else {
        price->validatore("real2");
        price->value( config.formatta(price->value(), 2));
      }
    }

    // ==============
    if (iterato==barcode) {
      QString frm_barcode=TRFORM((
        "palette=3\n"
        "status=Inserimento/Modifica codici a barre\n"
        "input=1(alato,barcode);2(alato,barcode)\n"
        "input=3(alato,barcode);4(alato,barcode)\n"
        "input=5(alato,barcode);6(alato,barcode)\n"
        "input=7(alato,barcode);8(alato,barcode)\n"
        "input=9(alato,barcode);A(alato,barcode)\n"
        "buttons=ok#ok|uscita#uscita"));
      FRM_PARAMS;
      bFORM(f,frm_barcode,fimgav);
      tr_widget*ok=f->pointer("ok");
      tr_widget*uscita=f->pointer("uscita");

      QString parametri="1|2|3|4|5|6|7|8|9|a";
      if (!barcodes.isNull())
              f->sets(parametri,barcodes.replace(',','|'));

      tr_widget* iterato=f->interazione();

      while (iterato!=NULL) {

        if (iterato==ok) {
            QStringList bcs=QStringList::split('|',f->gets(parametri),false);
            if (bcs.count()>0) barcodes=bcs.join(",");
            else barcodes="";
            iterato=NULL;
        }

        if (iterato==uscita) iterato=NULL;

        if (iterato) iterato=f->interazione();

      }

      xFORM(f,frm_barcode);

    } // barcode

    // ============== lang
    if (iterato==lang) {
      if (::cmd(tok_elenco_descrizioni_articolo,grp.id_articolo)) {
        QStringList elenco=srv->rcv_list();
        srv->eop();
        QString frm_descr=TRFORM((
          "palette=3\n"
          //"minmax=80%|40%\n"
          "status=Inserimento/Modifica descrizioni stampa\n"));

        TROWSLI(elenco) {
          QString st="input=%1#%2(alato)\n";
          st=st.replace("%1",elenco[i].section('|',1,1));
          st=st.replace("%2",QSN(i));
          frm_descr+=st;
        }

        frm_descr+=TRFORM(("buttons=ok#ok|uscita#uscita"));

        FRM_PARAMS;
        bFORM(f,frm_descr,fimgav);
        CTRL_FRM(f);

        CTRL(ok);
        CTRL(uscita);
        CTRL(descr);

        // compila campi per modifica
        TROWSLI(elenco) {
          tr_input *p=(tr_input*)f->pointer(QSN(i));
          if (p) p->setText(elenco[i].section('|',3,3));
          else cout << tr("Non trovato puntatore per ") << QSN(i) << endl;
        }

        f->interazione();
        while (f->iterato!=NULL) {
          if (f->iterato==ok) {
            TROWSLI(elenco) {
              tr_input *p=(tr_input*)f->pointer(QSN(i));
              if (!CMD(tok_modifica_descrizioni_articolo,
                grp.id_articolo,
                elenco[i].section('|',0,0),
                elenco[i].section('|',2,2),
                p->text())) break;
            } // trowsli
            f->iterato=NULL;
          } // ok
          if (f->iterato==uscita) f->iterato=NULL;
          if (f->iterato) f->interazione();
        }
        xFORM(f,frm_descr);

      } // cmd
      else srv->eop();
    } // iterato==lang

    // ============== OK
    if (iterato==ok || ((iterato==recipe || iterato==whouse) && op==o_ins)) {

      if (elem!=e_grp && (
          (!qta().isEmpty() && qta().toDouble()<=0) ||
          (!factor().isEmpty() && factor().toDouble()<=0))) {
        tools.msgbox(tr("qta e fattore devono essere positivi"));
        iterato=fimgav->interazione();
        continue;
      }


      QString stampanti=printers(",");
      if (!printers.listview->isEnabled()) stampanti="";

      if ((iterato==recipe || iterato==whouse ) && op==o_ins) {
        if (tools.msgbox(tr(
          "Attenzione<br>Questa operazione richiede il salvataggio "
          "dell'articolo/variante/gruppo.<br>Si vuole proseguire?"),SINO)!=SI) {
          iterato=fimgav->interazione();
          continue;
        }
      }
      else
        iterato=NULL;

      QString prz,prc;

      if (elem==e_var) {
        int n=type.value().toInt();
        if ( n==0 ) {
          prc=price(); prz="";
        }
        else {
          prz=price(); prc="";
        }

        if ( moreless.isOn() ) grp.tipo_articolo(GRUPPO_ARTICOLO_PIUMENO);
        if ( comment->isOn() ) {
          grp.tipo_articolo(GRUPPO_ARTICOLO_COMMENTO);
          code.value("");
          vat.value(0);
        }

      }
      else
      {
        if (ask_prz.isOn()) {
          grp.tipo_articolo(GRUPPO_ARTICOLO_PREZZO_RICHIESTO);
          grp.tipo_articolo(GRUPPO_ARTICOLO_SPECIALE);
        }
        else prz=price();
      } // elem!=e_var

      bool registrato=false;

      // gestione stampanti ed ereditarieta'
      if (printers.listview->isEnabled() && (elem!=e_grp))
	{
		// anomalia: un gruppo di primo livello che
		// deve uscire su tutte le stampanti ha, di fatto,
		// le stesse stampanti di suo padre (che non esiste
		// in quanto il gruppo 0 non esiste :-) quindi anche
		// in quel caso non può ereditare le stampanti del padre

		 if (stampanti!=stampanti_padre || !grp.id_padre || msg.isOn())
		        ereditarieta^=tr_comanda::er_prn_comanda;
		 else
		 {
        		if (op == o_ins) stampanti="";
		        ereditarieta|=tr_comanda::er_prn_comanda;
		 }
	}
      if (op==o_ins) {

        grp.id_listino=id_listino;

        grp.is=0;

        if (weight->isOn()) grp.tipo_articolo(GRUPPO_ARTICOLO_A_PESO);
        if (msg->isOn()) grp.tipo_articolo(GRUPPO_ARTICOLO_MESSAGGIO | GRUPPO_ARTICOLO_CONSOLIDANTE);

        // se e' un inserimento delle varianti e non c'e' il gruppo, prima lo crea

        if ( elem==e_var ) {
          int id_varianti=0;
          //if ( padre ) id_varianti = padre->id_varianti_articolo;
          if ( !gruppi->is_grp_varianti().isEmpty() ) id_varianti=gruppi->is_grp_varianti().toInt();
          if ( id_varianti<1 ) {

            // ============ inserimento grp VARIANTI

            if (::cmd(tok_modifica_gruppo,QSN(255),"0",QSN(grp.id_padre),
                      "",
                      tr("VARIANTI"),"0","0","0",QSN(gasv),"", QSN(grp.id_listino),
                      QSN(grp.id_listino),"0" )) {
              grp.id_padre=srv->rcv().section('|',0,0).toInt();
              ricarica=true;
            }

            else {
              tools.msgbox( tr( "Riscontrate difficolta' nella creazione del gruppo varianti" ) );
              break;
            }
          }
          else
            grp.id_padre=id_varianti;
        } // if elem=e_var

      // =============================================
      // ============ inserimento o_ins ==============
      // =============================================

      if (::cmd(tok_modifica_gruppo,QSN(ereditarieta),QSN(grp.is),
              QSN(grp.id_padre), code(), name(), prz,prc,
              vat(), QSN(grp.tipo_articolo()), stampanti,
              QSN(grp.id_listino), QSN(grp.id_listino), dep())) {

        QString rcv=srv->rcv();
        grp.is=rcv.section('|',0,0).toInt();
        grp.id_articolo=rcv.section('|',1,1);
        srv->eop();
        registrato=true;
        aggiorna_gruppi=true;
        ricarica=false;
        }
      else
        srv->eop();

      } // if op=o_ins

      if (op==o_mod) {

        // =============================================
        // ============ inserimento o_mod ==============
        // =============================================

        if (CMD(tok_modifica_gruppo,QSN(ereditarieta),QSN(grp.is), QSN(grp.id_padre),
          code(), name(), price(),"0",vat(),QSN(grp.tipo_articolo()),
          stampanti,QSN(grp.id_listino),"0",dep()))
        {
          registrato=true;
          aggiorna_gruppi=true;
        }
      } // o_mod

      //
      // barcode e ricetta e extra
      //
      if (registrato) {

        if (MODULO(modulo_barcode)) {
          if (!barcodes.isEmpty()) CMD(tok_modifica_barcode,QSN(grp.is),barcodes);
          else CMD(tok_modifica_barcode,QSN(grp.is),"NULL");
        }

        if (MODULO(modulo_ricettario)) {
          if (elem!=e_grp && (qta()!="1" || um()!="N" ||
                            factor()!="1" || mm()!="N")) {
            if (qta()=="") {
              um.value("");
              factor.value(QString(""));
              mm.value("");
              load_vat.value("");
            }
            if (CMD(tok_modifica_ricetta,grp.id_articolo,qta(),grp.id_articolo,um(),factor(),mm(),load_vat()))
              aggiorna_gruppi=true;
          }
        } // if modulo_ricettario

        ::CMD(tok_modifica_porzioni,grp.id_articolo,nporzioni.value());
      } // if registrato

      stato_pulsanti();
    } // ok

    // ==============
    if (iterato==recipe) {
      denu_elenco_ricetta;
      QString str_recipe=TRFORM((
        "minmax=93%|70%|93%|80%\n"
        "palette=3\n"
        "status=Inserimento/Modifica ricetta di |%art% \n"
        "input=Articolo#art(min:100,tok:%egm%);art;QTA#qta(ricette_magazzino.quantita);hole=/*;"
          "UM#um;fattore#factor(val:real3);MM#mm;"
          "IVA.Carico#load_vat(listino_detail.iva,oneline)*/\n"
        "listview=(header:ARTICOLO|QTA/*|UM|FATT|MM|IVA*/,onclick,tok:%tok%)\n"
        "buttons=ok#ok|cancella#cancel|uscita#uscita\n"
        "links=art|qta|ok\n"
        "links=listview|cancel\n"
        ));

      FRM_PARAMS;
      FRM_PARAM("%egm%", QSN(tok_egm));
      FRM_PARAM("%art%", grp.descrizione);
      FRM_PARAM("%tok%", QSN(tok_elenco_ricetta)+"|"+grp.id_articolo);

      bFORM(f,str_recipe,fimgav);
      CTRL_FRM(f);
      CTRL(art);
      CTRL(qta);
      //CTRL(um);
      //CTRL(mm);
      //CTRL(factor);
      //CTRL(load_vat);
      CTRL(listview);
      CTRL(ok);
      CTRL(cancel);
      CTRL(uscita);

      tr_widget* iterato=f->interazione();

      while (iterato!=NULL) {

        if (iterato==ok) {
          if (!art.input->_elenco.id.isEmpty()) {
            if (CMD(tok_modifica_ricetta,
              grp.id_articolo, qta(), art.input->_elenco.id/*,
            um(),factor(),mm(),load_vat()*/)) {
              listview.listview->aggiorna();
              aggiorna_gruppi=true;
            }
          }
        }

        if (iterato==cancel) {
          if (CMD(tok_modifica_ricetta,
            grp.id_articolo,QSN(0),listview(),
            LEAVE,LEAVE,LEAVE,LEAVE)) {
            listview.listview->aggiorna();
            aggiorna_gruppi=true;
          }
        }

        if (iterato==listview) {
          QString data=listview.listview->record();
          art.value(data.section('|',p_descrizione_istanza_base,p_descrizione_istanza_base));
          qta.value(data.section('|',p_qta,p_qta));
          //um.value(data.section('|',p_um,p_um));
          //mm.value(data.section('|',p_mm,p_mm));
          //factor.value(data.section('|',p_fattore,p_fattore));
          //load_vat.value(data.section('|',p_iva,p_iva));
          art.input->_elenco.id=listview();
        }

        if (iterato==uscita) iterato=NULL;

        if (iterato) iterato=f->interazione();

      }

      if (listview.listview->righe()) disab_ric=true; else  {
        if (CMD(tok_modifica_ricetta,grp.id_articolo,"",grp.id_articolo,LEAVE,LEAVE,LEAVE,LEAVE))
        aggiorna_gruppi=true;
        disab_ric=false;
      }

      xFORM(f,frm_barcode);
    }

    // ==============
    if (iterato==whouse) {
      QString str_magazzino=TRFORM((
        //"minmax=80%|60%\n"
        "palette=3\n"
        "status=Carico per articolo| %art%\n"
        "validatore=giacenze_magazzino\n"
        "hole=60;input=Giacenza iniziale#start_qta(.giacenza_iniziale);Prezzo iniziale#start_value(.valore_iniziale)\n"
        "label=Soglia<br>scorta;input=in esaurimento#ex_qta(.soglia_scorta_in_esaurimento);esaurita#out_qta(.soglia_scorta_esaurita)\n"
        "buttons=ok#ok|uscita#exit\n"
        "links=start_qta|start_value|ex_qta|out_qta|ok"
        ));
      FRM_PARAMS;
      FRM_PARAM("%art%", grp.descrizione);


      bFORM(whouse,str_magazzino,fimgav);
      CTRL_FRM(whouse);
      CTRL(start_qta);
      CTRL(start_value);
      CTRL(ex_qta);
      CTRL(out_qta);
      CTRL(ok);
      CTRL(exit);

      tr_widget* iterato=whouse->interazione();

      while (iterato!=NULL) {
        if (iterato==ok) {
          QString id_magazzino="1";
          if (CMD(tok_modifica_giacenza_magazzino,
                  grp.id_articolo,id_magazzino,
                  tools.nz(start_qta(),"0"),
                  tools.nz(start_value(),"0"),
                  tools.nz(ex_qta(),"0"),
                  tools.nz(out_qta(),"0"))) {
                    iterato=NULL;
                    aggiorna_gruppi=true;
          }
        }
        if (iterato==exit) iterato=NULL;
        if (iterato) iterato=whouse->interazione();
      }

      xFORM(whouse,str_magazzino);

      // "I_id_articolo,I_id_magazzino,d_giacenza_iniziale,d_valore_iniziale,d_soglia_scorta_esaurita,d_soglia_scorta_in_esaurimento",
      // "r,d_giacenza_iniziale,d_soglia_scorta_esaurita,d_soglia_scorta_in_esaurimento"),

    }

    if (disab_ric) {
      fimgav->disabilita(qta,um,factor,mm,load_vat);
      qta.value("");
      um.value("");
      factor.value("");
      mm.value("");
      load_vat.value("");
    }
    else fimgav->abilita(qta,um,factor,mm,load_vat);
    if (disab_ric_pulsante) fimgav->nascondi(recipe); else fimgav->scopri(recipe);

    // ==============
    if (iterato==exit) iterato=NULL;

    // ==============
    if (iterato) iterato=fimgav->interazione();

  } // while iterato

  xFORM(fimgav,str_fimgav);

  if (ricarica) menu->cambia( grp.id_listino );

  return;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm::proprieta_clicked() {

  tr::menu::voce* voce=menu->selezionata();
  if ( !voce && gruppi->selezionato().isEmpty()) BEEPER;

  tr::gruppi::gruppo grp;

  if ( !voce ) {
    grp.parse(gruppi->record());
    // grp.carica(gruppi->selezionato().toInt());
    modifica_gruppo(grp);
    return;
  }

  if (voce && voce->tipo_articolo.gav()) {
    grp.carica(voce->is);
    // modifica_gruppo(grp);
    QString nuova_desc=tools.tastiera(this,tr("Nuova descrizione variante"),grp.descrizione);
    if (!nuova_desc.isNull()) {
      /*::cmd(tok_modifica_gruppo,QSN(ereditarieta),QSN(grp.is),
              QSN(grp.id_padre), code(), name(), prz,prc,
              vat(), QSN(grp.tipo_articolo()), stampanti,
              QSN(grp.id_listino), QSN(grp.id_listino), dep()))
                */
      ::cmd(tok_modifica_gruppo,LEAVE,QSN(grp.is),
        LEAVE,LEAVE,nuova_desc,LEAVE,LEAVE,
        LEAVE,LEAVE,LEAVE,LEAVE,LEAVE,
        LEAVE,LEAVE);
    }
    return;
  }


  nascondi( exit );

  FORM( f, frm_cm_proprieta );

  if (voce && !voce->tipo_articolo.gav()) {
    f->modifica(
        voce->descrizione_originale,
        voce->descrizione.section( "<br>", 0, 0 ),
        voce->descrizione.section( "<br>", 1, 1 ),
        voce->descrizione.section( "<br>", 2, 2 ),
        voce->colore,
        voce->icona);
    // se e' un pulsante di corolla, nasconde i colori
    if ( voce->corolla() ) f->no_colori();
  }

  QString descr;
  QColor c;
  QString icona="";
  bool r=f->accepted();
  if ( r ) {

    if (voce) {

      // cambia colore

      if ( f->colore().isValid() ) c=f->colore();

      // cambia icona

      icona=f->icona();

      // cambia descrizione

      descr=f->desc1->text()+"<br>" +f->desc2->text()+"<br>" +f->desc3->text();
      while ( descr.right( 4 )=="<br>" ) descr.truncate( descr.length()-4 );

    }
  }

  xFORM( f, frm_cm_proprieta );

  if ( r && voce ) {
    menu->selezionata()->colore=c;
    menu->selezionata()->icona=icona;
    menu->selezionata()->descrizione=descr;
    menu->selezionata()->salva();
    menu->cambia( id_listino, true, true );

  }
  scopri( exit );

  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm::gruppi_clicked( QListViewItem* ) {
  menu->deseleziona();
  stato_pulsanti();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm::frm_gruppi_stampa() {
  QString str_grps=TRFORM((
       "palette=3\n"
       "events.ignore=cards\n"
       "minmax=95%|95%\n"
       "status=Gruppi Stampa\n"
       "listview=#groups(header:ARTICOLO!240|GRP.STP.!100,hl:%st% and %gs%?##000055,tok:%tok1%|0,onclick,rollone,noexit);"
          "#grpprn(header:DESCRIZIONE,tok:%tok2%,onclick,+-)\n"
       "input=Descrizione#descr(alato)\n"
       "buttons=associa#link|dissocia#unlink|inserisci#ins|modifica#mod|cancella#cancella|uscita#exit\n"
       "links=grpprn|mod|cancella\n"
       "links=descr|ins\n"
       ));


  denu_elenco_associazioni_gruppi_stampa;
  FRM_PARAMS;
  FRM_PARAM( "%st%", QSN( p_tipo_gruppo ) );
  FRM_PARAM( "%gs%", QSN( GRUPPO_ARTICOLO_SELEZIONE ) );
  FRM_PARAM( "%tok1%", QSN( tok_elenco_associazioni_gruppi_stampa ) );
  FRM_PARAM( "%tok2%", QSN( tok_elenco_gruppi_stampa ) );

  bFORM( frm_grps, str_grps, this );

  CTRL_FRM( frm_grps );
  CTRL( groups );
  CTRL( grpprn );

  /* a questo punto ha acquisito i gruppi stampa. Ore li rinumera e li
     riscrive numerati di 100 in 100 */
  for (int i=grpprn.listview->elenco_dati().count();i>0;i--) {
    denu_elenco_gruppi_stampa;
    tr::mse::riga r=grpprn.listview->riga(i-1);
    if (r[p_ordine]!=QSN((grpprn.listview->elenco_dati().count()-i+1)*100)) {
      r[p_ordine]=QSN((grpprn.listview->elenco_dati().count()-i+1)*100);
      if (!CMD(tok_modifica_gruppo_stampa,r[p_id_gruppo],r[p_descrizione_gruppo],r[p_ordine]))
        break;
      grpprn.listview->riga(i-1,r);
    }
  } // for i

  CTRL( descr );
  CTRL( link );
  CTRL( unlink );
  CTRL( ins );
  CTRL( mod );
  CTRL( cancella );

  CTRL( exit );

  /*
  groups.listview->tipo_fittizio("+IS|-COD|-#PRZ|$DESCR<|-FIGLI|-TIPO|-RAGRP|-RIS|!PADRE|-LISTINO|-IVA|-REPARTO|GRPPRN");
  groups.listview->aggiungi(gruppi_esteso.elenco);
  groups.listview->aggiorna();
  */
  frm_grps->nascondi(link,unlink);
  while (frm_grps->interazione()!=exit) {
    bool aggiorna=false;
    bool aggiorna_grp=false;

    // --------------------------
    if (frm_grps->iterato==grpprn) descr.value(grpprn.listview->descrizione());

    // --------------------------
    if (frm_grps->iterato==grpprn->listview->more && !grpprn().isEmpty()) {
      denu_elenco_gruppi_stampa;
      tr::mse::riga cr(grpprn.listview->record());
      tr::mse::riga pr(grpprn.listview->record(+1));
      if (!cr().isEmpty() && !pr().isEmpty()) {
        aggiorna=CMD(tok_modifica_gruppo_stampa,cr[p_id_gruppo],cr[p_descrizione_gruppo],pr[p_ordine]);
        aggiorna=CMD(tok_modifica_gruppo_stampa,pr[p_id_gruppo],pr[p_descrizione_gruppo],cr[p_ordine]);
      }
    }

    // --------------------------
    if (frm_grps->iterato==grpprn->listview->less && !grpprn().isEmpty()) {
      denu_elenco_gruppi_stampa;
      tr::mse::riga cr(grpprn.listview->record());
      tr::mse::riga sr(grpprn.listview->record(-1));
      if (!cr().isEmpty() && !sr().isEmpty()) {
        aggiorna=CMD(tok_modifica_gruppo_stampa,cr[p_id_gruppo],cr[p_descrizione_gruppo],sr[p_ordine]);
        aggiorna=CMD(tok_modifica_gruppo_stampa,sr[p_id_gruppo],sr[p_descrizione_gruppo],cr[p_ordine]);
      }
    }

    // --------------------------
    if (frm_grps->iterato==link || frm_grps->iterato==unlink) {
      QString id_articolo;
      denu_elenco_gruppi_stampa;
      QString id_gruppo;

      if (frm_grps->iterato==link) id_gruppo=grpprn[p_id_gruppo];
      else id_gruppo="0";

      QListViewItemIterator it( groups.listview->elenco->currentItem() );
      QListViewItem *stop=groups.listview->elenco->currentItem()->nextSibling();
      while ( it.current() && it.current()!=stop ) {
        QString k=groups.listview->chiave(it.current());
        if (!k.isEmpty()) {
          if (!id_articolo.isEmpty()) id_articolo+=",";
          id_articolo+=k;
          // groups.listview->aggiorna(it.current(),
        }
        ++it;
      }

      aggiorna_grp=CMD(tok_modifica_associazione_gruppo_stampa, id_articolo, id_gruppo );

    } // link & unlink

    // --------------------------
    if (frm_grps->iterato==ins || frm_grps->iterato==mod) {
      denu_elenco_gruppi_stampa;
      QString id_gruppo;
      if (frm_grps->iterato==ins) id_gruppo="";
      else id_gruppo=grpprn[p_id_gruppo];
      int pos=0;
      if (!grpprn().isEmpty()) {
        pos=grpprn[p_ordine].toInt();
        // inserisce nel mezzo
        int mid_pos=((pos%100)/2);
        if (!mid_pos) mid_pos=50;
        pos+=mid_pos;
      }
      else
        pos=(grpprn.listview->righe()+1)*100;

      aggiorna=CMD(tok_modifica_gruppo_stampa,id_gruppo,descr(),QSN(pos));
    } // int & mod

    // --------------------------
    if (frm_grps->iterato==cancella) {
      denu_elenco_gruppi_stampa;
      QString id_gruppo=grpprn[p_id_gruppo];
      aggiorna=CMD(tok_elimina_gruppo_stampa,id_gruppo);
    } // cancella


    // aggiorna stato
    if (aggiorna_grp) {
      QString sel=groups();
      groups->listview->aggiorna();
      if (!sel.isEmpty()) groups->listview->seleziona(sel);
    }
    if (aggiorna) {
      QString sel=grpprn();
      grpprn->listview->aggiorna();
      if (!sel.isEmpty()) grpprn->listview->seleziona(sel);
    }

    // --------------------------  stato pulsanti
    if (!groups().isEmpty() && !grpprn().isEmpty())
      frm_grps->scopri(link); else frm_grps->nascondi(link);
    if (!groups().isEmpty() && !groups[p_id_gruppo_stampa].isEmpty() && groups[p_id_gruppo_stampa]!="0")
      frm_grps->scopri(unlink); else frm_grps->nascondi(unlink);

  } // while interazione

  xFORM( frm_grps, str_grps );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm::frm_stampa(QString tipo) {
  QString id_grp=gruppi->selezionato();

  if (id_grp.isEmpty()) {
    tools.msgbox(TR("Scegliere una voce dall'elenco gruppi"));
    return;
  }

  if (tipo=="varianti") {
    QString rcv=::rcv(tok_ha_varianti,id_grp);
    QString id=rcv.section('|',0,0);
    if (id!="0") CMD(tok_stampa_gruppo,id,TR("VARIANTI"));
    else tools.msgbox(TR("Questo gruppo non ha varianti"));
  }
  else
    CMD(tok_stampa_gruppo,id_grp,gruppi->articolo().descrizione);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm::barcodeReaded(QString barcode)
{
  QString is=::rcv(tok_cerca_articolo_barcode,barcode+"|"+QSN(id_listino));
  if (!is.isEmpty() && is!="EOP") {
    tr::gruppi::gruppo grp;
    QString record=::rcv(tok_elenco_gruppo,is);
    if (!record.isNull()) {
      grp.parse(record);
      modifica_gruppo(grp);
    }
  } // if c'e' associato un'istanza
  else tools.msgbox(TR("Nessun articolo associato al barcode:")+barcode);
} // keyPressEvent
