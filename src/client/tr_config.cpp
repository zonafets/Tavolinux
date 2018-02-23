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

#ifndef TR_CONFIG_CPP
#define TR_CONFIG_CPP

#include "tr_config.hpp"
#include "tr.hpp"
#include "tr_display.hpp"

QString config_path="/opt/trendservizi/tavolinux/etc";
uint MYFLAGS=WRUNFLG;

#include <cmath>
#include <qobject.h>
#include <stdlib.h>

#define init_CFG(x,y) x=y; x = leggiValore("/"#x, x);
#define init_s_CFG(x,y,z) y=z; y = leggiValore("/"#x, y);
#define init_sm_CFG(x,y,z) { y=z; y = leggiValore(x,y); }

//if(x!=y)cout<<#x<<endl; cout<<"reg:"<<#x<<endl;


void tr_config::salva_clienti_rapidi()
{
    clienti_rapidi_ids="";
    if (clienti_rapidi_id.count()) clienti_rapidi_ids=clienti_rapidi_id.join(",");
    qs.writeEntry("/clienti_rapidi_ids",clienti_rapidi_ids);
}

void tr_config::carica_extra()
{
    
    // legge l'elenco clienti e allinea descrizioni a id
    tr::mse clienti(tok_elenco_clienti);
    clienti.carica();
    
    clienti_rapidi_id = QStringList::split(',',clienti_rapidi_ids,true);
    
    bool salva_x_clienti_scomparsi=false;
    
    for ( uint i=0; i<clienti_rapidi_id.count(); i++ )
	if (clienti_rapidi_id[i]!="-1") {
	FORJ(clienti.count()) {
	    tr::mse::riga c(clienti[j]);
	    if (c[p_id]==clienti_rapidi_id[i]) clienti_rapidi_rs[i]=c[p_descrizione];
	} // for
	if (clienti_rapidi_rs[i].simplifyWhiteSpace().isEmpty()) {
	    clienti_rapidi_id[i]="-1";
	    salva_x_clienti_scomparsi=true;
	}
    } // if c'e'
    else clienti_rapidi_rs[i]=" ";
    
    if (salva_x_clienti_scomparsi) salva_clienti_rapidi();
    
    
}

bool tr_config::cmd_ko( QString s ) {
    if ( s.left( 3 )=="-KO" ) return true;
    return false;
}

int tr_config::ndec(QString maschera)
{
    return maschera.section('.',1,1).length();
}

money tr_config::arrotonda( money num, short num_decimali ) {
    if ( num_decimali==-1 ) num_decimali=max_decimali;
    return QString( "" ).sprintf( "%'." +QVariant( num_decimali ).toString()+"f", num ).toDouble();
    /* double f = 10.0 * num_decimali; double intera, frazione; frazione = (((num+5 / f ) * f));
  frazione = modf(frazione, &intera) ; return intera / f; */
}

money tr_config::arrotonda_x_difetto( money num, short num_decimali ) {
    if ( num_decimali==-1 ) num_decimali=max_decimali;
    money fattore=10.0;
    for ( short i=num_decimali; i>1; i-- ) fattore*=10.0;
    num=num*fattore;
    money intera;
    modf( num, &intera );
    intera/=fattore;
    return intera;
}

money tr_config::arrotonda_x_eccesso( money num, short num_decimali ) {
    if ( num_decimali==-1 ) num_decimali=max_decimali;
    money fattore=10.0;
    for ( short i=num_decimali; i>1; i-- ) fattore*=10.0;
    num=( num*fattore )+0.5;
    money intera;
    money frazione=modf( num, &intera );
    if ( frazione>=0.5 ) intera++;
    intera/=fattore;
    return intera;
}


QString tr_config::formatta( double num, short num_decimali ) {
    if ( num_decimali==-1 ) num_decimali=max_decimali;
    // if ( num==0 ) return "0";
    // return QString::number(num, "f", num_decimali);
    QString r=QString( "" ).sprintf( "%'." +QVariant( num_decimali ).toString()+"f", num );
    return r.replace( '.', segno_decimale );
}

QString tr_config::formatta( QString num_st, short num_decimali ) {
    if ( num_decimali==-1 ) num_decimali=max_decimali;
    double num=num_st.toDouble();
    // modificato il 14/10/05. Non si conoscono gli effetti.
    if ( num==0 ) return "";
    // return QString::number(num, "f", num_decimali);
    return QString( "" ).sprintf( "%'." +QVariant( num_decimali ).toString()+"f", num ).replace( '.', segno_decimale );
}

