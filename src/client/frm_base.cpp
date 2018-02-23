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

/* HISTORY: 01-03-05 finalmente corretto bug loop infinito su clic rapido in conto tra msgbox "coperti zero" e menu' stampa
04-08-05 parametro minmax, cambiato separatore , con | */

#include "frm_base.hpp"

#include <signal.h>
#include <qvariant.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qobjectlist.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qheader.h>
#include <qmime.h>
#include <qmovie.h>

#include "tr_dmacro.hpp"
#include "tr_dmacro2.hpp"
#include "tr_tools.hpp"
#include "tr_common.hpp"
#include "tr_globals.hpp"
#include "tr_comanda.hpp"
#include "tr_menu_dim.hpp"
#include "tr_menu.hpp"
#include "tr_canvas.hpp"

extern tr_tools tools;
extern tr_globals glb;

frm_base*frm_base::splash=NULL;

/* *  Constructs a frm_main as a child of 'parent', with the *  name 'name' and widget flags set to 'f'. *
*  The dialog will by default be modeless, unless you set 'modal' to *  TRUE to construct a modal dialog. */
frm_base::frm_base( QWidget*parent, const char*name, bool modal, WFlags fl ):QDialog( parent, name, modal, fl ) {

  widget_was_grabbing=NULL;

  hide_counter=0;

  if ( !parent ) {
    reparent( tools.find_right_base_parent( this ), fl, pos() );
    parent=( QWidget* ) this->parent();
  }
  if ( !parent ) cout<<"Parent non trovato per " <<this->name()<<endl;

  costruttore_comune( parent );

  if ( fl&Qt::WType_Popup ) {
    base_alza();
    nascondi_stato();
    nascondi_titolo();
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::costruttore_comune( QWidget*parent ) {
  palette=tools.palette( this );

  if ( !parent ) {
    parent=tools.find_right_base_parent();
    reparent( tools.find_right_base_parent( this ), MYFLAGS, QPoint( 0, 0 ) );
  }

  setBackgroundOrigin( QDialog::WindowOrigin );
  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  setMinimumSize( QSize( 0, 0 ) );
  setMaximumSize( QSize( config.finestra_larghezza, config.finestra_altezza ) );

  QFont f( font() );
  f.setPixelSize( config.finestra_font_size );
  setFont( f );

  int maxdx=config.finestra_larghezza-1;
  int maxdy=config.finestra_altezza-1;

  NEW( frm_layout, QGridLayout( this, 1, 1, 0, 0, "frm_layout" ) );

  NEW( tabs, tr_tab_widget( this, "tabs" ) );
  tabs->setMinimumSize( QSize( 0, 0 ) );
  tabs->setMaximumSize( QSize( maxdx, maxdy ) );
  tabs->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  tabs->setBackgroundOrigin( QFrame::WindowOrigin );
  frm_layout->addWidget( tabs, 0, 0 );


  NEW( base, QFrame( this, "base" ) );

  base->setMinimumSize( QSize( 0, 0 ) );
  base->setMaximumSize( QSize( maxdx, maxdy ) );
  base->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

  base->setBackgroundOrigin( QFrame::WindowOrigin );
  base->setFrameShape( QFrame::NoFrame );
  base->setFrameShadow( QFrame::Plain );
  base->setMidLineWidth( 0 );

  tabs->addWidget( base );

  NEW( barre, QFrame( base, "barre" ) );
  NEW( area, QFrame( base, "area" ) );

  NEW( pulsantiera, QFrame( base, "pulsantiera" ) );

  QSize s0( 0, 0 );
  QSize sx( maxdx, maxdy );
  base->setMinimumSize( s0 );
  base->setMaximumSize( sx );
  barre->setMinimumSize( s0 );
  barre->setMaximumSize( QSize( maxdx, config.finestra_altezza_titolo+config.finestra_altezza_stato ) );
  area->setMinimumSize( s0 );
  area->setMaximumSize( sx );

  barre->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
  tabs->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  area->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  pulsantiera->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );

  barre->setFrameShape( QFrame::NoFrame );
  area->setFrameShape( QFrame::NoFrame );
  pulsantiera->setFrameShape( QFrame::NoFrame );

  barre->setFrameShadow( QFrame::Plain );
  area->setFrameShadow( QFrame::Plain );
  pulsantiera->setFrameShadow( QFrame::Plain );

  NEW( base_layout, QVBoxLayout( base, 0, 0, "base_layout" ) );
  NEW( barre_layout, QVBoxLayout( barre, 0, 0, "barre_layout" ) );
  area_layout=NULL;
  NEW( pulsantiera_layout, QGridLayout( pulsantiera, 1, 1, 0, 0, "pulsantiera_layout" ) );

  area->setBackgroundOrigin( QLabel::ParentOrigin );

  base_layout->addWidget( barre, 0, Qt::AlignTop );
  base_layout->addWidget( area, 0 /* , Qt::AlignCenter */ );
  base_layout->addWidget( pulsantiera, 0, Qt::AlignBottom );


  NEW( barra_titolo, tr_barra_titolo( barre, "barra_titolo" ) );
  barre_layout->addWidget( barra_titolo );
  titolo=barra_titolo;

  NEW( barra_stato, tr_barra_stato( barre, "barra_stato" ) );
  barre_layout->addWidget( barra_stato );
  stato=barra_stato;

  pulsantiera->setMinimumSize( QSize( 0, 60 ) );
  pulsantiera->setMaximumSize( QSize( maxdx, maxdy ) );
  pulsantiera->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  pulsantiera->setBackgroundOrigin( QFrame::WindowOrigin );

  if ( !config.cursor ) setCursor( QCursor( Qt::BlankCursor ) );

  centered=true;

  // patch di Luca: Ctrl-S non si puo' usare !
  NEW( accel_sshot, QAccel( this ) ); // create accels for myWindow
  accel_sshot->connectItem( accel_sshot->insertItem( Key_X+CTRL ), // adds Ctrl+X accelerator
     this, SLOT( screen_shot() ) );

  NEW( accel_prnsshot, QAccel( this ) ); // create accels for myWindow
  accel_prnsshot->connectItem( accel_prnsshot->insertItem( Key_P+CTRL ), // adds Ctrl+P accelerator
     this, // connected to myWindow's
     SLOT( print_screen_shot() ) );


  for ( int i=0; i<max_links; i++ ) links[i].init();

  init();

  stato_form=2;
  iterato=NULL;
  iterando=false;
  exit_on_key_event=false;
  exit_on_custom_event=false;
  barcodectrls=QString::null;
  _barcode_enabled=false;

  events_ignore_cards=false;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::add_base() {
  int maxdx=config.finestra_larghezza-1;
  int maxdy=config.finestra_altezza-1;
  QSize s0( 0, 0 );
  QSize sx( maxdx, maxdy );
  NEW( base1, QFrame( this, "base1" ) );
  tabs->addWidget( base1 );
  base1->setMinimumSize( s0 );
  base1->setMaximumSize( sx );
  base1->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  base1->setBackgroundOrigin( QFrame::WindowOrigin );
  base1->setFrameShape( QFrame::NoFrame );
  base1->setFrameShadow( QFrame::Plain );
  base1->setMidLineWidth( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::init() {
  setPalette( palette );
  add_base();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* *  Destroys the object and frees any allocated resources */
frm_base::~frm_base() {

  iterato=NULL;
  if ( !isHidden() ) {
    tools.removeWin( this );
    QDialog::hide();
  }
  glb.menu_popup=NULL;

  if ( barcode_enabled() ) releaseKeyboard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int frm_base::exec() {
  int r=QDialog::exec();
  return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::base_alza() {
  int dim=2;
  if ( !config.pocket.isEmpty() ) dim=0;
  else {
    base->setFrameShape( QFrame::WinPanel );
    base->setFrameShadow( QFrame::Raised );
  }
  base->setMidLineWidth( dim );
  base->setMargin( dim );
  base_layout->setMargin( dim );

  QSize s( dim*3, dim*3 );
  if ( area ) area->setMaximumSize( area->maximumSize()-s );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::pulsantiera_alza() {
  pulsantiera->setFrameShape( QFrame::Panel );
  pulsantiera->setFrameShadow( QFrame::Raised );
  pulsantiera->setLineWidth( 2 );
  pulsantiera->setMargin( 2 );
  pulsantiera_layout->setMargin( 2 );
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::stato_pulsanti() {
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void frm_base::pulsanti( QString nomi, bool verticale, tr_label**ws[], TRDiritti::TRDirittiUtente diritti[], bool hide ) {

  int riga=0;

  QPixmap bp=QPixmap( PIXMAP( config.pulsante_barra ) );
  uint pulsanti_dx=bp.width();
  uint pulsanti_dy=bp.height();

  int posizione=0;

  QStringList etichette=QStringList::split( '|', nomi, true );

  TROWSLI( etichette ) {

    tr_label*p=NULL;
    QString nome=NZ( etichette[i] );
    QString etichetta=QString::null;
    QString icona;

    if ( nome.isEmpty() ) {
      posizione++;
      continue;
    }

    if ( nome.find( '#' )>1 ) {
      etichetta=nome.section( '#', 0, 0 );
      nome=nome.section( '#', 1, 1 );
      icona=tools.label_to_name( nome )+".png";
      if ( QMimeSourceFactory::defaultFactory()&&!QMimeSourceFactory::defaultFactory()->data( icona ) ) {
        icona="az_" +icona;
        if ( QMimeSourceFactory::defaultFactory()&&!QMimeSourceFactory::defaultFactory()->data( icona ) ) {
          icona=tools.label_to_name( etichetta );
          icona="az_" +icona+".png";
          if ( QMimeSourceFactory::defaultFactory()&&!QMimeSourceFactory::defaultFactory()->data( icona ) ) {
            icona=QString::null;
          }
        }
      }
    }
    else {
      etichetta=nome;
      icona=tools.label_to_name( etichetta );
      icona="az_" +icona+".png";
      if ( QMimeSourceFactory::defaultFactory()&&!QMimeSourceFactory::defaultFactory()->data( icona ) )
        icona=QString::null;
    }

    if ( etichetta.find( '\\' )!=-1 ) etichetta=etichetta.mid( etichetta.find( '\\' )+1 );

    NEW( p, tr_label( pulsantiera, nome ) );

    if ( ws&&p )*ws[posizione]=p;

    if ( hide ) p->hide();
    p->setMinimumSize( pulsanti_dx, pulsanti_dy );
    p->setMaximumSize( pulsanti_dx, pulsanti_dy );
    p->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
    p->setBackgroundOrigin( QLabel::WindowOrigin );
    p->setTextPixelSize( config.pulsanti_font_size );
    p->pulsante_animato();
    QString pulsante=palette.pulsante;
    if ( pulsante.isEmpty() ) pulsante=config.pulsante_menu;
    QColor c=p->aspetto_bottone( pulsante );

    QString text;
    if ( !icona.isEmpty() ) {
      etichetta=TR( etichetta );
      text=PTEXT( icona, etichetta );
    }
    else {
      text=TR( etichetta );
    }
    p->setText( text );

    _pointers.insert( nome, p );

    if ( diritti ) p->indice=diritti[i];

    if ( c!=pulsantiera->paletteBackgroundColor() )
      pulsantiera->setPaletteBackgroundColor( c );

    if ( ( getWFlags()&WType_Popup )==WType_Popup ) {
      connect( p, SIGNAL( clicked() ), this, SLOT( pulsante_clicked() ) );
    }

    if ( verticale ) pulsantiera_layout->addWidget( p, posizione, riga );
    else {
      int align;
      if ( etichette.count()==1 ) align=Qt::AlignCenter;
      else {
        if ( i==0 ) align=Qt::AlignLeft;
        if ( i==etichette.count()-1 ) align=Qt::AlignRight;
        if ( etichette.count()>2&&i>0&&i<etichette.count()-1 )
          align=Qt::AlignCenter;
      }
      pulsantiera_layout->addWidget( p, riga, posizione, align );
    }
    posizione++;
    if ( hide ) p->hide();
    palette_dark( p );

  }

  if ( !verticale )
    pulsantiera->setMinimumWidth( pulsanti_dx*etichette.count()+etichette.count()-1 );


  if ( ( getWFlags()&WType_Popup )==WType_Popup ) {

    centered=false;

    resize( sizeHint() );

    tr_label*pulsante=( tr_label* ) parent();
    if ( verticale ) {
      QPoint p( pulsante->mapToGlobal( QPoint( -base->margin()/2, -sizeHint().height() ) ) );
      move( p.x(), p.y() );
    }
    else {
      QPoint p( pulsante->mapToGlobal( QPoint( -sizeHint().width()-base->margin()/2, -base->margin()*2 ) ) );
      if ( p.x()<0 ) {
        p.setX( pulsante->mapToGlobal( QPoint( pulsante->width()+base->margin()/2, -base->margin()*2 ) ).x() );
      }
      move( p.x(), p.y() );
    }

    glb.menu_popup=this;

  }

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::iniz_area( int righe, int colonne, int margine, int spazio ) {
  NEW( area_layout, QGridLayout( area, righe, colonne, margine, spazio, "area_layout" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::center() {
  if ( MYFLAGS==WRUNFLG ) {
    QWidget*d=( QWidget* ) parent();
    if ( d ) {
      setGeometry( QRect( pos(), sizeHint() ) );
      QPoint p( d->geometry().center()-rect().center() );
      setGeometry( QRect( p, sizeHint() ) );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::hide() {


  hide_counter--;

  if ( !isHidden() ) {

    if ( iterato||isPopup() ) {
      exit_loop();
      iterando=false;
    }
    else {
      QDialog::hide();
    }

    if ( isHidden() ) {
      tools.removeWin( this );
    }

  }


};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::show() {

  hide_counter++;

  if ( isHidden() ) {
    if ( centered ) center();
  }

  int w=sizeHint().width();
  QDialog::show();

  if ( !isHidden() ) {

    QDictIterator<tr_widget>it( _pointers );
    for ( ; it.current(); ++it ) {
      tr_widget*w=( tr_widget* ) it.current();
      if ( qstrcmp( w->className(), "tr_listview" )==0 ) {
        tr_listview*lv=( tr_listview* ) w;
        lv->adatta();
        if ( !isHidden() ) QDialog::hide();
      }
    }

  }

  int ww=sizeHint().width();
  if ( ww!=w ) {
    center();
  }
  if ( isHidden() ) {
    QDialog::show();
  }

  if ( !isHidden() ) {
    tools.actWin( this );
  }

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::nascondi_stato() {
  barra_stato->hide();
  barre->setMaximumHeight( maximumHeight()-config.finestra_altezza_stato );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::scopri_stato() {
  barra_stato->show();
  barre->setMaximumHeight( maximumHeight()-config.finestra_altezza_stato );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::nascondi_titolo() {
  barra_titolo->hide();
  barre->setMaximumHeight( maximumHeight()-config.finestra_altezza_titolo );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::scopri_titolo() {
  barra_titolo->show();
  barre->setMaximumHeight( maximumHeight()-config.finestra_altezza_titolo );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::adatta_stato() {
  return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::build( QWidget*ws[], int margine, int spazio, bool dim_fissa ) {

  int n=0;
  while ( ws[n]!=0||ws[n+1]!=0 ) {
    n++;
  }

  int righe, colonne=0;
  int i=0;
  while ( ws[i++]!=0 ) colonne++;
  righe=1;
  while ( ws[i]!=0||ws[i+1]!=0 ) {
    if ( !ws[i] ) righe++;
    i++;
  }
  righe++;

  iniz_area( righe, colonne, margine, spazio );

  if ( dim_fissa ) area_layout->setResizeMode( QLayout::Fixed );
  colonne++;

  i=0;
  if ( margine ) {
    i=0;
    QSize s( margine*2, margine*2 );
    while ( ws[i]!=0||ws[i+1]!=0 ) {
      if ( ws[i]!=( QWidget* ) 0xffffffff&&ws[i]!=0 ) {
        if ( ws[i]->parent() ) {
          QWidget*w=( QWidget* ) ws[i]->parent();
          if ( ws[i]->maximumSize()>( w->maximumSize()-s ) ) {
            ws[i]->setMaximumSize( w->maximumSize()-s );
          }
        }
      }
      i++;
    }
  }

  i=0;
  while ( ws[i]!=0||ws[i+1]!=0 ) {
    int dx=1;
    if ( ws[i]!=( QWidget* ) 0xffffffff&&ws[i]!=0 ) {

      int j=i+1;
      while ( ws[j++]==ws[i] ) dx++;

      int dy=1;
      j=i+colonne;
      while ( j<n&&ws[j]==ws[i] ) {
        dy++;
        j+=colonne;
      }

      int x=( i%colonne );
      int y=( i/colonne );


      if ( dx==1&&dy==1 ) {
        if (y==righe-1)
          area_layout->addWidget( ws[i], y, x,Qt::AlignBottom );
        else area_layout->addWidget( ws[i], y, x );
      }
      else {
        if (y==righe-1)
          area_layout->addMultiCellWidget( ws[i], y, y+dy-1, x, x+dx-1,Qt::AlignBottom );
        else
          area_layout->addMultiCellWidget( ws[i], y, y+dy-1, x, x+dx-1 );
      }

      for ( j=0; j<dy; j++ )
        for ( int k=0; k<dx; k++ ) ws[i+j*colonne+k]=( QWidget* ) 0xffffffff;
    }

    i+=dx;

  }


}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::setMinMax( int min_x, int min_y, int max_x, int max_y ) {
  if ( max_x==-1 ) max_x=min_x;
  if ( max_y==-1 ) max_y=min_y;
  setMinimumSize( min_x, min_y );
  setMaximumSize( max_x, max_y );
  if ( base ) {
    base->setMaximumSize( max_x, max_y );
  }
  if ( area ) area->setMaximumSize( max_x, max_y );
  if ( pulsantiera ) area->setMaximumSize( max_x, pulsantiera->maximumHeight() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::scopri( tr_widget*w1, tr_widget*w2, tr_widget*w3, tr_widget*w4, tr_widget*w5, tr_widget*w6, tr_widget*w7,
   tr_widget*w8, tr_widget*w9, tr_widget*w10 ) {
     tr_widget*w[]= { w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, NULL };
     for ( int i=0; w[i]!=NULL; i++ ) {
       if ( w[i]!=( tr_widget* )-1 ) w[i]->show();
     }
}

void frm_base::nascondi( tr_widget*w1, tr_widget*w2, tr_widget*w3, tr_widget*w4, tr_widget*w5, tr_widget*w6, tr_widget*w7,
   tr_widget*w8, tr_widget*w9, tr_widget*w10 ) {
     tr_widget*w[]= { w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, NULL };
     for ( int i=0; w[i]!=NULL; i++ ) {
       if ( w[i]!=( tr_widget* )-1 ) w[i]->hide();
     }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::abilita( tr_widget*w1, tr_widget*w2, tr_widget*w3, tr_widget*w4, tr_widget*w5, tr_widget*w6, tr_widget*w7,
   tr_widget*w8, tr_widget*w9, tr_widget*w10, tr_widget*w11 ) {
     tr_widget*w[]= { w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, w11, NULL };
     abilita( w );
}

void frm_base::abilita( tr_widget*w[], bool ) {
  for ( int i=0; w[i]!=NULL; i++ ) {
    if ( w[i]!=( tr_widget* )-1 ) {
      w[i]->setEnabled( true );
      if ( link_disabilitati.find( w[i] ) )
        link_disabilitati.remove( w[i] );
    }
  }
}


void frm_base::disabilita( tr_widget*w1, tr_widget*w2, tr_widget*w3, tr_widget*w4, tr_widget*w5, tr_widget*w6, tr_widget*w7,
   tr_widget*w8, tr_widget*w9, tr_widget*w10, tr_widget*w11 ) {
     tr_widget*w[]= { w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, w11, NULL };
     disabilita( w );
}

void frm_base::disabilita( tr_widget*w[], bool ) {
  for ( int i=0; w[i]!=NULL; i++ ) {
    if ( w[i]!=( tr_widget* )-1 ) {
      w[i]->setEnabled( false );
      if ( !link_disabilitati.find( w[i] ) )
        link_disabilitati.insert( w[i], w[i] );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::setPaletteForegroundColor( const QColor&c ) {
  QDialog::setPaletteForegroundColor( c );
  base->setPaletteForegroundColor( c );
  area->setPaletteForegroundColor( c );
}

void frm_base::setPaletteBackgroundColor( const QColor&c ) {
  QDialog::setPaletteBackgroundColor( c );
  base->setPaletteBackgroundColor( c );
  area->setPaletteBackgroundColor( c );
}

void frm_base::setPaletteBackgroundPixmap( const QPixmap&c ) {
  QDialog::setPaletteBackgroundPixmap( c );
  base->setPaletteBackgroundPixmap( c );
  area->setPaletteBackgroundPixmap( c );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_label*frm_base::vuoto( int dx, int dy ) {
  tr_label*p;
  NEW( p, tr_label( area ) );
  p->setPaletteBackgroundColor( area->paletteBackgroundColor() );
  if ( dx!=-1&&dy==-1 ) {
    p->setMinimumSize( dx, config.altezza_riga );
    p->setMaximumWidth( dx );
    p->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding ) );
  }
  if ( dx!=-1&&dy!=-1 ) p->setMinMax( dx, dy );
  return p;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::link( tr_input*input, QWidget*ws[] ) {
  int i=0;
  while ( !links[i].last() ) i++;

  links[i].input=input;
  input->link( ws );
  connect( input, SIGNAL( textChanged( const QString& ) ), this, SLOT( link_textChanged( const QString& ) ) );
  links_clicked();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::link( tr_label*check_box, QWidget*ws[], tr_label::tr_stato attiva_su ) {
  int i=0;
  while ( !links[i].last() ) i++;

  links[i].check_box=check_box;
  check_box->link( ws, attiva_su );
  connect( check_box, SIGNAL( clicked() ), this, SLOT( link_clicked() ) );
  links_clicked();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::link( tr_listview*lv, QWidget*ws[] ) {
  int i=0;
  while ( !links[i].last() ) i++;

  links[i].listview=lv;
  lv->link( ws );
  connect( lv, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( link_clicked( QListViewItem* ) ) );
  links_clicked();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::links_clicked() {
  int i=0;
  tr_widget*ok=( tr_widget* ) child( "ok" );

  while ( !links[i].last() ) {
    tr_widget*w;
    if ( links[i].input!=NULL ) w=( tr_widget* ) links[i].input;

    if ( links[i].check_box!=NULL ) w=( tr_widget* ) links[i].check_box;

    if ( links[i].listview!=NULL ) w=( tr_widget* ) links[i].listview;

    if ( !link_disabilitati.find( w ) ) {
      w->setEnabled( true );
      w->show();
    }

    for ( int j=0; j<w->n_links; j++ ) {
      if ( !link_disabilitati.find( w ) ) {
        w->links[j].w->setEnabled( true );
        w->links[j].w->show();
      }
    }


    i++;
  }

  i=0;
  bool stato_ok=false;
  while ( !links[i].last() ) {

    bool stato=false;

    if ( links[i].input!=NULL ) {
      tr_input*input=links[i].input;
      stato=!input->text().isEmpty();
      if ( !stato_ok ) stato_ok=stato;
      input->link_clicked( stato );
    }

    if ( links[i].check_box!=NULL ) {
      ok=NULL;
      tr_label*chk=links[i].check_box;
      chk->link_clicked( chk->isOn() );
    }


    if ( links[i].listview!=NULL ) {
      tr_listview*lv=links[i].listview;
      stato=lv->selezionati().count();
      if ( !stato_ok ) stato_ok=stato;
      lv->link_clicked( stato );
    }

    i++;
  } // while

  if ( ok ) {
    if ( !stato_ok ) nascondi( ok );
    else
      scopri( ok );
  }

  QPtrDictIterator<tr_widget>it( link_disabilitati );
  for ( ; it.current(); ++it ) {
    it.current()->setEnabled( false );
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::link_textChanged( const QString& ) {
  links_clicked();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::link_clicked( QListViewItem* ) {
  links_clicked();
}

void frm_base::link_clicked() {
  links_clicked();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::timers() {
  iterato=( tr_widget* ) this;
  exit_loop();
  iterando=false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::barcode() {
  focusNextPrevChild( true );
  if ( qstrcmp( qApp->focusWidget()->className(), "tr_lineedit" )==0 ) {
    tr_lineedit*i=( tr_lineedit* ) qApp->focusWidget();
    i->selectAll();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::screen_shot() {
  static int numero=0;
  // QPixmap p=QPixmap::grabWidget(QApplication::desktop()->screen( 0 )
  QString file=config.exe_path+"sshot" +QSN( numero++ )+".png";
  // "xwd -frame -root -screen -out "	;
  QProcess proc;
  proc.addArgument( "/usr/bin/X11/xwd" );
  proc.addArgument( "-frame" );
  proc.addArgument( "-root" );
  proc.addArgument( "-screen" );
  proc.addArgument( "-out" );
  proc.addArgument( file );

  if ( proc.start() ) {
    qDebug( QString( "Immagine " +file+" prodotta" ).latin1() );
    while ( proc.isRunning() );
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::print_screen_shot() {
  QPrinter printer( QPrinter::PrinterResolution );
  printer.setFullPage( TRUE );
  if ( printer.setup( this ) ) { // printer dialog
    QPainter p;
    if ( !p.begin( &printer ) ) { // paint on printer
      tools.msgbox( "Stampa abortita" );
      return;
    }

    QPixmap pixmap=QPixmap::grabWindow( qApp->mainWidget()->winId() );
    p.drawPixmap( 0, 0, pixmap );
    printer.newPage();
    tools.msgbox( "Stampa completata" );

  }
  else {
    tools.msgbox( "Stampa abortita" );
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::customEvent( QCustomEvent*e ) {
  QDialog::customEvent( e );
  if ( exit_on_custom_event!=false ) {
    iterato=( tr_widget* ) this;
    exit_loop();
    iterando=false;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool frm_base::eventFilter( QObject*o, QEvent*e ) {
  QString st=o->name();
  st+=":";
  st+=tr_event( e ).name();
  qDebug( st.latin1() );
  return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::accept() {
  stato_form=QDialog::Accepted;
  if ( iterato||isPopup() ) {
    exit_loop();
    iterando=false;
  }
  else
    return QDialog::accept();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::reject() {
  stato_form=QDialog::Rejected;
  if ( iterato||isPopup() ) {
    exit_loop();
    iterando=false;
  }
  else
    return QDialog::reject();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool frm_base::accepted() {
  if ( stato_form!=QDialog::Accepted&&stato_form!=QDialog::Rejected )
    return ( exec()==QDialog::Accepted )?true:false;
  else
    return stato_form;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::setPalette( tr_palette p ) {

  palette=p;

  setPaletteBackgroundColor( p.area.bg );
  setPaletteForegroundColor( p.area.fg );
  barra_titolo->setPaletteBackgroundPixmap( PIXMAP( p.titolo.bm ) );
  barra_titolo->setPaletteForegroundColor( p.titolo.fg );

  barra_stato->setPaletteForegroundColor( p.stato.fg );
  if ( !p.stato.bm.isEmpty() ) barra_stato->setPaletteBackgroundPixmap( PIXMAP( p.stato.bm ) );
  else {
    barra_stato->setPaletteBackgroundColor( p.stato.bg );
  }

  QColor c;
  QDictIterator<tr_widget>it( _pointers ); // See QDictIterator
  for ( ; it.current(); ++it ) {
    tr_widget*w=it.current();
    if ( qstrcmp( w->className(), "tr_label" )==0 ) {
      tr_label*l=( tr_label* ) w;
      if ( l->aspetto()==tr_label::pulsante&&!p.pulsante.isEmpty() ) {
        c=l->aspetto_bottone( p.pulsante );
      }
    }
  } // for
  if ( !c.isValid() ) pulsantiera->setPaletteBackgroundColor( c );

}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SPLIT(sep,string) QStringList::split(sep,string,true)

//////////////////////////////////////////////
//           MODO INTERATTIVO               //
//////////////////////////////////////////////


class tr_build_data {

  enum tr_tokens_id {
    tid_barcodectrls, tid_events, tid_events_ignore, tid_events_timers, tid_readonly, tid_commento, tid_palette, tid_spazio,
       tid_margine, tid_title, tid_status, tid_status_notr, tid_valtab, tid_links, tid_buttons, tid_minmax,
       tid_remove, tid_conds, tid_extra,

       tid_input, tid_label, tid_listview, tid_hole, tid_last=99
  };


  friend class frm_base;

public:
  void build( frm_base*f );
  bool crea_widget( frm_base*f );
  QString token;
  tr_tokens_id token_id;
  QString nome;
  tr_stringlist parametri;
  QString dato;
  tr_widget*widget;
  int margine;
  int spazio;
  bool builded;
  int indice;
  uint dim;
  QWidget*ws[100];
  bool readonly;
  tr_widget*hided[100];
  QStringList conds;
  QStringList esclusi;
  QString valtab; // nome tabella base per validatore
  QStringList params;
  QString tr( QString );
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline QString tr_build_data::tr( QString txt ) {
  txt=TR( txt );
  return txt;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void tr_build_data::build( frm_base*f ) {
  if ( !builded ) {
    ws[indice]=NULL;
    if ( margine!=-1&&spazio!=-1 )
      f->build( ws, margine, spazio );
    else if ( margine!=-1&&spazio==-1 )
      f->build( ws, margine, spazio );
    else
      f->build( ws );
    builded=true;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_build_data::crea_widget( frm_base*f ) {

#ifdef dbg_tokens
  QTime t;
  t.start();
#endif

  if ( nome.isEmpty() ) nome=token;

  tr_widget*widgetjustexist=f->_pointers[nome];
  if ( token_id>=tid_input&&widgetjustexist ) {
    widget=widgetjustexist;
    return true;
  }

  switch ( token_id ) {

    case tid_input: {

        QString dato_testo;

        tr_input*input=NULL;
        int altezza=-1;
        bool alato=false;
        bool barcode=false;
        QString validatore=QString::null;
        bool onkey=false;
        bool readonly=false;
        if ( this->readonly ) readonly=true;
        QString tok;
        int nkeys=2;
        tr_listview::tr_tipo tp=tr_listview::indefinito;
        int min=-1;
        bool oneline=false;
        bool onclick=false;
        bool onupdate=false;
        bool right=false;
        bool disabled=false;
        QString dim;

        enum {
          on, off, no
        }
        chk=no;

        QStringList post_params;

        TROWSLX( parametri ) {
          QString param=tr( parametri[x] );
          if ( param.find( '.' )!=-1 ) {
            if ( param[0]=='.' ) {
              validatore=tools.validatore( valtab+param );
            }
            else
              validatore=tools.validatore( param );
          }
          if ( param.left( 4 )=="dim:" )
            dim=param.mid( 4 );
          if ( param=="right" ) right=true;
          if ( param=="disabled" ) disabled=true;
          if ( param=="alato" ) alato=true;
          if ( param=="barcode" ) barcode=true;
          if ( param=="today" ) dato_testo=tools.data();
          if ( param=="readonly" ) readonly=true;
          if ( param=="onclick" ) onclick=true;
          if ( param=="onupdate" ) onupdate=true;
          if ( param=="ggmmaaaa" ) validatore="date";
          if ( param=="hhmm" ) validatore="time";
          if ( param=="onkey" ) onkey=true;
          if ( param.left( 4 )=="tok:" ) tok=param;
          if ( param=="chk:on" ) chk=on;
          if ( param=="chk:off" ) chk=off;
          if ( param.left( 2 )=="##" ) dato_testo=param;
          if ( param.left( 3 )=="tp:" )
            tp=( tr_listview::tr_tipo ) param.section( ':', 1, 1 ).toInt();
          if ( param.left( 2 )=="h:" )
            altezza=param.section( ':', 1, 1 ).toInt()*config.altezza_riga;
          if ( param.left( 4 )=="val:" )
            validatore=param.mid( 4 );
          if ( param.left( 4 )=="min:" )
            min=param.section( ':', 1, 1 ).toInt();
          if ( param=="oneline" ) oneline=true;
          if ( param.left( 4 )=="lbg:" ) post_params<<param;
          if ( param.left( 4 )=="lfg:" ) post_params<<param;
          if ( param.left( 3 )=="bg:" ) post_params<<param;
          if ( param.left( 3 )=="fg:" ) post_params<<param;
        } // TROWSLX

        NEW( input, tr_input( f->area, nome, tr( dato ), dato_testo, validatore, alato ) );

        if ( !dim.isEmpty() ) {
          QStringList dims=QStringList::split( '|', dim, true );
          TROWSLI( dims )
          if ( !dims[i].isEmpty()&&dims[i].toInt()<config.altezza_riga/2 ) dims[i]=QSN( dims[i].toInt()*config.altezza_riga );

          if ( dims.count()>0&&!dims[0].isEmpty() )
            input->setMinimumWidth( dims[0].toInt() );
          if ( dims.count()>1&&!dims[1].isEmpty() )
            input->setMinimumHeight( dims[1].toInt() );
          if ( dims.count()>2&&!dims[2].isEmpty() ) {
            input->setMaximumWidth( dims[2].toInt() );
            input->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding ) );
          }
          if ( dims.count()>3&&!dims[3].isEmpty() )
            input->setMaximumHeight( dims[3].toInt() );
        }
        if ( tp!=tr_listview::indefinito )
          input->tp( tp );
        if ( readonly ) input->edit->setReadOnly( true );
        if ( altezza>-1 ) input->setMaximumHeight( altezza );
        if ( min>-1 ) input->edit->setMinimumWidth( min );
        if ( oneline ) input->label->oneline( true );
        if ( !tok.isEmpty() ) {
          TRTokens t=( TRTokens )tok.section( ':', 1, 1 ).toInt();
          if ( tok.contains( ':' )>1 ) nkeys=tok.section( ':', 2, 2 ).toInt();
          input->tok( t, QString::null, nkeys );
        }
        if ( readonly ) input->setReadOnly( true );
        if ( right ) input->edit->setAlignment( Qt::AlignRight );
        if ( onupdate ) {
          input->onupdate=true;
          f->connect( input, SIGNAL( textUpdated() ), f, SLOT( pulsante_clicked() ) );
        }
        if ( onclick ) {
          f->connect( input, SIGNAL( textChanged( const QString& ) ), f, SLOT( pulsante_clicked() ) );
        }
        if ( onkey&&!barcode ) {
          f->connect( input, SIGNAL( textChanged( const QString& ) ), f, SLOT( edit_textChanged( const QString& ) ) );
        }
        if ( barcode ) {
          input->barcode=barcode;
          f->connect( input, SIGNAL( barcodeReaded() ), f, SLOT( barcode() ) );
        }
        input->onkey=onkey;
        if ( chk==on ) input->setCheck( true );
        if ( chk==off ) input->setCheck( false );

        TROWSLX( post_params ) {
          QString param=tr( post_params[x] );
          if ( param.left( 3 )=="bg:" ) {
            if ( param.mid( 3 )=="light" )
              input->edit->setPaletteBackgroundColor( input->edit->paletteBackgroundColor().light() );
            else
              input->edit->setPaletteBackgroundColor( QColor( param.mid( 3 ) ) );
          }
          if ( param.left( 3 )=="fg:" ) {
            if ( param.mid( 3 )=="light" )
              input->edit->setPaletteForegroundColor( input->edit->paletteBackgroundColor().light() );
            else
              input->edit->setPaletteForegroundColor( QColor( param.mid( 3 ) ) );
          }
          if ( param.left( 4 )=="lbg:" ) {
            if ( param.mid( 4 )=="light" )
              input->label->setPaletteBackgroundColor( input->label->paletteBackgroundColor().light() );
            else
              input->label->setPaletteBackgroundColor( QColor( param.mid( 3 ) ) );
          }
          if ( param.left( 4 )=="lfg:" ) {
            if ( param.mid( 4 )=="light" )
              input->label->setPaletteForegroundColor( input->label->paletteBackgroundColor().light() );
            else
              input->label->setPaletteForegroundColor( QColor( param.mid( 3 ) ) );
          }

          if ( disabled ) input->setEnabled( false );

        } // trowslx


        widget=input;
        f->_pointers.insert( nome, widget );
      } // case input
    break;

    case tid_label: {
        bool oneline=false;
        bool notr=false;
        bool radio=false;
        tr_label*label;
        NEW( label, tr_label( f->area, nome ) );
        TROWSLX( parametri ) {
          QString param=parametri[x];
          if ( param=="notr" ) notr=true;
          if ( param.left( 4 )=="dim:" ) {
            QStringList dim=QStringList::split( '|', param.mid( 4 ), true );
            TROWSLI( dim )
            if ( !dim[i].isEmpty()&&dim[i].toInt()<config.altezza_riga/2 ) dim[i]=QSN( dim[i].toInt()*config.altezza_riga );
            if ( dim.count()>0&&!dim[0].isEmpty() ) label->setMinimumWidth( dim[0].toInt() );
            if ( dim.count()>1&&!dim[1].isEmpty() ) label->setMinimumHeight( dim[1].toInt() );
            if ( dim.count()>2&&!dim[2].isEmpty() ) label->setMaximumWidth( dim[2].toInt() );
            if ( dim.count()>3&&!dim[3].isEmpty() ) label->setMaximumHeight( dim[3].toInt() );
          }
          if ( param=="oneline" ) oneline=true;
          if ( param=="onclick" ) {
            f->connect( label, SIGNAL( clicked() ), f, SLOT( pulsante_clicked() ) );
          }
          if ( param=="bw" ) {
            label->setPaletteBackgroundColor( Qt::white );
            label->setPaletteForegroundColor( Qt::black );
          }
          if ( param=="chk" ) label->aspetto_check( false );
          if ( param=="cc" ) label->set_color_cycling();
          if ( param=="chk:off" ) label->aspetto_check( false );
          if ( param=="chk:on" ) label->aspetto_check( true );
          if ( param.left( 3 )=="bg:" ) {
            if ( param.mid( 3 )=="light" ) label->setPaletteBackgroundColor( label->paletteBackgroundColor().light() );
            else
              label->setPaletteBackgroundColor( QColor( param.mid( 3 ) ) );
          }
          if ( param.left( 3 )=="fg:" ) {
            if ( param.mid( 3 )=="light" ) label->setPaletteForegroundColor( label->paletteBackgroundColor().light() );
            else
              label->setPaletteForegroundColor( QColor( param.mid( 3 ) ) );
          }
          if ( param.left( 6 )=="radio:" ) {
            QStringList c=QStringList::split( '|', param.mid( 6 ) );
            label->aspetto_radio( c );
            radio=true;
          }
          if ( param.left( 2 )=="n:" ) {
            label->number( param.mid( 2 ).toInt() );
          }
          if ( param.left( 7 )=="pixmap:" ) {
            label->setPixmap( PIXMAP( param.mid( 7 ) ) );
          }
          if ( param.left( 6 )=="movie:" ) {
            QString name=config.exe_path+"icone/" +param.mid( 6 );
            QMovie movie( name );
            if ( !movie.isNull() ) {
              label->label->setMovie( movie );
            }
          } // movie
          if ( param=="right" ) label->setAlignment( label->alignment()|Qt::AlignRight );
        } // TROWSLX
        if ( oneline ) label->oneline( true );
        if ( !radio ) {
          if ( notr ) label->setText( dato );
          else
            label->setText( tr( dato ) );
        }

        widget=label;
        f->_pointers.insert( nome, widget );
      }
    break;

    case tid_listview: {
        tr_listview*lv;
        NEW( lv, tr_listview( f->area, nome ) );
        lv->nascondi( tr_listview::e_abc );
        lv->scopri( tr_listview::e_exit );
        f->connect( lv->exit, SIGNAL( clicked() ), f, SLOT( pulsante_clicked() ) );

        bool exit=true;
        bool onclick=false;
        TROWSLX( parametri ) {
          QString param=parametri[x];

          if ( param=="noexit" ) {
            lv->nascondi( tr_listview::e_exit );
            exit=false;
          }
          if ( param=="dxsx" ) {
            lv->scopri( tr_listview::e_dxsx );
          }
          if ( param=="onclick" ) {
            onclick=true;
          }
          if ( param=="sortable" ) {
            QHeader*h=lv->elenco->header();
            if ( h ) {
              if ( lv->connect( h, SIGNAL( clicked( int ) ), lv, SLOT( header_clicked( int ) ) ) )
                lv->elenco->header()->setClickEnabled( true );
            }
          }
          if ( param=="rollone" ) {
            lv->rollone=true;
          }
          if ( param=="nobar" ) lv->nascondi( tr_listview::e_barra );
          if ( param=="small" ) {
            QFont f=lv->elenco->font();
            f.setPixelSize( 18 );
            lv->elenco->setFont( f );
            lv->altezza_riga( 24 );
          }
          if ( param=="abc" ) {
            lv->scopri( tr_listview::e_abc );
          }
          if ( param=="+-" ) {
            lv->scopri( tr_listview::e_moreless );
            f->connect( lv->more, SIGNAL( clicked() ), f, SLOT( pulsante_clicked() ) );
            f->connect( lv->less, SIGNAL( clicked() ), f, SLOT( pulsante_clicked() ) );
          }
          if ( param.left( 6 )=="header" ) {
            // lv->scopri( tr_listview::e_header );
            if ( param.find( ':' )!=-1 ) {
              lv->dati.header=param.section( ':', 1, 1 );
            }
          }
          if ( param=="tutti" || param=="all" ) lv->selezionati( true );
          if ( param=="nessuno" || param=="nobody" ) lv->selezionati( false );
          if ( param[0]=='%' ) {
            lv->setFixedWidth( ( int )( f->width()/100.0*param.mid( 1 ).toInt() ) );
          }
          if ( param.left( 5 )=="minr:" )
            lv->setMinimumHeight( param.mid( 5 ).toInt()*config.altezza_riga );
          if ( param.left( 4 )=="dim:" ) {
            QStringList dim=QStringList::split( '|', param.mid( 4 ), true );
            TROWSLI( dim )
            if ( !dim[i].isEmpty()&&dim[i].toInt()<config.altezza_riga/2 ) dim[i]=QSN( dim[i].toInt()*config.altezza_riga );
            if ( dim.count()>0&&!dim[0].isEmpty() ) lv->setMinimumWidth( dim[0].toInt() );
            if ( dim.count()>1&&!dim[1].isEmpty() ) lv->setMinimumHeight( dim[1].toInt() );
            if ( dim.count()>2&&!dim[2].isEmpty() ) lv->setMaximumWidth( dim[2].toInt() );
            if ( dim.count()>3&&!dim[3].isEmpty() ) lv->setMaximumHeight( dim[3].toInt() );
          }
          if ( param.left( 5 )=="sort:" ) {
            QString col=param.section( ':', 1, 1 );
            lv->elenco->setSortColumn( col.toInt() );
          }
          if ( param.left( 3 )=="hl:" ) {
            lv->highlights<<param.section( ':', 1, 1 );
          }
          if ( param.left( 3 )=="tp:" ) {
            bool ok;
            int valore=param.section( ':', 1, 1 ).section( '|', 0, 0 ).toInt( &ok );
            if ( ok ) {
              int p=param.find( '|' );
              if ( p==-1 ) {
                lv->tipo( ( tr_listview::tr_tipo ) valore );
                lv->carica();
              }
            }
          } // tp:
          if ( param.left( 4 )=="tok:" ) {
            bool ok;
            int valore=param.section( ':', 1, 1 ).section( '|', 0, 0 ).toInt( &ok );
            if ( ok ) {
              int p=param.find( '|' );
              if ( p==-1 ) lv->tipo_elenco( ( TRTokens )valore );
              else {
                QString par=param.mid( p+1 );
                lv->tipo_elenco( ( TRTokens )valore, par );
              } // else
            } // if ok
          } // if tok:
        } // TROWSLX

        if ( /* exit|| */ onclick )
          f->connect( lv, SIGNAL( clicked( QListViewItem* ) ), f, SLOT( pulsante_clicked( QListViewItem* ) ) );

        widget=lv;
        f->_pointers.insert( nome, widget );
      }
    break;

    case tid_title:
      f->scopri_titolo();
      f->titolo->setText( tr( dato ) );
    break;

    case tid_valtab:
      valtab=dato;
    break;

    case tid_status:
      f->scopri_stato();
      f->stato->tipo( tr_barra_stato::titolo );
      f->stato->setText( tr( dato ) );
    break;

    case tid_status_notr:
      f->scopri_stato();
      f->stato->tipo( tr_barra_stato::titolo );
      f->stato->setText( dato );
    break;

    case tid_hole:
      if ( parametri.count() )
        widget=f->vuoto( parametri[0].remove( '|' ).toInt() );
      else
        widget=f->vuoto();
    break;

    case tid_extra: {
        if ( dato=="canvas" ) {
          tr_canvas*canvas=new tr_canvas( ( tr_widget* ) f->area, nome );
          widget=canvas;
        }
        if ( dato=="comanda" ) {
          tr_comanda*comanda=new tr_comanda( f->area, nome );
          TROWSLI( parametri ) {
            QString param=parametri[i];
            if ( param=="ffm" ) comanda->modalita( tr_comanda::m_ffm );
          } // TROWSLI

          widget=comanda;
        }
        if ( dato=="menu_old" ) {
          tr_menu*menu=new tr_menu( f->area, nome );
          widget=menu;
        }
        if ( dato=="menu" ) {
          int dx=0;
          int dy=0;
          tr_menu_dim*menu=new tr_menu_dim( f->area, nome );
          TROWSLI( parametri ) {
            if ( parametri[i].left( 3 )=="sp:" )
              menu->spessore=parametri[i].section( ':', 1, 1 ).toInt();
            if ( parametri[i].left( 6 )=="bsize:" ) {
              QStringList e=QStringList::split( '|', parametri[i].section( ':', 1, 1 ) );
              menu->bsize=QSize( e[0].toInt(), e[1].toInt() );
            }
            if ( parametri[i].left( 5 )=="size:" ) {
              QStringList e=QStringList::split( '|', parametri[i].section( ':', 1, 1 ) );

              if ( e.count()>2 )
                menu->setMinMax( e[0].toInt(), e[1].toInt(), e[2].toInt(), e[3].toInt() );
              else
                menu->setMinMax( e[0].toInt(), e[1].toInt() );
            }
            if ( parametri[i].left( 3 )=="rc:" ) {
              QString str="";
              QStringList sz=QStringList::split( '|', parametri[i].mid( 3 ) );
              dy=sz[0].toInt();
              if ( sz.count()>1 ) dx=sz[1].toInt();
              else
                dx=dy;
              FORJ( dy )FORK( dx ) {
                if ( k==0||k==dx-1||j==0||j==dy-1 ) { // corolla
                  if ( k==dx-1&&j==0 ) str+="2";
                  else if ( k==dx-1&&j==dy-1 ) str+="3";
                  else
                    str+="1";
                  if ( k==dx-1&&j!=dy-1 ) str+="\n";
                }
                else
                  str+="i";
              }
              tr_menu_dim*inside=menu->build( str );
              if ( inside ) { // costuisce percorso
                str="";
                FORK( dx-2 ) str+='5';
                str+="\n";
                FORK( dx-2 ) str+='i';
                inside->build( str );
              }
            } // if parametri
          } // TROWSLI

          widget=( tr_widget* ) menu;
        }
        f->_pointers.insert( nome, widget );
      }
    break;

    case tid_buttons: {
        tr_label_array puls;
        tr_stringlist etichette=SPLIT( "|", dato );
        QString nuovo_dato;
        if ( dato.find( '?' )!=-1 ) {
          int i=0;
          TROWSLI( etichette ) {
            if ( etichette[i] [0]=='?' ) {
              int p=etichette[i].find( ':' );
              if ( conds.findIndex( etichette[i].mid( 1, p-1 ) )>=0 ) {
                if ( nuovo_dato.length() ) nuovo_dato+='|';
                nuovo_dato+=etichette[i].mid( p+1 );
              }
              else {
                int pn=etichette[i].find( '#' );
                if ( pn==-1 ) esclusi<<etichette[i].mid( p+1 );
                else
                  esclusi<<etichette[i].mid( pn+1 );
              }
            }
            else {
              if ( nuovo_dato.length() ) nuovo_dato+='|';
              nuovo_dato+=etichette[i];
            }
          } // while
          etichette=SPLIT( "|", nuovo_dato );
        }
        else
          nuovo_dato=dato;

        f->pulsanti( nuovo_dato, false, NULL, NULL );
        TROWSLI( etichette ) {
          QString nome;
          if ( etichette[i].find( '#' )>1 ) nome=etichette[i].section( '#', 1, 1 );
          else
            nome=etichette[i];
          tr_label*l=f->lbs( nome );
          f->connect( l, SIGNAL( clicked() ), f, SLOT( pulsante_clicked() ) );
        }
        f->scopri_pulsantiera();
      }
    break;

    case tid_links: {
        build( f );
        tr_stringlist nomi=SPLIT( '|', dato );
        bool abortire=false;
        int x1=0;
        QStringList parametri;
        TROWSLX( nomi ) {
          int p=nomi[x].find( '(' );
          if ( p==-1&&esclusi.findIndex( nomi[x] )!=-1 )
            continue;
          if ( p!=-1&&esclusi.findIndex( nomi[x].left( p ) )!=-1 )
            continue;
          if ( p==-1 )
            ws[x1]=f->pointer( nomi[x] );
          else {
            ws[x1]=f->pointer( nomi[x].left( p ) );
            if ( ws[x1] ) {
              QString parametro=nomi[x].mid( p+1, nomi[x].find( ')', p )-p-1 );
              parametri<<parametro;
              if ( parametro=="clear" &&qstrcmp( ws[x1]->className(), "tr_input" )==0 ) {
                tr_input*lb=( tr_input* ) ws[x1];
                lb->clear_on_link=true;
              }
            }
          }
          if ( !ws[x1] ) {
            LOG( TR( "LINKS:non ho trovato il puntatore per " )+nomi[x] );
            if ( !x1 ) abortire=true; // manca origine
            // break;
          }
          else
            x1++;
          ws[x1+1]=NULL;
        } // TROWSLX
        if ( !abortire&&nomi.count() ) {
          if ( qstrcmp( ws[0]->className(), "tr_input" )==0 )
            f->link( ( tr_input* ) ws[0], &ws[1] );
          if ( qstrcmp( ws[0]->className(), "tr_listview" )==0 )
            f->link( ( tr_listview* ) ws[0], &ws[1] );
          if ( qstrcmp( ws[0]->className(), "tr_label" )==0 ) {
            tr_label*lb=( tr_label* ) ws[0];
            if ( lb->aspetto()==tr_label::check ) {
              tr_label::tr_stato s=tr_label::on;
              if ( parametri.count()&&parametri[0]=="chk:off" ) s=tr_label::off;
              f->link( lb, &ws[1], s );
            }
            else
              f->link( lb, &ws[1] );
          } // if tr_label
        } // if !abortire...
      } // tid_links
    break;

    case tid_commento:
    break;

    case tid_palette: {
        int n=dato.toInt();
        tr_palette p=tools.palette( n );
        f->setPalette( p );
      }
    break;

    case tid_spazio:
      spazio=dato.toInt();
    break;

    case tid_readonly:
      readonly=true;
    break;

    case tid_barcodectrls:
      f->barcodectrls=dato;
    break;

    case tid_events_ignore: {
        tr_stringlist events=SPLIT( "|", dato );
        TROWSLI( events ) {
          if ( events[i]=="cards" ) f->events_ignore_cards=true;
        } // TROWSLI
      }
    break;

    case tid_events_timers: {
        tr_stringlist secs=SPLIT( "|", dato );
        TROWSLI( secs ) {
          QTimer::singleShot( secs[i].toInt()*100, f, SLOT( timers() ) );
        } // TROWSLI
      }
    break;

    case tid_events: {
        tr_stringlist events=SPLIT( "|", dato );
        TROWSLI( events ) {
          if ( events[i]=="keypress" ) f->exit_on_key_event=true;
          if ( events[i]=="custom" ) {
            f->exit_on_custom_event=true;
          }
        } // TROWSLI
      }
    break;

    case tid_margine:
      margine=dato.toInt();
    break;

    case tid_minmax: {
        int cxy[4], i;
        int windim[]= {config.finestra_larghezza,config.finestra_altezza,
                      config.finestra_larghezza,config.finestra_altezza};
        if ( dato=="screen" )
          dato=QSN( 0 )+"|" +QSN( 0 )+"|" +QSN( config.finestra_larghezza )+"|" +QSN( config.finestra_altezza );
        QStringList s=QStringList::split( '|', dato, true );
        for ( int i=0; i<s.count(); i++ )
          if ( s[i].find( '%' )!=-1 ) cxy[i]=s[i].remove( '%' ).toInt()/100.0*windim[i];
          else
            cxy[i]=s[i].toInt();
        if ( s.count()>2 ) {
          f->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
          f->setMinMax( cxy[0], cxy[1], cxy[2], cxy[3] );
        }
        else
          f->setMinMax( cxy[0], cxy[1] );
      }
    break;

    case tid_last:
      LOG( TR( "token sconosciuto:" )+token );
      return false;

  }

  parametri.clear();
  token="";
  nome="";
  dato="";
  token_id=tid_last;
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool frm_base::build( QString struttura, QStringList params ) {

  if ( struttura.contains( '%' )>1 ) {
    for ( uint p=0; p<params.count(); p++ ) {
      int sep=params[p].find( '|' );
      struttura.replace( params[p].left( sep ), params[p].mid( sep+1 ) );
    }
  }

  bool righe_attive[100];
  int start_col[100];

  setMinMax( 0, 0, int( config.finestra_larghezza*0.95 ), int( config.finestra_altezza*0.95 ) );

  base_alza();

  nascondi_titolo();
  nascondi_stato();
  nascondi_pulsantiera();

  tr_build_data data;
  data.margine=-1;
  data.spazio=-1;
  data.readonly=false;
  data.hided[0]=NULL;
  data.params=params;

  QString global_tokens="barcodectrls|events|events.ignore|events.timers|readonly|#|palette|spazio|margine|title|status|status.notr|validatore|links|"
     "buttons|minmax|remove|conds";

  QString _tokens_names=global_tokens+"|extra|input|label|listview|hole";

  tr_stringlist tokens_names=SPLIT( '|', _tokens_names );

  // elimina i commenti
  int fine_commenti=0;
  int inizio_commenti=struttura.find( "/*" );
  while ( inizio_commenti!=-1 ) {
    fine_commenti=struttura.find( "*/", inizio_commenti );
    if ( fine_commenti==-1 ) {
      LOG( TR( "Errore nei commenti della form" ) );
      break;
    }
    struttura.remove( inizio_commenti, fine_commenti-inizio_commenti+2 );
    inizio_commenti=struttura.find( "/*" );
  }

  tr_stringlist righe=SPLIT( '\n', struttura );

  while ( righe.last()=="\n" ) righe.pop_back();

  _pointers.clear();

  uint n_colonne=0;
  uint n_colonne_body=0;
  int n_righe=0;
  bool token_globale;

  QString prev_token;
  TROWSLI( righe ) {
    token_globale=false;
    if ( righe[i].left( 6 )=="conds=" ) {
      data.conds=SPLIT( '|', righe[i].mid( 6 ) );
    }

    if ( righe[i] [0]=='?' ||righe[i] [0]=='!' ) {
      int p=righe[i].find( ':' );
      QString cond=righe[i].mid( 0, p );
      tr_stringlist conds=SPLIT( ',', cond );
      start_col[i]=p+1;
      bool enabled=true;
      TROWSLJ( conds ) {
        if ( conds[j] [0]=='!' &&data.conds.findIndex( conds[j].mid( 1 ) )!=-1 ) enabled=false;
        if ( conds[j] [0]=='?' &&data.conds.findIndex( conds[j].mid( 1 ) )==-1 ) enabled=false;
      }
      righe_attive[i]=enabled;
      if ( enabled ) n_righe++;
    }
    else // gestione condizioni
    {
      righe_attive[i]=true;
      n_righe++;
      start_col[i]=0;
    }
    if ( righe[i]=="\n" ||!righe[i].length() ) righe_attive[i]=false;

    if ( !righe_attive[i] ) {
      continue;
    }

    tr_stringlist tokens=SPLIT( ';', righe[i] );
    if ( n_colonne<tokens.count() ) n_colonne=tokens.count();

    data.token=tokens[0].section( '=', 0, 0 ).stripWhiteSpace();
    data.token=data.token.remove( "[]" );
    prev_token=data.token;
    tokens=SPLIT( '|', global_tokens );
    TROWSLJ( tokens )
    if ( tokens[j]==data.token ) token_globale=true;
  }

  data.builded=false;
  data.dim=n_righe*( n_colonne )+1+1;

  data.indice=0;

  uint colonna=0;
  int n_riga=0;

  TROWSLI( righe ) {

    if ( !righe_attive[i] ) {
      continue;
    }

    QString riga;
    if ( start_col[i] ) riga=righe[i].mid( start_col[i] );
    else
      riga=righe[i];

    tr_stringlist tokens;
    data.widget=NULL;

    data.token=riga.section( '=', 0, 0 ).stripWhiteSpace();

    token_globale=false;
    tokens=SPLIT( '|', global_tokens );
    TROWSLJ( tokens )
    if ( tokens[j]==data.token ) token_globale=true;

    tokens=SPLIT( ';', riga );

    QString prev_token="";
    TROWSLJ( tokens ) {

      QString valori="";
      if ( tokens[j].find( '=' )!=-1 ) {
        data.token=tokens[j].section( '=', 0, 0 ).stripWhiteSpace();
        valori=tokens[j].section( '=', 1, 1 ).stripWhiteSpace();
      }
      else {
        data.token="";
        valori=tokens[j].stripWhiteSpace();
      }
      if ( data.token.isEmpty() )
        data.token=prev_token;
      else
        prev_token=data.token;

      data.token_id=tr_build_data::tid_last;
      data.token=data.token.remove( "[]" );
      TROWSLX( tokens_names ) {
        if ( tokens_names[x]==data.token ) {
          data.token_id=( tr_build_data::tr_tokens_id ) x;
          break;
        }
      }

      data.dato=valori;
      data.nome="";

      int par_begin=-1;
      int par_end=-1;
      if ( data.token!="links" ) {
        par_begin=data.dato.find( '(' );
        par_end=data.dato.find( ')', par_begin );
      }

      if ( par_begin!=-1&&par_end!=-1 ) { // ci sono parametri
        data.nome=data.dato.mid( 0, par_begin ).lower();
        par_begin++;
        data.parametri=SPLIT( ',', data.dato.mid( par_begin, par_end-par_begin ) );
        data.dato=data.dato.left( par_begin-1 )+data.dato.mid( par_end+1 );
      }
      else
        data.nome=data.dato.lower();

      if ( data.token_id!=tr_build_data::tid_buttons ) {
        if ( data.dato.find( '#' )>-1 ) data.dato=data.dato.section( '#', 0, 0 );
        if ( data.nome.find( '#' )>-1 ) data.nome=data.nome.section( '#', 1, 1 );
      }

      data.dato=data.dato.replace( 9, '#' );
      data.dato=data.dato.replace( 8, ';' );
      data.crea_widget( this );

      if ( !token_globale&&n_riga<n_righe ) {
        data.ws[data.indice++]=data.widget;
        colonna++;
      }

    } // TROWSLJ

    if ( !token_globale ) {
      if ( n_riga<n_righe ) {
        if ( colonna<n_colonne )
          for ( uint j=colonna; j<n_colonne; j++ ) {
            data.ws[data.indice++]=data.widget;
          }

        data.ws[data.indice++]=NULL; // chiude la riga

        n_riga++;
        colonna=0;
      }
    }


  } // TROWSLI

  if ( !data.builded ) {
    data.ws[data.indice]=NULL; // chiude la griglia (doppione)
    data.build( this );
  }

  if ( data.hided[0]!=NULL ) {
    int ii=0;
    while ( data.hided[ii]!=NULL ) nascondi( data.hided[ii++] );
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_base::tr_intres frm_base::interazione() {
  iterato=NULL;
  if ( isHidden() ) {
    if ( parent()==NULL ) {
      move( ( QApplication::desktop()->width()-this->width() )/2, ( QApplication::desktop()->height()-this->height() )/2 );
    }
    show();
  }
  iterando=true;
  qApp->enter_loop();
  return ( frm_base::tr_intres ) iterato;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_widget*frm_base::pointer( QString nome ) {
  return _pointers[nome];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::pointers( QString nomi, tr_ctrl ws[] ) {
  QStringList n=QStringList::split( '|', nomi, true );
  TROWSLI( n ) {
    tr_ctrl*c=&ws[i];
    tr_widget*w=pointer( n[i] );
    *c=w;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::pulsante_clicked() {
  iterato=( tr_widget* ) sender();
  if ( !isHidden() ) accept();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::pulsante_clicked( QListViewItem*it ) {
  if ( it ) {
    iterato=( tr_widget* ) sender();
    if ( !isHidden() ) accept();
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString frm_base::gets( QString elenco ) {
  QString r="";
  QStringList inputs=QStringList::split( '|', elenco, true );
  TROWSLI( inputs ) {
    if ( inputs[i].isEmpty() ) r+='|';
    else {
      tr_input*inp=( tr_input* ) pointer( inputs[i] );
      if ( inp ) {
        if ( i>0 ) r+='|';
        r+=inp->text();
      }
      else {
        LOG( TR( "Puntatore " +inputs[i]+" non trovato in " +name() ) );
        r+='|';
      }
    } // ! empty
  }
  return r;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::sets( QString elenco, QString valori ) {
  QStringList inputs=QStringList::split( '|', elenco, true );
  QStringList datas=QStringList::split( '|', valori, true );
  int n=inputs.count();
  if ( datas.count()<n ) n=datas.count();
  for ( int i=0; i<n; i++ ) {
    if ( !inputs[i].isEmpty() ) {
      tr_input*inp=( tr_input* ) pointer( inputs[i] );
      if ( inp ) {
        inp->setText( datas[i] );
        if ( !datas[i].isEmpty() ) inp->label->stato( tr_label::on );
      }
      else
        LOG( TR( "sets:" +inputs[i]+"? in " +name() ) );
    } // ! empty
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::card_readed() {

  if ( exit_on_custom_event!=false ) {
    iterato=( tr_widget* ) this;
    exit_loop();
    iterando=false;
    return;
  }

  if ( splash ) {
    exit_loop();
    return;
  }

  if ( !card.count() ) {
    QString str_splash="minmax=100%|100%\n""label=Inserire la chiave per sbloccare\n""label=(pixmap:tessera.png)\n";

    mem_check_begin();
    NEW( splash, frm_base( this, "str_splash" ) );
    splash->build( str_splash, QStringList() );
    splash->interazione();
    xFORM( splash, str_splash );
    splash=NULL;
    return;
  }

  enu_tessera;
  if ( !events_ignore_cards ) {
    if ( card.contains( "|" )==p_rftag ) tools.msgbox( tr( "Tessera non inserita nel database" ) );
    else
      tools.msgbox( tr( "Tessera non gestita in questa maschera" ) );
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::keyPressEvent( QKeyEvent*e ) {
  if ( barcode_enabled()&&MODULO( modulo_barcode ) ) {
    QString barcode=QString::null;
    if ( e ) barcode=tools.barcode( e->text() );
    if ( e ) e->ignore();
    if ( !barcode.isNull() ) barcodeReaded( barcode );
  } // if MODULO

} // keyPressEvent

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::barcodeReaded( QString barcode ) {
  emit barcodeReaded( barcode );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void frm_base::exit_loop() {
  int c=0;
  while ( qApp->hasPendingEvents()&&c<10 ) {
    qApp->processEvents();
    c++;
  }
  qApp->exit_loop();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <qapplication.h>

QString frm_base::run_form(QString structure, QStringList params)
{
  QApplication *temp_app;
  if (!qApp) temp_app=new QApplication(0,NULL);

  QString ctrl_name;
  FRM_PARAMS;
  if (params.count()>0) _frm_params=params;

  frm_base *form;
  if (temp_app) form=new frm_base(NULL,"aform");
  else form=new frm_base(tools.actWin(),"aform");

  form->build(structure,_frm_params);

  if (temp_app) temp_app->setMainWidget(form);

  form->interazione();
  if (form->iterato)
    ctrl_name=form->iterato->name();
  delete form;

  if (temp_app) delete temp_app;

  return ctrl_name;
}
