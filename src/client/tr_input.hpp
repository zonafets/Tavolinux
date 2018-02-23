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

#ifndef TR_INPUT_H
  #define TR_INPUT_H

  #include <qobject.h>
  #include <qwidget.h>
  #include <qdatetime.h>
  #include <qlayout.h>
  #include <qvariant.h>
  #include "tr_lineedit.hpp"
  #include "tr_label.hpp"
  #include "money.hpp"
  #include "tr.hpp"
  #include "tr_listview.hpp"
  #include "tr_config.hpp"

class tr_input:public tr_widget {
  Q_OBJECT

  public:

  explicit tr_input( QWidget*parent, const QString&name, QString etichetta, QString valore, QString validatore=
     QString::null, bool alato=false );

  static const int margine=0;
  static const int spazio=4;

  enum tr_tipo {
    tp_data, tp_ora, tp_numero, tp_stringa, tp_elenco
  };

  // funzioni che leggono il valore di tipo specifico;
  QString text();
  QString dataiso( bool yeszerotime=false );
  // QDate data();
  // QTime ora();
  QString stringa();

  double numero() {
    return text().toDouble();
  }

  void setLabel( QString v );
  void setCheck( bool abilita_su_on=true );
  void setValidatore( QString val );
  void setText( QString v );
  void setText( QDate v );
  void setText( QTime v );
  void setText( int v );
  void setText( double v );

  bool isNull() {
    return text().isNull();
  }

  bool isEmpty() {
    return text().isEmpty();
  }

  void setReadOnly( bool v );

  void setSelection( int s, int l ) {
    edit->setSelection( s, l );
  }

  bool modificato();

  void upcase( bool uc ) {
    edit->upcase( uc );
  }

  bool upcase() {
    return edit->upcase();
  }

  void setRegExp( QString rx ) {
    edit->setRegExp( rx );
  }

  void adatta() {
    edit->adatta();
  }

  void show();

  void setEnabled( bool stato );

  tr_input&operator=( const tr_input&in );

  public slots:void icona_clicked();
  void edit_textChanged( const QString& );
  void edit_textUpdated();
  void checked();

public:
  tr_tipo _tipo;
  QString _validatore;
  tr_label*label;
  tr_lineedit*edit;
  tr_label*icona;
  QString _originale;
  bool barcode;
  bool onkey;
  bool onupdate;
  bool into_keyboard;
  bool clear_on_link;

  QString id() {
    return _elenco.id;
  }

  void id( QString _id ) {
    _elenco.id=_id;
    if ( _id.isEmpty() ) descrizione( "" );
  }

  QString descrizione() {
    return text();
  }

  void descrizione( QString d ) {
    setText( d );
  }

  void setText( TRTokens tok, QString _id ) {
    if ( _id.isEmpty() ) {
      id( "" );
      descrizione( "" );
    }
    else {
      tr::mse elenco( tok, _id );
      id( elenco.id() );
      descrizione( elenco.descrizione() );
    }
  }



  class tr_elenco {
  public:
    tr_elenco() {
      nkeys=0;
      token=QString::null;
      tp=tr_listview::indefinito;
    };

    QStringList elenco;
    QString struttura;
    QString token;
    int nkeys;
    QString id;
    tr_listview::tr_tipo tp;
  }
  _elenco;



  void elenco( const QStringList, const QString struttura=QString::null );

  void tok( TRTokens _tok, QString parametri=QString::null, int nkeys=0 );

  void tp( tr_listview::tr_tipo tp ) {
    _elenco.tp=tp;
    _tipo=tp_elenco;
    if ( icona ) icona->setPixmap( PIXMAP( "az_elenco.png" ) );
  }

  void costruzione();

signals:
  void clicked();
  void textChanged( const QString& );
  void barcodeReaded();
  void textUpdated();
  //void returnPressed ();
  //void lostFocus ();
  //void selectionChanged () ;


};
#endif
