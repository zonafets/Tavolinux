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

#include "frm_ok_exit.hpp"
//#include "frm_msgbox.hpp"
#include "frm_calendario.hpp"
#include "frm_calcolatrice.hpp"
#include "tr_tools.hpp"
#include "tr_forms.hpp"

extern tr_tools tools;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      FRM_OK_EXIT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_ok_exit::frm_ok_exit( frm_base*parent, const char*name, bool modal, WFlags fl ):frm_base( parent, name, modal, fl ) {
  setMinMax( 0, 0, int( config.finestra_larghezza*0.95 ), int( config.finestra_altezza*0.95 ) );
  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  nascondi_titolo();
  base_alza();

  tr_label**ps[]= { &ok, &annulla, &exit };

  /* aggiunta pulsanti */
  QString nomi=TRFORM( "ok#ok|annulla#annulla|uscita#uscita" );
  pulsanti( nomi, false, ps, NULL );

  CONNECT( ok );
  CONNECT( annulla );
  CONNECT( exit );

  setPaletteBackgroundColor( pulsantiera->paletteBackgroundColor().light() );

  nascondi( annulla );
  _annullato=false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      FRM_MSGBOX
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* frm_msgbox::frm_msgbox( frm_base* parent , const char* name , bool modal , WFlags fl  )
: frm_ok_exit( parent, name, modal, fl) { NEW ( msg , tr_label( area,"msg") );
msg->setPaletteBackgroundColor( QColor( 255, 247, 187 ) ); msg->setPaletteForegroundColor( paletteBackgroundColor() );
QWidget* w[]={msg,0,0}; build(w,2); nascondi(exit);

QLayoutIterator it = pulsantiera_layout->iterator(); QLayoutItem *child; while ( (child = it.current()) != 0 ) {
if (child->widget() == ok) l_ok = child; if (child->widget() == exit) l_exit = child;
if (child->widget() == annulla) l_annulla = child; ++it; } l_ok->setAlignment( Qt::AlignCenter );
l_exit->setAlignment( Qt::AlignCenter ); l_annulla->setAlignment( Qt::AlignCenter );

stato (s_ok); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_msgbox::stato(frm_msgbox::tr_stato x) { _stato = x; // enum tr_stato {s_ok=1, s_exit=2, s_annulla=4};
if (x & s_ok) scopri(ok); else nascondi(ok); if (x & s_exit) scopri(exit);  else nascondi(exit);
if (x & s_annulla) scopri(annulla); else nascondi(annulla);

if (x == s_ok) { l_ok->setAlignment( Qt::AlignCenter ); //cout << "l_ok allineato al centro\n"; } else {

int c = s_ok | s_exit; if ((x & c)==c) { l_ok->setAlignment( Qt::AlignLeft ); l_exit->setAlignment( Qt::AlignRight );
scopri( ok, exit ); } c = s_ok | s_exit | s_annulla; if ((x & c)==c) { l_ok->setAlignment( Qt::AlignLeft );
l_exit->setAlignment( Qt::AlignRight ); scopri( ok, exit, annulla ); } } }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_msgbox::setText(QString txt) { txt.replace("\n","<br>"); msg->setText(txt+"<br>"); //msg->setMinMax(msg->sizeHint());
//msg->adjustSize(); msg->resize(msg->sizeHint()); adjustSize(); } */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                          FRM_CALENDARIO
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_calendario::frm_calendario( QWidget*parent, const char*name, bool modal, WFlags fl ):frm_base( parent, name, modal, fl ) {
  titolo->setText( tr( "calendario" ) );

  base_alza();
  nascondi_titolo();

  tr_label**ps[]= { &anno_prec, &anno_succ, &cancella, &exit };
  QString etichette=QString::number( QDate::currentDate().year()-1 )+"#anno_prec_|"
     +QString::number( QDate::currentDate().year()+1 )+"#anno_succ_|" +
     "cancella#cancella" +"|" +
     "uscita#uscita" ;

  pulsanti( etichette, false, ps, NULL );

  CONNECT( anno_prec );
  CONNECT( anno_succ );
  CONNECT( cancella );
  CONNECT( exit );

  setPaletteBackgroundColor( pulsantiera->paletteBackgroundColor().light() );

  barra_stato->setPaletteForegroundColor( WHITE );
  _colore=paletteBackgroundColor();

  colore_domenica=QColor( 150, 0, 0 );

  const int margine=4;
  iniz_area( 6, 9, margine, 0 );

  _data=QDate::currentDate();

  int j;

  const short dx=LARGHEZZA_PULSANTE_CALENDARIO;
  const short dy=ALTEZZA_PULSANTE_CALENDARIO;

  // crea la prima linea di intestazione: XXX  DOM LUN MAR MER GIO VEN SAB XXX
  QString nomi_giorni[]= { tr( "LUN" ), tr( "MAR" ), tr( "MER" ), tr( "GIO" ), tr( "VEN" ), tr( "SAB" ), tr( "DOM" ) };
  for ( j=0; j<7; j++ ) {
    tr_label*giorno;
    NEW( giorno, tr_label( area, nomi_giorni[j] ) );
    giorno->setMinimumSize( dx, dy );
    if ( j==6 ) giorno->setPaletteForegroundColor( colore_domenica );
    if ( ( j%1 )==1 ) giorno->setText( nomi_giorni[j], -4 );
    else giorno->setText( nomi_giorni[j], -4 );
    area_layout->addWidget( giorno, 0, j+1 );
  }


  // crea le due file dei mesi
  mese_sel=NULL;
  QString nomi_mesi[]= {
    tr( "GEN" ), tr( "FEB" ), tr( "MAR" ), tr( "APR" ), tr( "MAG" ), tr( "GIU" ), tr( "LUG" ), tr( "AGO" ), tr( "SET" ), tr( "OTT" ), tr( "NOV" ), tr( "DIC" )
  };
  for ( j=0; j<12; j++ ) {
    tr_label*mese;
    NEW( mese, tr_label( area, nomi_mesi[j] ) );
    mese->setText( nomi_mesi[j] );
    mesi[j]=mese;
    mese->setMinimumSize( int( dx*1.25 ), dy );
    mese->setPaletteBackgroundColor( COLORE_MESE );
    mese->setPaletteForegroundColor( paletteBackgroundColor() );
    if ( j<6 ) area_layout->addWidget( mese, j+1, 0 );
    else
      area_layout->addWidget( mese, j-5, 8 );
    connect( mese, SIGNAL( clicked() ), this, SLOT( mese_clicked() ) );
  }

  // crea i tasti dei giorni
  giorno_sel=NULL;
  for ( j=0; j<42; j++ ) {
    tr_label*giorno;
    NEW( giorno, tr_label( area, QString::number( j ) ) );
    giorni[j]=giorno;
    giorno->setPaletteBackgroundColor( COLORE_GIORNO );
    if ( ( j%7 )==6 ) giorno->setPaletteForegroundColor( colore_domenica );
    else
      giorno->setPaletteForegroundColor( paletteBackgroundColor() );
    area_layout->addWidget( giorno, j/7+1, ( j%7 )+1 );
    connect( giorno, SIGNAL( clicked() ), this, SLOT( giorno_clicked() ) );
  }

  data( _data ); // imposta la data mettendo gi i numeri ed evidenziando quello corrente
  _cancellato=false;

  init();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_calendario::~frm_calendario() { }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_calendario::init() { }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool frm_calendario::data( QDate d ) {
  if ( !d.isValid() ) return false;

  _data=d;

  // toglie precedenti evidenziazioni

  if ( giorno_sel ) giorno_sel->setPaletteBackgroundColor( COLORE_GIORNO );
  if ( mese_sel ) mese_sel->setPaletteBackgroundColor( COLORE_MESE );

  // evidenzia nuovo mese
  mese_sel=mesi[_data.month()-1];
  mese_sel->setPaletteBackgroundColor( mese_sel->paletteBackgroundColor().light( fattore_illuminazione ) );

  // riempie la tabella dei giorni con i giorni di questo mese

  // trova il primo giorno (lunedi) del mese corrente o del mese precedente
  bool cambia=false;
  if ( _da<=_data&&_data<=_a ) d=_da; // se la data �inclusa in quelle gi�visualizzate
  else {
    cambia=true;
    d=QDate( d.year(), d.month(), 1 );
    while ( d.dayOfWeek()!=1 ) d=d.addDays( -1 );
  }

  // comincia la distribuzione

  int i;
  _da=d;
  for ( i=0; i<42; i++ ) {
    if ( cambia ) {
      giorni[i]->setText( QString::number( d.day() ) );
      if ( d.month()!=_data.month() ) // se il mese �diverso da quello corrente
           giorni[i]->setPaletteForegroundColor( _colore.dark() );
      else
        giorni[i]->setPaletteForegroundColor( _colore );
    }

    // evidenzia giorno corrente

    if ( d==_data ) {
      giorno_sel=giorni[i];
      giorno_sel->setPaletteBackgroundColor( giorno_sel->paletteBackgroundColor().light( fattore_illuminazione ) );
    }

    d=d.addDays( 1 );
  }
  _a=d.addDays( -1 );

  barra_stato->setText( tools.data( data() ) );

  anno_prec->cambia_testo( QString::number( data().year()-1 ) );
  anno_succ->cambia_testo( QString::number( data().year()+1 ) );

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_calendario::exit_clicked() { reject(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_calendario::cancella_clicked() {
  accept();
  _cancellato=true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_calendario::anno_succ_clicked() {
  data( data().addYears( 1 ) );
  anno_succ->cambia_testo( QString::number( data().year()+1 ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_calendario::anno_prec_clicked() {
  data( data().addYears( -1 ) );
  anno_prec->cambia_testo( QString::number( data().year()-1 ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_calendario::giorno_clicked() {
  // deevidenzia il giorno prec. selezionato
  // if (giorno_sel) giorno_sel->setPaletteBackgroundColor( COLORE_GIORNO );

  tr_label*gs=( tr_label* ) sender();

  // evidenzia il nuovo giorno
  // giorno_sel->setPaletteBackgroundColor( COLORE_GIORNO_EVIDENZIATO );

  // recupera il numero di tasto associato al giorno selezionato
  int i=0;
  while ( giorni[i]!=gs ) i++;

  if ( i<43 ) data( _da.addDays( i ) );

  qApp->processEvents();

  accept();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_calendario::mese_clicked() {

  tr_label*ms=( tr_label* ) sender();
  // recupera il numero di tasto associato al giorno selezionato
  int mese=0;
  while ( mesi[mese]!=ms ) mese++;
  mese++;

  if ( mese<13 ) data( _data.addMonths( mese-_data.month() ) );

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                    FRM_CALCOLATRICE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_calcolatrice::frm_calcolatrice( QWidget*parent, const char*name, bool modal, WFlags fl ):
   frm_base( parent, name, modal, fl ) {
     titolo->setText( tr( "calcolatrice" ) );

     setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

     nascondi_stato();
     nascondi_pulsantiera();
     base_alza();

     barra_titolo->setMinimumSize( QSize( 240, 18 ) );
     barra_titolo->setMaximumSize( QSize( 240, 18 ) );
     barra_titolo->setAlignment( int( QLabel::AlignCenter ) );

     iniz_area( 1, 1 );

     NEW( calcolatrice, tr_calcolatrice( area, "calcolatrice" ) );
     pannello=calcolatrice->pannello;

     area_layout->addWidget( calcolatrice, 0, 0, Qt::AlignCenter );

     CONNECT( calcolatrice );

     //resize( QSize(253, 256).expandedTo(minimumSizeHint()) );
     //resize( QSize(200, 200).expandedTo(minimumSizeHint()) );
     init();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_calcolatrice::~frm_calcolatrice() {
  // no need to DELETE child widgets, Qt does it all for us
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_calcolatrice::init() { }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_calcolatrice::calcolatrice_clicked() {
  if ( calcolatrice->accepted() ) accept();
  if ( calcolatrice->rejected() ) reject();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_calcolatrice::impostazioni( const QString titolo, QString valore, QString validatore,
   tr_calcolatrice::modo _modalita ) {
     barra_titolo->setText( titolo, true );
     calcolatrice->impostazioni( valore, validatore, _modalita );
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                    FRM_WAIT
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_wait::frm_wait( QWidget*parent, const char*name, bool modal, WFlags fl ):
   frm_base( parent, name, modal, fl|Qt::WStyle_StaysOnTop ) {
     titolo->setText( tr( "elaborazione in corso" ),true );

     nascondi_stato();
     nascondi_pulsantiera();
     base_alza();

     NEW( icona, QLabel( area, "icona" ) );
     icona->setFixedSize( 52, 52 );
     //icona->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding ));

     NEW( testo, tr_label( area, "testo" ) );
     testo->setText( tr( "  attendere prego\n elaborazione in corso " ) );
     testo->setFixedSize( 240, 34*3 );

     QWidget*ws[]= { icona, testo, 0, 0 };
     build( ws, 20 );

     tipo=clessidra;

     switch ( tipo ) {
       case clessidra:
         n_icone=4;
         base="clessidra0";
       break;
       case computer:
         n_icone=5;
         base="computer0";
       break;
       case caffe:
         n_icone=4;
         base="caffe0";
     }

     n_icona=-1;

     avanza();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_wait::~frm_wait() { }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_wait::avanza() {
  n_icona++;
  if ( n_icona==n_icone ) n_icona=0;
  icona->setPaletteBackgroundPixmap( PIXMAP( base+QString::number( n_icona )+".png" ) );
  qApp->processEvents();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                   FRM_TASTIERA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define AZ_SX "<img src=\"az_sx.png\">"
#define AZ_DX "<img src=\"az_dx.png\">"

frm_tastiera::frm_tastiera( QWidget*parent, const char*name, bool modal, WFlags fl ):frm_base( parent, name, modal, fl ) {
  titolo->setText( tr( "video tastiera" ) );

  base_alza();
  nascondi_titolo();

  const int margine=4;
  iniz_area( 3, 3, margine, 0 );

  NEW( pannello, tr_input( area, "pannello", "", "", QString::null, true ) );
  pannello->setPaletteBackgroundColor( config.colore_selezione );
  area_layout->addMultiCellWidget( pannello, 0, 0, 0, 2 );
  connect( pannello->edit, SIGNAL( returnPressed() ), this, SLOT( ok_clicked() ) );
  nascondi( pannello->icona );

  QRect dim_tastiera=QRect( 0, 0, 8, 5 );
  NEW( tastiera, QFrame( area, "tastiera" ) );
  area_layout->addWidget( tastiera, 2, 0 );
  QGridLayout*tastiera_layout;
  NEW( tastiera_layout, QGridLayout( tastiera, 5, 8, 0, 0 ) );

  QRect dim_pad=QRect( 0, 0, 4, 5 );
  NEW( pad, QFrame( area, "pad" ) );
  area_layout->addWidget( pad, 2, 2 );
  QGridLayout*pad_layout;
  NEW( pad_layout, QGridLayout( pad, 5, 4, 0, 0 ) );

  area_layout->addColSpacing( 1, 10 );
  area_layout->addRowSpacing( 1, margine );


  const int n_tasti=37;
  NEW( tasti, ( tr_label* ) [n_tasti+1] );
  tasti[n_tasti]=NULL;

  const short dx=LARGHEZZA_PULSANTE_TASTIERA;
  const short dy=ALTEZZA_PULSANTE_TASTIERA;

  QRect vuoto=QRect( 3, 4, 3, 1 ); // la parte della barra spazio da evitare
  QRect dim=dim_tastiera;
  int y, x, tasto=0;
  for ( y=0; y<dim.height(); y++ ) {
    for ( x=0; x<dim.width(); x++ ) {
      if ( !vuoto.contains( x, y ) ) {
        tr_label*p;
        NEW( p, tr_label( tastiera, QString::number( tasto ) ) );
        p->setMinimumSize( dx, dy );
        p->aspetto_bottone();
        p->setPaletteBackgroundColor( COLORE_TASTIERA );
        p->setPaletteForegroundColor( paletteBackgroundColor() );
        if ( x==2&&y==4 ) tastiera_layout->addMultiCellWidget( p, 4, 4, 2, 5 );
        else
          tastiera_layout->addWidget( p, y, x );
        tasti[tasto]=p;
        if ( tasto==t_bs ) tasti[tasto]->setTextFormat( Qt::PlainText );
        switch ( tasto ) {
          case t_spazio:
            spazio=p;
            connect( spazio, SIGNAL( clicked() ), this, SLOT( spazio_clicked() ) );
            p->setPaletteBackgroundColor( COLORE_PAD );
          break;
          case t_bs:
            bs=p;
            connect( bs, SIGNAL( clicked() ), this, SLOT( bs_clicked() ) );
            p->setPaletteBackgroundColor( COLORE_PAD );
          break;

          default:
            connect( p, SIGNAL( clicked() ), this, SLOT( tasto_clicked() ) );
        } // switch
        tasto++;
      } // if
    } // for x
  } // for y

  // disegna il pad numerico

  QString c[]= { "C", "/", "*", "-", "7", "8", "9", "+", "4", "5", "6", "", "1", "2", "3", "00", "0", ".", "=" };

  vuoto=QRect( 3, 3, 1, 1 );
  dim=dim_pad;
  tasto=0;
  for ( y=0; y<dim.height(); y++ ) {
    for ( x=0; x<dim.width(); x++ ) {
      if ( !vuoto.contains( x, y ) ) {
        tr_label*p;
        NEW( p, tr_label( pad, QString::number( tasto ) ) );
        p->setMinimumSize( dx, dy );
        p->setPaletteBackgroundColor( COLORE_TASTIERA );
        if ( x==3 ) p->setPaletteBackgroundColor( COLORE_PAD );
        if ( y==0||y==4 ) p->setPaletteBackgroundColor( COLORE_PAD );
        p->setPaletteForegroundColor( paletteBackgroundColor() );
        p->aspetto_bottone();
        p->setText( c[tasto] );
        if ( x==3&&y==2 ) pad_layout->addMultiCellWidget( p, 2, 3, 3, 3 );
        else
          pad_layout->addWidget( p, y, x );
        switch ( tasto ) {
          case t_canc:
            canc=p;
            connect( canc, SIGNAL( clicked() ), this, SLOT( canc_clicked() ) );
          break;
          case t_ok:
            ok=p;
            connect( ok, SIGNAL( clicked() ), this, SLOT( ok_clicked() ) );
          break;
          default:
            connect( p, SIGNAL( clicked() ), this, SLOT( tasto_clicked() ) );
        } // switch
        tasto++;
      } // if
    } // for x
  } // for y

  imposta_maiuscole();

  ok->setPixmap( PIXMAP( "az_ok.png" ) );
  canc->setPaletteForegroundColor( QColor( 200, 0, 0 ) );
  bs->setPaletteForegroundColor( QColor( 0, 0, 200 ) );

  /* aggiunta pulsanti */
  tr_label_array puls;
  tr_label**ps[]= { &maiuscole, &minuscole, &simboli, &sinistra, &destra, &cancella, &exit };
  QString etichette=TRFORM( "MAIUSC#maiusc|minusc#minusc|simboli#simboli|sx#sx|dx#dx|cancella#cancella|uscita#uscita" );

  pulsanti( etichette, false, ps, NULL );

  CONNECT( maiuscole );
  connect( minuscole, SIGNAL( clicked() ), this, SLOT( minuscole_clicked() ) );
  connect( simboli, SIGNAL( clicked() ), this, SLOT( simboli_clicked() ) );
  connect( sinistra, SIGNAL( clicked() ), this, SLOT( sx_clicked() ) );
  connect( destra, SIGNAL( clicked() ), this, SLOT( dx_clicked() ) );
  connect( cancella, SIGNAL( clicked() ), this, SLOT( cancella_clicked() ) );
  connect( exit, SIGNAL( clicked() ), this, SLOT( exit_clicked() ) );

  init();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_tastiera::~frm_tastiera() { DELETE( tasti ); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::init() { barra_stato->setText( tr( "Inserimento valore" ) ); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::imposta_maiuscole() {
  QString c= tr(
    "A|B|C|D|E|F|G|<-|"
    "H|I|J|K|L|M|N|O|"
    "P|Q|R|S|T|U|V|W|"
    "X|Y|Z|.|,|(|)|:|"
    "|| |!|'"
  );
  QStringList e=QStringList::split('|',c,true);
  int i=0;
  while ( tasti[i]!=NULL ) {
    tasti[i]->setText( e[i] );
    i++;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::imposta_minuscole() {
  QString c= tr(
    "a|b|c|d|e|f|g|<-|"
    "h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|"
    "x|y|z|.|,|(|)|:||| |!|'|");
  QStringList e=QStringList::split('|',c,true);
  int i=0;
  while ( tasti[i]!=NULL ) {
    tasti[i]->setText( e[i] );
    i++;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::imposta_simboli() {
  QString c=tr(
    "|||||{|}|<-|[|]|@||#|<|>||\\|||%|&|^|_||\"|$|~|\xC4\x84|\xE3\x81\x8D|||||||| ||'|");
    QStringList e=QStringList::split('|',c,true);
  int i=0;
  while ( tasti[i]!=NULL ) {
    // tasti[i]->setText( QString::fromUtf8( c[i] ) );
    tasti[i]->setText( e[i] );
    i++;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::dx_clicked() { pannello->edit->cursorForward( false ); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::sx_clicked() { pannello->edit->cursorBackward( false ); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::tasto_clicked() {
  tr_label*b=( tr_label* ) sender();
  pannello->edit->insert( b->text() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::maiuscole_clicked() { imposta_maiuscole(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::minuscole_clicked() { imposta_minuscole(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::simboli_clicked() { imposta_simboli(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::cancella_clicked() {
  pannello->setText( "" );
  accept();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::ok_clicked() { accept(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::exit_clicked() { reject(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void frm_tastiera::bs_clicked() {
  if ( pannello->edit->selectedText()==pannello->text() ) // se tutto selezionato
  {
    pannello->edit->setSelection( 0, 0 );
    pannello->edit->setCursorPosition( pannello->text().length() );
  }
  pannello->edit->backspace();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::spazio_clicked() { pannello->edit->insert( " " ); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_tastiera::canc_clicked() { pannello->edit->clear(); }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      TR_FORMS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_Dict<QString>tr_forms::forms( 107 );

tr_forms::tr_forms( QWidget*parent, QString name ):frm_base( parent, name, MYFLAGS ) {
  QString form_str="";
  if ( forms[name] ) form_str=*forms[name];
  if ( form_str.isEmpty() ) {
    cout<<"Form " <<name<<QApplication::tr( " non presente in archivio\n" );
    return;
  }

  build( form_str, QStringList() );
}

tr_forms::~tr_forms() { }


void tr_forms::add_form( QString parent_dot_name_form, QString params ) {

  forms.setAutoDelete( true );

  int pos=parent_dot_name_form.findRev( '.' );
  QString parent=parent_dot_name_form.left( pos );

  /* procedimento: 1) estrapola la form base 2) applica la trasformazione tramite params
  3) salva nel dizionario la nuova form */

  QString*form_str;
  NEW( form_str, QString( "" ) );

  // scandisce le form base
  if ( parent=="scelta" ) {
    *form_str="status=<status>\n""listview=(tok:<lv_tok>,<tutti>,<noexit>)\n""buttons=ok|uscita\n"
       "links=listview|conto normale|ok";
  }

  if ( !form_str->isEmpty() ) {
    QStringList parametri=QStringList::split( ';', params );
    TROWSLI( parametri ) {
      if ( parametri[i] [0]=='-' ) {
        QString virgolaparametro="," +parametri[i].mid( 1 );
        if ( form_str->find( virgolaparametro )!=-1 ) { form_str->remove( virgolaparametro ); }
        else { form_str->remove( parametri[i].mid( 1 ) ); }
      } // if -<parametro>
      else {
        QString nome=parametri[i].section( '=', 0, 0 );
        QString valore=parametri[i].section( '=', 1, 1 );
        form_str->replace( nome, valore );
      }
    } // TROWLI parametri

    forms.insert( parent_dot_name_form, form_str );
  }
  else { cout<<QApplication::tr( "Form genitore non riconosciuta:" )<<parent<<endl; }
}