QString tr_config::formatta( QString num_st, QString maschera ) {
    int n_dec = maschera.section('.',1,1).length();
    return formatta( num_st, n_dec );
}

QString tr_config::formatta( double num, QString maschera ) {
    int n_dec = maschera.section('.',1,1).length();
    return formatta( num, n_dec );
}


QString tr_config::formato_data_esteso() {
    QString s;
    for ( uint i=0; i<formato_data.length(); i++ ) {
	if ( s!="" ) s+="/";
	if ( formato_data[i]=='Y' ) s+=tr( "AAAA" );
	if ( formato_data[i]=='M' ) s+=tr( "MM" );
	if ( formato_data[i]=='D' ) s+=tr( "GG" );
    }
    return s;
}

tr_config::tr_config():qs( QSettings::Ini ) {
    char*c=getenv( "TAVOLINUX_BASEPATH" );
    if ( c!=NULL ) {
	config_path=QString( c );
	if ( config_path.left( 1 )=="/" ) config_path+="etc";
	else config_path+="/etc";
    }
    
    // Imposta i parametri
    
    qs.setPath( "trendservizi.it", "tavolinux" );
    qs.insertSearchPath( QSettings::Unix, config_path );
    
    // Entriamo nella sezione di configurazione vera e proprio
    
    qs.beginGroup( "/Tavolinux" );
    qs.beginGroup( "/Server" );
    
    // Tutto bene. Controlliamo se la configurazione �gi�stata
    // precedentemente registrata
    
    // SERVER o COMMON
    
    init_CFG( arrotonda_conto_diviso, arr_eccesso );
    init_CFG( database, "tavolinux" );
    init_CFG( arrotonda_conto_diviso, arr_eccesso );
    init_CFG( tipo_socket, tcpip_socket );
    
    init_CFG( msg_server_tipo, 0 );
    init_CFG( msg_server, "localhost" );
    
    qs.endGroup();
    
    // Display (se c'e')
    
    qs.beginGroup ("/Display");
    init_s_CFG ( tipo, cfg_display.tipo, 0 );
    init_s_CFG ( porta, cfg_display.porta , "/dev/ttyS0" );
    init_s_CFG ( parametri_porta, cfg_display.parametri_porta , "default" );
    
    for (int x=0;x < tr_display::tr_contesto_nullo;x++)
	init_sm_CFG ( QString ("/formato_%1").arg (tr_display::descrizione_formato(x)), cfg_display.formati[x], "default");
    
    init_s_CFG ( sequenza_selezione, cfg_display.sequenze.seleziona, "null");
    init_s_CFG ( sequenza_deselezione, cfg_display.sequenze.deseleziona, "null");
    init_s_CFG ( lunghezza_descrizione, cfg_display.lunghezze.descrizione, 12);
    init_s_CFG ( lunghezza_quantita, cfg_display.lunghezze.quantita, 5);
    init_s_CFG ( lunghezza_prezzo, cfg_display.lunghezze.prezzo, 7);
    
    qs.endGroup();
    
    // parametri del client
    
    qs.beginGroup( "/Client" );
    
    QString st;
    
    init_CFG( nome_applicazione, "TAVOLINUX" );
    
    init_CFG( cursor, true );
    
    QString finestra;
    // pos  descrizione
    // 0    altezza
    // 1    larghezza
    // 2    rapporto
    // 3    larg_menu
    // 4    pulsanti_font_size
    // 5    alt_riga
    // 6    alt_titolo
    // 7    alt_stato
    // 8    alt_pulsanti
    // 9    default font_size
    // 10   nome pulsante barra
    // 11   nome pulsante dialogo
    // 12   nome pulsante menu
    // init_CFG( finestra, "600|800|9|34" );
    finestra = "600|800|1|420|12|34|14|34|60|24|pulsante_barra.jpg|pulsante_dialog.jpg|pulsante_menu.jpg";
    QStringList e = QStringList::split('|',finestra,true);
    finestra_altezza = e[0].toInt();
    finestra_larghezza = e[1].toInt();
    rapporto= e[2].toInt();
    finestra_larghezza_menu = e[3].toInt();
    pulsanti_font_size=e[4].toInt();
    altezza_riga =e[5].toInt();
    finestra_altezza_titolo = e[6].toInt();;
    finestra_altezza_stato =e[7].toInt();
    finestra_altezza_pulsanti = e[8].toInt();
    finestra_font_size = e[9].toInt();
    finestra_altezza_centrale=finestra_altezza-( finestra_altezza_titolo+finestra_altezza_stato+finestra_altezza_pulsanti );
    pulsante_barra=e[10];
    pulsante_dialog=e[11];
    pulsante_menu=e[12];
    
    // questi potrebbero essere calcolati sul pixmap
    tavolo_ox=1;
    tavolo_oy=1 ;
    tavolo_dx=64;
    tavolo_dy=64;
    init_CFG( tavolo_font_dimensione, 20 );
    init_CFG( tavolo_font_colore, QColor( 230, 230, 230 ) );
    init_CFG( vista_margine, 2 );
    
    barra_titolo_font_size=10;
    barra_titolo_colore_sfondo=QColor( 0, 0, 148 );
    barra_titolo_colore_testo=QColor( 255, 255, 255 );
    
    init_CFG( sfondo_stato, "white_marble.jpg" );
    init_CFG( sfondo_titolo, "sfondo_titolo.jpg" );
    init_CFG( colore_sfondo, QColor( 0, 117, 175 ) );
    init_CFG( colore_sfondo_stato, QColor( 220, 117, 175 ) );
    init_CFG( colore_testo, QColor( 255, 255, 127 ) );
    init_CFG( colore_selezione, QColor( 255, 237, 155 ) );
    init_CFG( colore_labels, QColor( 0, 0, 0 ) );
    init_CFG( colore_labels_bordo, QColor( 255, 237, 155 ) );
    init_CFG( colore_labels_dinamiche, QColor( 0, 128, 0 ) );
    init_CFG( colore_labels_dinamiche_bordo, QColor( 255, 237, 155 ) );
    
    init_CFG( colore_gruppi, QColor( 0, 0, 200 ) );
    
    init_CFG( barra_stato_font_size, 24 );
    
    init_CFG( segno_decimale, ',' );
    
    init_CFG( max_descrizione, 64 );
    max_decimali=3;
    
    //////////////////////////////////////////////////////////////////////////////////////////////// logon ///////////////////////
    
    init_CFG( tipo_terminale, "1" );
    init_CFG( tipo_protocollo, "1" );
    init_CFG( codice_utente, "1" );
    init_CFG( password, "" );
    init_CFG( sn, "123456789" );
    
    init_CFG( bar,  "BAR" );
    
    init_CFG( bar_stazione,  "" );
    
    init_CFG( beginner, true );
    
    init_CFG( iva, 10.0 );
    
    init_CFG( sala_ultima_id, 1 );
    // init_CFG(	sala_ultima_vista_a_griglia, false 				);
    
    init_CFG( pg_contanti, 401 );
    init_CFG( pg_tessere, 401 );
    
    init_CFG( server_comande_porta, 4400 );
    init_CFG( server_sync_porta, 4500 );
    init_CFG( server_addr, "localhost" );
    
    init_CFG( timeout_tcp, 20000 );
    
    // messaggi server  comanda
    // init_CFG(	cmd_ok,			"+OK Comando accettato" 	);
    // init_CFG(	cmd_sep,		"|"				);
    // init_CFG(	cmd_eod,		"EOD" 				);
    // init_CFG(	cod_listino_base,		"1" 				);
    cmd_ok="+OK Comando accettato";
    cmd_sep="|";
    cmd_eod="EOD";
    cmd_eop="EOP";
    cod_listino_base="1";
    init_CFG( listino_corrente, 1 );
    
    init_CFG( stampante_conti, "Conti" );
    
    // init_CFG(	max_tavoli_bis,		9 				);
    max_tavoli_bis=9;
    
    init_CFG( formato_data, "dd/mm/yyyy hh:mm:ss" );
    
    init_CFG( listino_no_intervalli, true );
    
    init_CFG( id_grp_pagamenti, 400 );
    
    init_CFG( stampa_comanda_nsq, 2 );
    
    init_CFG( riapri_conto_nsq, 2 );
    init_CFG( salva_in_uscita_se_conto_rapido_nsq, 2 );
    
    init_CFG( chiedi_su_articoli_p_nullo, true);
    init_CFG( chiedi_su_svuota, true);
    init_CFG( chiedi_su_coperti, false);
    init_CFG( cancella_comanda_non_conclusivo, false);
    init_CFG( verbose, 1 );
    
    // clienti rapidi
    init_CFG( clienti_rapidi_size, 5 );
    init_CFG( clienti_rapidi_ids, "-1,-1,-1,-1,-1" );
    for (short i=0;i<clienti_rapidi_size;i++) {
	clienti_rapidi_rs<<" ";
    }
    
    init_CFG( comanda_font,"Helmetcondensed [Xft]");
    init_CFG( comanda_font_size, 24 );
    init_CFG( comanda_child_font_size, 18 );
    init_CFG( comanda_altezza_riga, altezza_riga );
    init_CFG( comanda_totale_altezza_riga, 32 );
    init_CFG( comanda_totale_font_size, 24 );
    init_CFG( comanda_livelli_colore, QColor( 0, 200, 0 ) );
    init_CFG( comanda_totale_larghezza_min, 60 );
    init_CFG( comanda_totale_colore_fg, QColor( 255, 237, 155 ) );
    init_CFG( comanda_livelli_nomi, "" );
    
    init_CFG( menu_font,font_applicazione);
    init_CFG( menu_selezionato_colore,  QColor(246,246,255));
    init_CFG( menu_corolla_colore,      QColor(213,215,255));
    init_CFG( menu_percorso_colore,     QColor( 184, 255, 213 ));
    init_CFG( menu_elementi_colore,     QColor(174,228,255));
    init_CFG( menu_pulsanti_colore,     QColor( 177, 179, 217 ));
    init_CFG( menu_elenco_font_size,    24 );
    init_CFG( stile_menu,               1 );
    if (menu_corolla_colore!=QColor(213,215,255) ||
	menu_elementi_colore!=QColor(174,228,255)) stile_menu=2;
    menu_button_size_x=60;
    menu_button_size_y=54;
    menu_button_font_size=12;
    menu_testo_bold=true;
    
    init_CFG( elenchi_font_size,        24 );
    init_CFG( elenchi_altezza_riga,     32 );
    elenchi_color_fg=QColor(0,0,127);
    elenchi_color_bg=QColor(208,210,255);
    elenchi_color_sel=QColor(0,0,127);
    elenchi_button_size=50;
    elenchi_abc_font_size=12;
    
    init_CFG( calcolatrice, true );
    init_CFG( calcolatrice_dec_fisso, false );
    
    init_CFG( barcode, "||||" );
    init_CFG( id_menu_bar, "" );
    
    init_CFG( lampeggio, false );
    
    init_CFG( pagamenti_con_barra, false );
    
    init_CFG( chiusura_testo, TR("chiusura di cassa"));
    
    init_CFG( auto_logout, 10000 );
    init_CFG( auto_login, "" );
    
    init_CFG( formato_data_locale, QString::null);
    init_CFG( formato_ora_locale, QString::null);
    
    init_CFG( ricarica_tessere_incrementale, false );
    init_CFG( proforma_conclusiva, false );
    init_CFG( pulisci_nsq, 1 );
    
    init_CFG( uscita_conto_rapido_nsc, 1);
    
    init_CFG( multi_comanda, false );
    init_CFG( chiedi_estremi_ricarica_tessera, false);
    
    init_CFG( language, "");
    init_CFG( arguments, "");
    
    init_CFG( sconto_interattivo, false );
    
    conto_rapido_bis=false;
    
    init_CFG( ffm_vats, "");
    init_CFG( ffm_description, TR("Scontrino del %date% - Rif: %ref% - Totale IVA al %vat%: %sum_row%"));
    init_CFG( ffm_payments, "");
    
    init_CFG( scroll_ms, 80 );
    
    init_CFG( conto_resto, -1 );
    init_CFG( conto_zero_nsq, 0 );
    
    init_CFG( ffm_chk_befor_print, "all" );
    
    icon_size=32;
    
    init_CFG( repeat_ms,300 );
    
    input_color_bg=QColor(240, 255, 240);
    
    init_CFG( format_table_state,"%coperti%,%tot_tav%;%dpc%,%duc%" );
    
    init_CFG( turns_ask_NoStartLogin, 0);
    
    init_CFG( stat_room_colors, "");
    
    init_CFG( font_applicazione,"Helvetica [Urw]");
    
    init_CFG( default_terminal,"/usr/bin/X11/xterm");
    
    init_CFG( splash, "<img src=\"tavolinux_bwhite.jpg\">|2000" );
    
    init_CFG( stat_colonne_sala,"0|1|2|3|4|5|6|7|8|9" );
    
    init_CFG( sleep_after_cmd, 100 );
    
    init_CFG( pocket,"1|240x320|1|1|8|Luxi Sans");
    icon_scale_factor=1;
}

#undef init_CFG

#endif
