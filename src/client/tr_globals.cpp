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


// variabili globali
#include <qwidget.h>
#include <qlistview.h>
#include <qmemarray.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qsqldatabase.h>
#include <qmime.h>
#include <qdragobject.h>
#include <qimage.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <qsocketdevice.h>
#include <qdatetime.h>
#include <qdict.h>
#include <qlcdnumber.h>
#include <qobjectlist.h>
#include "tr_config.hpp"
#include "tr_srvcmd.hpp"
#include "tr_tools.hpp"
#include "tr_widget.hpp"
#include "tr_globals.hpp"

tr_globals::tr_globals() {
  menu_popup=NULL;
  modo_bar=false;
  flags="";

  for ( int i=0; i<p_stato_last; i++ ) stato<<"";
  stato[tr_globals::p_listino_id]="1"; // listino base

  init_flags();
  init_modules();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_globals::init_flags() {
  while ( flags.length()<TRDiritti::p_last ) flags+='0';
  flags.fill( '0' );
}

void tr_globals::init_modules() {
  while ( moduli.length()<TRDiritti::p_last ) moduli+='0';
  moduli.fill( '0' );
}


void log( QString st ) {
  cout<<( st )<<endl;
}

void palette_dark( QPalette*p, int fattore ) {
  const QColorGroup::ColorRole gruppo_colori[17]= {
	QColorGroup::Base,
		QColorGroup::Foreground,
		QColorGroup::Background,
		QColorGroup::Text,
		QColorGroup::Button,
		QColorGroup::ButtonText,
		QColorGroup::Light,
		QColorGroup::Midlight,
		QColorGroup::Dark,
		QColorGroup::Mid ,
		QColorGroup::Shadow,
		QColorGroup::Highlight,
		QColorGroup::HighlightedText,
		QColorGroup::BrightText,
		QColorGroup::Link,
		QColorGroup::LinkVisited,
		QColorGroup::NColorRoles};
  // const QColorGroup *_cg=NEW QColorGroup(palette().disabled());
  QColorGroup cg( p->normal() );
  for ( int i=0; i<16; i++ )
    cg.setColor( gruppo_colori[i], cg.color( gruppo_colori[i] ).dark( fattore ) );
  p->setDisabled( cg );
}

void palette_dark_background( QWidget*w, int fattore ) {
  QPalette p=w->palette();
  QColorGroup cg( p.normal() );
  QColorGroup cg_dark( p.disabled() );
  cg_dark.setColor( QColorGroup::Background, cg.color( QColorGroup::Background ).dark( fattore ) );
  p.setDisabled( cg_dark );
  w->setPalette( p );
}

void palette_dark( QWidget*w[], int fattore ) {
  int i=0;
  while ( w[i]!=NULL ) {
    QPalette p=w[i]->palette();
    palette_dark( &p, fattore );
    w[i]->setPalette( p );
    i++;
  }
};

void palette_dark( QWidget*w, int fattore ) {
  QPalette p=w->palette();
  palette_dark( &p, fattore );
  w->setPalette( p );
};

/* scopo:	prende la palette dell'oggetto e la schiarisce di un fattore 200 */
void palette_light( QPalette*p, int fattore ) {
  const QColorGroup::ColorRole gruppo_colori[17]= {
	QColorGroup::Base,
		QColorGroup::Foreground,
		QColorGroup::Background,
		QColorGroup::Text,
		QColorGroup::Button,
		QColorGroup::ButtonText,
		QColorGroup::Light,
		QColorGroup::Midlight,
		QColorGroup::Dark,
		QColorGroup::Mid ,
		QColorGroup::Shadow,
		QColorGroup::Highlight,
		QColorGroup::HighlightedText,
		QColorGroup::BrightText,
		QColorGroup::Link,
		QColorGroup::LinkVisited,
		QColorGroup::NColorRoles};
  // const QColorGroup *_cg=NEW QColorGroup(palette().disabled());
  QColorGroup cg( p->normal() );
  for ( int i=0; i<16; i++ )
    cg.setColor( gruppo_colori[i], cg.color( gruppo_colori[i] ).dark( fattore ) );
  p->setDisabled( cg );
}

void palette_light_background( QWidget*w, int fattore ) {
  QPalette p=w->palette();
  QColorGroup cg( p.normal() );
  QColorGroup cg_dark( p.disabled() );
  cg_dark.setColor( QColorGroup::Background, cg.color( QColorGroup::Background ).dark( fattore ) );
  p.setDisabled( cg_dark );
  w->setPalette( p );
}

void palette_light( QWidget*w[], int fattore ) {
  int i=0;
  while ( w[i]!=NULL ) {
    QPalette p=w[i]->palette();
    palette_light( &p, fattore );
    w[i]->setPalette( p );
    i++;
  }
};

void palette_light( QWidget*w, int fattore ) {
  QPalette p=w->palette();
  palette_light( &p, fattore );
  w->setPalette( p );
};

/* QDict<QPixmap> img_cache;

QPixmap& uic_load_pixmap( const QString name ) { QPixmap *pix= img_cache.find(name); if (pix) return *pix;
qDebug("carica :" + name); pix = NEW QPixmap("./images/" + name);;
if (pix->isNull()) qDebug("Immagine " + name + " non trovata"); img_cache.insert(name, pix); return *pix; } */

ostream&operator<<( ostream&o, QListView&lv ) {

  QListViewItemIterator it( &lv );

  while ( it.current() ) {
    cout<<it.current()->text( 0 )<<endl;
    ++it;
  }

  return o;
}
