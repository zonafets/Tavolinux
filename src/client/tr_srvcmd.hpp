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

#ifndef TR_SRVCMD_HPP
  #define TR_SRVCMD_HPP

  #include <iostream>
using namespace std;

  #include <sys/poll.h>


  #include <cstdlib>

  #include <qtimer.h>
  #include <qstring.h>
  #include <qcstring.h>
  #include <qwidget.h>
  #include <qlistview.h>
  #include <qlabel.h>
  #include <qsocketdevice.h>
  #include <qtextstream.h>
  #include <qstringlist.h>
  #include <qprocess.h>

  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/select.h>
  #include <sys/wait.h>
  #include <sys/un.h>
  #include <netinet/in.h>
  #include <unistd.h>

  #include "tr_common.hpp"
  #include "tr_enumeratori.hpp"

class tr_tavoli_view;
  #include "cmdlangtoken.hpp"


  #define CLIENT
  #include "cmddiritti.hpp"

  #ifndef MAIN_CPP
class tr_srvcmd;
extern tr_srvcmd*srv;
  #endif

  #include "tr_globals.hpp"
extern tr_globals glb;



  #define LEAVE QString("§")
  #define FLAGS(x) (glb.flags[TRDiritti::x-1]=='1')
  #define MODULO(x) (glb.moduli[(int)x]=='1')
  #define flags(x) (x==TRDiritti::p_none || glb.flags[x-1]=='1')

////////////////////////////
// SERVER UDP //
//////////////////////////

class tr_server_udp:public QObject {
  Q_OBJECT;



public:
  tr_server_udp();
  ~tr_server_udp();
  QValueList<tr_tavoli_view*>tavoli;

  public slots:void timer();

private:
  friend class tr_srvcmd;
  QTimer*_timer;
  QSocketDevice*_udp;
  void log( QString st );
};



/////////////////////////////
// SERVER CMD //
///////////////////////////


// Classe di interfaccia con il server

class tr_srvcmd:public QObject {
  Q_OBJECT;



public:

  tr_srvcmd( bool usa_udp, bool usa_unix_socket );
  tr_srvcmd( short porta, bool usa_udp, bool usa_unix_socket );
  ~tr_srvcmd();

  enum tr_stato {
    err_connessione=1, err_comando=2, err_dati_comando=4, err_secondo=8, // quando l'errore precedente non �stato gestito
    err_cmd, ok=0
  };

  void msg( QString _msg );

  // Definizione dei metodi
  void cmd( QString direct_cmd );

  // svuota dati serve fino a eop
  bool eop();
  QString rcv();

  // legge la successiva stringa dal server
  QString rcv( TRTokens tok, QString params );

  // legge le successive stringhe fino all'EOD
  QStringList rcv_list();

  QStringList rcv_list( TRTokens tok, QString params=QString::null );
  QStringList log_buffer;

  /* log dei comandi e risposte */

  tr_stato stato() {
    return _stato;
  }

  void stato( tr_stato s ) {
    _stato=s;
  }

  void tavoli( tr_tavoli_view*t ) {
    if ( srv_udp ) {
      if ( t ) srv_udp->tavoli.append( t );
      else
        srv_udp->tavoli.pop_back();
    }
  };

  tr_tavoli_view*tavoli() {
    return srv_udp?srv_udp->tavoli.back():NULL;
  };

  bool silent;
  bool nolog;

  QString last_rcv() {
    return _last_rcv;
  }

  void debug( TRTokens t=tok_endoftokens ) {
    _debug=t;
  }

private:

  TRTokens _debug;
  bool _debug_on;

  bool cmd( TRTokens id, QStringList parametri, bool managed );

