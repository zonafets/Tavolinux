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

#ifndef TR_LISTVIEW_H
  #define TR_LISTVIEW_H

  #include <qlistview.h>
  #include <qlineedit.h>
  #include <qlabel.h>
  #include <qlayout.h>
  #include <qbutton.h>

  #include "tr.hpp"
  #include "tr_srvcmd.hpp"
  #include "tr_label.hpp"
  #include "tr_dmacro.hpp"
  #include "tr_common.hpp"

  #include <typeinfo>

  #define EV_UPDATE 65432
  #define LV_LINK( tr_listview ) connect( tr_listview, SIGNAL(clicked(QListViewItem*)), this, SLOT( tr_listview##_clicked(QListViewItem*) ))

/* composta da una qlistview, e una barra di scorrimento che a sua volta �composta da freccia su, ABC, ..., XYZ, freccia_giu */

class tr_listviewitem: public QListViewItem {
public:
  tr_listviewitem( QListView *parent , QString string1 );
  tr_listviewitem( tr_listviewitem *parent , QString string1 );
  void paintCell (
    QPainter * p,const QColorGroup & cg, int column,
    int width, int align );

private:
  friend class tr_listview;
};

class Q_EXPORT tr_listview:public tr_widget {
  Q_OBJECT;
public:

  tr_listview( QWidget*parent=0, const QString name=QString::null );
  ~tr_listview();

  // riporta la riga delle listbox
  QString selezionato(); // chiave di quello correntemente selezionato o null

  static const QString e_next;
  static const QString e_last;
  static const QString e_none;
  static const QString e_first;

  bool seleziona( QString chiave ); // false se riga è fuori dal range e != da e_last
  bool seleziona( QListViewItem *lit );
  // selezione multipla o check multiplo
  bool seleziona( QStringList elenco_chiavi );

  bool deseleziona( QString chiave );
  bool deseleziona( QListViewItem *lit );

  private: bool _unselectable;
  public:
  bool unselectable() {return _unselectable;}
  void unselectable(bool b) {_unselectable=b;}

  //  bool trova( QString chiave ); // -1 se non trovato
  int righe() { return elenco->childCount(); } // riporta il numero di righe nella lista (compresi i figli)

  // nel caso di selezione multipla
  // restituisce l'array delle righe selezionate nella selezione multipla
  // restituisce l'array delle righe checkkate nel caso di elenco con chk_box
  QStringList selezionati();
  void selezionati( bool tutti ); // seleziona tutti gli elementi

  // con "gruppi" vengono visualizzati nella struttura ad albero anche i sottogruppi
  // con "articoli" vengono visualizzati solo quelli del livello specificato
  enum tr_tipo {
    gruppi, articoli, pagamenti, sale, listini, listini_modifica,
    clienti, clienti_esteso, clienti_rapidi, selezione_clienti,
    camerieri, stampanti, turni,
    selezione_pagamenti, //
    // appaiono i pagamenti con checkbox
    tipo_tavoli, tipo_oggetti, tipo_pavimenti, tavoli, bis,
    conti_ieri, conti_ultimi, log_com, tipo_doc, tipo_doc_e_pagamenti,
    messaggi,prenotazioni, tavoli_semplici,
    indefinito
  };

  // imposta il contenuto. Nel caso degli articoli, id rappresenta il gruppo da caricare
  void tipo( tr_tipo tipo );
  // simile al precedente, converte il token in tipo
  void tipo_elenco( TRTokens token, QString params=QString::null );
  void aggiorna();

  // gestione elenco fittizio
  void tipo_fittizio(QString struttura, QStringList records=QStringList());
  void aggiungi(QString record);
  void aggiungi(QStringList records);
  void cancella(QString record);
  void aggiorna( QString chiave, QString record ) {
    tr_key*k=key( chiave );
    if (k) dati.dati.elenco[k->idx]=record;
  }
  void aggiorna( QListViewItem *lit, QString record ) {
    tr_key*k=key( lit );
    if (k) dati.dati.elenco[k->idx]=record;
  }

  tr_tipo tipo() { return _tipo; }

  // carica la struttura specificata da tipo o la ricarica
  bool carica( QString id_selezione=QString::null, QString id_gruppo=QString::null );

  // carica la struttura di dati generici elenco formattandoli secondo struttura
  bool carica( QStringList elenco, QString struttura=QString::null,
               bool chk_box=false, bool icone=false, QString maschera=QString::null );

  // funzioni di comodo per chiamante
  QString id(); // restituisce la chiave dell'elemento selezionato
  QString descrizione(); // restituisce la descrizione dell'elemento selezionato
  QString record(int prel=0); /* restituisce la descrizione dell'elemento
                                  selezionato se prel=0
                                  precedente se prel<0
                                  successivo se prel>0
                              */
  void onoff(bool v=true) {_onoff=v;}
  QString is_grp_varianti() { return _is_grp_varianti; } // valido solo su carica gruppi

