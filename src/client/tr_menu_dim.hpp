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

#ifndef TR_MENU_DIM_HPP
#define TR_MENU_DIM_HPP

#include "tr_label.hpp"

#include <qdict.h>

class tr_menu_dim;
class frm_base;

class tr_menu_button:public tr_label
{
    Q_OBJECT;
public:
    tr_menu_button(tr_menu_dim * parent, QString name, WFlags f = 0 );
    ~tr_menu_button() {};

    enum tr_tipo {
      t_nondefinito,t_corolla,
      t_next_corolla,t_next_elementi,
      t_elemento,
      t_path,t_path_main};

    void setidx(int i);
    int getidx() {return idx;}

    void settipo(tr_tipo tp);
    void settipo(char c);
    tr_tipo gettipo() {return tipo;}

protected:
    friend class tr_menu_dim;
    int idx;		   // idice nell'elenco delle voci di menu corrente
    tr_tipo tipo;  // ricavato anche da parent()->voci[idx][p_stato]
};

class tr_menu_dim: public tr_frame
{
    Q_OBJECT;
public:
    tr_menu_dim( QWidget * parent, const char * name, WFlags f = 0 );
    ~tr_menu_dim() {};

    enum tr_locazione {
      loc_indefinita=-1, loc_corolla=0, loc_elementi=1
    };

    // costruisce il menu' secondo la struttura specificata

    tr_menu_dim*build(QString struttura);

    // carica il menu del listino
    bool carica(QString id_listino);
    void enable_cache(bool enable); 	// abilita cache per tutti i listini

    QString grp;				// per il set di un pulsante dall'esterno
    QSize bsize;
    uint spessore;      // spessore pulsante

    bool cd(int idx);   /* cambia directory del percorso di menu'
                            idx indica l'indice di voci ma
                            si veda button_clicked() e i dettagli
                            nel corpo */

signals:
    void clicked();

public slots:
    void button_clicked();

protected:

    friend class tr_menu_button;
    friend class frm_base;

    static tr_menu_dim* top_owner;  /* puntera' al contenitore piu' esterno
                                cui indirizzare gli eventi di pulsante premuto */
    static tr_menu_dim* last_inside; /* ultimo spazio interno creato */
    static tr_menu_button*last_path; /* ultimo pulsante del path usato */

    QDict<QStringList> listini;		// cache listini
    bool cache;

    QStringList voci;	// elenco menu listino corrente
    uint page_cor_size;           // dimensione in pulsanti della corolla
    uint page_cor;                // pagina corolla corrente
    uint page_ele_size;           // dimensione in pulsanti della corolla
    uint page_ele;                // pagina corolla corrente

    QFrame *percorso;             // contenitore bottoni percorso
    QFrame *articoli;             // contenitore bottoni articoli

    void aggiorna(int idx=-1);    // ridisegna in base a page_cor e page_ele
};


/*
struttura della stringa da passare alla funzione build()
es x formato classico:
"111112\n
 1iiii1\n
 1iiii1\n
 1iiii1\n
 1iiii1\n
 1iiii1\n
 111113"

 oppure:

"??????\n   crea un layout verticale con pannelli separati per ?????? e iiiii
 iiiiii"

  oppure

"1iiiii\n
 1iiiii\n
 1iiiii\n
 1iiiii\n
 111113"


 dove:
     1 	pulsante gruppo
     2	pulsante prossima pagina corolla
     3	pulsante prossima pagina pannello articoli
     5  pulsante percorso (il primo e' main) contenuto nella prima riga dei 4
     4 	pulsante articolo
     -  pulsante esteso al successivo (la sequenza deve finire con 4)
     i ha una doppia valenza e significa sia ignora che inside o interno e funge
       da segna posto per sezioni interne e idefinite che verranno poi specificate
       ad esempio con:

     55555		pulsante percorso
     66666    pulsante articolo unico
     44444		pulsanti articolo o sottogruppo
     44444		l'ultimo pulsante in basso a destra (o sinistra per mancini)
              viene sostituito dal 3
              se nella corolla non viene specificato

*/


#endif
