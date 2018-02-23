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

#ifndef TR_H
  #define TR_H

  #include <qstring.h>
  #include <qstringlist.h>
  #include <qptrlist.h>
  #include <qdict.h>
  #include <qintdict.h>
  #include <qptrdict.h>
  #include <qcolor.h>
  #include "cmdcodicigruppi.hpp"
  #include "tr_dmacro.hpp"
  #include "tr_srvcmd.hpp"

  #define IS_VARIANTI -1

/* definizione delle strutture base dei dati ricevuti dal server

struttura di queste strutture: class "nome della classe relativa a ogni struttura espressa dal server"
class "nome di una eventuale sotto struttura o elemento" membro
membro parse( QString )   funzione di nome generico e comune per il parsing di una stringa dati e compilazione dei membri
QString unparse           fa il contrario QStringList elenco	  eventuale lista delle righe dati risultato di un comando
bool carica( parametri specifici) funzione di nome generico e comune per il caricamento di questa struttura dati dal server.
Incormpora l'uso del comando membro eventuale ulteriore membro specifico di questa struttura
salva                     chiama unparse e po fa il contrario di carica
sotto_singolo parse(int riga) parsa la riga di elenco e restituisce una struttura singola (esempio la voce di menu) */

/* simboli per struttura # colonna contenente numero in virgola mobile 0 colonna contenente numero intero
@ colonna chiave tipo stringa (se non presente viene assunto !) corrisponde alle colonne di !
Posto (nella tr_listview) nella qdict "indici" ! colonna di raggruppamento (se non c'è @ questo vale per loro)
alias (id del padre) N.B. ora il costruttore grafico si aspetta che il server lo mandi già in ordine per raggruppamento. Es:
@      !       $ nodo1  || (=pipe pime = vuoto o nullo ) corr1  nodo1 riga1  corr1 riga3  nodo2    // errore riga2  corr2
corr1  nodo2    // errore perchè mai trovato/creato genitore nodo2 nodo2  || corr1  nodo2

@      !      $ 1      ||     nodo1 2      1      corr1 ...


. colonna di raggruppamento semplice per nome si aspetta che il server mandi dati gia' ordinati. Es: #id|$nome|.gruppo
1|bar|salotto 2|bar.1|salotto 3|tavX|cuccetta 4|tavY|cuccetta ^ colonna di ordinameno (crescente)
- colonna da non visualizzare $ colonna nome gruppo (se non specificata è la medesima di !)
se ce n'e' piu' di uno, viene valutato l'ultimo se != da "" Viene automaticvamente posto nella prima colonna
+ colonna chiave tipo numerico (se non presente viene assunto !) corrisponde alle colonne di !
Posto (nella tr_listview) nella qdict "indici"

N.B.: se la chiave è numerica si intende che anche la chiave di raggruppamento è numerica

N.B.1: se è solo presente (@|+), la lista è una normale lista con chiave se è presente solo (!) la chiave è tutto il record

N.B.2: se sono assenti tutti i (@,+,!), la chiave diviene tutto il record

N.B.3: se (@|+) e (!) sono presenti, si intende che la struttura espositiva sarà un albero

(ordine) posto alla fine (es. PRZ(2)) attualmente non implementato


Es. "@IS|-COD|PRZ|$DESCR^|-FIGLI|-TIPO|-RAGRP|$ALTER|!PADRE|LISTINO"

nella listview, questi dati vengono raggruppati per PADRE, usando la descrizione ALTER o DESCR
nella seconda colonna viene posto il PRZ

dati: 10|C10|2.98|codice 10|1|0|0||0|1 11|C11|2.30|codice 11|1|0|0||0|1 12|C12|2.20|codice 12|1|0|0||11|1
13|C13|1.10|codice 13|1|0|0|tredici|11|1

risultato: C10        2.98     // x gruppo 0 si intende la listview C11        2.30 C12      2.20 tredici  1.10

Parallelamente viene creato un indice basato e composto da
IS|LISTINO  <->  QListViewItem*  <->  posizione numerica nell'elenco dati
(da verificare se dopo sort() i puntatori puntano ancora ai medesimi elementi )

*/



  #define money double
  #define tipo_tipo_articolo uint