  bool cmd( TRTokens id, QString p1=QString::null, QString p2=QString::null, QString p3=QString::null, QString p4=
     QString::null, QString p5=QString::null, QString p6=QString::null, QString p7=QString::null, QString p8=QString::null,
     QString p9=QString::null, QString p10=QString::null, QString p11=QString::null, QString p12=QString::null, QString p13=
     QString::null, QString p14=QString::null, QString p15=QString::null, QString p16=QString::null, QString p17=QString::null,
     QString p18=QString::null, bool managed=false ) {
       QStringList parametri;
       QString p[]= {
      p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18};
       int i=0;
       int k=-1;
       while ( i<COUNT( p ) ) {
         if ( !p[i].isEmpty() ) {
           k=i;
         }
         i++;
       }
       if ( k>=0 ) {
         i=0;
         while ( i<=k ) {
           if ( p[i].isEmpty() ) parametri<<LEAVE;
           else {
             if ( p[i].find( '|' )!=-1 ) {
               parametri+=QStringList::split( '|', p[i], true );
             }
             else
               parametri<<p[i];
           }
           i++;
         } // while
       }
       return cmd( id, parametri, managed );
  }

  friend bool cmd( TRTokens id, QStringList s );
  friend bool CMD( TRTokens id, QStringList s );

  friend bool cmd( TRTokens id, QString p1, QString p2, QString p3, QString p4, QString p5, QString p6, QString p7, QString p8,
     QString p9, QString p10, QString p11, QString p12, QString p13, QString p14, QString p15, QString p16,
     QString p17, QString p18 );

  friend bool CMD( TRTokens id, QString p1, QString p2, QString p3, QString p4, QString p5, QString p6, QString p7, QString p8,
     QString p9, QString p10, QString p11, QString p12, QString p13, QString p14, QString p15, QString p16,
     QString p17, QString p18 );

  void genera_comandi();
  QStringList comandi;

  QString _last_rcv;

  QString falso( QString tabella_campo );

  bool _usa_std_err;
  tr_stato _stato;
  bool _eop;


  bool connetti( short porta, bool usa_udp, bool usa_unix_socket );

  bool sbuffer( bool wait=true );
  bool data();
  bool wait_data();

  Q_LONG readLine( char*data, Q_ULONG maxlen ) {
    Q_LONG r=0;
    r=unix_socket->readLine( data, maxlen );
    return r;
  }

  Q_LONG readBlock( char*data, Q_ULONG maxlen ) {
    Q_LONG r=0;
    r=unix_socket->readBlock( data, maxlen );
    return r;
  }

  Q_LONG writeBlock( QCString s ) {
    Q_LONG r=0;
    const char *c=s;
    r=unix_socket->writeBlock( s, s.length() );
    unix_socket->flush();
    return r;
  }

  Q_ULONG bytesAvailable() const;

  tr_server_udp*srv_udp;
  QFile*unix_socket;
  int _ifd;

  union {
    struct sockaddr_un indirizzoSocketUnix;
    struct sockaddr_in indirizzoSocketTcpip;
  };

  QStringList lines_buf;

  QString nome_tipo_socket;

  QString tab_risposta;
  QString tab_comando;

  uint log_buffer_maxlines;
  void log( QString st );
};

inline QString rcv( TRTokens tok, QString params ) {
  return srv->rcv( tok, params );
}

inline bool cmd( TRTokens id, QStringList s ) {
  return srv->cmd( id, s, true );
}

inline bool CMD( TRTokens id, QStringList s ) {
  return srv->cmd( id, s, false );
}

inline bool cmd( TRTokens id, QString p1=QString::null, QString p2=QString::null, QString p3=QString::null, QString p4=
   QString::null, QString p5=QString::null, QString p6=QString::null, QString p7=QString::null, QString p8=QString::null,
   QString p9=QString::null, QString p10=QString::null, QString p11=QString::null, QString p12=QString::null, QString p13=
   QString::null, QString p14=QString::null, QString p15=QString::null, QString p16=QString::null, QString p17=QString::null,
   QString p18=QString::null ) {
     return srv->cmd( id, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, true );
}

inline bool CMD( TRTokens id, QString p1=QString::null, QString p2=QString::null, QString p3=QString::null, QString p4=
   QString::null, QString p5=QString::null, QString p6=QString::null, QString p7=QString::null, QString p8=QString::null,
   QString p9=QString::null, QString p10=QString::null, QString p11=QString::null, QString p12=QString::null, QString p13=
   QString::null, QString p14=QString::null, QString p15=QString::null, QString p16=QString::null, QString p17=QString::null,
   QString p18=QString::null ) {
     return srv->cmd( id, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, false );
}


#endif
