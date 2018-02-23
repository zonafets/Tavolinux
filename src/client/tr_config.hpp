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

#ifndef TR_CONFIG_HPP
  #define TR_CONFIG_HPP

/*
reg exp

time               | \\d{2}[:]\\d{2}
date               | \\d{2}[/]\\d{2}[/]\\d{4}
real               | ^-?\\d*(\\.\\d+)?
int                | ^-?\\d*
real2              | ^-?\\d*(\\.\\d{0,2})?
real3              | ^-?\\d*(\\.\\d{0,3})?

*/

//#include <iostream>
//using namespace std;

  #include <qvariant.h>
  #include <qstring.h>
  #include <qstringlist.h>
  #include <qvariant.h>
  #include <qcolor.h>
  #include <qpixmap.h>
  #include <qimage.h>
  #include <qapplication.h>
  #include <qobject.h>
  #include <cmath>
  #include <typeinfo>
  #include <qsettings.h>

  #include "money.hpp"
  #include "tr.hpp"

#define WDBGFLG (Qt::WStyle_Customize|Qt::WStyle_NormalBorder|Qt::WStyle_Minimize)
#define WRUNFLG (Qt::WStyle_Customize | Qt::WStyle_NoBorder )
// | Qt::WStyle_Tool | Qt::WStyle_Title )

#define dbg_hvmenu 1024

// macro di uscita
#define EXIT_ERROR 0
#define EXIT_OK 1

extern QString config_path;
extern uint MYFLAGS;

#define TR(x) QApplication::tr( x )

// defiunizioni globali per colori
  #define WHITE QColor(255,255,255)

// macro di utilit�per rapida impostazione parametro di cofigurazione:
// CFG( nome_membro_classe_config, valore )
  #define CFG(x,y) config.x=y; if (!config.qs.writeEntry("/"#x, config.x)) config.errore("/"#x, QVariant(config.x));

  #define DEFAULT_DECIMALI 2


class tr_config:public QObject, public tr::enumeratori::cliente {

public:
  tr_config();

  virtual ~tr_config() { };

  QSettings qs;

  void errore( QString chiave, QVariant valore ) {
    QString st=tr( "Errore nella scrittura della chiave " )+chiave+tr( " valore " )+valore.toString()
         +tr( " nel file di configurazione\n" );
    cout<<st.latin1();
  }

  QString leggiValore( QString chiave, QString s_default= QString::null ) {
    bool ok;
    QString s=qs.readEntry( chiave, s_default, &ok );
    if ( !ok && !s_default.isNull()) {
      // Inserisci il valore nella lista
      s=s_default;
      s.replace( "\n", "\\<-" );
      if ( !qs.writeEntry( chiave, s ) ) errore( chiave, QVariant( s ) );
    }
    return s.replace( "\\<-", "\n" );
  }

  QString leggiValore( char*chiave, char*s_default ) { return leggiValore( chiave, QString( s_default ) ); }

  int leggiValore( QString chiave, int s_default ) {
    bool ok; int valore=qs.readNumEntry( chiave, s_default, &ok );
    if ( !ok )
      if ( !qs.writeEntry( chiave, s_default ) ) errore( chiave, QVariant( s_default ) );
    return valore;
  }

  uint leggiValore( QString chiave, uint s_default ) { return ( uint )leggiValore( chiave, double( s_default ) ); }

  uint leggiValore( QString chiave, short s_default ) { return leggiValore( chiave, int( s_default ) ); }

  double leggiValore( QString chiave, double s_default ) {
    bool ok; double valore=qs.readDoubleEntry( chiave, s_default, &ok );
    if ( !ok )
      if ( !qs.writeEntry( chiave, s_default ) ) errore( chiave, QVariant( s_default ) );
    return valore;
  }

  bool leggiValore( QString chiave, bool s_default ) {
    bool ok; double valore=qs.readBoolEntry( chiave, s_default, &ok );
    if ( !ok )
      if ( !qs.writeEntry( chiave, s_default ) ) errore( chiave, QVariant( s_default ) );
    return valore;
  }

  QColor leggiValore( QString chiave, QColor s_default ) {
    return QVariant( leggiValore( chiave, s_default.name() ) ).toColor();
  }

