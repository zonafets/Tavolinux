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

#ifndef FRM_BASE_HPP
#define FRM_BASE_HPP

/* History:
   26/08/04  riscritta completamente la gestione dei pulsanti
*/

/* dichiarazoine di  classe che comporta in una form fatta da:
   titoletto
   barra stato
   widget-centrale-non definito
   barra pulsanti

*/

//#include <qt.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qaccel.h>
#include <qapplication.h>

#include "tr_label.hpp"
#include "tr_barra_stato.hpp"
#include "tr_config.hpp"
#include "tr_input.hpp"
#include "tr_listview.hpp"
#include "tr_dmacro.hpp"
#include "tr_tab_widget.hpp"
#include "tr_canvas.hpp"
#include "cmddiritti.hpp"

typedef QStringList tr_stringlist;

extern tr_config config;

extern const bool _mcd_check;
#ifdef _MCD_CHECK

// macro di comodit�di creazione delle form, utile per le statistiche di memoria
#define FORM( variabile, classe ) \
mem_check_begin() \
classe *variabile; NEW (variabile, classe( this, #classe ) )

#define NEW_FRM bFORM
#define FRM_ITER(form) form->interazione()
#define FRM_CTRL tr_widget*
#define FRM_RUN(structure) frm_base::run_form(structure)

// macro di comodit�di creazione delle form, utile per le statistiche di memoria
#define FORM3( padre, variabile, classe ) \
mem_check_begin() \
classe *variabile; NEW (variabile, classe( padre, #classe ) )

// macro per creazione form interpretate

#define FRM_PARAMS QStringList _frm_params
#define FRM_PARAM(name,value) _frm_params<<QString(name)+"|"+QString(value)

#define bFORM( variabile, struttura, padre)\
mem_check_begin(); \
frm_base *variabile; \
NEW(variabile, frm_base(padre,#struttura)); \
variabile->build(struttura,_frm_params)

#define trFORM( variabile, nome, padre)\
mem_check_begin(); \
tr_forms *variabile; \
NEW(variabile, tr_forms(padre,#nome)); \

/* per meglio usare questa forma di controllo della memoria, �opportuno che
   l'effetto di ci�che �stato specificato nella form, avvenga dopo la cancellazione.
   Esempio:
       FORM( f, frm_clienti)
       if (f->accepted() && f->modificato()) b=true;  // segna l'effetto
       xFORM( f, frm_clienti)	// controllo memoria
       if (b) { ....		// qui ora �possibile alterare la memoria
       Questo perch�la coppia FORM, xFORM lavora per differenze di prima e dopo e
       quindi deve allocare un blocco e deallocarlo tutto
       */

// la seguente serve solo per distinguere il testo delle forms dal resto
#define TRFORM(x) x

#define xFORM( variabile, classe) \
DELETE (variabile); \
mem_check_end(classe)

#define DEL_FRM xFORM

#define goFORM( classe) \
mem_check_begin()\
classe *f; NEW ( f , classe( this, #classe ) );\
f->exec(); \
DELETE (f); \
mem_check_end(classe)

#else

// macro di comodit�di creazione delle form, utile per le statistiche di memoria
#define FORM( variabile, classe ) classe *variabile; NEW ( variabile , classe( NULL, #classe ) )

#define xFORM( variabile, classe) DELETE(variabile);

#define goFORM( classe) \
classe *f; NEW ( f , classe( this, #classe ) );\
f->exec(); \
DELETE (f);

#endif


///////////////////////////////////////////////////////////////////////////////////////////

class tr_palette {
    public:

    class tr_colore {
	public:
	QColor fg;	// testo
	QColor sfg;	// colore testo alternativo (es. : num tavolo)
	QColor bg;;	// sfondo piatto
	QString bm;	// bitmap
    };

    tr_colore titolo;
    tr_colore stato;
    tr_colore area;

    QString pulsante;
    QColor selezione;

    // inizializza con i colori di base presi dal file di configurazione

    tr_palette();
};

#define CTRL_FRM(frm) frm_base *_def_ctrl_frm=frm
#define CTRL(nome) frm_base::tr_ctrl nome(#nome,_def_ctrl_frm)

/*
Il metodo link di questa form gestisce la connessione tra un controllo di input e l'abilitazione o disabilitazione
degli altri controlli

classe usata nelle funzioni di link per l'abilitazione/disab. automatica dei controlli
della form, legati logicamente al valore di un altro */

#define max_links 100

///////////////////////////////////////////////////////////////////////////////////////////
class frm_base: public QDialog, public TRDiritti {
    Q_OBJECT

public:

    frm_base( QWidget* parent = 0, const char* name = 0, bool modal = true, WFlags fl = MYFLAGS );
    ~frm_base();

    ////////////////////////////////////////////////////
    // per la modalita' interattiva si veda alla fine //
    ////////////////////////////////////////////////////


    // costruttori semplificati; vedere il corpo per ulteriori dettagli
    void build( QWidget* widgets[], int margine = 8, int spazio = 6, bool dim_fissa=false);
    tr_label *vuoto(int dx=-1, int dy=-1);	// restiuisce uno spazio vuoto dimensionabile

    // imposta contesto

    tr_barra_titolo* barra_titolo;
    tr_barra_titolo* titolo;		// per comodit�= a barra_titolo
    tr_barra_stato* barra_stato;
    tr_barra_stato* stato;		// per comodit�= a barra_stato
    QFrame* base;
    QFrame* base1;
    QFrame* barre;
    QFrame* area;
    QFrame* area1;
    tr_tab_widget *tabs;
    QFrame* pulsantiera;

    int exec();
    bool accepted();
    void show();
    void hide();
    void center();
    bool centered;

    void accetta() {accept(); }
    void rigetta() {reject(); }

    void accept();
    void reject();

    int margin()			{ return (layout() ? layout()->margin() : 0); }

    QGridLayout* frm_layout;
    QGridLayout* area_layout;
    QVBoxLayout* base_layout;
    QVBoxLayout* barre_layout;
    QGridLayout* pulsantiera_layout;

    void 	nascondi_titolo();
    void 	scopri_titolo();
    void	nascondi_stato();
    void	scopri_stato();
    void	nascondi_pulsantiera()				{ pulsantiera->hide(); };
    void	scopri_pulsantiera()				{ pulsantiera->show(); };
    void 	base_alza();					// conferisce al frame di base un aspetto rialzato

    void	adatta_stato();					// scala il contenuto fino a quando ci sta

    void 	pulsantiera_alza();

    void  add_base();             // aggiunge area1 per prova

    /* creazione pulsanti:
       es:  sposta|conto|comanda|exit
         la punteggiatura e lo spazio divengono _
         dai nomi si ricava il nome dell'icona preponendo az_ e postponendo .png
         nomi diversi possono venir specificati in icone con omissione
         ||az_com.png|az_uscita.png
         I puntatori si ricavano con la funzione pointer("nome_pulsante")
         il nome
         */
    void    pulsanti(
        QString nomi, bool verticale,
        tr_label **ws[],
        TRDiritti::TRDirittiUtente diritti[],
        bool hide=false);

    int stato_form;  // accepted, rejected o indefinito(2)

    // visualizza o scopri i pulsanti in base al contasto
    virtual void stato_pulsanti();

    void	setMinMax(int min_x, int min_y, int max_x=-1, int max_y=-1);	// imposta dimensioni min e max

    void abilita( tr_widget*ws[], bool forza = false );
    void abilita( tr_widget*w1, tr_widget*w2=NULL, tr_widget*w3=NULL, tr_widget*w4=NULL, tr_widget*w5=NULL,
		  tr_widget*w6=NULL, tr_widget*w7=NULL, tr_widget*w8=NULL, tr_widget*w9=NULL, tr_widget*w10=NULL,
                  tr_widget*w11=NULL
                  );
    void disabilita( tr_widget*ws[], bool forza = false );
    void disabilita( tr_widget*w1, tr_widget*w2=NULL, tr_widget*w3=NULL, tr_widget*w4=NULL, tr_widget*w5=NULL,
		  tr_widget*w6=NULL, tr_widget*w7=NULL, tr_widget*w8=NULL, tr_widget*w9=NULL, tr_widget*w10=NULL,
                  tr_widget*w11=NULL
                  );

    void scopri( tr_widget*w1, tr_widget*w2=NULL, tr_widget*w3=NULL, tr_widget*w4=NULL, tr_widget*w5=NULL,
			  tr_widget*w6=NULL, tr_widget*w7=NULL, tr_widget*w8=NULL, tr_widget*w9=NULL, tr_widget*w10=NULL);
    void nascondi( tr_widget*w1, tr_widget*w2=NULL, tr_widget*w3=NULL, tr_widget*w4=NULL, tr_widget*w5=NULL,
			  tr_widget*w6=NULL, tr_widget*w7=NULL, tr_widget*w8=NULL, tr_widget*w9=NULL, tr_widget*w10=NULL);


    void setPaletteForegroundColor ( const QColor & );
    void setPaletteBackgroundColor ( const QColor & );
    void setPaletteBackgroundPixmap ( const QPixmap & );
    void setPalette( tr_palette p);

    void link(tr_input *input, QWidget*ws[]);

    void link(
      tr_label *check_box, QWidget*ws[],
      tr_label::tr_stato attiva_su=tr_label::on);

    void link(tr_listview *list_view, QWidget*ws[]);

    struct {
      tr_input *input;
      tr_label *check_box;
      tr_listview *listview;
      bool last() {return (input==NULL && check_box==NULL && listview==NULL)?true:false;}
      void init() {input=NULL;check_box=NULL;listview=NULL;}
    } links[max_links];
    void links_clicked();

    void enable_barcode(bool v=true)  {_barcode_enabled=v;v?grabKeyboard():releaseKeyboard();}
    bool barcode_enabled()            {return _barcode_enabled;}

signals:
    void barcodeReaded();

public slots:
    void link_textChanged( const QString &);
    void link_clicked();
    void link_clicked( QListViewItem * );
    void screen_shot();
    void print_screen_shot();
    void barcode();
    void card_readed();
    void timers();

public:
    QStringList card;

protected:

    virtual void barcodeReaded( QString );

    friend class tr_menu_dim;

    void iniz_area(int righe, int colonne, int margine=0, int spazio=0);	// inizializza il layout di area

    bool eventFilter( QObject *o, QEvent *e );
    // bool event( QEvent *e );
    void customEvent(QCustomEvent *);

    tr_palette palette;

private:
  void costruttore_comune(QWidget *parent);

  QAccel *accel_sshot;
  QAccel *accel_prnsshot;
  void init();

  tr_PtrDict<tr_widget>  link_disabilitati;

  bool iterando;

  QWidget *widget_was_grabbing;


public:

  static frm_base *splash;

/* questo secondo costruttore è per la modalità interattiva
  vedere ad esempio prenotazioni() in frm_main
  La struttura consta di righe del tipo:

    conds:token=nome/contenuto(parametro);token=nome/contenuto(param)\n

    il nome è opzionale e se non specificato viene usato quello generico del token
    anche i parametri sono opzionali
    (quindi listview=(xxx) significa listview di nome listview con parametro xxx

  dove conds puo' essere
  ?cond:         se cond e' definita
  ?cond1,?cond2:  se cond1 e cond2 sono definiti
  ?cond1,!cond2: se cond1 e' definata e cond2 no

  dove token può essere:
    slash* ... *slash       commenti come in C
    readonly        (true) pone i campi della maschera in sola lettura
    extra           attiva controlli extra
    conds           flags di condizione delle righe successive. Ad es.
                    cond=art|grp
                    ?art:token=...   // se art e' specificato
                    !art:token=...   // se art non e' specificato
                    ?art,!grp:token= // set art è specif. e grp non spec.
    palette         numero di palette relativa a libello di sovrapposizione di finestra
                    (normalmente 1 e 2 sono blu, 3 e' verde)
    events          attiva l'uscita su eventi particolari
                    keypress  (non ancora attivo)
                    custom      richiamato da tr_srvcmd
    events.ignore   cards,...,...
    events.timers   secs[|secs|secs]
    margine         margine in pixel del bordo (default=8)
    spazio          spazio in pixel tra i ctrls (Default=6)
    title           contenuto del titolo
    minmax          dmx%|dmy%[|dMx%|dMy%] dimensioni minime e massime
                    in % della dimensione dello screen (config.finestra_*)
                    screen    prende config./altezza|larghezza/_finestra
    status          2 modalita':
      stringa singola
      barra di stato se la stringa contiene dei |||||
    status.notr     titolo senza TR()
    validatore      nome base del validatore
    listview        tr_listview
    input           tr_input
    label           tr_label
    token[]         nome|nome|...
    hole            larghezza  (e' una label vuota segna posto)
    buttons[].icons icona_specifica|[salta]|icona_specifica|-(nessuna)|...
    buttons[]       nome/icona->label|nome/icona->label|...
    links[]         capo|dipendente|...  (puo' essercene + di uno)

    (le pèarentesi [] sono facoltative)

  per ogni token ci sono opzioni possibili:
    token:          opzione:
    input           alato         attiva la forma con etichetta a lato
                                  invece che sopra
                    ggmmaaaa      input di tipo data
                    hhmm          tipo ora
                    testo         tipo testo (default)
                    #.#           maschera variabile per tipo numerico
            		    chk:on        attiva una chkbox sull'etichetta con link:attivo su on
		                chk:off       attiva una chkbox sull'etichetta con link:attivo su off
                    tok:cmd       vedi listview:tok:cmd
                                  condiziona l'input e attiva il pulsante
                                  di apertura di popup di scelta
                    onkey         abilita uscita su textchanged
                    onclick       abilita uscita su textchanged
                    onupdate      abilita uscita su return o lostfocus
                    h:n_righe     forza l'altezza come n*config.altezza_riga
                    barcode       abilita la lettura di barcode e il tab
                                  automatico al controllo successivo
                    readonly      imposta a sola lettura
                    oneline       forza la label su una sola riga
                    tok:cmd[:nkeys] dove cmd è il valore numerico
                                  del comando (tipo tok_elenco_camerieri)
                    se nkeys omesso, nkeys=2
                    se nkeys=0,
                    tp:listview   collega alle forms scelta
                    dim:minw,[minh],[maxw],[maxh] specifica le dimensioni
                    bg:           #rrggbb o color o light
                    disabled:     disabilita il controllo
    listview        tok:cmd|par   dove cmd è il valore numerico
                                  del comando (tipo tok_elenco_camerieri)
                                  par e' il parametro
                    noexit        nascondi il pulsante exit
                    abc           mostra la barra abc
                    +-            mostra i due pulsanti
                    nobar         nasconde la barra di scorrimento
                    tutti         multisalezione su tutti
                    nessuno       multiselezione su nessuno
                    %width        dimensione in % del padre
                    onclick       abilita uscita dall'iterazione su click
                    rollone       solo un gruppo espanso per volta
                    small         riduce a 18 il font size
                    hl:           highlight evidenzia la riga
                                  secondo una delle formule
                                  nCOL eq VAL?#rrggbb|icona.png
                                  nCOL and VAL?#rrggbb|icona.png
                                  nCOL ** VAL?##[rrggbb]|icona.png (grassetto)
                    header:col[!w]|col|col
                                  !w specifica la dimensione fissa della colonna
                                  !!ATTENZIONE!! se si usa un esclamativo,
                                  metterlo su tutte le colonne. Cioe'
                                  o tutto relativo o tutto assoluto.
                    sortable:     abilita la visualizzazione dell'indicatore di
                                  sort e la possibilita' di cliccare sulla
                                  colonna dell'heder per ordinare su o giu
                                  alternativamente
                    dim:minw,[minh],[maxw],[maxh] specifica le dimensioni
                                  minime e massime
                    minr:n        numero minimo di righe (height=n*config.altezza_riga)

    hole            pixels        larghezza in pixel
    label           chk:on        check box a on
		                chk:off       check box a off
                    fg:           #rrggbb o color o light
                    bg:           #rrggbb o color o light
                    pixmap:       name
                    movie:        file_name (./icone/file_name)
                    radio:A|B|*C|...  *C sara' quello di default
                    n:            imposta il radio button
                    onclick       abilita uscita dall'iterazione su click
                    oneline       forza la label su una sola riga
                    dim:minw,[minh],[maxw],[maxh] specifica le dimensioni
    conds           flag|flag|flag
    extra           =menu
                    sp:1          spessore pulsante
                    bsize:60|54   dimensione minima del pulsante (dy|dx)
                    rc:9|7        dimensione in (r)ighe e (c)olonne
    extra           =comanda
    extra           =convas       attiva il controllo per diseno a dito libero

*/

bool build( QString struttura, QStringList params );

typedef tr_widget* tr_intres;
tr_intres interazione();
QString last_key_text;
bool events_ignore_cards;

class tr_ctrl {
  public:
  enum e_tipo {t_input,t_label,t_listview,t_canvas,t_unk} tipo;
  enum {pointer,pointer_pointer} tipo_tipo;

  e_tipo get_tipo() {
    if (w) {
      if (!qstrcmp(w->className(),"tr_label")) return t_label;
      if (!qstrcmp(w->className(),"tr_input")) return t_input;
      if (!qstrcmp(w->className(),"tr_listview")) return t_listview;
      if (!qstrcmp(w->className(),"tr_canvas")) return t_canvas;
    }
    return t_unk;
  }

  tr_ctrl(char*nome, frm_base *f)        {
    w=f->pointer(nome);tipo_tipo=pointer;tipo=get_tipo();if (w==NULL) {
      w=(tr_widget*)-1;
      qDebug(TR("Attenzione controllo %s non trovato in form %s"), nome,f->name());
      }
  }
  tr_ctrl()                 {w=(tr_widget*)-1;}
  tr_ctrl(tr_input *p)      {input=p;tipo=t_input;tipo_tipo=pointer;}
  tr_ctrl(tr_label *p)      {label=p;tipo=t_label;tipo_tipo=pointer;}
  tr_ctrl(tr_listview *p)   {listview=p;tipo=t_listview;tipo_tipo=pointer;}
  tr_ctrl(tr_input **p)      {input=(tr_input*)p;tipo=t_input;tipo_tipo=pointer_pointer;}
  tr_ctrl(tr_label **p)      {label=(tr_label*)p;tipo=t_label;tipo_tipo=pointer_pointer;}
  tr_ctrl(tr_listview **p)   {listview=(tr_listview*)p;tipo=t_listview;tipo_tipo=pointer_pointer;}

  union {
    tr_widget*w;
    tr_input*input;
    tr_label*label;
    tr_listview*listview;
    tr_canvas*canvas;
  };

  operator tr_widget*() {return w;}
  // operator const char *() const

  QString operator[](int idx) {
    if (tipo==t_listview && listview)
      return listview->record().section('|',idx,idx);
    return QString("");
  }

  tr_ctrl *operator->() {return this;}

  tr_ctrl& operator=(tr_widget*p) {
    if (tipo_tipo==pointer) w=p;
    else {
      *((tr_widget**)w)=p;
      w=p;
    }
    return *this;
  }

  tr_ctrl& operator=(QString d) {value(d);return *this;}
  // QString operator=() {return value();}

  QString operator()() { return value(); }
  QString operator()(QString sep) {
    if (tipo==t_listview) return listview->selezionati().join(sep);
    else return value();
  }



  void value(int v) {value(QSN(v));}
  void value(double v) {value(QSN(v));}
  void value(const char *v) {value(QString(v));}
  void value(QString v) {
    if (tipo==t_input) input->setText(v);
    if (tipo==t_listview) listview->seleziona(v);
    if (tipo==t_canvas) canvas->data(v);
    if (tipo==t_label) {
      if (label->aspetto()==tr_label::check) {
        if (v=="vero" || v=="true") label->stato(tr_label::on);
        if (v=="falso" || v=="false") label->stato(tr_label::on);
      }
      else
      if (label->aspetto()==tr_label::radio) label->number(v.toInt());
      else label->setText(v);
    }
  }
  void value(QStringList v) {
    if (tipo==t_listview) listview->seleziona(v);
  }
  void value(QString v, char sep) {
    if (tipo==t_listview) values(QStringList::split(sep,v,true));
  }

  void caption(QString v) {
    if (tipo==t_input) input->setLabel(v);
    if (tipo==t_listview) listview->setText(v);
  }

  void validatore(QString v) {
    if (tipo==t_input) input->setValidatore(v);
  }

  QString value() {
    if (tipo==t_input) return input->text();
    if (tipo==t_listview) return listview->selezionato();
    if (tipo==t_label && label->aspetto()==tr_label::radio)
      return QSN(label->number());
    if (tipo==t_canvas) return canvas->data();
    return QString("");
  }

  QStringList values() {
    if (tipo==t_listview) listview->selezionati();
    return QStringList();
  }

  // mmh... funzione ambigua sia di carico che di selezione
  void values(const QStringList s) {
    if (tipo==t_listview)
      if (!listview->dati().count()) listview->aggiungi(s);
      else listview->seleziona(s);
  }

  void value(bool v) {
    if (tipo==t_label && label->aspetto()==tr_label::check)
      v?label->setOn():label->stato(tr_label::off);
    if (tipo==t_input && input->label->aspetto()==tr_label::check)
      v?input->label->setOn():input->label->stato(tr_label::off);
  }

  void value(QString id, TRTokens tok) {
    if (tipo==t_input) input->setText(tok,id);
    if (tipo==t_label) {
      tr::mse elenco(tok,id);
      label->setText(elenco.descrizione());
    }
  }

  bool isOn() {
    if (tipo==t_label) return label->stato()==tr_label::on?true:false;
    if (tipo==t_input) return input->label->stato()==tr_label::on?true:false;
    return false;
  }

  QString dataiso(bool yeszerotime=false) {
    if (tipo==t_input) return input->dataiso(yeszerotime);
    return QString("");
  }

}; // classe tr_ctrl

tr_widget* pointer(QString nome);
/* es. pointer("nome!nome!nome",ctrls) */
void pointers(QString nomi, tr_ctrl ctrls[]);

tr_listview *lvs(QString nome) {return (tr_listview*)pointer(nome);}
tr_label    *lbs(QString nome) {return (tr_label*)pointer(nome);}
tr_input    *ips(QString nome) {return (tr_input*)pointer(nome);}

QString      gets(QString elenco);
void         sets(QString elenco, QString valori);

public slots:
  void pulsante_clicked();
  void pulsante_clicked(QListViewItem*);

protected:
  void keyPressEvent(QKeyEvent *e);

private:
  friend class tr_build_data;
  QDict<tr_widget> _pointers;
  bool exit_on_key_event;
  int exit_on_custom_event;
  QString barcodectrls;
  int hide_counter;
  bool _barcode_enabled;

  void exit_loop();

public:
  tr_widget* iterato;

  static QString run_form(QString structure, QStringList params=QStringList());

};

#endif
