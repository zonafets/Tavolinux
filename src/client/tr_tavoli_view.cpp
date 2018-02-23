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

#include <qpainter.h>
#include <qimage.h>

#include "tr_tavoli_view.hpp"
#include "tr_srvcmd.hpp"

#include "tr_globals.hpp"
#include "tr_display.hpp"

extern tr_globals glb;
extern tr_display display_lcd;

bool itm_tavolo::lampeggio=true;

#define COLORE_LABELS_ACCESE QColor(255,80,80)

//--------------------------------------------------------------------------------------------------------------- class  itm_tavolo
tr_tavolo::tr_genere tr_tavolo::genere( QString pixmap ) {
  tr_genere t=nessuno;
  QStringList s=glb.pixmaps.grep( pixmap );
  if ( s.size()>0 ) {
    QString st=s[0];
    int tipo=st.section( '|', tr::pixmap::p_tipo, tr::pixmap::p_tipo ).toInt();
    return genere( tipo );
  }
  return t;
}

//--------------------------------------------------------------------------------------------------------------- class  itm_tavolo
tr_tavolo::tr_genere tr_tavolo::genere( int tipo ) {
  tr_genere t=nessuno;
  if ( tipo==T_TAVOLO_STANDARD ) t=tavolo;
  if ( tipo&( T_TAVOLO_BIS_BAR ) ) t=tavolo;
  if ( tipo&( T_TAVOLO_TAKEAWAY ) ) t=tavolo;
  if ( tipo&( T_TAVOLO_PIANTA ) ) t=arredo;
  if ( tipo&( T_TAVOLO_OROLOGIO ) ) t=dinamico;
  if ( tipo&( T_TAVOLO_PAVIMENTO ) ) t=pavimento;
  return t;
}