class enumeratori { // classe di comodo
public:

  enum menu {
    mn_id_listino, mn_id_istanza, mn_id_istanza_padre, mn_tipo_articolo, mn_locazione, mn_disposizione, mn_pagina,
       mn_descrizione, mn_id_varianti_globali, mn_id_varianti_gruppo, mn_id_varianti_articolo, mn_colore, mn_icona, mn_pixmap
  };

  enum comanda {
    com_qta, com_descrizione, com_tot_riga, com_livello, com_id_riga, com_id_istanza, com_id_art, com_prezzo, com_stato,
       com_offerto, com_id_listino, com_portata
  };

  enum portata {
    p_id_riga, p_id_istanza, p_livello, p_id_art, p_qta, p_descrizione, p_prezzo, p_tot_riga, p_stato, p_offerto,
       p_id_listino, p_portata
  };

  enum {
    com_descr=1, com_prz=2
  };

  enum articolo {
    art_istanza, art_codice, art_prezzo, art_descrizione, art_figli, art_tipo, art_tipo_raggruppamento, art_alternativa,
       art_is_padre, art_id_listino
  };

  enum {
    col_desc, col_is, col_id, col_prz, col_figli, col_stato, col_stato_extra, col_id_art, col_is_padre, col_id_listino
  };

  enum {
    col_descrizione, col_istanza, col_articolo
  };

  enum {
    ct_vuoto=0, ct_comanda=1, ct_dividi=2
  };

  enum docs {
    doc_ricevuta=2, doc_ricevuta_divisa=( doc_ricevuta+( 1<<8 ) ), doc_fattura=4, doc_fattura_divisa=( doc_fattura+( 1<<8 ) ),
       doc_scontrino=5, doc_scontrino_divisa=( doc_scontrino+( 1<<8 ) ), doc_proforma=6, doc_proforma_divisa=( doc_proforma+( 1<<8 ) ),
       doc_concordato=15, doc_concordato_divisa=( doc_concordato+( 1<<8 ) )
  };

  enum tipo_separa {
    a_conto=1, da_conto=2
  };

  enum conti {
    cnt_globale=1, cnt_separato=2, cnt_diviso=3
  };

  enum conti_chiusi {
    cnt_fittizio, cnt_definitivo, cnt_devnul
  };

  enum tr_ereditarieta {
    er_prezzo=1, er_prn_comanda=2, er_prn_conto=4
  };

  enum operazione_listino {
    listino_vuoto, listino_copiato, listino_maggiorato_assoluto, listino_maggiorato_relativo
  };



  class listino_esteso {
  public:
    enum indici {
      p_id, p_codice, p_descrizione, p_da_data, p_a_data, p_giorni, p_intervalli
    };
  };



  class cliente_esteso {
  public:
    enum tr_posizioni {
      p_id, p_codice, p_descrizione, p_via, p_citta, p_cap, p_telefono, p_fax, p_iva, p_cf, p_data_inserimento,
         p_data_modifica, p_data_ultima_fattura, p_pagamenti, p_aiva, p_descrizione_aiva
    };
  };



  class tavolo_esteso {
  public:
    // nome_tavolo|id_sala|x|y|tipo|n_posti|stato|ruotato|ordine|
    // id_tavolo|id_prima_comanda|id_tavolata|id_listino|pixmap|id_gruppo_listino
    enum tr_posizioni {
      p_nome, p_id_sala, p_x, p_y, p_tipo, p_posti, p_stato, p_ruotato, p_ordine, p_id, p_id_prima_comanda, p_id_tavolata,
         p_id_listino, p_pixmap, p_id_gruppo_listino, p_tipi_conto, p_last
    };
  };



  class tavolo {
  public:
    enum tr_posizioni {
      p_id, p_nome, p_stato, p_pixmap, p_sala, p_last
    };
  };



