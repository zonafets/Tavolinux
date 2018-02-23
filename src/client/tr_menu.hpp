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

#ifndef TR_MENU_HPP
  #define TR_MENU_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Il menu presenta 5 sezioni principali: 1. corolla esterna 2. percorso 3. elementi 4. pulsante pagina corolla successiva
5. pulsante pagina elementi successivi

La pagina elementi esprime il contenuto dell'ultimo elemento del percorso

a. Facendo click su un elemento di corolla (che sia anche un gruppo di selezione)
viene selezionato, messo nel primo elemento del percorso (/root) e poi espanso
b. La selezione di un elemento che a sua volta �anche un gruppo di selezione,
viene aggiunto al percorso (cd art_selezionato) ed espanso. c. Il click su un elemento che non sia gruppo di selezione (sia
che si trovi in corolla che tra gli elementi) provoca solo la sua selezione  */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  #include <qsize.h>
  #include <qwidget.h>
  #include <qdict.h>
  #include <qptrlist.h>
  #include <qvaluevector.h>

  #include "tr_widget.hpp"
  #include "tr_label.hpp"
  #include "tr.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  #define N_VOCI_MENU 1049
  #define N_VOCI_COROLLA  97  // numero primo vicino al numero di voci di 3 corolle
  #define N_VOCI_ELEMENTI 229 // numero primo vicino al numero di voci di 3 pagine

  #define PERCORSO_MAIN "<img name=""img"" src=""az_main.png"" width=""32"" height=""32""><br><b>main</b>"
/*
  #define COLORE_SELEZIONATO QColor(246,246,255)
  #define COLORE_SFONDO_COROLLA QColor(213,215,255)
  #define COLORE_SFONDO_ELEMENTO_IN_COROLLA QColor(212,227,254)
  #define COLORE_SFONDO_ELEMENTI QColor(174,228,255)
  #define COLORE_SFONDO_PULSANTI QColor( 177, 179, 217 )
  #define COLORE_SFONDO_PERCORSO QColor( 184, 255, 213 )
  #define COLORE_SFONDO_PERCORSO_EVIDENZIATO COLORE_SELEZIONATO // QColor( 214, 255, 243 )
  */

  #define INIT_DIM_COROLLA 26
  #define INIT_DIM_ELEMENTI 30

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef tr_IntDict < tr::menu::voce > tr_dict_voci;

// ==============================================================

/* questa classe dichiarata per convenienza riposta la geometria del menu
ossia la dimensione (in numero di pulsanti) della corolla e la dimensione (in numero di pulsanti) dell'area elementi */

class tr_size : public QSize
{
public:
  tr_size( int corolla, int elementi ) : QSize( corolla, elementi )
  {
  };

  int corolla()
  {
    return width();
  }

  int elementi()
  {
    return height();
  }
};


class tr_menu;

// ==============================================================

class tr_menu_pulsante : public tr_label
{
  Q_OBJECT;

  public:

  tr_menu_pulsante( QWidget * parent );

  ~tr_menu_pulsante(){};

  enum tr_tipo {t_corolla, t_elementi, t_main_percorso, t_percorso, t_pulsanti } _tipo;

  // in base al tipo restituisce il colore base di questo pulsante
  QColor colore();
  // imposta sfondo
  void sfondo( QColor c, QString testo, QString pixmap=QString::null );
  // reimposta sfondo ai valori originali
  void sfondo();
  // reimposta lo sfondo con i valori della voce di menu
  void sfondo( tr::menu::voce *voce );
  // impostazione iniziale
  void sfondo( tr_tipo tipo );

  void evidenzia();
};




// ==============================================================

class tr_menu : public tr_widget
{
  Q_OBJECT;

public:
  tr_menu( QWidget * parent, const char * name, WFlags f = 0 );
  ~tr_menu();

  // la geometria del percorso è data dalla larghezza della corolla - 2 e altezza 1
  // questa funzione fa:
  // 1. imposta la struttura o ristruttura se diversa dalla precedente
  //    riempiendo le strutture corolla, percorso, elementi
  // 2. pulisce se = alla precedente
  //    compensando il contenuto con le strutture corolla, percorso, elementi
  // Il chiamante si deve occupare di non chiamarla inutilmente
  void geometria(
  int elementi = 0, // è necessario che prima sia stata definita la dimensione
  int corolla = 0 ); // dello widget

  int larghezza(); // fornisce la larghezza calcolata in base alla corolla e alla risoluzione del desktop

  /* seleziona/cambia menu corrente carica la struttura menu e compila le strutture corolla, elementi e imposta la geometria */
  bool cambia( int id_listino , bool visualizza=true, bool mantieni_percorso=true );
  // la seguente versione gestisce la cache contenuta in glb.cache_menu_listino
  bool cambia( QString id_listino , bool visualizza=true, bool mantieni_percorso=true );

  private:

  // overloading comune alle precedenti
  bool cambia( QStringList elenco , bool visualizza=true, bool mantieni_percorso=true );

  public:

  // struttura indice per reperimento pulsante e voce da
  struct tr_key {
    tr_key() {is=0;}
    tr_menu_pulsante *pulsante;
    int is;
    void ridisegna(tr_menu *menu);
    private:
  };