//--------------------------------------------------------------------------------------------------------------- class  itm_tavolo
bool tr_tavolo::bar( QString pixmap ) {
  QStringList s=glb.pixmaps.grep( pixmap );
  if ( s.size()>0 ) {
    int tipo=s[0].section( '|', tr::pixmap::p_tipo, tr::pixmap::p_tipo ).toInt();
    if ( tipo&( T_TAVOLO_BAR ) ) return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------------------------------- class  itm_tavolo
bool tr_tavolo::bar( int tipo ) {
  if ( tipo&( T_TAVOLO_BAR ) ) return true;
  return false;
}

//--------------------------------------------------------------------------------------------------------------- class  itm_tavolo

tr_tavolo::tr_genere tr_tavolo::genere() {
  return genere( tipo() );
}

//--------------------------------------------------------------------------------------------------------------- class  itm_tavolo

bool tr_tavolo::bar() {
  return bar( tipo() );
}

//--------------------------------------------------------------------------------------------------------------- class  itm_tavolo

bool tr_tavolo::tway() {
  return ( ( tipo()&T_TAVOLO_TAKEAWAY )==T_TAVOLO_TAKEAWAY )?true:false;
}

//--------------------------------------------------------------------------------------------------------------- class  itm_tavolo

bool tr_tavolo::orologio() {
  return ( ( tipo()&T_TAVOLO_OROLOGIO )==T_TAVOLO_OROLOGIO )?true:false;
}

//--------------------------------------------------------------------------------------------------------------- class  itm_tavolo

bool tr_tavolo::prenotato() {
  return ( stato()&riservato );
}


//--------------------------------------------------------------------------------------------------------------- class  itm_tavolo

void tr_tavolo::set( tr::enumeratori::tavolo_esteso::tr_posizioni p, QString nuovo_valore ) {
  QStringList campi=QStringList::split( '|', _dati, true );
  campi[p]=nuovo_valore;
  _dati=campi.join( "|" );
}

//--------------------------------------------------------------------------------------------------------------- class  itm_tavolo

itm_tavolo::itm_tavolo( QCanvas*canvas ):QCanvasRectangle( canvas ) {
  setSize( config.tavolo_dx, config.tavolo_dy );
  selezionato=false;
  azzera_biss();
  bis=0;
  nascosto=false;
}

int itm_tavolo::rtti() const {
  return RTTI;
}


QRect itm_tavolo::dimensione() {
  QSize s;
  if ( !pixmap().isEmpty() ) s=QPixmap( PIXMAP( pixmap() ) ).size();
  else
    s=QSize( config.tavolo_dx, config.tavolo_dy );
  s=QSize( s.width()+6, s.height()+6 );
  return QRect( QPoint( ( int )x()-4, ( int )y()-4 ), s );
}

void itm_tavolo::drawShape( QPainter&p ) {
  bool loc_nascosto=nascosto;
  tr_tavoli_view*tavoli=( tr_tavoli_view* ) canvas()->parent();

  if ( bs()>0 ) return;

  bool _tavolata=tavolata();
  if ( loc_nascosto&&!tavoli->inserimento() ) return;

  if ( tavoli->inserimento() ) _tavolata=false;

  int xx=( int )x();
  int yy=( int )y();
  int dox=0; // 3
  int doy=0; // 3
  int ox=xx-dox;
  int oy=yy-doy;
  int w=width();
  int h=height();
  int dx=w+dox; // +4
  int dy=h+doy; // +4

  int tx=xx+config.tavolo_ox;
  int ty=yy+config.tavolo_oy;

  int st_width;
  int st_height;

  QPainter*pp=&p;
  if ( ( selezionato ) ) {
    pp->setPen( QPen( QColor( 10, 100, 10 ), 3 ) );
    pp->setBrush( QColor( 0, 250, 0 ) );
    pp->drawRoundRect( ox, oy, dx, dy, 40, 40 );
  }

  int lx;
  int ly;
  QPixmap tavolo=pixmap_from_tipo();
  if ( !tavolo.isNull() ) {
    if ( w!=tavolo.width()||h!=tavolo.height() ) {
      w=tavolo.width();
      h=tavolo.height();
      setSize( w, h );
    }
    lx=tavolo.width();
    ly=tavolo.height();

    if ( !_tavolata ) pp->drawPixmap( tx, ty, tavolo );
    else {
      lx/=2;
      ly/=2;
      QImage i;
      i=tavolo;
      i=i.scale( lx, ly );
      pp->drawImage( tx, ty, i );
      pp->drawImage( tx+lx, ty, i );
      pp->drawImage( tx, ty+ly, i );
      pp->drawImage( tx+lx, ty+ly, i );
    }
  }

  else {
    lx=w;
    ly=h;
    pp->setPen( QPen( QColor( 0, 100, 0 ), 2 ) );
    pp->setBrush( QBrush( QColor( 0, 150, 0 ) ) );
    if ( !selezionato ) {
      if ( !_tavolata ) {
        pp->drawRect( ox+1, oy+1, dx-1, dy-1 );
      }
      else {
        lx/=2;
        ly/=2;
        pp->drawRect( tx+1, ty+1, lx-1, ly-1 );
        pp->drawRect( tx+lx+1, ty+1, lx-1, ly-1 );
        pp->drawRect( tx+1, ty+ly+1, lx-1, ly-1 );
        pp->drawRect( tx+lx+1, ty+ly+1, lx-1, ly-1 );
      }
    }
  }


  tr_genere tg=genere();

  if ( tg==tr_tavolo::tavolo||tg==tr_tavolo::dinamico ) {

    int s=stato();
    QString nome=numero();

    bool con_tessera=false;
    enu_tessera;
    if ( tavoli->selezionato()==this&&card.count()>=p_id_t0&&( card[p_id_t0].toInt()==id()||this->bar() ) ) con_tessera=true;

    QString base=pixmap();
    if ( !base.isEmpty() ) {
      base.remove( ".png" );

      QPixmap prenotato, riesumato, piatto, tessera, soldi, bis;

      if ( !bar()&&!orologio() ) prenotato=PIXMAP( base+"_prenotato.png" );
      if ( !orologio() ) {
        riesumato=PIXMAP( base+"_riesumato.png" );
        piatto=PIXMAP( base+"_piatto.png" );
        tessera=PIXMAP( base+"_tessera.png" );
        soldi=PIXMAP( base+"_soldi.png" );
        bis=PIXMAP( "bis.png" );
      }

      if ( !_tavolata ) {
        QPixmap*pixs[]= { &prenotato, &tessera, &soldi, &piatto, &riesumato, &bis };
        bool cond[]= { s&riservato, con_tessera, s&conto, s&apparecchiato, s&conto_riesumato, s&con_bis };
        TROWI( pixs )
        if ( cond[i] ) pp->drawPixmap( tx, ty, *pixs[i] );
      }
      else {
        QPixmap*pixs[]= { &prenotato, &tessera, &soldi, &piatto, &riesumato, &bis };
        bool cond[]= { s&riservato, con_tessera, s&conto, s&apparecchiato, s&conto_riesumato, s&con_bis };
        TROWI( pixs ) {
          if ( cond[i] ) {
            QImage img=pixs[i]->convertToImage();
            img=img.scale( img.width()/2, img.height()/2 );
            pp->drawImage( tx, ty, img );
          } // if
        } // TROWI

      }
    }


    if ( nome.length()>0 ) {
      QFont f( p.font() );
      f.setBold( false );
      f.setPixelSize( config.tavolo_font_dimensione );
      QFontMetrics fm( f );
      st_width=fm.width( nome );
      st_height=fm.height();
      int _x=xx+w-st_width-5;
      int _y=yy+h-st_height+2;
      p.setFont( f );

      // Il tavolo "orologio" ha il suo disegno

      if ( orologio() ) {
        // Ricalcola gli offset di centratura orizzontale

        _x=xx+( ( w-st_width )>>1 );

        p.setBrush( config.colore_labels_dinamiche );
        p.setPen( config.colore_labels_dinamiche_bordo );
        p.drawRoundRect( xx, _y, tavolo.width(), st_height, 40, 40 );
        p.setPen( config.colore_labels_dinamiche_bordo );
        p.drawText( _x, yy+h-2, nome );

      }
      else {
        // Tutti gli altri tavoli

        if ( s&proforma_stampata&&lampeggio ) p.setBrush( COLORE_LABELS_ACCESE );
        else
          p.setBrush( config.colore_labels );
        p.setPen( config.colore_labels_bordo );
        p.drawRoundRect( _x, _y, st_width+6, st_height, 40, 40 );
        p.setPen( config.colore_labels_bordo );
        p.drawText( xx+w-st_width-2, yy+h-1, nome );
      }
    }

  }

  canvas()->setChanged( dimensione() );

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QPixmap itm_tavolo::componi_tavolo( QString pixmap, QString numero, const QColor&sfondo, bool selezionato, int stato ) {

  bool bis=false;

  QPixmap tavolo=PIXMAP( pixmap );
  QPainter p;
  QPainter*pp=&p;
  int w=64, h=64;
  QPixmap floor( w, h );
  pp->begin( &floor );
  pp->fillRect( 0, 0, w, h, QBrush( sfondo ) );

  if ( ( selezionato ) ) {
    pp->setPen( QPen( QColor( 10, 100, 10 ), 3 ) );
    pp->setBrush( QColor( 0, 250, 0 ) );
    pp->drawRoundRect( 0, 0, w-1, h-1, 40, 40 );
  }

  int lx=tavolo.width();
  int ly=tavolo.height();
  int tx=( w-lx )/2;
  int ty=( h-ly )/2;
  pp->drawPixmap( tx, ty, tavolo );

  QString base=pixmap;
  base.remove( ".png" );

  if ( stato&TAVOLO_CON_BIS==TAVOLO_CON_BIS ) bis=true;

  QPixmap prenotato;
  if ( pixmap.find( "bar" )==-1 ) prenotato=PIXMAP( base+"_prenotato.png" );
  QPixmap riesumato( PIXMAP( base+"_riesumato.png" ) );
  QPixmap piatto( PIXMAP( base+"_piatto.png" ) );
  QPixmap soldi( PIXMAP( base+"_soldi.png" ) );
  if ( stato&riservato ) pp->drawPixmap( tx, ty, prenotato );
  if ( stato&conto_riesumato ) pp->drawPixmap( tx, ty, riesumato );
  if ( stato&conto ) pp->drawPixmap( tx, ty, soldi );
  if ( stato&apparecchiato ) pp->drawPixmap( tx, ty, piatto );
  if ( bis ) pp->drawPixmap( tx, ty, PIXMAP( "bis.png" ) );

  if ( numero.length()>0 ) {
    QFont f( p.font() );
    f.setBold( false );
    f.setPixelSize( config.tavolo_font_dimensione );
    QFontMetrics fm( f );
    int st_width=fm.width( numero );
    int st_height=fm.height();
    int _x=w-st_width-5-2;
    int _y=h-st_height+2-2;
    p.setFont( f );

    if ( stato&proforma_stampata ) p.setBrush( COLORE_LABELS_ACCESE );
    else
      p.setBrush( config.colore_labels );
    p.setPen( config.colore_labels_bordo );
    p.drawRoundRect( _x, _y, st_width+6, st_height, 40, 40 );
    p.setPen( config.colore_labels_bordo );
    p.drawText( w-st_width-4, h-3, numero );
  }

  pp->end();

  return floor;
}

//--------------------------------------------------------------------------------------------------------------- pixmap_from_tipo
QPixmap itm_tavolo::pixmap_from_tipo() {
  return PIXMAP( pixmap() );
}


QString tr_tavolo::nome( int id, QString numero ) {
  if ( bs( id )!=0 ) {
    if ( numero[0]!='.' ) return numero.section( '.', 0, 0 )+"." +QString::number( bs( id ) );
    else
      return numero+"." +QString::number( bs( id ) );
  }
  else
    return numero;
}

tr_tavolo&tr_tavolo::operator=( const tr_tavolo&t ) {
  _dati=t._dati;
  nascosto=t.nascosto;
  selezionato=t.selezionato;
  aperto=t.aperto;
  card=t.card;
  return*this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

itm_tavolo&itm_tavolo::operator=( const itm_tavolo&t ) {
  _dati=t._dati;
  lampeggio=t.lampeggio;
  nascosto=t.nascosto;
  selezionato=t.selezionato;
  aperto=t.aperto;
  card=t.card;
  return*this;
}

tr_tavoli_view::tr_tavoli_view( QWidget*parent, const char*name, WFlags f ):QCanvasView( parent, name, f ) {
  setMargin( config.vista_margine );
  setVScrollBarMode( QScrollView::AlwaysOff );
  setHScrollBarMode( QScrollView::AlwaysOff );
  setMinimumSize( QSize( config.finestra_larghezza, config.finestra_altezza_centrale ) );
  setMaximumSize( QSize( config.finestra_larghezza, config.finestra_altezza_centrale ) );
  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  //setResizePolicy( QScrollView::AutoOneFit );
  _selezionato=NULL;
  _inserimento=false;
  _vista_a_griglia=false;
  _tavolata=false;
  _tavoli_aggregati="";
  _n_tavoli_aggregati=0;;
  _pavimento="";

  if ( config.rapporto!=1 ) {
    QWMatrix m=worldMatrix();
    m.scale( config.rapporto, config.rapporto );
    setWorldMatrix( m );
  }

}

//-------------------------------------------------------------------------------------------- seleziona

itm_tavolo*tr_tavoli_view::seleziona( itm_tavolo*tavolo, int*id ) {
  if ( !tavolata() ) {
    _tavoli_aggregati="";
    _n_tavoli_aggregati=0;
  }

  bool update;

  if ( _selezionato ) {
    _selezionato->selezionato=false;
    canvas()->setChanged( _selezionato->dimensione() );
    update=true;
  }

  _selezionato=NULL;

  if ( tavolo==NULL&&id==NULL ) {
    if ( update ) canvas()->update();
    return NULL;
  }

  QCanvasItemList l=canvas()->allItems();

  for ( QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it ) {
    itm_tavolo*i=( itm_tavolo* ) ( *it );
    if ( id&&i->id()==*id ) {
      _selezionato=i;
      break;
    }
    else if ( /* i->is_table() && */ i==tavolo ) {
      _selezionato=i;
      break;
    }
  }

  if ( _selezionato ) {
    _selezionato->selezionato=true;
    canvas()->setChanged( QRect( ( int )_selezionato->x()-2, ( int )_selezionato->y()-2, config.tavolo_dx+4,
       config.tavolo_dy+2 ) );
    update=true;
  }
  if ( update ) canvas()->update();

  return _selezionato;
}

//-------------------------------------------------------------------------------------------- tavolo

itm_tavolo*tr_tavoli_view::tavolo( int id ) {
  if ( id==0 ) return NULL;

  QCanvasItemList l=canvas()->allItems();

  for ( QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it ) {
    itm_tavolo*i=( itm_tavolo* ) ( *it );
    if ( i->is_table()&&i->id()==id ) return i;
  }

  return NULL;
}

//-------------------------------------------------------------------------------------------- quale_toccato

itm_tavolo*tr_tavoli_view::quale_toccato( QPoint pos ) {
  itm_tavolo*selezionato=NULL;
  QPoint p=inverseWorldMatrix().map( pos );
  QCanvasItemList l=canvas()->collisions( p );
  bool d=inserimento();
  for ( QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it ) {
    itm_tavolo*i=( itm_tavolo* ) ( *it );
    bool a=i->is_table();
    bool b=i->nascosto;
    bool c=i->aggregato();
    if ( a&&!b&&!c ) selezionato=i;
    if ( !b&&d ) selezionato=i;
  }

  return selezionato;
}

//-------------------------------------------------------------------------------------------- seleziona

itm_tavolo*tr_tavoli_view::seleziona( QMouseEvent*e ) {
  if ( e ) {
    itm_tavolo*selezionato=quale_toccato( e->pos() );

    if ( !tavolata() ) {
      _tavoli_aggregati="";
      _n_tavoli_aggregati=0;
    }

    bool update;

    if ( _selezionato ) {
      _selezionato->selezionato=false;
      canvas()->setChanged( QRect( ( int )_selezionato->x()-2, ( int )_selezionato->y()-2, config.tavolo_dx+4,
         config.tavolo_dy+2 ) );
      update=true;
      _selezionato->card.clear();
    }

    _selezionato=selezionato;

    if ( _selezionato ) {
      _selezionato->selezionato=true;
      canvas()->setChanged( QRect( ( int )_selezionato->x()-2, ( int )_selezionato->y()-2, config.tavolo_dx+4,
         config.tavolo_dy+2 ) );
      update=true;
    }

    if ( update ) canvas()->update();
  }

  return _selezionato;
}

itm_tavolo*tr_tavoli_view::aggrega_tavolo( QMouseEvent*e ) {
  itm_tavolo*selezionato=quale_toccato( e->pos() );
  if ( selezionato ) {
    if ( selezionato->id()==_selezionato->id() ) {
      BEEP;
      return selezionato;
    }
    if ( selezionato->stato()!=0 ) {
      BEEP;
      return selezionato;
    }
    if ( ::cmd( tok_aggiungi_tavolo_tavolata, QSN( _selezionato->id() ), QSN( selezionato->id() ) ) ) {
      QString st=srv->rcv();
      srv->eop();
      selezionato->dati( st );
      selezionato->hide();
      selezionato->aggiorna();
      if ( _tavoli_aggregati.isEmpty() ) _tavoli_aggregati+=( selezionato->numero() );
      else
        _tavoli_aggregati+=( "," +selezionato->numero() );
      _n_tavoli_aggregati++;
    }
    else
      BEEP;
  }
  else
    BEEP;
  return selezionato;
}

void tr_tavoli_view::tavolata( bool stato ) {
  _tavolata=stato;
  if ( stato&&!selezionato()->tavolata() ) { // la crea
    ::cmd( tok_crea_tavolata, QSN( selezionato()->id() ) );
    QString st=srv->rcv();
    selezionato()->dati( st );
    selezionato()->aggiorna();
    _tavoli_aggregati="";
  }
  if ( stato&&selezionato()->tavolata() ) {
    tavoli_aggregati( selezionato()->id() );
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_tavoli_view::sciogli_tavolata( int id ) {
  //bool update = false;
  QString _id=QSN( id );
  if ( ::cmd( tok_sciogli_tavolata, _id ) ) {
    QString st=srv->rcv();
    srv->eop();
    selezionato()->dati( st );
    selezionato()->aggiorna();
    QStringList tavoli=QStringList::split( ',', _tavoli_aggregati );
    _tavoli_aggregati="";

    QCanvasItemList list=canvas()->allItems();
    QCanvasItemList::Iterator it=list.begin();
    for ( ; it!=list.end(); ++it ) {
      itm_tavolo*t=( itm_tavolo* )*it;
      tr::mse::riga tavolo( t->dati() );
      if ( tavolo[tr::tavolo_esteso::p_id_tavolata]==_id ) {
        t->set( p_id_tavolata, "-1" );
        t->show();
        t->aggiorna();
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------- lampeggio_update

void tr_tavoli_view::lampeggio_update() {
  QCanvasItemList l=canvas()->allItems();
  bool cambiato=false;
  for ( QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it ) {
    QPainter p();
    itm_tavolo*t=( itm_tavolo* ) ( *it );
    if ( t->stato()&itm_tavolo::conto ) {
      canvas()->setChanged( QRect( ( int )t->x(), ( int )t->y(), config.tavolo_dx, config.tavolo_dy ) );
      t->show();
      cambiato=true;
    }
  }
  update( 0, 0, 800, 600 );
}

void tr_tavoli_view::tavoli_a_griglia( bool stato ) {
  if ( !stato ) {
    if ( !_vista_a_griglia ) return;
    _vista_a_griglia=false;
    carica_tavoli( _sala.id, true );
    return;
  }

  int x=0, y=0;
  int n_colonne; // numero di colonne
  int n_righe;
  int tdx, tdy; // dimensioni tavolo
  int max_y=0;
  tdx=config.tavolo_dx;
  tdy=config.tavolo_dy;

  n_colonne=clipper()->width()/tdx;
  n_righe=clipper()->height()/tdy;

  setUpdatesEnabled( false );
  if ( canvas()->allItems().size()>0 ) {
    QCanvasItemList list=canvas()->allItems();
    QCanvasItemList::Iterator it;
    enum {
      c_nome, c_addr
    };

    uint max_len=0;
    for ( it=list.begin(); it!=list.end(); ++it ) {
      itm_tavolo*i=( itm_tavolo* ) ( *it );
      uint l=i->nome().length();
      if ( max_len<l ) max_len=l;
    }

    QListView lordine;
    QListViewItem*lit;
    lordine.addColumn( "nome" );
    lordine.addColumn( "addr" );
    lordine.setSorting( 0 );
    for ( it=list.begin(); it!=list.end(); ++it ) {
      itm_tavolo*t=( itm_tavolo* ) ( *it );
      if ( t->is_table()&&!t->nascosto&&!t->aggregato() ) {
        bool ok;
        t->nome().toInt( &ok );
        QString s;
        if ( ok ) s=t->nome().rightJustify( max_len, '0' );
        else
          s=t->nome();
        NEW( lit, QListViewItem( &lordine, s, QString::number( ( int )t ) ) );
        lordine.insertItem( lit );
      }
      else
        t->hide();
    }
    lordine.sort();

    lit=lordine.firstChild();
    int ordine=0;
    while ( lit ) {
      itm_tavolo*t=( itm_tavolo* ) ( lit->text( c_addr ).toInt() );
      lit=lit->nextSibling();

      if ( t->is_table()&&!t->nascosto&&!t->aggregato() ) {
        x=tdx*( ordine%n_colonne );
        y=tdx*int( ordine/n_colonne );
        if ( max_y<y ) max_y=y;
        t->move( x, y );
        ordine++;
      }
      else
        t->nascosto=true;
    }
  }
  setUpdatesEnabled( true );
  if ( max_y>canvas()->height() )
    canvas()->resize( clipper()->width(), max_y );
  else {
    canvas()->setAllChanged();
    canvas()->update();
  }

  _vista_a_griglia=true;
}

//------------------------------------------------------------------------------------------ cencella_items
bool tr_tavoli_view::cancella_items() {
  if ( canvas()->allItems().size()>0 ) {
    // canvas()->allItems().clear();
    QCanvasItemList list=canvas()->allItems();
    QCanvasItemList::Iterator it=list.begin();
    for ( ; it!=list.end(); ++it ) {
      if ( *it ) {
        DELETE( *it );
      }
    }
    return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_tavoli_view::pavimento( QString pixmap ) {
  _pavimento=pixmap;
  canvas()->setBackgroundPixmap( PIXMAP( pixmap ) );
}

bool tr_tavoli_view::carica_tavoli( int sala_id, bool refresh ) {

  if ( ( sala_id!=-1&&_sala.id==sala_id )&&!refresh ) return false;
  if ( sala_id==-1 ) {
    cancella_items();
    _sala.id=-1;
    _sala.nome="";
    canvas()->update();
    return false;
  }

  bool selez=false;
  int selez_id;
  if ( selezionato() ) {
    selez=true;
    selez_id=selezionato()->id();
  }

  _sala.id=sala_id;

  tr::mse sale( tok_elenco_sale_esteso );
  sale.carica( QSN( sala_id ) );

  _selezionato=NULL;
  cancella_items();

  if ( _sala.id!=sala_id ) canvas()->setAllChanged();

  if ( !sale.count() ) {
    _sala.id=-1;
    _sala.nome="";
    canvas()->update();
    return false;
  }
  else {
    tr::mse::riga sala( sale[0] );
    _sala.id=sala[tr::enumeratori::sala_estesa::p_id].toInt();
    _sala.nome=sala[tr::enumeratori::sala_estesa::p_nome];
    QString pav=sala[tr::enumeratori::sala_estesa::p_pixmap];
    if ( !pav.isEmpty() ) {
      if ( pav!=pavimento() ) pavimento( pav );
    }
    else
      pavimento( "floor.jpg" );
  }

  tr::mse tavoli( tok_elenco_tavoli_esteso );
  tavoli.carica( QSN( _sala.id ) );
  if ( !tavoli.count() ) {
    canvas()->update();
    return false;
  }


  int tavoli_caricati=0;
  int tavoli_visualizzati=0;

  FORI( tavoli.count() ) {

    tr::mse::riga tavolo( tavoli[i] );
    int id=tavolo[p_id].toInt();

    if ( itm_tavolo::bs( id ) ) continue;

    int stato=tavolo[p_stato].toInt();

    if ( !( stato&( 1<<TAVOLO_NASCOSTO ) ) ) {

      itm_tavolo*t;
      NEW( t, itm_tavolo( canvas() ) );
      t->dati( tavoli[i] );

      t->posiziona( false );
      t->lampeggio=&lampeggio;

      if ( t->tway() )
        t->set( tr::tavolo_esteso::p_stato, "0" );


      tavoli_caricati++;

      if ( !t->aggregato()||inserimento() ) {
        t->show();
        tavoli_visualizzati++;
      }
    }
  }

  if ( selez ) seleziona( selez_id );

  if ( vista_a_griglia() )
    tavoli_a_griglia( true );
  else {
    canvas()->update();
  }

  return true;
}

//------------------------------------------------------------------------------------ cancella tavolo selezionato

bool tr_tavoli_view::cancella() {
  itm_tavolo*t=selezionato();
  seleziona( ( itm_tavolo* ) NULL );
  bool r=CMD( tok_cancella_tavolo, QSN( t->id() ) );
  if ( r ) {
    canvas()->removeChild( ( QObject* ) t );
    canvas()->setChanged( t->dimensione() );
    DELETE( t );
    canvas()->update();
  }
  return r;
}

//----------------------------------------------------------------------------------- aggiungi tavolo in posizione e

bool tr_tavoli_view::aggiungi( QPoint&p, QString pixmap ) {
  int x=p.x()/config.tavolo_dx;
  x*=config.tavolo_dx;
  int y=p.y()/config.tavolo_dy;
  y*=config.tavolo_dy;

  p.setX( x );
  p.setY( y );

  if ( p.x()+config.tavolo_dx>visibleWidth() ) BEEPERF;
  if ( p.y()+config.tavolo_dy>visibleHeight() ) BEEPERF;


  QPixmap pix( PIXMAP( pixmap ) );
  for ( int px=config.tavolo_dx/2; px<pix.width(); px+=config.tavolo_dx/2 )
    for ( int py=config.tavolo_dy/2; py<pix.height(); py+=config.tavolo_dy/2 ) {
      QPoint pxy( px+x, py+y );
      if ( quale_toccato( pxy ) ) BEEPERF;
    }


  const int range=0;
  QRect target_area( QPoint( p.x()+range, p.y()+range ), QSize( pix.width()-range, pix.height()-range ) );
  if ( collisioni( target_area ) ) BEEPERF

    itm_tavolo::tr_genere tipo=itm_tavolo::genere( pixmap );
  int ordine;
  ordine=ultimo_ordine( tipo )+1;

  QString nome;
  if ( tipo==itm_tavolo::tavolo ) nome=QString::number( ordine );
  if ( tipo==itm_tavolo::arredo ) nome="#" +QSN( ordine );

  if ( itm_tavolo::bar( pixmap ) ) {
    int ub=ultimo_bar()+1;
    if ( ub>0 ) nome=config.bar+QSN( ub );
    else
      nome=config.bar;
  }

  bool r=::cmd( tok_inserisci_tavolo, QSN( sala().id ), nome, QSN( ( int )p.x() ), QSN( ( int )p.y() ), QSN( ordine ), LEAVE, pixmap );
  if ( r ) {
    QString st=srv->rcv();
    srv->eop();
    itm_tavolo*t;
    NEW( t, itm_tavolo( canvas() ) );
    t->dati( st );
    t->posiziona();
    canvas()->setChanged( t->dimensione() );
    canvas()->update();

  }
  return r;
}

//---------------------------------------------------------------------- sposta tavolo selezionato in posizione e
bool tr_tavoli_view::sposta( QPoint&p ) {
  int x=p.x()/config.tavolo_dx;
  x*=config.tavolo_dx;
  int y=p.y()/config.tavolo_dy;
  y*=config.tavolo_dy;

  p.setX( x );
  p.setY( y );

  itm_tavolo*t=selezionato();

  const int range=0;
  QRect target_area( QPoint( p.x()+range, p.y()+range ), QSize( t->width()-range, t->height()-range ) );

  if ( collisioni( target_area ) ) BEEPERF;

  x=( int )p.x();
  y=( int )p.y();
  if ( CMD( tok_modifica_tavolo, QSN( t->id() ), LEAVE, QSN( x ), QSN( y ), LEAVE, LEAVE ) ) {
    canvas()->setChanged( t->dimensione() );
    selezionato()->move( p.x(), p.y() );
    canvas()->setChanged( t->dimensione() );
    canvas()->update();
  }
  else
    return false;
  return true;
}

QString tr_tavoli_view::tavoli_aggregati( int id_capo ) {
  _tavoli_aggregati="";
  _n_tavoli_aggregati=0;
  QCanvasItemList list=canvas()->allItems();
  QCanvasItemList::Iterator it=list.begin();
  for ( ; it!=list.end(); ++it ) {
    itm_tavolo*t=( itm_tavolo* ) ( *it );
    if ( t->bs()==0&&t->id_tavolata()==id_capo ) {
      if ( t->id()==id_capo ) {
        _n_tavoli_aggregati++;
        if ( !_tavoli_aggregati.isEmpty() ) _tavoli_aggregati=( t->numero()+"," +_tavoli_aggregati );
        else
          _tavoli_aggregati=t->numero()+_tavoli_aggregati;
      }
      else {
        _n_tavoli_aggregati++;
        if ( !_tavoli_aggregati.isEmpty() ) _tavoli_aggregati+=",";
        _tavoli_aggregati+=t->numero();
      }
    }
  }

  return _tavoli_aggregati;
}


QString tr_tavoli_view::tavoli_aggregati() {
  if ( _tavoli_aggregati=="" &&selezionato() ) {
    return tavoli_aggregati( selezionato()->id() );
  }
  else
    return _tavoli_aggregati;
}


bool tr_tavoli_view::collisioni( QRect rect ) {
  itm_tavolo*attuale_selezionato=selezionato();
  // canvas->allItems().clear();
  QCanvasItemList list=canvas()->allItems();
  QCanvasItemList::Iterator it=list.begin();
  for ( ; it!=list.end(); ++it ) {
    QCanvasRectangle*r=( QCanvasRectangle* ) ( *it );
    if ( r!=attuale_selezionato ) {
      if ( r->rect().intersects( rect ) ) return true;
    }
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int tr_tavoli_view::ultimo_bar() {
  QCanvasItemList list=canvas()->allItems();
  QCanvasItemList::Iterator it=list.begin();
  int ultimo=-1;
  for ( ; it!=list.end(); ++it ) {
    itm_tavolo*t=( itm_tavolo* ) ( *it );
    if ( t->bar() ) {
      QString num=t->nome();
      num=num.upper().replace( config.bar.upper(), "" );
      int numero=0;
      numero=num.toInt();
      if ( ultimo<numero ) ultimo=numero;
    }
  }
  return ultimo;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int tr_tavoli_view::ultimo_ordine( itm_tavolo::tr_genere g ) {
  QCanvasItemList list=canvas()->allItems();
  QCanvasItemList::Iterator it=list.begin();
  int ultimo=0;
  for ( ; it!=list.end(); ++it ) {
    itm_tavolo*t=( itm_tavolo* ) ( *it );
    itm_tavolo::tr_genere gg=t->genere();
    QString numero="";
    for ( uint i=0; i<t->numero().length(); i++ ) {
      QChar c=t->numero() [i];
      if ( c.isDigit() ) numero+=c;
    }
    if ( numero.isEmpty() ) numero="0";
    int ordine=numero.toInt();
    if ( gg==g&&ultimo<ordine ) ultimo=ordine;
  }
  return ultimo;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QStringList tr_tavolo::apri() {
  enu_tessera;
  if ( card.count()>=p_id_t1&&( id()==card[p_id_t0].toInt()||bar() ) ) {
    int id1=0;
    id1=card[p_id_t1].toInt();
    int id2=0;
    if ( !id1 ) {
      if ( ::cmd( tok_inserisci_tavolo_fittizio ) ) {
        QString id=srv->rcv();
        srv->eop();
        id1=id.toInt();
      }
    }
    if ( bar() ) id2=id();
    if ( !::cmd( tok_apri_tavolo_tag, card[p_rftag], card[p_id_t0], QSN( id1 ), QSN( id2 ) ) )
      return QStringList();
  }
  else if ( !::cmd( tok_apri_tavolo, QSN( id() ) ) ) return QStringList();

  QString stato=srv->rcv();
  srv->eop();
  aperto=true;
  return QStringList::split( config.cmd_sep, stato, true );

}

bool tr_tavolo::chiudi( bool conto ) {
  srv->silent=true;
  if ( !conto ) {
    if ( CMD( tok_chiudi_tavolo ) ) aperto=false;
  }
  else {
    if ( CMD( tok_chiudi_tavolo_tag ) ) aperto=false;
  }
  srv->silent=false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