  class sala_estesa {
  public:
    enum tr_posizioni {
      p_id, p_nome, p_pixmap, p_last
    };
  };



  class sala {
  public:
    enum tr_posizioni {
      p_id, p_is=p_id, p_nome, p_n_tavoli, p_last
    };
  };



  class pixmap {
  public:
    enum tr_posizioni {
      p_nome, p_descrizione, p_tipo, p_last
    };
  };



  class cliente {
  public:
    enum tr_posizioni {
      p_id, p_codice, p_ragione, p_descrizione=p_ragione, p_last
    };
  };



  class stampante {
  public:
    enum tr_posizioni {
      p_nome, p_last
    };
  };



  class stampante_gruppo:public stampante {
  };



  class cameriere {
  public:
    enum tr_posizioni {
      p_id, p_nome, p_last
    };
  };



  class conto {
  public:
    enum tr_posizioni {
      p_id, p_descrizione, p_data, p_last
    };
  };

}; // enumeratori



class tr:public enumeratori {
public:



  // classe multi struttura estesa
  class mse {
  public:
    mse() {
    };
    mse( TRTokens tid );
    /* il seguente costruttore e' abbinato a id() e descrizione() per reperire info specifica di singolo record */
    mse( TRTokens tid, QString id, QString=QString::null );



    class riga {
    public:
      riga( QString init ) {
        elenco=QStringList::split( '|', init, true );
      }

      QString&operator[] ( int p ) {
        return elenco[p];
      }

      QString operator() () {
        return elenco.join( "|" );
      }

      // private:
      QStringList elenco;
    };



    bool carica( QString id1=QString::null, QString id2=QString::null );

    bool carica( TRTokens _tid, QString id1=QString::null, QString id2=QString::null ) {
      tid=_tid;
      costruttore_comune();
      return carica( id1, id2 );
    }

    QString operator[] ( int p ) {
      return elenco[p];
    }

    QStringList operator() () {
      return elenco;
    }

    QString operator() ( QString sep ) {
      return elenco.join( sep );
    }

    int size() {
      return elenco.size();
    }

    int count() {
      return elenco.count();
    }

    QString struttura;
    QString maschera;
    QStringList elenco;

    QString id();
    QString descrizione();
    QString column( int n );

    TRTokens tid;

  private:
    void costruttore_comune();
    int p_id;
    int p_desc;
  }; // mse



  // ========================================================
  // classe generica con contenitori righe e struttura

  class base {
  public:
    QStringList elenco;
    QString struttura;

    bool carica( TRTokens c, QString p1=QString::null, QString p2=QString::null ) {
      elenco.clear();
      bool r=false;
      if ( !p1.isNull()&&!p2.isNull() ) r=cmd( c, p1, p2 );
      if ( !p1.isNull()&&p2.isNull() ) r=cmd( c, p1 );
      if ( p1.isNull()&&p2.isNull() ) r=cmd( c );
      if ( !r ) return false;
      elenco=srv->rcv_list();
      srv->eop();
      return true;
    }
  }; // class base



  // ========================================================


  struct tipo_articolo {
    //public:
    tipo_articolo( const tipo_tipo_articolo&valore ):stato( valore ) {
    }

    tipo_tipo_articolo stato;

    void operator() ( int valore ) {
      stato|=valore;
    }

    tipo_tipo_articolo operator() () {
      return stato;
    }

    tipo_tipo_articolo operator() ( tipo_tipo_articolo flag, bool _stato ) {
      if ( !_stato ) {
        flag=~flag;
        stato&=flag;
      }
      else
        stato|=flag;
    }

    QString descrizione();

    bool gap() {
      return ( ( stato&GRUPPO_ARTICOLO_PURO )?true:false );
    }

    bool gas() {
      return ( ( stato&GRUPPO_ARTICOLO_SELEZIONE )?true:false );
    }

