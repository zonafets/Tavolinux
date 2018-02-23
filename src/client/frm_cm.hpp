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

#ifndef FRM_CM_H
#define FRM_CM_H

/*
  form per inserimento comande
   */

#include "frm_base.hpp"
#include "tr_menu.hpp"
#include "tr_label.hpp"
#include "tr_comanda.hpp"
#include "tr_tavoli_view.hpp"
#include "tr_listview.hpp"
#include "frm_scelta.hpp"
#include "frm_ok_exit.hpp"
#include "tr_lineedit.hpp"
#include "tr_input.hpp"
#include "tr_time.hpp"
#include "tr.hpp"
#include "tr_iconviewer.hpp"
#include "tr_colori.hpp"
#include "tr_enumeratori.hpp"

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qstring.h>


class frm_cm: public frm_base {
    Q_OBJECT;

public:
    frm_cm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = MYFLAGS );
    ~frm_cm();

    tr_label* listino;
    tr_label* annulla;
    tr_label* libero;
    tr_label* aggiungi;
    tr_label* varianti;
    tr_label* altro;
    tr_label* proprieta;
    tr_label* cancella;
    tr_label* exit;
    tr_menu* menu;
    tr_listview* gruppi;

    void stato_pulsanti();
    bool annullato();		// indica  se c'�qualcosa che si pu�annullare in questa form
    void frm_gruppi_stampa();
    void frm_stampa(QString tipo);

    int id_listino;		// copia locale per gestione temporanea

    // variabili per persistenza e comodit�dell'inserimento ripetitivo/consecutivo di varianti e articoli
    money ultimo_prezzo;
    money ultima_percentuale;
    int ultimo_modo;

public slots:
    void show();

    void listino_clicked();
    void annulla_clicked();
    void aggiungi_clicked();
    void varianti_clicked();
    void altro_clicked();
    void proprieta_clicked();
    void cancella_clicked();
    void exit_clicked();

    void menu_clicked();
    void gruppi_clicked( QListViewItem *it ) ;


protected:
    void barcodeReaded(QString);

    void init();
    int precedente_grp_is;
    bool aggiorna_gruppi; // influisce in stato_pulsanti
    // void modifica_gruppo();
    void modifica_gruppo(tr::gruppi::gruppo& grp);

};

////////////////////////////////////////
//// FORM SECONDARIE
///////////////////////////////////////

class frm_cm_proprieta: public frm_ok_exit {
    Q_OBJECT;

    public:
	frm_cm_proprieta( frm_cm* parent = 0, const char* name = 0, bool modal = true, WFlags fl = MYFLAGS );
	~frm_cm_proprieta();

	QLabel* label_descrizione;
	tr_input* descrizione;
	tr_input* desc1;
	tr_input* desc2;
	tr_input* desc3;
        tr_input* prezzo;
        tr_colori* colori;
        tr_iconviewer *icone;

	tr_label* ok;
	tr_label* exit;

	QColor colore()			{ return colori->selected(); };
        QString icona()			{ return icone->selected(); };

	void no_colori();				// nasconde o disabilita i colori
        void modifica(
          QString descr,QString descr1, QString descr2,QString descr3,
          QColor colore, QString icona);

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class frm_cm_listini: public frm_scelta, public tr::enumeratori::listino_esteso {
    Q_OBJECT;

    public:
	frm_cm_listini( frm_cm* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = MYFLAGS );
	~frm_cm_listini();

	int id_listino;		// copia locale per gestione temporanea

	void stato_pulsanti();
	//void show();

    public slots:
	void elenco_clicked(QListViewItem*);
	void add_cop_mod_clicked();
	void cancella_clicked();

    protected:
	void init();

    private:
	frm_cm *form_cm;

    public:
    };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class frm_cm_listini_inserisci: public frm_ok_exit {
    Q_OBJECT

    public:
	frm_cm_listini_inserisci( frm_cm_listini* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = MYFLAGS );/*
		:frm_ok_exit(parent, name, modal, fl) {form_listini = parent; init();}*/
	~frm_cm_listini_inserisci();// {}

	tr_input* codice;
	tr_input* descrizione;
	tr_input* da_data, *a_data;
	tr_time *intervalli;
	tr_label* l_giorni;
	tr_label* lun,*mar,*mer;
	tr_label* gio,*ven,*sab;
	tr_label* dom;

	void init();
	int giorni();					// da chk_boxes a int
	void giorni(int giorni);				// da int ai chkl_boxes

    public slots:
	void ok_clicked();			// ridefinizione di frm_ok_exit


    private:
	frm_cm_listini* form_listini;
    };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
class frm_cm_agg : public frm_ok_exit {
  Q_OBJECT;
  public:
    frm_cm_agg( frm_cm* parent , const char* name );

    // composizione area
    tr_input *codice;
    tr_input *nome_gruppo;
    tr_input *desc_conto;
    tr_listview *stampanti;

    tr_input *qta;        // indica
    tr_input *um;         // unita' di misura di contenimento a magazzino
    tr_input *fattore;
    // fattore di default moltiplicativo per il carico
    //  se ad esempio si caricano caramelle, che solitamente vengono
    //  fornite a pacchi di 10 pacchetti costituiti da 20 caramelle,
    //  il fattore sara' 10*20.
    //  La tabella di carico sara' cosi' costituita:
    //   QTA(um fittizia)|ARTICOLO.plurale|*|fattore|UM|ARTICOLO.singolare|=|totale
    //   ARTICOLO.plurale e ARTICOLO.singolare sono gli stessi. Ad esempio:
    //   10pacchi|caramelle|*|200|N|caramelle|=|1000
    //

    void show();
    void init();

  protected slots:
    void barcode_clicked();

  protected:
    frm_cm *form_cm;
    QString barcodes;

    friend class frm_cm;

    };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class frm_cm_agg_grp: public frm_cm_agg {
    Q_OBJECT;

    public:
	frm_cm_agg_grp( frm_cm* parent , const char* name );

    };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class frm_cm_agg_art: public frm_cm_agg {
    Q_OBJECT;

    public:
	frm_cm_agg_art( frm_cm* parent , const char* name );

	// composizione area
	tr_input *iva;
	tr_input *prezzo;
	tr_input *reparto;
        tr_label *apeso;
        tr_label *messaggio;

    public slots:
      void apeso_clicked();

    };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// la variante e' come un articolo con il flag variante in pi
class frm_cm_agg_var: public frm_cm_agg {
    Q_OBJECT;

    public:
	frm_cm_agg_var( frm_cm* parent , const char* name );

	// composizione area
	tr_input *iva;
	tr_input *prezzo;
	tr_label *prezzo_tipo;
	enum { tipo_percentuale, tipo_fisso };
	tr_label* piu_meno;
	tr_label* commento;
        tr_label* specializza;    // specializza l'output a stampante

    public slots:
	void prezzo_tipo_clicked();
    };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// la variante e'come un articolo con il flag variante in pi
class frm_cm_mod: public frm_cm_agg {
    Q_OBJECT;

    public:
        frm_cm_mod( frm_cm* parent , const char* name );

        // composizione area
        tr_input *iva;
        tr_input *prezzo;
        tr_label *prezzo_tipo;
        tr_input *reparto;
        enum { tipo_percentuale, tipo_fisso };

    void modifica( const tr::gruppi::gruppo &grp);

    public slots:
        void prezzo_tipo_clicked();

    };

*/
#endif