  // funzioni che restituiscono i dati relativi all'elemento selezionato
  // se chiave è null, restituisce quello correntemente selezionato
  // la chiave si usa nel caso di selezioni multiple
  tr::gruppi::gruppo articolo( QString chiave=QString::null );
  tr::gruppi::gruppo pagamento( QString chiave=QString::null );
  tr::ordine::riga portata_comanda( QString chiave=QString::null );
  tr::tavoli::tavolo tavolo( QString chiave=QString::null );
  tr::mse::riga sala( QString chiave=QString::null ) { return dato(chiave);}
  tr::mse::riga listino( QString chiave=QString::null ) { return dato(chiave);}
  tr::mse::riga cliente( QString chiave=QString::null ) { return dato(chiave);}
  tr::mse::riga cliente_esteso( QString chiave=QString::null ) { return dato(chiave);}
  tr::mse::riga stampante( QString chiave=QString::null ) { return dato(chiave);}
  tr::mse::riga tipo_tavolo( QString chiave=QString::null ) { return dato(chiave);}
  tr::mse::riga tipo_pavimento( QString chiave=QString::null ) { return dato(chiave);}
  tr::mse::riga cameriere( QString chiave=QString::null ) { return dato(chiave);}
  tr::mse::riga conto( QString chiave=QString::null ) { return dato(chiave);}

  // per uso generico
  const QStringList elenco_dati(bool ordinato=false) // ritorna tutto l'elenco
  {
    if (!ordinato) return dati();
    else {
      QStringList lst;
      QListViewItemIterator it( elenco );
      while ( it.current() ) {
        tr_key *k=key(it.current());
        if (k) lst<<dati()[k->idx];
        ++it;
      } // while
      return lst;
    } // else
  }

  tr::mse::riga riga( QString chiave=QString::null ) { return dato(chiave);}
  void riga(int n_riga,tr::mse::riga r) {dati()[n_riga]=r();}
  tr::mse::riga riga(int n_riga) {tr::mse::riga r(dati()[n_riga]); return r;}

  void setFont(const QFont &f) {tr_widget::setFont(f);if (elenco) elenco->setFont(f);}


  QListView*elenco;
  bool rollone;

protected:
  bool event(QEvent *e);

  void adjustHeaderSize();

private:

  bool carica_struttura( QString stuttura, QString riga0=QString::null );

  // elementi grafici che compongo la tr_listview

  QFrame*barra; // contiene su+abc+giu
  tr_label*su;
  QFrame*abc;
  tr_label*space_above; // quando abc �nascosto, space fa da spaziatore
  bool _onoff;
  tr_label*destra;

public:
  tr_label*more;
  tr_label*exit;
  tr_label*less;
private:
  tr_label*sinistra;
  tr_label*space_below; // quando abc �nascosto, space fa da spaziatore
  tr_label*giu;
  QGridLayout*layout;
  QVBoxLayout*abc_layout;
  QVBoxLayout*barra_layout;
  QLabel*titolo; // etichetta per eventuale titolo

public:

  enum tr_elemento { e_barra, e_abc, e_exit, e_header, e_moreless, e_dxsx };

  void scopri( tr_elemento e );
  void nascondi( tr_elemento e );

  void setText( QString titolo ); // abilita e imposta il titolo

  // adatta w col 0 a w widget. Chiamare SOLO da frm::show
  void adatta( QString maschera=QString::null );

  // legge/imposta l'altezza della riga della listview
  int altezza_riga() { return _altezza_riga; }

  void altezza_riga( int ar ) { _altezza_riga=ar; }

  bool darkable() { return _darkable; }

  void darkable( bool d ) { _darkable=d; }
  void abc_clic_effect(bool valore)  {_abc_clic_effect=valore;}

  void clear();
  /* reimplementa la cancellazione della listview chiamando la DELETE della gestione di memoria locale */

  // da accesso dall'esterno ai dati, permettendo di escludere dei valori
  // se key contiene dei pipe (|) fa da filtro negativo. Es. |||RISTORANTE
  // filtra i dati della 4^ colonna escludendo quelli che contengono RISTORANTE
  // se inizia per !, significa inverso ossia nega. Es |||!RISTORANTE
  // se ci sono {} tratta come RegExp. Es  |{[\.]}|||RISTORANTE
  // e al negativo: |!{[\.]}|||RISTORANTE
  // se key contiene <X o >X o &X (x=numerico), viene applicato il filtro numerico
  // key=QString::null svuota l'elenco
  void escludi( QString key );

signals:
  void mouseButtonClicked( int, QListViewItem*it, const QPoint&, int );
  void clicked( QListViewItem*);
  void selectionChanged(QListViewItem*);
  void close();