    bool gav() {
      return ( ( stato&GRUPPO_ARTICOLO_VARIANTE )?true:false );
    }

    bool gavc() {
      return ( ( stato&( GRUPPO_ARTICOLO_VARIANTE|GRUPPO_ARTICOLO_COMMENTO )==
         ( GRUPPO_ARTICOLO_VARIANTE|GRUPPO_ARTICOLO_COMMENTO ) )?true:false );
    }

    bool gac() {
      return ( ( stato&GRUPPO_ARTICOLO_CONSOLIDANTE )?true:false );
    }

    bool gam() {
      return ( ( stato&GRUPPO_ARTICOLO_MESSAGGIO )?true:false );
    }

    bool gapm() {
      return ( ( stato&GRUPPO_ARTICOLO_PIUMENO )?true:false );
    }

    bool gavm() {
      return ( ( stato&GRUPPO_ARTICOLO_PIUMENO )?true:false );
    }

    bool gavf() {
      return ( ( stato&GRUPPO_ARTICOLO_VEROFALSO )?true:false );
    }

    bool gasv() {
      return ( ( gas()&&gav() )?true:false );
    }

    bool gapv() {
      return ( ( gap()&&gav() )?true:false );
    }

    bool gdv() {
      return ( ( stato&GRUPPO_DESCRIZIONE_VARIABILE )?true:false );
    }

    bool gpeso() {
      return ( ( stato&GRUPPO_ARTICOLO_A_PESO )?true:false );
    }

    bool gapr() {
      return ( ( stato&GRUPPO_ARTICOLO_PREZZO_RICHIESTO )?true:false );
    }
  }; // class tipo articolo



  // ========================================================

  class gruppi:public base {
  public:

    gruppi() {
      struttura="+IS|-COD|#PRZ|$DESCR<|-FIGLI|-TIPO|-RAGRP|-RIS|!PADRE|-LISTINO|-IVA|-REPARTO|-GRPPRN|-PORZ";
    }



    class gruppo {
    public:
      gruppo():tipo_articolo( 0 ) {
        is=-1;
        id_padre=-1;
        id_listino=-1;
        ta=&tipo_articolo;
        iva=-1;
        porzioni=0;
      }

      gruppo( int _ta ):tipo_articolo( _ta ) {
        is=-1;
        id_padre=-1;
        id_listino=-1;
        ta=&tipo_articolo;
        iva=-1;
        porzioni=0;
      }

      union {
        int is;
        int id_istanza;
      };

      QString codice;
      double prezzo;
      QString descrizione;
      bool ha_figli;
      tr::tipo_articolo tipo_articolo;
      tr::tipo_articolo*ta; // alias di comodo per tipo_articolo
      int stato_extra;
      QString id_articolo;

      union {
        int id_padre;
        int is_padre;
        int id_istanza_padre;
      };

      int id_listino;
      double iva;
      int reparto;
      int porzioni;

      enum tr_posizioni {
        p_id_istanza, p_codice, p_prezzo, p_descrizione, p_ha_figli, p_tipo_articolo, p_stato_extra, p_id_articolo, p_id_padre,
           p_id_listino, p_iva, p_reparto, p_grpstampa, p_porzioni, p_last
      };

      bool isNull() {
        return ( is==-1 /* &&is_padre==-1&&id_listino==-1 */ ?true:false );
      }

      bool parse( QString ); // parsa la stringa e riempie i campi della classe
      bool carica( int is ); // carica la 1^ riga. x ora non funziona
    };



    bool carica( int da_id=0, bool espandi=false, bool solo_articoli=false );

    gruppo parse( int row ) {
      gruppo g;
      g.parse( elenco[row] );
      return g;
    }

    enum categorie {
      segue=100, insieme_a=101, conto_diviso=200, pagamenti=400, pagamenti_a=999, articolo_base=1000
    };
  }; // class gruppo



  class varianti:public gruppi {
  public:
    varianti():gruppi() {
    }

    bool carica( int da_id=0 );
  };



  // ========================================================

