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

#ifndef TR_COMMON_H
#define TR_COMMON_H

#include <iostream>
using namespace std;

#include <qstring.h>
#include <qapplication.h>
#include <qdict.h>
#include <qptrlist.h>
#include <qintdict.h>
#include <qptrlist.h>
#include <qptrdict.h>
#include <qlistview.h>
#include <qdatetime.h>
#include <qvariant.h>

#include "tr_dmacro.hpp"

#define BEEP QApplication::beep()
#define BEEPER {QApplication::beep(); return; }
#define BEEPERF {QApplication::beep(); return false; }

#define QS(x) QString(x)
#define QSN(x) QString::number(x)
#define QSNP(x) QString::number(x,'f',2)

inline QString nz(QString s)	{if (s.isNull()) return QString(""); else return s;}

// macro di comodit�per debug
// Parameter
// PrintParameter
// FormName

template < class type >
class tr_Dict : public QDict<type> {
public:
  tr_Dict( int size = 17, bool caseSensitive = TRUE ) :
  QDict<type>( size, caseSensitive ) {}

  tr_Dict( const tr_Dict < type > & d ) : QDict < type > ( d ) {}

  ~tr_Dict() {
    clear();
  }

  tr_Dict < type > & operator = ( const tr_Dict < type > & d ) {
    return ( tr_Dict < type > & ) QDict < type >::operator = ( d );
  }

  void clear() {
    if ( autoDelete() ) {
      QDictIterator < type > it( * this );
      for ( ; it.current(); ++it ) DELETE( it.current() );
      setAutoDelete( false );
      QDict < type >::clear();
      setAutoDelete( true );
    }
    else
      QDict < type >::clear();
  }
};



template < class type >
class tr_IntDict : public QIntDict < type > {
public:
  tr_IntDict( int size = 17 ) : QIntDict < type > ( size ) {
  }

  tr_IntDict( const tr_IntDict < type > & d ) : QIntDict < type > ( d ) {
  }

  ~tr_IntDict() {
    clear();
  }

  tr_IntDict < type > & operator = ( const tr_IntDict < type > & d ) {
    return ( tr_IntDict < type > & ) QIntDict < type >::operator = ( d );
  }

  void clear() {
    bool ad=autoDelete();
    if ( ad ) {
      QIntDictIterator < type > it( * this );
      for ( ; it.current(); ++it ) {DELETE( it.current() );}
      setAutoDelete( false );
      QIntDict < type >::clear();
      setAutoDelete( ad );
    }
    else
      QIntDict < type >::clear();
  }
};


template < class type >
class tr_PtrDict : public QPtrDict < type > {
public:
  tr_PtrDict( int size = 17 ) : QPtrDict < type > ( size ) {}

  tr_PtrDict( const tr_PtrDict < type > & d ) : QPtrDict < type > ( d ) { }

  ~tr_PtrDict() {clear();}

  tr_PtrDict < type > & operator = ( const tr_PtrDict < type > & d ) {
    return ( tr_PtrDict < type > & ) QPtrDict < type >::operator = ( d );
  }

  void clear() {
    bool ad=autoDelete();
    if ( ad ) {
      QPtrDictIterator < type > it( * this );
      for ( ; it.current(); ++it ) {DELETE( it.current() );}
      setAutoDelete( false );
      QPtrDict < type >::clear();
      setAutoDelete( ad );
    }
    else
      QPtrDict < type >::clear();
  }
}; // tr_PtrDict<T>



template < class type >
class tr_PtrList : public QPtrList < type > {
public:

  tr_PtrList() : QPtrList < type > () { }

  tr_PtrList( const tr_PtrList < type > & l ) : QPtrList < type > ( l ) { }

  ~tr_PtrList() {
    clear();
  }

  tr_PtrList < type > & operator = ( const tr_PtrList < type > & l ) {
    return ( tr_PtrList < type > & ) QPtrList < type >::operator = ( l );
  }

  void clear() {
    bool ad=autoDelete();
    if ( ad ) {
      QPtrListIterator < type > it( * this );
      for ( ; it.current(); ++it ) {
        DELETE( it.current() );
      }
      setAutoDelete( false );
      QPtrList < type >::clear();
      setAutoDelete( ad );
    }
    else
      QPtrList < type >::clear();
  }
};

// questo template nell'eliminazione dell'elemento lo rimuove anche dal padre



template < class type >
class tr_widget_PtrList : public QPtrList < type > {
public:

  tr_widget_PtrList() : QPtrList < type > () {
  }

  tr_widget_PtrList( const tr_widget_PtrList < type > & l ) : QPtrList < type > ( l ) {
  }

  ~tr_widget_PtrList() {
    clear();
  }

  tr_widget_PtrList < type > & operator = ( const tr_widget_PtrList < type > & l ) {
    return ( tr_widget_PtrList < type > & ) QPtrList < type >::operator = ( l );
  }

  void append( const type * d ) {
    QPtrList < type >::append( d );
  }

  void clear(QLayout *l=NULL) {
    bool ad=autoDelete();
    if ( ad ) {
      QPtrListIterator < type > it( * this );
      for ( ; it.current(); ++it ) {
        QWidget *w = it.current();
        /*
        if (w) {
          QWidget *p = w->parentWidget();
          if (p) {
            if (p->layout()) {
              QLayout *l = p->layout();
              l->remove( w );
            }
          }
          */
        if (l) l->remove( w );
        DELETE( w );  // il distruttore di QObject lo rimuove anche dal parent
        } // for
        setAutoDelete( false );
        QPtrList < type >::clear();
        setAutoDelete( ad );
      } // if
    else
      QPtrList < type >::clear();
  }
};

inline ostream & operator<< (ostream& o, QRect r)
{
    o << "x=" << r.x() << ",y=" << r.y() << ",w=" << r.width() << ",h=" << r.height() << endl;
    return o;
}

inline ostream & operator<< (ostream& o, QSize s)
{
    o << "w=" << s.width() << ",h=" << s.height() << endl;
    return o;
}

inline ostream & operator<< (ostream& o, QPoint p)
{
    o << "x=" << p.x() << ",y=" << p.y() << endl;
    return o;
}

inline ostream & operator<< (ostream& o, QDate& p)
{
    o << QVariant( p).toString().latin1();
    return o;
}

inline ostream & operator<< (ostream& o, QTime& p)
{
    o << QVariant( p).toString().latin1();
    return o;
}

inline ostream & operator<< (ostream& o, QDateTime& p)
{
    o << QVariant( p).toString().latin1();
    return o;
}

inline ostream & operator<< (ostream& o, QStringList &s)
{
    cout << endl; // salta una riga dopo nome variabile
    uint i=0;
    QString st;
    while (i<s.size()) {
        o << i << " = ";
        st = s[i++];
        if (!st.isNull()) o  << st.latin1() << endl;
        else o<<"(null)\n";
    }
    return o;
}

inline ostream & operator<< (ostream& o, QColor& c)
{
    o << QVariant( c).toString().latin1();
    return o;
}

extern ostream & operator<< (ostream& o, QListView& lv);

/*
inline ostream & operator<< (ostream& o, QColor s)
{
    o << s.name().latin1();
    return o;
}
*/

inline bool operator <(QSize s, QSize s1)
{
    if (s.width()<s1.width() && s.height()<s1.height()) return true; else return false;
}

inline bool operator >(QSize s, QSize s1)
{
    if (s.width()>s1.width() && s.height()>s1.height()) return true; else return false;
}


#endif