  /* template <class X> X leggiValore (QString chiave, X s_default ) { QString* s; s = valori->find(chiave); if (s == NULL) {
  // Inserisci il valore nella lista QVariant v = QVariant(s_default); QString *s=NEW QString(v.toString());
  valori->insert (chiave, s ); } #if X == QString return *s; #endif #if X == int return s->toInt(); #endif }*/

  // IMPOSTA VALORE

  int ndec( QString mschera );
  money arrotonda( money, short num_decimali=-1 ); // arrotonda a max_decimali
  money arrotonda_x_difetto( money, short max_decimali=-1 );
  money arrotonda_x_eccesso( money, short max_decimali=-1 );

  QString num_interno( QString numero_di_zona ) {
    if ( segno_decimale!='.' &&( numero_di_zona.find( segno_decimale )!=-1 ) )
      return numero_di_zona.remove( '.' ).replace( segno_decimale, '.' );
    return numero_di_zona;
  }

  QString formatta( double, short num_decimali=-1 ); // formatta opportunamente il numero
  QString formatta( QString num_st, short=-1 ); //  a +/-###.000 x max_decimali
  QString formatta( double, QString maschera );
  QString formatta( QString num_st, QString maschera );

  QString disabilitato( QString nome ) { return nome.left( nome.length()-4 )+"_disabilitato" +nome.right( 4 ); };


  // SERVER

  enum tipo_arrotondamento { arr_difetto, arr_eccesso, arr_alternato };

  tipo_arrotondamento arrotonda_conto_diviso;
  /* indica se nella divisione dei conti si deve arrotondare il totale e quindi i conti per difetto, eccesso, alternato  o se
  l'ultimo si carica la differenza */
  QString database;
  int precisione_calcoli;
  QString server_address;
  QString working_path;

  enum tipo_tipo_socket { unix_socket=1, tcpip_socket=2 }
  tipo_socket;

  tipo_arrotondamento leggiValore( QString chiave, tipo_arrotondamento s_default ) {
    return ( tipo_arrotondamento )leggiValore( chiave, ( int )s_default );
  }

  tipo_tipo_socket leggiValore( QString chiave, tipo_tipo_socket s_default ) {
    return ( tipo_tipo_socket )leggiValore( chiave, ( int )s_default );
  }

  //    void	impostaValore(QString chiave, tipo_arrotondamento valore) {
  //	impostaValore(chiave,(int)valore);
  //    }


  // CLIENT

  bool cursor; // true se si deve vedere il cursore nelle form

  QString font_applicazione;
  QString nome_applicazione;
  short int vista_margine; // margine vista dei tavoli

  // caratteristiche generiche della finestra
  short int finestra_altezza;
  short int finestra_larghezza;
  short int finestra_larghezza_menu;
  double rapporto;
  short int finestra_altezza_titolo;
  short int finestra_altezza_stato;
  short int finestra_altezza_centrale;
  short int finestra_altezza_pulsanti;
  short int pulsanti_font_size;
  short int finestra_font_size;

  double icon_scale_factor;
  
  QString pulsante_barra;
  QString pulsante_dialog;
  QString pulsante_menu;


  short int tavolo_font_dimensione; // dimensione del font stampato sul tavolo in vista sala
  QColor tavolo_font_colore; // colore del font sul tavolo in vista sala
  short int tavolo_ox, tavolo_oy; // oridine del tavolo nel rettangolo di visualizzazione
  short int tavolo_dx, tavolo_dy; // dimensione area rettangolare che comprende il tavolo

  // logon
  QString tipo_terminale;
  QString tipo_protocollo;
  QString codice_utente;
  QString password;
  QString sn;

  bool beginner;
  /* indica il livello di utente. Influenza le conferme di operazioni critiche che verranno poste 2 volte */

  short int barra_titolo_font_size;
  QColor barra_titolo_colore_sfondo;
  QColor barra_titolo_colore_testo;

  QString sfondo_stato;
  QString sfondo_titolo;
  QColor colore_selezione;
  QColor colore_sfondo;
  QColor colore_sfondo_stato;
  QColor colore_testo;
  QColor colore_labels; // colore delle etichette
  QColor colore_labels_bordo; // colore del borto delle etichette
  QColor colore_labels_dinamiche; // colore delle etichette dinamiche (tavolo orologio)
  QColor colore_labels_dinamiche_bordo; // colore del borto delle etichette dinamiche


  QColor colore_gruppi;

  short int barra_stato_font_size;

