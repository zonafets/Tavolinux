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

#include "tr_time.hpp"
#include "tr_config.hpp"

extern tr_config config;

tr_table_intervalli::tr_table_intervalli( QWidget*parent=0, const char*name=0 ):QTable( parent, name ) {
  setVScrollBarMode( QScrollView::AlwaysOff );
  setHScrollBarMode( QScrollView::AlwaysOff );
  setSelectionMode( QTable::NoSelection );
  verticalHeader()->hide();
  horizontalHeader()->hide();
  setLeftMargin( 0 );
  setTopMargin( 0 );
  insertColumns( 0, 25 );
  insertRows( 0, 13 );

  int wh=32;
  int i;
  for ( i=0; i<=24; i++ ) {
    if ( i>0 ) setText( 0, i, QString::number( i-1 ) );
    setColumnWidth( i, wh );
    setColumnStretchable( i, false );
  }
  for ( i=0; i<=12; i++ ) {
    if ( i>0 ) setText( i, 0, QString( "0" +QString::number( ( i-1 )*5 ) ).right( 2 ) );
    setRowHeight( i, wh );
    setRowStretchable( i, false );
  }

  setReadOnly( true );
}

void tr_table_intervalli::paintCell( QPainter*p, int row, int col, const QRect&cr, bool selected, const QColorGroup&cg ) {

  if ( cr.width()==0||cr.height()==0 )
    return;

  QColorGroup _cg( cg );

  if ( row==0||col==0 ) _cg.setColor( QColorGroup::Base, QColor( 150, 150, 150 ) );
  if ( text( row, col )==" " ) _cg.setColor( QColorGroup::Base, config.colore_selezione );

  QTable::paintCell( p, row, col, cr, selected, _cg );

}

tr_time::tr_time( QWidget*parent, const char*name ):QWidget( parent, name ) {
  setPaletteBackgroundColor( parent->paletteBackgroundColor() );
  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

  layout=NEW QGridLayout( this, 5, 2 );

  qnibbles=NEW tr_table_intervalli( this, "nibbles" );


  connect( qnibbles, SIGNAL( currentChanged( int, int ) ), this, SLOT( qnibbles_currentChanged( int, int ) ) );
  connect( qnibbles, SIGNAL( clicked( int, int, int, const QPoint& ) ),
     this, SLOT( qnibbles_click_o_press( int, int, int, const QPoint& ) ) );

  layout->addMultiCellWidget( qnibbles, 0, 6, 0, 0 );

  tr_label**w[]= {&su,&sp1,&sx,&sp2,&dx,&sp3,&giu,NULL};
  QString nomi[]= {"su", "sp1","sx","sp2","dx","sp3","giu"};
  QString icone[]= {"az_su.png", "", "az_sx.png", "", "az_dx.png", "", "az_giu.png"};
  for ( int i=0; w[i]!=NULL; i++ ) {
    tr_label*p=NEW tr_label( this, nomi[i] );
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
}

tr_time::~tr_time() {
}

void tr_time::dir_clicked() {
  tr_label*s=( tr_label* ) sender();
  if ( qstrcmp( s->name(), "su" )==0 )
    qnibbles->verticalScrollBar()->setValue( qnibbles->verticalScrollBar()->value()-config.altezza_riga );
  if ( qstrcmp( s->name(), "giu" )==0 )
    qnibbles->verticalScrollBar()->setValue( qnibbles->verticalScrollBar()->value()+config.altezza_riga );
  if ( qstrcmp( s->name(), "sx" )==0 )
    qnibbles->horizontalScrollBar()->setValue( qnibbles->horizontalScrollBar()->value()-config.altezza_riga );
  if ( qstrcmp( s->name(), "dx" )==0 )
    qnibbles->horizontalScrollBar()->setValue( qnibbles->horizontalScrollBar()->value()+config.altezza_riga );
}

void tr_time::qnibbles_currentChanged( int row, int col ) {
  ccc=true;
  QString p="";
  int i, j;
  if ( row==0&&col==0 ) {
    if ( qnibbles->text( 1, 1 )=="" ) p=" ";
    for ( j=1; j<qnibbles->numRows(); j++ )
      for ( i=1; i<qnibbles->numCols(); i++ ) qnibbles->setText( j, i, p );
    return;
  }

  if ( row>0&&col>0 ) {
    if ( qnibbles->text( row, col )==" " ) qnibbles->setText( row, col, "" );
    else
      qnibbles->setText( row, col, " " );
    return;
  }

  int c;
  enum {
    vert, oriz
  }
  dir;
  int section;
  if ( row==0 ) {
    section=col;
    dir=vert;
  }
  else {
    section=row;
    dir=oriz;
  }
  if ( dir==oriz ) c=qnibbles->numCols();
  else c=qnibbles->numRows();

  if ( dir==vert&&qnibbles->text( 1, section )=="" ) p=" ";
  if ( dir==oriz&&qnibbles->text( section, 1 )=="" ) p=" ";

  for ( i=1; i<c; i++ ) {
    if ( dir==vert ) qnibbles->setText( i, section, p );
    else
      qnibbles->setText( section, i, p );
  }
}

void tr_time::qnibbles_click_o_press( int row, int col, int, const QPoint& ) {
  if ( !ccc )
    if ( row==qnibbles->currentRow()&&col==qnibbles->currentColumn() ) qnibbles_currentChanged( row, col );
  ccc=false;
}

char*tr_time::intervalli() {
  return NULL;
}