  public slots:

  void elenco_mouseButtonClicked( int, QListViewItem*, const QPoint&, int );
  void elenco_clicked( QListViewItem*it, const QPoint &, int );
  void elenco_selectionChanged( QListViewItem*it);
  void header_clicked(int section);
  void abc_clicked();
  void su_clicked();
  void giu_clicked();
  void more_clicked();
  void less_clicked();
  void destra_clicked();
  void sinistra_clicked();

  void exit_clicked() {emit close();}


private:
  // int _righe;
  bool _abc_clic_effect;

  QListViewItem *last_opened;  // vedere rollone

  QString _is_grp_varianti;

  /* questa classe si occupa di traslare sorgente fisica dei dati nella loro rappresentazione nella listview */

  friend class tr_listviewitem;

  // QListViewItem -> chiave,posizione
  class tr_key {
    public:
    tr_key() {it=NULL;key[0]='\0';idx=-1;stato=QCheckListItem::Off;
              estensione=QString::null;}
    QListViewItem *it;
    char key[80]; // chiave
    int idx; // indice per dati[idx]
    QCheckListItem::ToggleState stato; // stato precedente al click, usato
    // per sopperire a mancanze delle Qt
    QString estensione; // icona o colore del testo
  };

  tr_PtrDict<tr_key>indici;
  tr_key *key(QListViewItem *lit) { return indici.find(lit);}
  tr_key *key(QString chiave);

  tr_key* aggiungi( QString key, int idx, QListViewItem*it );

  // allinea lo stato di selezione degli item all'indice
  void aggiorna_stati();
  void clearSelection();

  // estrapola il record da dati[] relativo a chiave
  QString record( QString chiave );
  QString record( QListViewItem* );

  // usando queste funzioni , si tengono aggiornati anche gli indici
  void rimuovi( QListViewItem* );

  // funzioni di utilità che restituiscono la chiave di controparte
  public:
  QString chiave( QListViewItem* );
  QListViewItem*chiave( QString );

  // funzione di utilità (inline)
  enum tr_modo { e_chiave, e_descrizione };

  QString estrapola( const QStringList&fields, const QValueList<int>&colonne, tr_modo modo );

  QValueList<int>col_sort; // attualmente non usato
  QValueList<int>col_grp; // raggruppa per ...
  QValueList<int>col_nome; // ... e mostra con nome ...
  QValueList<int>col_chiave; // ... relativamente a
  QValueList<uint>col_max_size; // dimensione in char max di ogni colonna
  // ordine colonne
  QValueList<int>colonne;
  QValueList<int>col_num;
  QValueList<int>col_grps; // raggruppa per colonna stringa
  bool chiave_numerica;

  // verifica se la colonna di raggruppamento è numerica, così ottimizza
  bool grp_numerico;
  int grp_len;
  /* lunghezza a cui adeguare il codice numerico perchè sia ordinabile come stringa */

  // array di chiavi per esclusione elementi
  QStringList esclusioni;

  // filtri per evidenziazione righe
  QStringList highlights;

  // maschera di visualizzazione (come Microsoft impone)
  QString maschera_di_visualizzazione;

  tr_tipo _tipo;
  bool _darkable;
  int _altezza_riga;

  tr::mse::riga dato(QString chiave); // restituisce la riga
  /* contiene i dati caricati dal server e le string list corrispondenti al contenuto della listview */
  struct tr_dati {
    tr::gruppi gruppi;
    tr::tavoli tavoli;
    tr::mse dati;
    bool check_box,espandi,icone;
    QString maschera;
    QString params;
    void carica();
    tr_listview *lv;
    QString header;
    QStringList operator()(){return dati.elenco;}
  }
  dati;

  public:
  void header(QString hd) {dati.header=hd;}


  friend class frm_base;

};



inline QString tr_listview::estrapola( const QStringList&e, const QValueList<int>&cols, tr_listview::tr_modo modo ) {

  QString string;

  switch ( modo ) {

    case e_chiave:
      for ( uint j=0; j<cols.size(); j++ ) {
        QString key_part=e[cols[j]];
        if ( !string.isEmpty() ) string+='|';
        string+=key_part;
      }
    break;

    case e_descrizione:
      if ( cols.count()>0 ) {
        int j=cols.count()-1;
        while ( j>=0 ) {
          if ( !e[cols[j]].isEmpty() ) {
            string=e[cols[j]];
          break;
          }
          j--;
        }
      }
      else
        string=estrapola( e, cols, e_chiave );

  } // switch
  return string;
};

#endif
