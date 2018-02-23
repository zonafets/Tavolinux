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

/* questo file racchiude il corpo di tutti i controlli usati
   in ordine alfabetico */

#include <typeinfo>
#include <qimage.h>
#include <qpainter.h>

#include "tr_barra_stato.hpp"
#include "tr_input.hpp"
#include "tr_label.hpp"
#include "tr_lineedit.hpp"
#include "tr_table.hpp"
#include "tr_time.hpp"
#include "tr_globals.hpp"
#include "tr_tools.hpp"
#include "tr_config.hpp"

extern tr_globals glb;
extern tr_tools tools;
extern tr_config config;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      TR_BARRA_TITOLO
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_barra_titolo::tr_barra_titolo(
  QWidget* parent, const char* name , WFlags fl ):
  tr_label(parent, name, fl)
{
    setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ));
    setMinMax( 0, config.finestra_altezza_titolo, 32767, config.finestra_altezza_titolo );
    QFont barra_titolo_font(  font() );
    barra_titolo_font.setPixelSize( config.barra_titolo_font_size );
    barra_titolo_font.setBold( TRUE );
    setFont( barra_titolo_font );
    //setFrameShape( QLabel::MShape );
    //setFrameShadow( QLabel::MShadow );
    setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
}

void tr_barra_titolo::setText(QString t,bool solo_t_senza_piva)
{
  if (!solo_t_senza_piva)
    t = config.nome_applicazione + " -" + t + "- " +
        glb.ragione_sociale+" - P.I./C.F. "+glb.piva;
  // t.prepend("<table border=0 cellspacing=0 cellpadding=0><tr><td align=\"left\" valign=\"middle\">Design by S.Zaglio 2003-2005</td><td align=\"right\">");
  //t+="</td></tr></table>";
  //PP(t);
  tr_label::setText(t);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      TR_BARRA_STATO
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_barra_stato::tr_barra_stato( QWidget*parent, const char*name, WFlags fl ):tr_widget( parent, name, fl ) {

  _tipo=nullo;
  testo=NULL;
  layout=NULL;
  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  //barra_stato->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)4, 0, 0, barra_stato->sizePolicy().hasHeightForWidth() ) );
  //setMinimumSize( QSize( 0, config.finestra_altezza_stato ) );
  //setMaximumSize( QSize( config.finestra_larghezza, config.finestra_altezza_stato ) );
  setPaletteForegroundColor( QColor( 0, 0, 148 ) );
  setBackgroundOrigin( QLabel::WindowOrigin );
  QFont barra_stato_font( font() );
  barra_stato_font.setPixelSize( config.barra_stato_font_size );
  barra_stato_font.setBold( true );
  setFont( barra_stato_font );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_barra_stato::tipo( tr_barra_stato::tr_tipo tipo ) {
  if ( _tipo!=nullo ) {
    cout<<tr( "Tentativo di cambio formato alla barra di stato" );
    abort();
  }

  if ( _tipo==tipo ) return; // se era gi�stato impostato

  _tipo=tipo;
  if ( _tipo==stato ) {

    NEW( layout, QGridLayout( this, 1, 10, 0, 0 ) );

    char*nome_icone[]=
      { "stato_tavolo.png", "stato_sala.png", "stato_cameriere.png", "stato_listino.png", "stato_cliente.png" };
    tr_label**icone[]= {
      &ico_tavolo, &tavolo, &ico_sala, &sala, &ico_cameriere, &cameriere, &ico_listino, &listino, &ico_cliente, &cliente, NULL
    };

    int dx=( config.finestra_larghezza-( config.altezza_riga*5 ) )/5;
    int i=0;
    while ( icone[i]!=NULL ) {
      tr_label*p;
      NEW( p, tr_label( this ) );
      *icone[i]=p;
      p->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
      p->setFrameShape( QLabel::NoFrame );
      p->setMargin( 2 );
      p->setAlignment( int( QLabel::WordBreak|QLabel::AlignCenter ) );
      p->setScaledContents( false );
      p->setPaletteForegroundColor( paletteForegroundColor() );
      p->setPaletteBackgroundColor( paletteBackgroundColor() );
      p->label->setBackgroundOrigin( QWidget::WindowOrigin );

      if ( i&1==1 ) {
        p->setMinimumSize( QSize( dx, config.finestra_altezza_stato ) );
        p->setMaximumSize( QSize( dx, config.finestra_altezza_stato ) );
        p->resize( dx, config.finestra_altezza_stato );
      }
      else {
        p->setMinimumSize( QSize( config.icon_size, config.finestra_altezza_stato ) );
        p->setMaximumSize( QSize( config.icon_size, config.finestra_altezza_stato ) );
        p->setPixmap( PIXMAP( nome_icone[i/2] ) );
        p->resize( config.icon_size, config.finestra_altezza_stato );
      }
      i++;
    }

    // pone in layout griglia  tr_label *ws[] = {
    // ico_tavolo, tavolo, ico_sala, sala, ico_cameriere, cameriere,
    // ico_in orizzontale
    tr_label*ws[]=
      { ico_tavolo, tavolo, ico_sala, sala, ico_cameriere, cameriere, ico_listino, listino, ico_cliente, cliente };
    TROWI( ws )layout->addWidget( ws[i], 0, i, Qt::AlignCenter );
    QPixmap pix;
    if (paletteBackgroundPixmap()) pix=*paletteBackgroundPixmap();
    if ( !pix.isNull() ) { TROWI( ws )ws[i]->setPaletteBackgroundPixmap( pix ); }

    connect( tavolo, SIGNAL( clicked() ), SIGNAL( tavolo_clicked() ) );
    connect( sala, SIGNAL( clicked() ), SIGNAL( sala_clicked() ) );
    connect( cameriere, SIGNAL( clicked() ), SIGNAL( cameriere_clicked() ) );
    connect( listino, SIGNAL( clicked() ), SIGNAL( listino_clicked() ) );
    connect( cliente, SIGNAL( clicked() ), SIGNAL( cliente_clicked() ) );

    connect( ico_tavolo, SIGNAL( clicked() ), SIGNAL( ico_tavolo_clicked() ) );
    connect( ico_sala, SIGNAL( clicked() ), SIGNAL( ico_sala_clicked() ) );
    connect( ico_cameriere, SIGNAL( clicked() ), SIGNAL( ico_cameriere_clicked() ) );
    connect( ico_listino, SIGNAL( clicked() ), SIGNAL( ico_listino_clicked() ) );
    connect( ico_cliente, SIGNAL( clicked() ), SIGNAL( ico_cliente_clicked() ) );

  }

  else {
    NEW( layout, QGridLayout( this, 1, 1, 0, 0 ) );
    //testo = NEW tr_label( this, "testo" );
    NEW( testo, tr_label( this, "testo" ) );
    testo->setMinimumHeight( config.finestra_altezza_stato );
    testo->setMaximumHeight( config.finestra_altezza_stato );
    //testo->setFixedSize(500,config.barra_stato_dy );
    //testo->setMinMax( 0,0, 32767, config.altezza_riga);
    testo->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
    //testo->setFrameShape( QLabel::NoFrame );
    //testo->setMargin( 0 );
    testo->setAlignment( int( QLabel::SingleLine|QLabel::AlignCenter ) );
    testo->setScaledContents( true );
    testo->setPaletteForegroundColor( paletteForegroundColor() );
    testo->setPaletteBackgroundColor( paletteBackgroundColor() );
    layout->addWidget( testo, 0, 0, Qt::AlignCenter );

  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_barra_stato::~tr_barra_stato() { }

void tr_barra_stato::show() {
  QWidget::show();
  if ( testo ) {
    //testo->adjustSize();
  }
  //adjustSize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_barra_stato::aspetto_titolo() {
  /* testo->setFrameShape( QLabel::NoFrame ); testo->setFrameShadow( QLabel::Plain ); testo->setLineWidth( 0 );
  testo->setTextFormat(Qt::RichText); */
  testo->label->setAlignment( int( QLabel::SingleLine|QLabel::AlignCenter ) );
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// se in txt c'�qualcosa tipo xxx|yy|xxx  o solo  xxx|yy, yy viene evidenziato
void tr_barra_stato::setText( QString txt, int /* pixelSize=0 */, bool /* bold = false */ ) {
  int ccode=txt.find( "|" );
  if ( ccode!=-1 ) {
    QColor c( paletteBackgroundColor() );
    c.setRgb( 255-c.red(), 255-c.green(), 255-c.blue() );
    while (txt.contains('|')) {
      txt.replace( ccode, 1, "<font color=\"" +c.name()+"\">" );
      ccode=txt.find( "|" );
      if ( ccode!=-1 ) txt.replace( ccode, 1, "</font>" );
      else txt+="</font>";
      ccode=txt.find( "|" );
    }
  }
  if ( tipo()==nullo ) tipo( titolo );
  adatta();
  testo->setText( txt );
  testo->label->setAlignment( int( QLabel::SingleLine|QLabel::AlignCenter ) );
  adatta( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* void tr_barra_stato::setText( QString st ) { if (tipo()==nullo) tipo( titolo ); adatta();
testo->setText( st ); adatta(false); } */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void tr_barra_stato::setText( QString _tavolo, QString _sala, QString _cameriere, QString _listino, QString _cliente ) {
  if ( tipo()==nullo ) tipo( stato );
  adatta();
  tavolo->setText( _tavolo );
  sala->setText( _sala );
  if ( !_cameriere.isEmpty() ) cameriere->setText( _cameriere );
  else
    cameriere->setText( glb.stato[tr_globals::p_utente_descrizione] );
  if ( !_listino.isEmpty() ) listino->setText( _listino );
  else
    listino->setText(  glb.stato[tr_globals::p_listino_descrizione] );
  if (!_cliente.isEmpty()) cliente->setText( _cliente );
  mem_check_begin();
  showMemStats();
  adatta( false );
  mem_check_end( adatta( false ) );
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_barra_stato::setText( tr_label *campo, QString txt)
{
  if (!campo) return;
  adatta();
  campo->setText( txt );
  adatta( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_barra_stato::adatta( bool start ) {
  QLabel**w;
  if ( tipo()==stato ) {
    NEW( w, ( QLabel* ) [6] );
    w[0]=tavolo->label;
    w[1]=sala->label;
    w[2]=cameriere->label;
    w[3]=listino->label;
    w[4]=cliente->label;
    w[5]=NULL;
  }
  else {
    //w=w2
    //testo->adatta();
    return;
  }

  int i=0;
  while ( w[i]!=NULL ) {
    if ( start ) {
      QFont f( w[i]->font() );
      f.setPixelSize( config.barra_stato_font_size );
      w[i]->setFont( f );
    }
    else {

      /* le seguenti 2 righe correggono un difetto delle Qt */
      QString testo_originale=w[i]->text();
      if (!testo_originale.isEmpty()) {
        w[i]->setText( tools.solo_testo( w[i]->text() ) );

        bool fai=false;
        while ( !fai ) {
          /* if ( w[i]==testo && (w[i]->sizeHint().width() < w[i]->contentsRect().size().width() ||
          w[i]->sizeHint().height() < w[i]->contentsRect().size().height())) fai = true;
          if ( w[i]!=testo && (w[i]->sizeHint().width() > w[i]->contentsRect().size().width() ||
          w[i]->sizeHint().height() > w[i]->contentsRect().size().height())) fai = true; */
          if ( w[i]->sizeHint().width()>w[i]->contentsRect().size().width()
               ||w[i]->sizeHint().height()>w[i]->contentsRect().size().height() )fai=true;

          if ( fai ) {

            QFont f( w[i]->font() );
            f.setPixelSize( f.pixelSize()-1 );
            w[i]->setFont( f );
            if ( f.pixelSize()<9 ) break;
            w[i]->adjustSize();
            fai=false;
          }
          else
            break;
        }

        w[i]->setText( testo_originale );
        //w[i]->setAlignment( int( QLabel::AlignCenter ) );
      } // se not empty

    }
    i++;
  }
  DELETE_ARRAY( w );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_barra_stato::setPaletteForegroundColor( QColor c ) {
  QWidget::setPaletteForegroundColor( c );
  if ( tipo()==titolo&&testo ) testo->setPaletteBackgroundColor( c );
  if ( tipo()==stato ) {
    tr_label*ws[]=
      { ico_tavolo, tavolo, ico_sala, sala, ico_cameriere, cameriere, ico_listino, listino, ico_cliente, cliente };
    TROWI( ws )ws[i]->setPaletteForegroundColor( c );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_barra_stato::setPaletteBackgroundColor( QColor c ) {
  QWidget::setPaletteBackgroundColor( c );
  if ( tipo()==titolo&&testo ) testo->setPaletteBackgroundColor( c );
  if ( tipo()==stato ) {
    tr_label*ws[]=
      { ico_tavolo, tavolo, ico_sala, sala, ico_cameriere, cameriere, ico_listino, listino, ico_cliente, cliente };
    TROWI( ws )ws[i]->setPaletteBackgroundColor( c );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_barra_stato::setPaletteBackgroundPixmap( QPixmap p ) {
  QWidget::setPaletteBackgroundPixmap( p );
  if ( tipo()==titolo&&testo ) testo->setPaletteBackgroundPixmap( p );
  if ( tipo()==stato ) {
    tr_label*ws[]=
      { ico_tavolo, tavolo, ico_sala, sala, ico_cameriere, cameriere, ico_listino, listino, ico_cliente, cliente };
    TROWI( ws )ws[i]->setPaletteBackgroundPixmap( p );
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      TR_LINEEDIT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QValidator::State tr_validator::validate( QString&input, int&pos ) const {
  QValidator::State s;
  if ( ( s=QRegExpValidator::validate( input, pos ) )==Invalid ) BEEP;
  return s;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_lineedit::tr_lineedit( QWidget*parent, const char*name ):QLineEdit( parent, name )
{
  _upcase=true;
  keypressed=false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_lineedit::tr_lineedit( const QString&contents, QWidget*parent, const char*name ):QLineEdit( contents, parent, name ) {
  _upcase=true;
  keypressed=false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_lineedit::~tr_lineedit() { }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_lineedit::keyPressEvent( QKeyEvent*e ) {
  keypressed=true;
  lastkey=QChar(e->ascii());
  if ( e&&upcase() ) {
    QChar c=( e->text().upper() [0] );
    QKeyEvent E( e->type(), e->key(), ( int )c, ( int )e->state(), e->text().upper(), e->isAutoRepeat(), e->count() );
    e->ignore();
    QLineEdit::keyPressEvent( &E );
  }
  else
    QLineEdit::keyPressEvent( e );

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_lineedit::setRegExp( QString rx ) {
  QRegExp rexp(rx);
  if ( !validator() ) {
    tr_validator*rxp;
    NEW( rxp, tr_validator( rexp, this ) );
    setValidator( rxp );
  }
  else
    ( ( tr_validator* ) validator() )->setRegExp( rexp );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_lineedit::adatta() {

  if ( !geometry().size().isNull() ) { // attiva solo se ha acquisito una dimensione
    int start,len,cp;
    bool hst;
    cp=cursorPosition();
    hst=hasSelectedText();
    if (hst) getSelection(&start,&len);
    //cout << "verifico..\n";

    QFont f( font() );
    int l=QFontMetrics( f ).width( text() );
    //cout << "l vale " << l << ", width() vale " << geometry().width() << " e size :" << size() << "\n";
    if ( geometry().width()>0&&l>geometry().width() ) {
      setMinimumSize( size() ); // fissa la dimnesione in modo che non si adatti al font
      while ( l>geometry().width() ) {
        if ( f.pixelSize()<9 ) break;
        f.setPixelSize( f.pixelSize()-1 );
        setFont( f );
        l=QFontMetrics( f ).width( text() );
        //cout << "fontS =" << f.pixelSize() << " e l=" << l << "testo=" << text() << "|" << endl;
      }
    }

    if ( geometry().width()>0&&l<geometry().width() ) {
      while ( l<geometry().width() ) {
        if ( f.pixelSize()>23 ) break;
        f.setPixelSize( f.pixelSize()+1 );
        QFontMetrics fm( f );
        l=fm.width( text() );
        if ( l<geometry().width() ) {
          setFont( f );
          QFontMetrics fm( f );
          l=fm.width( text() );
        }
      }
    }

  setCursorPosition(cp);
  if (hst && start && len) setSelection(start,len);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      TR_TABLE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// se abilitato disegna le fasce alternate come sfondo
//#define sfondo_alternato false

// --------------------------------------------------------------------------------------------------- tr_table
tr_table::tr_table( QWidget*parent, const char*name ):QTable( parent, name ) {
  as_original=false;
  // setSelectionMode(QTable::Single);
  setMinimumSize( QSize( 0, 0 ) );
  setMaximumSize( QSize( 32767, 32767 ) );
  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  setSelectionMode( QTable::NoSelection );
  setShowGrid( true );
  verticalHeader()->hide();
  horizontalHeader()->hide();
  setLeftMargin( 0 );
  setTopMargin( 0 );
  setVScrollBarMode( QScrollView::AlwaysOff );
  setHScrollBarMode( QScrollView::AlwaysOff );
  setFocusStyle( QTable::FollowStyle );

  // imposta il colore di selezione a giallo
  QPalette p=palette().copy();
  QColorGroup cg( p.normal() );
  cg.setColor( QColorGroup::HighlightedText, config.colore_selezione );
  cg.setColor( QColorGroup::Button, QColor( 255, 255, 255 ) );
  p.setDisabled( cg );
  setPalette( p );
  altezza_riga=config.altezza_riga;

  /* la precreazione  delle celle non migliora di molto le prestazioni insertColumns(0,20); insertRows(0,100);
  for (int i=0;i<numRows();i++) { setRowReadOnly(i,true); setRowHeight( i, config.altezza_riga ); } */

  // n_oggetti_collegati = 0;
  // selezione = NULL;
}

// ------------------------------------------------------------------------------------------------ paintCell

void tr_table::activateNextCell()
{
  int next_row, next_col;
  if (currentColumn()==numCols()) {next_row=currentRow()+1;next_col=0;}
  else  {next_row=currentRow();next_col=currentColumn()+1;}
  if (next_row>=numRows()) next_row--;
  setCurrentCell(next_row,next_col);
}

// ------------------------------------------------------------------------------------------------ paintCell

void tr_table::paintCell( QPainter*p, int row, int col, const QRect&cr, bool selected, const QColorGroup&cg ) {
  if (as_original) return QTable::paintCell(p,row,col,cr,selected,cg);

  if ( cr.width() == 0 || cr.height() == 0 ) return;

  const QColorGroup::ColorRole gruppo_colori[17]= {
    QColorGroup::Base, QColorGroup::Foreground, QColorGroup::Background, QColorGroup::Text, QColorGroup::Button,
         QColorGroup::ButtonText, QColorGroup::Light, QColorGroup::Midlight, QColorGroup::Dark, QColorGroup::Mid,
         QColorGroup::Shadow, QColorGroup::Highlight, QColorGroup::HighlightedText, QColorGroup::BrightText, QColorGroup::Link,
         QColorGroup::LinkVisited, QColorGroup::NColorRoles
  };

  QColorGroup _cg( cg );

  // ogni riga dispari �un poco pi scura per distinguerla meglio
#ifdef sfondo_alternato
  if ( row&1==1 ) _cg.setColor( QColorGroup::Base, _cg.color( QColorGroup::Base ).dark( 110 ) );
#endif
  if ( text( row, com_offerto )=="1" ) _cg.setColor( QColorGroup::Base, _cg.color( QColorGroup::Base ).dark( 110 ) );

  // evidenzia meglio le colonne con i valori numerici
  if ( ( col==0||col==2 ) ) {
    _cg.setColor( QColorGroup::Foreground, _cg.color( QColorGroup::Foreground ).light( 140 ) );
    _cg.setColor( QColorGroup::Text, _cg.color( QColorGroup::Text ).light( 140 ) );
  }

  // se �disabilitato scurisce la palette
  if ( !isEnabled() ) {
    for ( int i=0; i<16; i++ )
      _cg.setColor( gruppo_colori[i], _cg.color( gruppo_colori[i] ).dark( 200 ) );
  }

  /* if (selezione) if (selezione->count()>0) { selected = false; QTableSelection *s=selezione->first(); while (s) {
  if (s->topRow() == row ) if (!isEnabled()) { _cg.setColor(QColorGroup::Base, QColor(229, 204, 128).dark(200));
  _cg.setColor(QColorGroup::Highlight, QColor(229, 204, 128).dark(200)); _cg.setColor(QColorGroup::HighlightedText,
  QColor(0, 0, 127 ).dark(200)); break;} else { _cg.setColor(QColorGroup::Base, QColor(229, 204, 128));
  _cg.setColor(QColorGroup::Highlight, QColor(229, 204, 128)); _cg.setColor(QColorGroup::HighlightedText, QColor(0, 0, 127 ));
  break; } s=selezione->next(); } }  else selected=false; */

  if ( selected ) {
    if ( !isEnabled() ) {
      _cg.setColor( QColorGroup::Highlight, QColor( 229, 204, 128 ).dark( 200 ) );
      _cg.setColor( QColorGroup::HighlightedText, QColor( 0, 0, 127 ).dark( 200 ) );
    }
    else {
      _cg.setColor( QColorGroup::Highlight, QColor( 229, 204, 128 ) );
      _cg.setColor( QColorGroup::HighlightedText, QColor( 0, 0, 127 ) );
    }
  }


  // se e' un figlio di articolo lo scrive pi piccolo
  if ( text( row, com_descrizione ).left( 1 )==" " ) {
    QFont forig( p->font() );
    QFont f( font() );
    f.setPixelSize( config.comanda_child_font_size );
    p->setFont( f );
    QTable::paintCell( p, row, col, cr, selected, _cg );
    p->setFont( forig );
  }
  else
  {
    QFont forig( p->font() );
    QFont f( font() );
    f.setPixelSize( config.comanda_font_size );
    p->setFont( f );
    QTable::paintCell( p, row, col, cr, selected, _cg );
    p->setFont( forig );
  }

  // se sta disegnado la cella di mezzo tra comanda e portata, traccia una
  // riga pi spessa come sul don
  if ( row>0&&row<numRows()&&text( row, com_portata )!=text( row-1, com_portata ) ) {

    int w=cr.width();
    int x2=w-1;

    QPen pen( p->pen() );
    QPen p1( QColor( 0, 0, 0 ) );
    p1.setWidth( 2 );
    p->setPen( p1 );
    p->drawLine( 0, 0, x2, 0 );
    p->setPen( pen );
  }

}

void tr_table::elimina_righe() { // non esiste un equiv in QTable
  QMemArray<int>righe( numRows() );
  for ( int i=0; i<numRows(); i++ ) righe[i]=i;
  removeRows( righe );
  //setUpdatesEnabled( false );
  //while (numRows()>0) removeRow(0);
  //setUpdatesEnabled( true );
};

void tr_table::setRowHeight( int r, int h ) {
  if ( rowHeight( r )!=h ) { QTable::setRowHeight( r, h ); }
}

void tr_table::setMinMax( int min_w, int min_h, int max_w, int max_h ) {
  if ( max_w==-1 ) max_w=min_w;
  if ( max_h==-1 ) max_h=min_h;
  // deve chiamare le funzioni derivate xch�ad esempio tr_label le amplia
  setMinimumSize( min_w, min_h );
  setMaximumSize( max_w, max_h );
}

void tr_table::adjustColumn( int col ) {
  int w=0; // topHeader->sectionSizeHint( col, fontMetrics() ).width();
  // if ( topHeader->iconSet( col ) )
  // w += topHeader->iconSet( col )->pixmap().width();
  w=QMAX( w, 20 );
  for ( int i=0; i<numRows(); ++i ) {
    QTableItem*itm=item( i, col );
    if ( !itm ) {
      QWidget*widget=cellWidget( i, col );
      if ( widget )
        w=QMAX( w, widget->sizeHint().width() );
    }
    else {
      if ( itm->colSpan()>1 )
        w=QMAX( w, itm->sizeHint().width()/itm->colSpan() );
      else
        w=QMAX( w, itm->sizeHint().width() );
    }
  }
  w=QMAX( w, QApplication::globalStrut().width() );
  if ( w!=columnWidth( col ) ) { setColumnWidth( col, w ); }
}

// ----------------------------------------------------------------------------------------- setCurrentCell

void tr_table::setCurrentCell( int row, int col ) {
  QTable::setCurrentCell( row, col );
  clearSelection();
  if ( col==-1&&row<numRows() && row>-1 ) selectRow( row );
  if ( row>=numRows() ) ensureCellVisible( numRows()-1, 0 );
}

// --------------------------------------------------------------------------------------------- aggiorna
void tr_table::aggiorna( QStringList contenuto, bool conserva_selezione ) {
  if ( !contenuto.size() ) { elimina_righe(); return; }

  int cols=contenuto[0].contains( config.cmd_sep[0] )+1; // +1 xch�dopo l'ultimo sep c'�una colonna
  if ( cols>numCols() ) {
    elimina_righe();
    insertColumns( 0, cols-numCols() );
    for ( int i=3; i<numCols(); i++ ) { hideColumn( i ); }
  }

  setUpdatesEnabled( false );

  int cr=currentRow();
  if ( contenuto.size()>( uint )numRows() ) insertRows( numRows(), contenuto.size()-( uint )numRows() );
  uint i=0;

  while ( i<contenuto.size() ) {
    QStringList dati=QStringList::split( config.cmd_sep[0], contenuto[i], true );
    setRowHeight( i, altezza_riga );
    setRowReadOnly( i, true );

    agg_riga( i, dati ); // aggiorna riga i con cont...
    i++;
  }

  while ( i<( uint )numRows() ) removeRow( numRows()-1 );

  if ( conserva_selezione ) setCurrentCell( cr );

  setUpdatesEnabled( true );
  repaintContents( true );

  adatta();
}

// --------------------------------------------------------------------------------------------- agg_riga

void tr_table::agg_riga( int riga, QStringList&dati ) {
  const uint gav=GRUPPO_ARTICOLO_VARIANTE, gam=GRUPPO_ARTICOLO_MESSAGGIO;
  const uint gap=GRUPPO_ARTICOLO_PURO, gac = gav | GRUPPO_ARTICOLO_COMMENTO;


  comanda ordine[]= {
    com_id_riga, com_id_istanza, com_livello, com_id_art, com_qta, com_descrizione, com_prezzo, com_tot_riga, com_stato,
         com_offerto, com_id_listino, com_portata
  };

  for ( uint i=0; i<( uint )numCols(); i++ ) {
    QString st;
    if ( i<dati.size() ) {
      if ( i==p_qta&&p_stato<dati.size() ) {
        int stato=dati[p_stato].toInt();
        money qta=dati[p_qta].toDouble();
        if ( ( stato&gav ) || ( stato&gam ) ) {
          if ( qta==-1 ) st="-";
          else if ( qta==0 ) st=" ";
          else if ( qta==1 ) st="+";
          else
            st=dati[p_qta];
        }
        else if ( qta!=0 ) st=dati[p_qta];
        else st="";
        // st.prepend(" ");st+=" "; a mettere gli spazi, perde la giustificazione automatica
      }
      else {
        if ( i==p_descrizione&&p_livello<dati.size() ) {
          int livello=dati[p_livello].toInt();
          if ( livello ) st=QString( "" ).fill( ' ', livello*4 );
          else st="";
          st+=dati[i].stripWhiteSpace();
        }
        else
          st+=dati[i].stripWhiteSpace();
          if ( i==p_tot_riga&&p_stato<dati.size() ) {
            int stato=dati[p_stato].toInt();
	    int ids=dati[p_id_istanza].toInt();
            money prz=dati[p_tot_riga].toDouble();
            if (!prz && ((stato & gam) || (ids == tr::gruppi::segue) ||
			 (ids == tr::gruppi::insieme_a) ||
			 (stato & gac))) st="";
          }
      }
    } // if i<dati.size

    /* QTableItem* cell; cell = NEW QTableItem(this, QTableItem::Never, st); cell->setText( st ); setItem( riga, i, cell ); */
    setText( riga, ordine[i], st );

  }


}

void tr_table::adatta() {
  QFontMetrics fm( font() );
  adjustColumn( com_qta );
  int w=columnWidth( com_qta );
  if ( w<altezza_riga ) { w=altezza_riga; setColumnWidth( com_qta, w ); }
  adjustColumn( com_prz );
  // setColumnWidth( col_qta, fm.width(config.articoli_falso_qta) );
  // setColumnWidth( col_tot_riga, fm.width(config.articoli_falso_prz) );
  w=width();
  setColumnWidth( com_descrizione, ( w-( columnWidth( com_qta )+columnWidth( com_prz ) )-2 ) );
}

// ----------------------------------------------------------------------------------------- event

bool tr_table::event( QEvent*e ) {
  if ( e->type()==QEvent::MouseButtonDblClick ) {
    ( ( QMouseEvent* ) e )->ignore();
    return false;
  }

  return QTable::event( e );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      TR_TIME
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_table_intervalli::tr_table_intervalli( QWidget*parent, const char*name ):QTable( parent, name ) {
  setMouseTracking( true );
  setVScrollBarMode( QScrollView::AlwaysOff );
  setHScrollBarMode( QScrollView::AlwaysOff );
  setSelectionMode( QTable::NoSelection );
  verticalHeader()->hide();
  horizontalHeader()->hide();
  setLeftMargin( 0 );
  setTopMargin( 0 );
  insertColumns( 0, 26 ); // aggiunge header + ore + header
  insertRows( 0, 14 ); // aggiunge header + frazioni da 5 minuti + header

  int wh=32; // config.altezza_riga
  int i;
  for ( i=0; i<numCols(); i++ ) {
    if ( i>0&&i<numCols()-1 ) {
      setText( 0, i, QString::number( i-1 ) );
      setText( numRows()-1, i, QString::number( i-1 ) );
    }
    setColumnWidth( i, wh );
    setColumnStretchable( i, false );
  }
  for ( i=0; i<numRows(); i++ ) {
    if ( i>0&&i<numRows()-1 ) {
      setText( i, 0, QString( "0" +QString::number( ( i-1 )*5 ) ).right( 2 ) );
      setText( i, numCols()-1, QString( "0" +QString::number( ( i-1 )*5 ) ).right( 2 ) );
    }
    setRowHeight( i, wh );
    setRowStretchable( i, false );
  }

  setReadOnly( true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_table_intervalli::paintCell( QPainter*p, int row, int col, const QRect&cr, bool selected, const QColorGroup&cg ) {

  if ( cr.width()==0||cr.height()==0 )
    return;

  QColorGroup _cg( cg );

  // ogni riga dispari �un poco pi scura per distinguerla meglio
  if ( row==0||col==0||row==numRows()-1||col==numCols()-1 ) _cg.setColor( QColorGroup::Base, QColor( 150, 150, 150 ) );
  if ( text( row, col )==" " ) _cg.setColor( QColorGroup::Base, config.colore_selezione );

  QTable::paintCell( p, row, col, cr, selected, _cg );

}

void tr_table_intervalli::evidenzia( int row, int col ) {
  QString p="";
  int i, j;
  int uc=numCols()-1, ur=numRows()-1;

  bool corner=( ( row==0&&col==0 )||( row==0&&col==uc )||( row==ur&&col==0 )||( row==ur&&col==uc ) );

  if ( corner ) {
    if ( text( 1, 1 )=="" ) p=" ";
    for ( j=1; j<ur; j++ )
      for ( i=1; i<uc; i++ ) setText( j, i, p );
    return;
  }

  if ( config.listino_no_intervalli ) {
    if ( row<ur/2 ) row=0;
    else row=ur;
    if ( col==0||col==uc ) return;
  }

  if ( row>0&&row<ur&&col>0&&col<uc ) {
    if ( text( row, col )==" " ) setText( row, col, "" );
    else
      setText( row, col, " " );
    return;
  }

  enum { vert_giu, vert_su, oriz_dx, oriz_sx }
  dir;
  int section;
  if ( row==0 ) dir=vert_giu;
  if ( row==ur ) dir=vert_su;
  if ( col==0 ) dir=oriz_dx;
  if ( col==uc ) dir=oriz_sx;

  int start, stop;

  switch ( dir ) {
    case vert_giu:
      section=col;
      if ( text( 1, section )=="" ) p=" ";
      start=1;
      stop=ur;
    break;

    case vert_su:
      section=col;
      if ( text( ur-1, section )=="" ) p=" ";
      start=1;
      stop=ur;
    break;

    case oriz_dx:
      section=row;
      start=1;
      stop=uc;
      if ( text( section, 1 )=="" ) p=" ";
    break;

    case oriz_sx:
      section=row;
      start=1;
      stop=uc;
      if ( text( section, uc-1 )=="" ) p=" ";
    break;
  }

  for ( i=start; i<stop; i++ ) {
    if ( dir==vert_giu||dir==vert_su ) setText( i, section, p );
    else
      setText( section, i, p );
  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_time::tr_time( QWidget*parent, const char*name ):QWidget( parent, name ) {
  setPaletteBackgroundColor( parent->paletteBackgroundColor() );
  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

  NEW( layout, QGridLayout( this, 5, 2 ) );

  NEW( qnibbles, tr_table_intervalli( this, "nibbles" ) );


  // QTableItem *it = NEW QTableItem ( qnibbles
  connect( qnibbles, SIGNAL( currentChanged( int, int ) ), this, SLOT( qnibbles_currentChanged( int, int ) ) );
  connect( qnibbles, SIGNAL( pressed( int, int, int, const QPoint& ) ),
       this, SLOT( qnibbles_click_o_press( int, int, int, const QPoint& ) ) );

  layout->addMultiCellWidget( qnibbles, 0, 6, 0, 0 );

  tr_label**w[]= { &su, &sp1, &sx, &sp2, &dx, &sp3, &giu, NULL };
  QString nomi[]= { "su", "sp1", "sx", "sp2", "dx", "sp3", "giu" };
  QString icone[]= { "az_su.png", "", "az_sx.png", "", "az_dx.png", "", "az_giu.png" };
  for ( int i=0; w[i]!=NULL; i++ ) {
    tr_label*p;
    NEW( p, tr_label( this, nomi[i] ) );
    *w[i]=p;
    int dy;
    if ( i==1||i==3||i==5 ) dy=10;
    else dy=50;
    p->setMinMax( 50, dy );
    p->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
    p->setBackgroundOrigin( QLabel::ParentOrigin );
    if ( icone[i]!="" ) p->setPixmap( PIXMAP( icone[i] ) );
    p->setAlignment( int( QLabel::AlignCenter ) );
    layout->addWidget( p, i, 1 );
    connect( p, SIGNAL( clicked() ), this, SLOT( dir_clicked() ) );
  }

  ccc=false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_time::~tr_time() { }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_time::dir_clicked() {
  static bool sx=true;
  static bool up=true;
  int uc=qnibbles->numCols()-1, ur=qnibbles->numRows()-1;

  tr_label*s=( tr_label* ) sender();
  if ( qstrcmp( s->name(), "su" )==0 ) up=true;
  if ( qstrcmp( s->name(), "giu" )==0 ) up=false;
  if ( qstrcmp( s->name(), "sx" )==0 ) sx=true;
  if ( qstrcmp( s->name(), "dx" )==0 ) sx=false;

  if ( up&&sx ) qnibbles->ensureCellVisible( 0, 0 );
  if ( up&&!sx ) qnibbles->ensureCellVisible( 0, uc );
  if ( !up&&sx ) qnibbles->ensureCellVisible( ur, 0 );
  if ( !up&&!sx ) qnibbles->ensureCellVisible( ur, uc );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_time::qnibbles_currentChanged( int row, int col ) {
  evidenzia( row, col );
  ccc=true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_time::qnibbles_click_o_press( int row, int col, int, const QPoint& ) {
  if ( !ccc ) evidenzia( row, col );
  ccc=false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QByteArray tr_time::intervalli() {
  QByteArray v( dim_intervalli ); // 24*12/8
  uint i, j;
  for ( i=0; i<v.count(); i++ ) v[i]=0;

  bool modificato=false;

  // la tabella viene ruotata di 90 perch�si considera una sequenza
  // di intervalli da 5 minuti e non ore
  uint w=qnibbles->numRows()-2; // 12
  uint h=qnibbles->numCols()-2; // 24
  for ( j=0; j<h; j++ )
    for ( i=0; i<w; i++ ) {
      int byte=( j*w+i )/8;
      int bit=1<<( ( j*w+i )%8 );
      // cout << P(j) << P(i) << P(byte) << P(bit) << P(w) << " text=" << qnibbles->text(i+1,j+1) << endl;;
      if ( qnibbles->text( i+1, j+1 )==" " ) {
        v[byte]|=bit;
        modificato=true;
      }
    }

  if ( modificato ) return v;
  else return QByteArray();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_time::intervalli( QByteArray valori ) {
  uint i, j;

  // la tabella viene ruotata di 90 perch�si considera una sequenza
  // di intervalli da 5 minuti e non ore
  uint w=qnibbles->numRows()-2; // 12
  uint h=qnibbles->numCols()-2; // 24
  for ( j=0; j<h; j++ )
    for ( i=0; i<w; i++ ) {
      int byte=( j*w+i )/8;
      int bit=1<<( ( j*w+i )%8 );
      // cout << P(j) << P(i) << P(byte) << P(bit) << P(w) << "valori[byte]:" << int(valori[byte]) << endl;;
      if ( valori.isNull() ) qnibbles->setText( i+1, j+1, "" );
      else if ( ( valori[byte]&bit )==bit ) qnibbles->setText( i+1, j+1, " " );
      else
        qnibbles->setText( i+1, j+1, "" );
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                         TR_CALCOLATRICE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TR_CALCOLATRICE_CPP
#define TR_CALCOLATRICE_CPP

#include "tr_calcolatrice.hpp"

#define SEGNO '.' // config.segno_decimale

tr_calcolatrice::tr_calcolatrice(QWidget *parent, const char *nome) :
tr_widget(parent,nome), limiti(parent)
{

  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ));


  QString nomi[] = { 	"7", 	"8", 	"9", 	"<img name=\"img\" src=\"az_exit.png\" width=\"32\" height=\"32\">",
                  "4", 	"5", 	"6", 	"<-",
                  "1", 	"2", 	"3", 	"+/-",
                  "0", 	"000", 	"00", 	"<img name=\"img\" src=\"az_ok.png\" width=\"32\" height=\"32\">"	};

  QGridLayout *area_layout;
  NEW(area_layout, QGridLayout(this,2,2));

  NEW ( pannello , QLCDNumber( this, "pannello" ) );
  pannello->setMinimumSize( QSize( 240, config.altezza_riga /*40*/ ) );
  pannello->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ));
  pannello->setMaximumSize( QSize( 32767, config.altezza_riga ) );
  pannello->setPaletteForegroundColor( QColor( 109, 218, 0 ) );
  pannello->setPaletteBackgroundPixmap( QPixmap::fromMimeSource( "sfondo_display.jpg" ) );
  QFont pannello_font(  pannello->font() );
  pannello_font.setBold( TRUE );
  pannello->setFont( pannello_font );
  pannello->setNumDigits( 12 );
  pannello->setSegmentStyle( QLCDNumber::Filled );
  area_layout->addWidget( pannello, 0, 0 );

  QFrame *pulsantiera;
  NEW ( pulsantiera , QFrame( this , "pulsantiera" ) );
  pulsantiera->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ));
  pulsantiera->setFrameShape( QFrame::NoFrame );
  pulsantiera->setFrameShadow( QFrame::Plain );
  area_layout->addWidget(pulsantiera, 1, 0);

  QGridLayout *layout;
  NEW ( layout , QGridLayout( pulsantiera, 4, 4 , 0, -1, "layout_pulsantiera") );

  for (int j = 0; j<4; j++)
      for (int i = 0; i<4; i++) {
      tr_label *tasto;
      NEW ( tasto , tr_label( pulsantiera, nomi[j*4+i] ) );
      tasto->aspetto_bottone();
      tasto->setMinimumSize( QSize( 60, config.altezza_riga /*46*/ ) );
      tasto->setMaximumSize( QSize( 32767, config.altezza_riga /*46*/ ) );
      tasto->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ));
      // tasto->setMaximumSize( QSize( 60, 46 ) );
      tasto->setPaletteForegroundColor( QColor( 0, 0, 127) );
      tasto->setPaletteBackgroundColor( QColor( 200, 200, 200) );
      if ((i==3 || j==3))
          if (i>0) { // scartando il tasto in basso a sinistra, se appartiene al lato desto o al fondo, cambia colore
          tasto->setPaletteBackgroundColor( QColor(170, 170, 170) );
      }
      //tasto->setPixmap( QPixmap::fromMimeSource( "tasto_1.png" ) );
      tasto->setScaledContents( TRUE );
      if (nomi[j*4+i]=="<-") tasto->label->setTextFormat(Qt::PlainText);
      tasto->setText(nomi[j*4+i]);
      if (nomi[j*4+i]=="00") tasto_decimale_zerozero=tasto;
      layout->addWidget( tasto, j, i );
      connect(tasto, SIGNAL(clicked()), this, SLOT(tasto_clicked()));
  }

  // pannello laterale con fattori
  NEW( fattori, QFrame( this, "fattori" ) );
  fattori->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding ) );
  fattori->setMinimumSize( QSize( 50, config.altezza_riga ) );
  fattori->setMaximumSize( QSize( 50, 32767 ) );
  fattori->setFrameShape( QFrame::NoFrame );
  fattori->setFrameShadow( QFrame::Plain );
  //abc->setPaletteBackgroundColor( QColor( 0, 117, 175 ) );
  fattori->setPaletteBackgroundColor( paletteBackgroundColor() );
  //abc->setBackgroundOrigin( QFrame::WidgetOrigin );	// non scurisce
  //abc->setBackgroundOrigin( QFrame::ParentOrigin );
  // abc->setPaletteBackgroundColor( QColor( 255, 231, 47 ) ); // giallo intenso

  NEW( fattori_layout, QVBoxLayout( fattori, 0, 0, "fattori_layout" ) );

  char*etichette[]= { "10x", "11x", "12x", "13x", "14x", NULL };
  int i=0;
  while ( etichette[i]!=NULL ) {
    tr_label*p;
    NEW( p, tr_label( fattori, etichette[i] ) );
    // p->aspetto_bottone();
    p->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding ) );
    QFont f( p->font() );
    f.setPixelSize( 12 );
    f.setBold( true );
    p->setFont( f );
    p->setPaletteForegroundColor( config.colore_selezione );
    p->setMinimumSize( QSize( 50, config.altezza_riga ) );
    p->setMaximumSize( QSize( 50, 32767 ) );
    p->setText( QString( etichette[i] ).upper() );
    p->setBackgroundMode( Qt::NoBackground );
    fattori_layout->addWidget( p );
    connect( p, SIGNAL( clicked() ), this, SLOT( fattori_clicked() ) );
    i++;
  }
  fattori->hide();
  area_layout->addMultiCellWidget(fattori, 0,1, 1,1);

  primo_tasto = true;
  snum = "0";	// contiene la versione stringa. Serve per le situazione in cui snum = "*.". Il valore numerico
  // verrebbe sempre tradotto in .0 e questo verrebbe eliso.
  accettato=rigettato=false;
  num_decimali=0;
  modalita=decimale_fisso;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_calcolatrice::impostazioni(
  QString valore,
  QString validatore,
  tr_calcolatrice::modo _modalita )
{
    modalita=_modalita;
    QString st=" ";
    st[0]=config.segno_decimale;
    if (modalita!=decimale_fisso)
      tasto_decimale_zerozero->setText(st);

    pannello->setSmallDecimalPoint( false );
    pannello->setSegmentStyle ( QLCDNumber::Filled );
    pannello->setNumDigits( 12 );  // dimensione fissa perch�sen�il numero viene centrato nel display
    num_decimali=0;
    snum=valore;
    if (!validatore.isEmpty()) {
      if (validatore=="real") num_decimali=4;
      if (validatore=="real2") num_decimali=2;
      if (validatore=="real3") num_decimali=3;
      bool gia_regexp=false;
      if (validatore.find('{')!=-1 ||
          validatore.find('[')!=-1 ||
          validatore.find('-')!=-1 ||
          validatore.find("\\w")!=-1 ||
          validatore.find("\\d")!=-1 ||
          validatore.find("\\.")!=-1) gia_regexp=true;
      if (!gia_regexp) validatore=tools.validatore(validatore);
      /* un altro modo èper determinare il numero di decimali
      e' quello di vedere nel validatore, trasformato in
      espressione rebolare estrapolando il 2° numero dalla
      porzione: (\\.\\d{0,2})?
      */
    }
    else
    {
      int p = snum.find(config.segno_decimale);
      if (p!=-1) num_decimali=snum.length()-(p-1);
      if (num_decimali<0) num_decimali=0;
    }
    snum.replace(config.segno_decimale, SEGNO );
    pannello ->display( snum );
    if (!validatore.isNull()) {
      validatore.replace("\\,","\\.");
      limiti.setRegExp(QRegExp(validatore));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_calcolatrice::display(double num)
{
  primo_tasto=true;
  snum=config.formatta(num,num_decimali);
  pannello->display(snum);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_calcolatrice::text()
{
  return config.formatta(value(), num_decimali);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_calcolatrice::tasto_clicked()
{
    accettato=rigettato=false;

    tr_label *l = (tr_label*)sender();

    QString st = l->text();

    int cod = st.toInt();
    if (st == "000") cod = ts_000;
    if (st == "00") cod = ts_00;
    if (st == "<-" ) cod = ts_back_space;
    if (st.find("az_exit") != -1 ) cod = ts_exit;
    if (st.find("az_ok") != -1 ) cod = ts_ok;
    if (st == ",") st = QChar(SEGNO);
    if (st == ".") st = QChar(SEGNO);
    if (st[0] == QChar(SEGNO)) cod = ts_segno;
    if (st == "+/-" ) cod = ts_cambia_segno;

    QString nsnum = snum;
    bool segno=false;
    if (nsnum[0]=='-') {segno=true;nsnum=nsnum.mid(1);}

    switch (cod) {

    case ts_segno:
        if (snum.find(SEGNO) != -1) BEEPER;
        snum += SEGNO;
        if (modalita == decimale_fisso) {
            uint i = num_decimali;
            while (i) {snum.append('0'); i--;}
        }
        pannello->display ( snum );
        primo_tasto = false;
        return;
        break;

    case ts_back_space:
        if (primo_tasto) nsnum="";
        else nsnum.truncate(nsnum.length()-1);
        if (num_decimali>0 && modalita==decimale_fisso) {
          nsnum.remove(SEGNO);
          while (nsnum.length()<=num_decimali) nsnum.prepend('0');
          if (nsnum.length()-num_decimali>0) nsnum.insert(nsnum.length()-num_decimali, SEGNO);
        }
        primo_tasto = false;
        break;

    case ts_000:
        if (primo_tasto) nsnum = "000"; else nsnum += "000";
        if (num_decimali>0 && modalita==decimale_fisso) {
          nsnum.remove(SEGNO);
          while (nsnum.length()<=num_decimali) nsnum.prepend('0');
          if (nsnum.length()-num_decimali>0) nsnum.insert(nsnum.length()-num_decimali, SEGNO);
        }
        primo_tasto = false;
        break;

    case ts_00:
        if (primo_tasto) nsnum = "00"; else nsnum += "00";
        if (num_decimali>0 && modalita==decimale_fisso) {
          nsnum.remove(SEGNO);
          while (nsnum.length()<=num_decimali) nsnum.prepend('0');
          if (nsnum.length()-num_decimali>0) nsnum.insert(nsnum.length()-num_decimali, SEGNO);
        }
        primo_tasto = false;
        break;

    case ts_cambia_segno:
        // if (nsnum[0] == '-') nsnum=nsnum.mid(1); else nsnum = "-" + nsnum;
        if (segno) segno=false; else nsnum.prepend('-');
        primo_tasto = false;
        break;

    case ts_ok:
        accettato=true;
        break;

    case ts_exit:
        rigettato=true;
        break;

    default:
        if (primo_tasto) nsnum = st; else nsnum += st;
        if (num_decimali>0 && modalita == decimale_fisso)
        {
                nsnum.remove(SEGNO);

                if ( nsnum.length()<num_decimali)
                            nsnum.prepend(SEGNO);
                    else
                        nsnum.insert(nsnum.length()-num_decimali, SEGNO);
        }

        primo_tasto = false;
    }

    if (!rigettato) {
      if (nsnum.left(2)!="0.") while (nsnum[0]=='0') nsnum=nsnum.mid(1);
      if (segno) nsnum.prepend('-');
      int pos=0;
      QString num_interno = config.num_interno(nsnum);
      if (nsnum.length()>0 && nsnum != "-" && limiti.validate( num_interno, pos) != QValidator::Acceptable) BEEPER;

      snum = nsnum;

      pannello->display( snum );
    }

    emit clicked();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_calcolatrice::wide()
{
  fattori->show();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_calcolatrice::fattori_clicked()
{
  snum=sender()->name();
  snum.truncate(snum.length()-1);
  pannello->display( snum.toDouble() );
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                         TR_COLORI
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TR_COLORI_CPP
#define TR_COLORI_CPP

#include "tr_colori.hpp"
#include "tr_dmacro.hpp"

#ifndef TR_CONFIG_CPP
extern tr_config config;
#include "tr_config.hpp"
#endif

////////////////////////////////////////////////////////////////////7

tr_colori::tr_colori(QWidget *parent, const char *name):
tr_widget(parent,name)
{

  QColor _colori[COLROWS][COLCOLS]= { {
    config.menu_elementi_colore, QColor( 255, 0, 0 ), QColor( 0, 255, 0 ), QColor( 0, 127, 0 ), QColor( 255, 127, 0 ),
         QColor( 210, 210, 210 )
    }, {
      QColor( 255, 255, 0 ), QColor( 200, 127, 100 ), QColor( 0, 255, 255 ), QColor( 255, 127, 127 ), QColor( 255, 0, 255 ),
           QColor( 127, 255, 127 )
    }
  };

  QGridLayout *layout;
  // il +1 è per l'etichetta
  NEW(layout, QGridLayout(this,COLROWS+1,COLCOLS,4,4) );

  NEW( label, tr_label( this, "label_colore" ) );
  label->setText( tr( "Colori" ) );
  label->setPaletteForegroundColor( config.colore_selezione );
  label->setPaletteBackgroundColor( paletteBackgroundColor() );
  palette_dark( label );

  layout->addMultiCellWidget( label, 0, 0 ,0,5 );

  FORJI(COLROWS,COLCOLS) {
    tr_label *colore;
    NEW( colore, tr_label( this ) );
    colori[j][i]=colore;
    colore->setMinimumSize( 54, 44 );
    colore->setText( "" );
    colore->setPaletteBackgroundColor( _colori[j][i] );
    layout->addWidget( colore, j+1, i );
    CONNECT(colore);
    palette_dark( colore->label );
  }

  selezionato=NULL;

}

////////////////////////////////////////////////////////////////////7

void tr_colori::select(QColor colore)
{
  FORJI(COLROWS,COLCOLS)
    if (colori[j][i]->paletteBackgroundColor()==colore) {
      colori[j][i]->aspetto_bottone(2);
      selezionato=colori[j][i];
    }
    else
      colori[j][i]->aspetto_bottone(0);
}

////////////////////////////////////////////////////////////////////7

QColor tr_colori::selected()
{
  return selezionato?selezionato->paletteBackgroundColor():QColor();
}

////////////////////////////////////////////////////////////////////7

void tr_colori::colore_clicked()
{
  tr_label*c=(tr_label*)sender();
  if (selezionato) selezionato->aspetto_bottone(0);
  selezionato=c;
  selezionato->aspetto_bottone(2);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                         TR_ICONVIEWWER
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TR_ICONVIEWER_CPP
#define TR_ICONVIEWER_CPP

#include "tr_iconviewer.hpp"
#include "tr_dmacro.hpp"

#ifndef TR_CONFIG_HPP
#include "tr_config.hpp"
extern tr_config config;
#endif

/////////////////////////////////////////////////////////////////////////7

tr_iconviewer::tr_iconviewer(QWidget *parent, const char* name,
QStringList _icone, int _nx, int _ny, int _dx, int _dy, int bordo)
:tr_widget(parent, name)
{
  ny=_ny;
  nx=_nx;
  if (_dx==-1) _dx=config.altezza_riga;
  if (_dy==-1) _dy=config.altezza_riga;
  dx=_dx-bordo*2;
  dy=_dy-bordo*2;
  elenco_icone=_icone;


  icone.resize(nx*ny-2);

  QGridLayout *layout;
  NEW( layout, QGridLayout(this,ny,nx,bordo,bordo) );

  page_base=0;

  FORUJI(ny,nx) {
    tr_label *icona;
    NEW(icona,tr_label(this));
    icona->setFixedSize( _dx, _dy );
    icona->setBackgroundOrigin( QLabel::ParentOrigin );
    icona->setBackgroundMode( Qt::NoBackground );
    icona->setAlignment( int( QLabel::AlignCenter ) );
    CONNECT(icona);

    if (j==(ny-1) && !i) prev=icona;
    else
    if (j*i==(ny-1)*(nx-1)) next=icona;
    else {
      if (page_base<elenco_icone.count() && !elenco_icone[page_base].isEmpty())
        icona->setText(
          ("<img src=\""+
          config.exe_path+"icone/" +
          elenco_icone[page_base] +
          "\" width=\""+QSN(dx)+"\" height=\""+QSN(dy)+"\">"));

      icone[page_base++]=icona;
    }

    layout->addWidget(icona,j,i);
  }

  page_base=0;

  prev->setPixmap( PIXMAP( "az_sx.png" ) );
  next->setPixmap( PIXMAP( "az_dx.png" ) );
  prev->ripetizione(true);
  next->ripetizione(true);
  prev->setPaletteBackgroundColor( prev->paletteBackgroundColor().dark(100));
  next->setPaletteBackgroundColor( next->paletteBackgroundColor().dark(100));

  selezionata=-1;

}

/////////////////////////////////////////////////////////////////////////7

void tr_iconviewer::visualizza_icone(int spostamento)
{
  int range=icone.count();

  if (spostamento<0 && page_base+spostamento<0)
    BEEPER;
  if (spostamento>0 && page_base+range-1+spostamento>=elenco_icone.count())
    BEEPER;

  QString sel=selected();
  select( sel );  // deseleziona

  if (spostamento<0 && page_base+spostamento>=0) page_base+=spostamento;

  if (spostamento>0 && page_base+range-1+spostamento<elenco_icone.count()) page_base+=spostamento;

  FORUI(range) {
      icone[i]->setText(
        ("<img src=\""+
        config.exe_path+"icone/" +
        elenco_icone[page_base+i] +
        "\" width=\""+QSN(dx)+"\" height=\""+QSN(dy)+"\">"));
  } // forji

  select( sel );
}

/////////////////////////////////////////////////////////////////////////7


void tr_iconviewer::select(QString icona)
{
  if (icona.isEmpty()) {selezionata=-1;return;}

  const int range=icone.count();

  int pos=-1;
  TROWSLI(elenco_icone) if (elenco_icone[i]==icona) {pos=i;break;}

  if (pos==-1) {selezionata=-1;return;}

  bool pos_dentro=false;
  if (pos>=page_base && pos<page_base+range) pos_dentro=true;

  bool sel_dentro=false;
  if (selezionata>=page_base && selezionata<page_base+range) sel_dentro=true;

  // deselezione
  if (sel_dentro) {
    icone[selezionata-page_base]->dark();
  }

  if (selezionata==pos) {
    selezionata=-1;
    return;
  }

  if (pos_dentro) {
    icone[pos-page_base]->light();
  }

  selezionata=pos;

}

/////////////////////////////////////////////////////////////////////////7

QString tr_iconviewer::selected()
{
  if (selezionata==-1) return "";
  else return elenco_icone[selezionata];
}

/////////////////////////////////////////////////////////////////////////7

void tr_iconviewer::visualizza(QString icona)
{
  if (icona.isEmpty()) return;

  const int range=icone.count();

  int pos=-1;
  TROWSLI(elenco_icone) if (elenco_icone[i]==icona) {pos=i;break;}

  if (pos==-1) return;

  bool pos_dentro=false;
  if (pos>=page_base && pos<page_base+range) pos_dentro=true;

  if (!pos_dentro) {
    page_base=pos-range+1;
    visualizza_icone();
  }

}

/////////////////////////////////////////////////////////////////////////7

void tr_iconviewer::icona_clicked()
{
  tr_label *icona=(tr_label*)sender();

  if (icona==prev) visualizza_icone(-1);
  else
  if (icona==next) visualizza_icone(+1);
  else

  FORUI(icone.count()) if (icone[i]==icona) {
    select(elenco_icone[page_base+i]);
    break;
  }

}

#endif