  // riporta la voce di menù corrente (l'ultima selezionata)
  tr_key * corrente(); // in modalità m_uso è sempre NULL
  // complementare di corrente() restituisce la voce selezionata
  tr::menu::voce * selezionata(); // in modalità m_uso riporta l'ultima selezionata
                                  // in modalita' m_design è quella di corrente
  tr::menu::voce * annullato() const;

  void deseleziona();

  // entra nel gruppo e/o lo aggiunge a percorso
  enum cd_azioni {varianti=-2, precedente=-1, main=0 /*,id_istanza ... */};
  tr_key* cd( int id_azione_o_istanza, int is_extra=-1 );

  /*  restituisce l'ultimo elemento del percorso */
  tr_key * cd();

  int voci() { return menu.count();}

  // aggiunge un articolo su corrente()
  bool aggiungi( tr::gruppi::gruppo * grp );
  // inserisce un articolo sul corrente e sposta tutti gli altri
  bool inserisci( tr::gruppi::gruppo * grp );
  bool rimuovi(); // rimuove tutto il menu dal server
  bool cancella(); // cancella la voce corrente()
  bool disponi( const QStringList elenco ); // aggiunge a cd()

  // cancella a video e in memoria. Usato per sincronizzare con la cancellazione
  // del gruppo
  void cancella(int id);

  void annulla(); // ripristina l'elemento del menu cancellato (vedi cancella())
  void design() {modalita=m_design;}
  void uso() {modalita=m_uso;}

public slots:
  void pagina_corolla_clicked();
  void pagina_elementi_clicked();
  void corolla_elementi_clicked();
  void percorso_clicked();

signals:
  void clicked();

private:

  tr_menu_pulsante* pagina_corolla, *pagina_elementi;

  // modalita' di funzionamento del menu
  enum {m_non_specificato, m_uso, m_design} modalita;

  tr_dict_voci menu; // autoDeleted, contiene tutte le voci di menu

  /* una futura implementazione ottimizzata puo' essere costituita da
  QValueList<QStringList> menu;
  */

  bool modificato; // indica se il menù a subito delle modifiche

  /* impostato a 0.9 nel caso dell'800x600  per i tasti del menu */
  double aspect_ratio;

  int id; // id listino associato al menu e viceversa
  tr_size dimensione; // vedere geometria(e,c)
  tr::menu::voce *voce_annullata; // vedere cancella()

  // elementi grafici
  // i layout si ricavano dal membro layout degli widgets
  QFrame *frame_percorso;         // area su cui stanno i pulsanti percorso
  QFrame *frame_elementi;         // area interna dove stanno gli elementi
  QFrame *frame_contenitore;

  tr_key *_corrente;              // puntatore a ultimo pulsante/voce premuto

  class tr_key_list: public QValueVector<tr_key> {
    public:
    tr_key_list():QValueVector<tr_key>() {}
    void clear(QLayout *l=NULL);
    tr_key* chiave(tr_menu_pulsante *p);
    tr_key* chiave(tr::menu::voce *v);
    tr_key* chiave(int id_istanza);
    void append(tr_menu_pulsante *p, tr::menu::voce *v=NULL);
    //iterator append(tr_menu::tr_key &k) {return QValueList<tr_key>::append(k);}
    void append(tr_menu::tr_key &k) {return QValueVector<tr_key>::append(k);}
    void ridisegna();  // ridisegna tutti i pulsanti
    int posizione(tr_menu_pulsante *p);
    int posizione(tr_key *k);
    int posizione(); // il primo pulsante libero
    void pagina_clicked();

    private:
    friend class tr_menu;
    tr_menu *menu;
    int pagina;
    int prec_pagina;
    tr::menu::voce::tr_locazione locazione;
    int ultima_pagina( int id_padre );
    void vuota(); // toglie voci (*.voce=null)
    void init(
      tr_menu *m,
      tr::menu::voce::tr_locazione loc=tr::menu::voce::loc_indefinita)
      { menu=m; pagina=0; prec_pagina=-1; locazione=loc; };
  };

  // array di pulsanti, relativi al campo "disposizione" delle voci di menu
  enum tr_indici {corolla,percorso,elementi,num_indici};
  tr_key_list indici[num_indici];

  // funzioni di reperimento chiave in base al campo
  tr_key* chiave(tr_menu_pulsante *p,
    tr::menu::voce::tr_locazione dove=tr::menu::voce::loc_indefinita);
  // funzioni di reperimento chiave in base al campo
  tr_key* chiave(tr::menu::voce *v);

  // recupera la voce dal menu con la seguente chiave
  tr::menu::voce* voce(tr::menu::voce::tr_locazione, int id_padre, int disposizione );

  // recupera la voce dal menu con la seguente chiave
  tr::menu::voce* voce(tr::menu::voce::tr_locazione, int disposizione );

public:
  // recupera la voce dal menu con la seguente chiave
  tr::menu::voce* voce( int is );

private:
  // recupera la voce dal menu con chiave k o perdefault da CD()
  tr::menu::voce* voce( tr_key *k );

  tr::menu::voce* voce_padre( int is_padre );

  // verifica se la posizione è visibile a video ossia nella pagina corrente
  bool visibile(tr::menu::voce *v);

  // funzioni per ridisegno grafico
  void adatta_testo( tr_label * l ); // sillaba etc..
};

inline tr::menu::voce * tr_menu::annullato() const
{
  return voce_annullata;
}

#endif
