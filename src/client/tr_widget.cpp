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


#include "tr_widget.hpp"
#include "tr_dmacro.hpp"
#include "tr_tools.hpp"

#include <qapplication.h>
#include <qfocusdata.h>
#include <qobjectlist.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      tr_widget
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern QWidget*menu_popup;
extern tr_tools tools;

QWidget::~QWidget() {
  QFocusData*f=focusData( FALSE );
  if ( f ) {
    QPtrListIterator<QWidget>it( f->focusWidgets );
    QWidget*w;
    while ( ( w=it.current() ) ) {
      ++it;
      QWidget*p=w;
      while ( p&&p!=this )
        p=p->parentWidget();
      if ( p ) // my descendant
           f->focusWidgets.removeRef( w );
    }
  }
  --instanceCounter;

  if ( QApplication::main_widget==this ) { // reset main widget
    QApplication::main_widget=0;
    if ( qApp )
      qApp->quit();
  }

  if ( hasFocus() )
    clearFocus();

  if ( isTopLevel()&&isShown()&&winId() )
    hide();

  if ( childObjects ) {
    QObjectListIt it( *childObjects );
    QObject*obj;
    while ( ( obj=it.current() ) ) {
      ++it;
      obj->parentObj=0;
      childObjects->removeRef( obj );
      DELETE( obj );
    }
    delete childObjects;
    childObjects=0;
  }

  QApplication::removePostedEvents( this );
  if ( extra ) {
    deleteExtra();
  }

  destroy();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_widget::~tr_widget() {
  if ( links ) {
    DELETE_ARRAY( links );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_widget::setMinMax( int min_w, int min_h, int max_w, int max_h ) {
  if ( max_w==-1 ) max_w=min_w;
  if ( max_h==-1 ) max_h=min_h;
  setMinimumSize( min_w, min_h );
  setMaximumSize( max_w, max_h );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_widget::link( QWidget*ws[], bool su_stato ) {

  link_su_stato=su_stato;
  int i=0;
  n_links=0;
  for ( i=0; ws[i]!=NULL; i++ ) n_links++;

  NEW( links, tr_link[n_links] );

  for ( i=0; i<n_links; i++ ) {
    links[i].w=ws[i];
    links[i].stato=true;
  }
}

void tr_widget::link_clicked( bool stato ) {
  if ( !links ) return;

  for ( int i=0; i<n_links; i++ ) {
    tr_link*l=&links[i];
    if ( stato==link_su_stato||!l->stato ) {
      l->w->setEnabled( false );
      if ( qstrcmp( l->w->className(), "tr_label" )==0 ) {
        tr_label*lb=( tr_label* ) l->w;
        if ( lb->aspetto()==tr_label::pulsante ) lb->hide();
      }
      if ( qstrcmp( l->w->className(), "tr_input" )==0 ) {
        tr_input*lb=( tr_input* ) l->w;
        if ( lb->clear_on_link ) lb->setText( "" );
      }
    }
    //else
    //{
    //l->w->setEnabled( true );
    //}
  }
}

/* bool tr_widget::event(QEvent *e) { return QWidget::event(e); } */

void tr_widget::setEnabled( bool stato ) {

  QWidget::setEnabled( stato );
  /* if ( _darkable ) //if (!ha_pixmap()) { if ( backgroundMode()!=Qt::NoBackground ) { if ( stato ) {
  setPaletteBackgroundColor( palette().active().background() ); } if ( !stato ) {
  setPaletteBackgroundColor( palette().disabled().background() ); } } */
}
