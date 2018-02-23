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

#ifndef TR_TOOLS_H
  #define TR_TOOLS_H

  #include <qobject.h>
  #include <qwidget.h>
  #include <qstring.h>
  #include <qregexp.h>
  #include <qpixmap.h>
  #include <qstringlist.h>
  #include <qdatetime.h>
  #include <qptrstack.h>

  #include "frm_base.hpp"
  #include "frm_calcolatrice.hpp"
  #include "frm_calendario.hpp"
  #include "frm_tastiera.hpp"
  #include "frm_wait.hpp"
  #include "frm_scelta.hpp"
  #include "tr_tavoli_view.hpp"
  #include "tr_enumeratori.hpp"
  #include "tr_label.hpp"

  #define LOG(x) tools.log(QString(x))
  #define NZ(x) tools.nz(x)

  #define SINO tr("si#si|no#no")
  #define SIANNULLANO tr("si#si|annulla#annulla|no#no")
  #define SI "si"
  #define ANNULLA "annulla"
  #define NO "no"

class tr_tools:public QObject, public tr::enumeratori {
  Q_OBJECT public:

  tr_tools::tr_tools() {
    log_event_buffer_maxlines=100;
  }

  ///////////////////////////////////////////////////////////////////////////
  // funzioni di pubblica utilita' radunate qui per comodita' organizzativa

  static QString tr_tools::label_to_name( QString etichetta );
  static QString tr_tools::solo_testo( QString st );
  static bool tr_tools::sillaba( QChar corrente, QChar succ );
  static QString stringa( QByteArray t );
  static QByteArray tr_tools::ba( QString st );
  static QString inverti_data( QString s, char separatore='/' );

  QString nz( QString s ) {
    return s.isNull()?"" :s;
  }

  QString nz( QString s, QString val ) {
    return s.isEmpty()?val:s;
  }

  static void activateAllLayouts( QWidget*w );

  ///////////////////////////////////////////////////////////////////////////

  void setPalette( frm_base*form );
  tr_palette palette( frm_base*form );
  tr_palette palette( int livello );

  static QString conforma_data( QString s );
  bool data_valida( QString s, QDate*d );
  bool ora_valida( QString s, QTime*t );
  bool dataora_valida( QString s, QDateTime*dt );
  QString data_dataiso( QString data, bool yeszerotime=false );
  QString dataiso_data( QString dataiso, bool yeszerotime=false );

  QString data( QDate d=QDate::currentDate() ) {
    return dataiso_data( d.toString( Qt::ISODate ) );
  }

  QString validatore( QString tabella_campo );
  QString segna_posto( QString validatore );

  QString tastiera( QWidget*parent, QString titolo, QString valore_iniziale, QRegExp rx, bool pwd=false,
     tr_input::tr_elenco elenco=tr_input::tr_elenco() );
  QString tastiera( QWidget*parent=0, QString titolo="", QString valore_iniziale="", QString regexp=
     QString::null, bool pwd=false );
  QString tastiera( QString titolo );
  QString tastiera( QString titolo, QRegExp rx );
  QString tastiera( QWidget*parent, tr_input*le /* , QString titolo="" */ );
  QString input_descrizione( QWidget*parent=0, QString titolo="", QString valore_iniziale="", bool password=false );



  struct tr_elenco {
    QString record;
    QString id;
    QString descrizione;
  };



  tr_elenco elenco( const QStringList elenco, QString titolo=QString::null, const QString struttura=QString::null );

  QString elenco( tr_input*in );


  frm_wait*wait( QWidget*parent=NULL, QString titolo=QString::null );

  void wait( int millisecondi );

  QString msgbox( QWidget*parent, QString testo, QString titolo=QString::null, QString pulsanti=
     QString::null, int wait_seconds=-1 ); // default = tr("si")

  QString msgbox( QString testo, int wait_seconds=-1 ) {
    return msgbox( NULL, testo, QSNull, QSNull, wait_seconds );
  }

  QString msgbox( QString testo, QString pulsanti, int wait_seconds=-1 ) {
    return msgbox( NULL, testo, QSNull, pulsanti, wait_seconds );
  }

  QString msgbox( QString testo, QString titolo, QString pulsanti, int wait_seconds=-1 ) {
    return msgbox( NULL, testo, titolo, pulsanti, wait_seconds );
  }

  bool calendario( QWidget*parent, QDate*data );
  bool calendario( QLineEdit*le );
  QString calcolatrice( QWidget*parent, const QString&titolo, QString valore, QString validatore=QString::null,
     bool virgola_mobile=false );
  QString calcolatrice( QWidget*parent, const QRect&pos, const QString&titolo, QString valore, QString validatore=
     QString::null, bool virgola_mobile=false );
  QString calcolatrice( tr_input*input, const QString&titolo, QString validatore, bool virgola_mobile=false );
  QString hvmenu( tr_label*pulsante, QString str_menu, TRDiritti::TRDirittiUtente diritti[]=NULL );

  ////////////////////////////////////////////////////////////////////////////////////////////////////////

  bool scelta( tr_listview::tr_tipo tipo, QString&id, QString&descrizione, QString&record, QString gruppo=
     QString::null, bool imposta=true );

  bool scelta( tr_listview::tr_tipo tipo, QString&id, QString&descrizione ) {
    QString niente="";
    return scelta( tipo, id, descrizione, niente );
  }

  bool scelta( QString gruppo, tr_listview::tr_tipo tipo, QString&id, QString&descrizione ) {
    QString niente="";
    return scelta( tipo, id, descrizione, niente, gruppo );
  }

  bool removeWin( QWidget*set );
  void actWin( QWidget*set );
  QWidget*actWin();
  frm_base*find_right_base_parent( frm_base*new_window=NULL );
  frm_base*find_base( tr_widget*w );

  ////////////////////////////////////////////////////////////////////////////////////////////////////////

  void log( QString st );
  uint log_event_buffer_maxlines;
  QStringList log_event_buffer;

private:
  bool print( tr_tavolo*t, QString tipo_conto, QString nuovo_totale=QString::null );

  QPtrStack<QWidget>win_queue;

public:
  bool conto_clicked( tr_label*pulsante, tr_tavolo*t, QStringList stato_tavolo, double totale=0 );

  QString barcode( QString txt=QString::null );

  QString get_cmd_struct( TRTokens tk, int pos_desc=-1, int pos_key=-1 );

  void sleep(int ms);

};
#endif