  union {
	char segno_decimale; // generico per tutti i numeri qta e prezzi
	char decimals_dot;
  };
  short int max_descrizione; // indica lunghezza del campo descrizione per input_descrizione
  short int max_decimali;

  QString bar;

  QString bar_stazione;   // identifica il nome del bar per la stazione corrente

  money iva;

  int sala_ultima_id;
  /* numero ultima sala in gestione prima di chiusura programma */
  bool sala_ultima_vista_a_griglia;
  int pg_contanti; // indica quale �il testo del pagamento in contanti
  int pg_tessere; // indica quale �il testo del pagamento in contanti

  union {
	short altezza_riga;
	short row_height;
  };
  /* indica l'altezza delle righe di tabelle e listbox, per la selezione */

  short server_comande_porta;
  short server_sync_porta; // porta udp
  int timeout_tcp; // numero millisecondi di timeout per attesa rete
  QString server_addr;

  // messaggi server comanda
  QString cmd_ok; // messaggio comando ok
  bool cmd_ko( QString ); //
  QString cmd_sep; // separatore di parametro
  QString cmd_eod; // end of data
  QString cmd_eop; // end of packet
  QString cod_listino_base;
  int listino_corrente; // id listino corrente

  char dir_sep() { return '/'; }; // per compatibilit�tra piattaforme diverse

  QString stampante_conti;

  uint max_tavoli_bis; // indica il numero massimo di suddivisioni del tavolo

  void carica_extra();
  void salva_clienti_rapidi();
  QString clienti_rapidi_ids; // elenco id separati da ,
  QStringList clienti_rapidi_id;
  QStringList clienti_rapidi_rs;
  short clienti_rapidi_size; // indica quanti pulsanti sono stati specificati
  // e quanti clienti rapidi gestisce

  QString formato_data; // non piu' usato
  QString formato_data_locale; // come da postgres e impostato dal server
  QString formato_ora_locale; // come da postgres e impostato dal server
  // vedere tools.data(QString) e tools.stringa( QDate ) e tools.data_valida, ora_valida, etc.
  QString formato_data_esteso(); // converte DMY in GG/MM/AAAA

  bool listino_no_intervalli; // disabilita la specificit�dei 5 minuti
  int id_grp_pagamenti; // gruppo padre dei pagamenti

  int stampa_comanda_nsq; // 0=no 1=si 2=msgbox
  int riapri_conto_nsq; // 0=no 1=si 2=msgbox
  bool chiedi_su_articoli_p_nullo; // default true
  bool chiedi_su_svuota;           // default true
  bool chiedi_su_coperti;           // default false
  int salva_in_uscita_se_conto_rapido_nsq; // 0=no 1=si 2=msgbox

  int verbose; // 0=no; 1=tranne tavolio; 2=tutto

  // dimensioni font della comanda/conto
  QString comanda_font;
  int comanda_font_size;
  int comanda_child_font_size;
  int comanda_altezza_riga;
  int comanda_totale_altezza_riga;
  int comanda_totale_font_size;
  QColor comanda_livelli_colore;
  int comanda_totale_larghezza_min;
  QColor comanda_totale_colore_fg;
  QString comanda_livelli_nomi;

  QString menu_font;
  QColor menu_selezionato_colore;
  QColor menu_corolla_colore;
  QColor menu_percorso_colore;
  QColor menu_elementi_colore;
  QColor menu_pulsanti_colore;
  int menu_elenco_font_size;
  int menu_button_size_x;
  int menu_button_size_y;
  int menu_button_font_size;
  bool menu_testo_bold;

  int elenchi_font_size; // dimensione generale elenchi
  int elenchi_abc_font_size;
  int elenchi_button_size;
  QColor elenchi_color_fg;
  QColor elenchi_color_bg;
  QColor elenchi_color_sel;
  int elenchi_altezza_riga;

  QString exe_path;

  int stile_menu; // 1=default=pixmaps&colori_std,2=pulsanti+alti
  // 4=pulsanti normali e geometrie multiple
  // 6=pulsanti+alti e geometrie multiple
  // se i colori non sono standard, lo stile passa a 2
  //

  bool calcolatrice; // indica se la calcolatrice e' attiva in comanda
  bool calcolatrice_dec_fisso; // indica se la calcolatrice e'
                               // gestita con decimale fisso come il
                               // registratore di cassa

