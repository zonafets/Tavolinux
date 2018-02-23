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

#ifndef TR_WIDGET_H
  #define TR_WIDGET_H

  #include <qobject.h>
  #include <qwidget.h>
  #include <qframe.h>
  #include <qsize.h>

// MAX Size
  #define SMAX 32767
  #define TR_WIDGET(x) NEW ( x , tr_widget( this, #x ) )
  #define TR_FRAME(x) NEW ( x , tr_frame( this, #x ) )

extern void palette_dark( QPalette*p, int fattore=200 );
extern void palette_light( QPalette*p, int fattore=200 );
extern void palette_dark( QWidget*w, int fattore=200 );
extern void palette_light( QWidget*w, int fattore=200 );
extern void palette_dark( QWidget*w[], int fattore=200 );
extern void palette_light( QWidget*w[], int fattore=200 );
extern void palette_dark_background( QWidget*w, int fattore=200 );
extern void palette_light_background( QWidget*w, int fattore=200 );



class tr_widget:public QWidget {
  Q_OBJECT;



public:
  tr_widget( QWidget*parent=0, const QString&name=QString::null, WFlags f=0 );
  ~tr_widget();
  void setMinMax( int min_w, int min_h, int max_w=-1, int max_h=-1 );

  void setMinMax( QSize s ) {
    setMinMax( s.width(), s.height() );
  }

  virtual void link( QWidget*ws[], bool su_stato=false );
  void link_clicked( bool stato );



  struct tr_link {
    QWidget*w;
    bool stato;
  }
  *links;



  int n_links;
  bool link_su_stato;

  bool darkable() {
    return _darkable;
  }

  void darkable( bool d ) {
    _darkable=d;
  }

  void setEnabled( bool stato );

protected:

  bool _darkable;

private:
};



inline tr_widget::tr_widget( QWidget*parent, const QString&name, WFlags f ):QWidget( parent, name, f ) {
  links=NULL;
  // setMouseTracking(true);
}



class tr_frame:public QFrame {
  Q_OBJECT;



public:
  tr_frame( QWidget*parent=0, const QString&name=QString::null, WFlags f=0 ):QFrame( parent, name, f ) {
  };

  void setMinMax( int min_w, int min_h, int max_w=-1, int max_h=-1 ) {
    if ( max_w==-1 ) max_w=min_w;
    if ( max_h==-1 ) max_h=min_h;
    setMinimumSize( min_w, min_h );
    setMaximumSize( max_w, max_h );
  }

  void setMinMax( QSize s ) {
    setMinMax( s.width(), s.height() );
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