  class menu:public base {
  public:
    menu() {
      struttura="";
    } // non ustata



    class voce {
    public:
      voce():tipo_articolo( 0 ) {
        is=0;
        id_listino=0;
        locazione=loc_indefinita;
        disposizione=-1;
        geometria=-1;
        descrizione="";
        icona=LEAVE;
        pixmap=LEAVE;
      }
      voce( const tr::gruppi::gruppo&g );

      int id_listino;

      union {
        int id_istanza;
        int is;
      };

      union {
        int id_istanza_padre;
        int is_padre;
      };

      tr::tipo_articolo tipo_articolo;

      enum tr_locazione {
        loc_indefinita=-1, loc_corolla=0, loc_elementi=1
      };

      tr_locazione locazione;
      int disposizione;
      int geometria;
      QString descrizione;
      QString descrizione_originale;
      QColor colore;
      QString icona;
      QString pixmap;

      bool corolla() {
        if ( locazione==loc_corolla ) return true;
        else
          return false;
      }

      bool elementi() {
        if ( locazione==loc_elementi ) return true;
        else
          return false;
      }

      enum tr_posizioni {
        p_id_listino, p_id_istanza, p_id_istanza_padre, p_tipo_articolo, p_locazione, p_disposizione, p_geometria,
           p_descrizione, p_descrizione_originale, p_colore, p_icona, p_pixmap, p_last
      };

      bool varianti();
      bool parse( const QString ); // parsa la stringa e riempie i campi della classe
      bool salva(); // aggiorna/aggiunge questa voce al server
    }; // class voce



    bool carica( int id_listino );

  private:
    QStringList _data; // per futura implementazione e ottimizzazione
  }; // class menu



  // ========================================================

  class ordine:public base { // contiene la portata e comanda assieme
  public:

    ordine() {
      struttura="+IR|-IS|-LIV|-ART|#QTA|DESCR|-PRZ|#TOT_RIGA|-TIPO|-OFFERTO|-LISTINO|+COMANDA";
    }



    class riga {
    public:
      riga():tipo_articolo( 0 ) {
      }

      int id_riga;
      int id_istanza;
      int livello;
      QString id_art;
      money qta;
      QString descrizione;
      money prezzo;
      money tot_riga;
      tr::tipo_articolo tipo_articolo;
      bool offerto;
      int id_listino;
      bool portata;

      enum tr_posizioni {
        p_id_riga, p_id_istanza, p_livello, p_id_art, p_qta, p_descrizione, p_prezzo, p_tot_riga, p_stato, p_offerto,
           p_id_listino, p_portata, p_last
      };

      bool parse( QString ); // parsa la stringa e riempie i campi della classe
    }; // class riga



    QString totale;
    QString resto;
    QStringList elenco;
    int riga_portate; // da quale riga in elenco cominciano le portate
    QStringList documenti;
    QString coperti;
    QString non_prezzati;

    enum tipo {
      t_comande, t_portate, t_ordine, t_separata, t_conto, t_conti, t_pagamenti
    };

    bool carica( tipo cosa );

    riga parse( int n_riga ) {
      riga r;
      r.parse( elenco[n_riga] );
      return r;
    }


  }; // class ordine



  // ========================================================

  class tavoli:public base {
  public:
    tavoli() {
      struttura="+id_tavolo|$nome_tavolo|-id_sala|-pixmap|-nome_sala";
    }



    class tavolo {
    public:
      int id;
      QString nome;
      uint stato;
      QString pixmap;
      QString sala;

      enum tr_posizioni {
        p_id, p_nome, p_stato, p_pixmap, p_sala, p_last
      };

      bool parse( QString s );
    }; // class tavolo



    enum tr_ordine {
      ordine_nessuno=-1, ordine_sala_tavolo=0, ordine_sala_tavolo_inverso=1, ordine_id=2
    };

    bool carica( tr_ordine ordine=ordine_nessuno );
  }; // class tavoli


}; // class tr

// ========================================================

// ========================================================

#endif