  QString barcode;  /* "da|start|end|dim|regexp" indica se la gestione
                       dei codici a barre e' attiva e come riconoscerlo:
                       da: vuoto indica tastiera oppure il dispositivo seriale
                       start: indica il codice di inizio della sequenza
                                        che contraddistingue il codice a barre
                       end: indica il codice di fine della sequenza
                       dim: indica quanti caratteri lo compongono
                                  (incluso lo start e l'end)
                       rxp: indica la regular expression che verra'
                            applicata per trasformare il barcode
                       Sono tutti facoltativi
                       */

  QString id_menu_bar;  // indica l'id di menu di default all'apertura del bar

  bool lampeggio;

  bool conto_rapido_bis; /* abilita (T)/disabilita(F) la gestione
                               del conto rapido sui bis */

  bool pagamenti_con_barra;
  bool cancella_comanda_non_conclusivo;

  int msg_server_tipo;
  QString msg_server;

  QString chiusura_testo;

  int auto_logout;         /* esprime dopo quanti millisecondi eseguire l'autologout
                              su login da tessera. Per ora funziona solo se il
                              contesto e' frm_main. Per forzare su altri contesti
                              si puo' usare valore negativo invece di un altro
                              flag */

  QString auto_login;       /* nome utente per auto login
                               per ora con protocollo senza password */

  bool ricarica_tessere_incrementale;

  bool proforma_conclusiva; // chiude la form dopo la proforma

  int pulisci_nsq;

  bool multi_comanda;           // attiva la modalita' a multi comanda
  bool chiedi_estremi_ricarica_tessera;

  int uscita_conto_rapido_nsc;  // 0 = no; 1=si (tranne bar); 2=custom (su bit tavolo)

  QString language;             // linguaggio di default per tavolinux
                                // vedere parametro -lang
  QString arguments;            /* argomenti di default che verranno ACCODATI
                                   senza controllo
                                   a quelli passati via linea di comando */

  bool sconto_interattivo;

  QString ffm_vats;             // is|iva;is|iva;...
  QString ffm_description;      // es. %date% di %ref% al %vat% per %sum_row%
  QString ffm_payments;         // is_pag1|is_pag2|...
  QString ffm_chk_befor_print;  // per ora solo "all" ma potra' divenire *|*|*|*

  int scroll_ms;                // millisecondi per ripetizione di scroll

  int conto_resto;              // -1=ok; 0=niente; n=secondi attesa
  int conto_zero_nsq;           // stampa conso con totale a zero

  QString pocket;               // parametri pocket e quindi attivazione implicita

  short int icon_size;          // dimensione icone

  short int repeat_ms;          // millisecondi per ripetizione autoclic

  QColor input_color_bg;

  QString format_table_state;   /* %campo%,%campo%;%campo% la virgola separa
                                le colonne e il ; le righe. Se i valori sono
                                dispari, crea una colonna allargata
                                Una parola tra parentesi [] indica il nome dell'icona
                                [orologio.png]%campo%*/
                                /* parametri:
                                coperti
                                tot_tav totale tavolo occupato
                                tot_ult_f totale ulticomo conto fiscale
                                tot_odi_f totale da ultima chiusura fiscale
                                tot_ult_nf totale ulticomo conto non fiscale
                                tot_odi_nf totale da ultima chiusura non fiscale
                                cop_med coperto medio tot_tav/coperti
                                dpc     distanza prima comanda  hh:mm
                                duc     distanza ultima comanda hh:mm
                                */

  int turns_ask_NoStartLogin;  // 0=no;1=Start;2=Login

  QString stat_room_colors;

  QString default_terminal;

  QString splash; /* nome dell'immagine per formato fullscreen o codice HTML
                     per formato box */

  QString stat_colonne_sala;  // n|n|n|... riporta le colonne visibili

  int sleep_after_cmd;

  /* ---------------------------------------------------------------------- */

  // Dati relativi al display

  struct {
    int tipo;
    QString porta, parametri_porta;

    // formati output

    QString formati[16];

    // Sequenze di selezione / deselezione

    struct {
    QString seleziona, deseleziona;
    } sequenze;

    // Lunghezze massime dei parametri

    struct {
    int quantita, descrizione, prezzo;
    } lunghezze;
  } cfg_display;
};


extern QPixmap PIXMAP(QString nome);
#define BUTTON(x) PIXMAP(x)

//   #define PIXMAP(nome) QPixmap::fromMimeSource( nome )

#endif
