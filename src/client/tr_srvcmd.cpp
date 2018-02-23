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

#define CMDSRV_LANGUAGE
#include "tr_srvcmd.hpp"

#include <qapplication.h>
#include <qlineedit.h>
#include <qtextcodec.h>
#include <qdatetime.h>

#include <qstring.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>

#include "tr_tavoli_view.hpp"
#include "tr_tools.hpp"
#include "tr_globals.hpp"
#include "cmdpaycard.hpp"

#define MAX_RX_BUF_SIZE			4096

#ifdef DBGSRVB
#define DBGSRV // indent+=" ";cout<<indent<<__FUNCTION__<<":"<<__LINE__<<endl;
#define DBGSRVEND //indent.truncate(indent.length()-1)
#else
#define DBGSRV
#define DBGSRVEND
#endif


static QString indent="";

extern tr_tools tools;
extern tr_globals glb;

extern void main_exit();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_srvcmd::genera_comandi() {
  for ( int i=0; i<tok_endoftokens; i++ ) comandi<<"";
  for ( int i=0; ::comandi[i].token!=tok_endoftokens; i++ )
    comandi[::comandi[i].token]=::comandi[i].istruzione;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_srvcmd::log( QString st ) {

  static bool puntini=false;
  bool local=false;

  if ( st.left( tab_risposta.length() )==tab_risposta ) {
    st=st.replace( 0, tab_risposta.length(), ' ' );
    local=true;
  }

  if ( st.left( tab_comando.length() )==tab_comando ) {
    st=st.mid( 1 );
    local=true;
  }

  if ( local ) {
    if ( config.verbose>0 ) {
      if ( !nolog ) {
        cout<<st<<endl;
        puntini=false;
      }
      else {
        if ( !puntini ) {
          cout<<" ..." <<endl;
          puntini=true;
        }
      }
    }
  }
  else
    LOG( st ); // passa al log globale

  log_buffer<<st;
  if ( log_buffer.count()>log_buffer_maxlines ) log_buffer.pop_front();

}

Q_ULONG tr_srvcmd::bytesAvailable() const {
  DBGSRV;
  struct pollfd p;
  p.fd=_ifd;
  p.events=POLLIN|POLLPRI;
  DBGSRVEND;
  if ( poll( &p, 1, 10 )>0&&p.revents&POLLIN ) return 1;
  return 0;
}

QString tr_srvcmd::rcv( TRTokens tok, QString params ) {
  DBGSRV;
  QStringList ps;
  if ( !params.isNull() ) ps=QStringList::split( '|', params );
  if ( cmd( tok, ps, true ) ) {
    QString _rcv=rcv();
    if ( _rcv=="EOP" ) _eop=true;
    eop();
    DBGSRVEND;
    return _rcv;
  }
  else {
    DBGSRVEND;
    return "";
  }
};

QStringList tr_srvcmd::rcv_list( TRTokens tok, QString params ) {
  DBGSRV;
  QStringList ps;
  if ( !params.isNull() ) ps=QStringList::split( '|', params );
  if ( cmd( tok, ps, true ) ) {
    QStringList ls=rcv_list();
    eop();
    DBGSRVEND;
    return ls;
  }
  else {
    DBGSRVEND;
    return QStringList();
  }
}

tr_srvcmd::tr_srvcmd( bool usa_udp, bool usa_unix_socket ) {
  connetti( config.server_comande_porta, usa_udp, usa_unix_socket );
}

tr_srvcmd::tr_srvcmd( short porta, bool usa_udp, bool usa_unix_socket ) {
  connetti( porta, usa_udp, usa_unix_socket );
}

bool tr_srvcmd::connetti( short porta, bool usa_udp, bool usa_unix_socket ) {

  _stato=ok;
  srv_udp=NULL;
  _debug_on=false;
  _debug=tok_endoftokens;

  tab_risposta="____";
  tab_comando="!";
  log_buffer_maxlines=24*10;
  unix_socket=NULL;
  _eop=false;
  silent=false;
  nolog=false;

  if ( !usa_unix_socket ) nome_tipo_socket="tcpip";
  else
    nome_tipo_socket="unix";

  log( "Sto aprendo la connessione su socket " +nome_tipo_socket );

  if ( config.verbose>0 ) {
    log( " porta:" +QSN( porta ) );
    if ( usa_udp ) log( " udp ABILITATO" );
    else
      log( " udp NON abilitato" );
  }

  int c;
  if ( !usa_unix_socket ) {
    _ifd=::socket( PF_INET, SOCK_STREAM, 0 );
    indirizzoSocketTcpip.sin_family=AF_INET;
    indirizzoSocketTcpip.sin_port=htons( porta );
    struct hostent*host;
    host=gethostbyname( config.server_addr );
    memcpy( &indirizzoSocketTcpip.sin_addr, host->h_addr, host->h_length );
    c=::connect( _ifd, ( const sockaddr* )&indirizzoSocketTcpip, sizeof( indirizzoSocketTcpip ) );
  }
  else {
    _ifd=::socket( PF_UNIX, SOCK_STREAM, 0 );
    indirizzoSocketUnix.sun_family=AF_UNIX;
    strcpy( indirizzoSocketUnix.sun_path, "/tmp/tavolinux" );
    c=::connect( _ifd, ( const sockaddr* )&indirizzoSocketUnix, sizeof( indirizzoSocketUnix ) );
  }

  if ( c!=-1 ) {
    NEW( unix_socket, QFile() );
    unix_socket->open( IO_ReadWrite|IO_Raw, _ifd );
  }
  else {
    log( tr( "connessione " )+nome_tipo_socket+tr( " non attivata" ) );
    stato( err_connessione );
    return false;
  }

  if ( usa_udp ) {
    NEW( srv_udp, tr_server_udp() );
  }
  else
    log( tr( "connessione UDP non attivata" ) );

  genera_comandi();
  return true;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_srvcmd::~tr_srvcmd() {

  if ( !stato() ) cmd( tok_quit );

  if ( srv_udp ) {
    DELETE( srv_udp );
    log( "ho deallocato srv_udp" );
  }

  if ( unix_socket ) {
    DELETE( unix_socket );
    log( tr( "ho chiuso il socket " )+nome_tipo_socket );
    close( _ifd );
  }

}


QStringList tr_srvcmd::rcv_list() {
  DBGSRV;
  QString st;
  QStringList data;
  if ( _eop ) {
    DBGSRVEND;
    return data;
  }
  while ( ( st=rcv() )!=config.cmd_eod ) data<<st;
  DBGSRVEND;
  return data;
}


inline bool tr_srvcmd::eop() {

  if ( _eop ) return true;

  DBGSRV;

  QString st;
  if ( wait_data() ) {
    QTime t;
    frm_wait*w=NULL;
    t.start();
    while ( ( st=rcv() )!=config.cmd_eop ) {
      if ( !w&&t.elapsed()>2000 )
        w=tools.wait();
      if ( w&&( ( t.elapsed()%250 )==0 ) ) w->avanza();
      if ( w&&t.elapsed()>=config.timeout_tcp ) {
        cout<<TR( "Comunicazione interrotta con il server\n" );
        stato( err_dati_comando );
        break;
      } // if
    } // while
    if ( w ) {
      DELETE( w );
    }

    if ( st!=config.cmd_eop ) main_exit();
    else {
      if ( lines_buf.count()>0 ) {
        cout<<"Buffer non esaurito.\n";
        lines_buf.clear();
      }
    }

  }
  else {
    DBGSRVEND;
    return false;
  }

  _eop=true;
  DBGSRVEND;
  return _eop;

};


bool tr_srvcmd::data() {
  DBGSRV;
  if ( bytesAvailable()>0 ) {
    DBGSRVEND;
    return true;
  }
  DBGSRVEND;
  return false;
};

void tr_srvcmd::cmd( QString direct_cmd ) {
  nolog=false;

  eop();

  _eop=false;

  direct_cmd+="\r\n";
  writeBlock( direct_cmd.utf8() );
  log( tab_comando+direct_cmd );

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool tr_srvcmd::cmd( TRTokens id, QStringList parametri, bool managed ) {
  DBGSRV;

  nolog=false;

  eop();

  _eop=false;

  QString c=comandi[id];

  TROWSLI( parametri ) {
    c+='|';
    if ( parametri[i]!=LEAVE ) c+=parametri[i];
  }

  for ( uint i=0; ::comandi[i].token!=tok_endoftokens; i++ )
    if ( ::comandi[i].token==id&&parametri.count()!=::comandi[i].argomenti ) {
      log( tr( "aggiunti " )+QSN( ::comandi[i].argomenti-parametri.count() )+tr( " parametri fittizi||..." ) );
      while ( ::comandi[i].argomenti>parametri.count() ) {
        parametri<<"|";
        c+="|";
      };
      break;
    }

  if ( _debug==id ) _debug_on=true;
  else
    _debug_on=false;

  log( tab_comando+c ); // messo prima per evitare il doppio \r\n
  c+="\r\n";
  writeBlock( c.utf8() );

  // attende dati e ok
  if ( !wait_data() ) {
    DBGSRVEND;
    return false;
  }


  _last_rcv=rcv(); // legge +OK o -KO

  if ( _last_rcv.left( 3 )!=config.cmd_ok.left( 3 ) ) {
    if ( !silent ) tools.msgbox( _last_rcv.mid( 3 ) );
    DBGSRVEND;
    return false;
  }

  if ( managed ) {
    DBGSRVEND;
    return true;
  }

  bool b=eop();
  DBGSRVEND;
  return b;

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_srvcmd::wait_data() {
  DBGSRV;
  if ( lines_buf.count()>0 ) {
    DBGSRVEND;
    return true;
  }

  QTime t;
  frm_wait*wait=NULL;
  t.start();
  int t_prec=0;
  while ( !data()&&!_stato&&t.elapsed()<config.timeout_tcp ) {
    if ( !wait&&t.elapsed()>2000 ) {
      wait=tools.wait();
      t_prec=t.elapsed();
    }
    if ( wait&&t.elapsed()>t_prec+250 ) {
      wait->avanza();
      t_prec=t.elapsed();
    }
  }
  if ( wait ) {
    DELETE( wait );
  }

  if ( !data()&&( t.elapsed()>=config.timeout_tcp||_stato ) ) {
    log( tr( "timeout in lettura dal server comande" ) );
    _stato=err_dati_comando;
    main_exit();
    DBGSRVEND;
    return false;
  }

  DBGSRVEND;
  return true;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_srvcmd::msg( QString _msg ) {
  DBGSRV;
  _msg+="\n\r";
  srv_udp->_udp->writeBlock( _msg.utf8(), _msg.length() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_server_udp::tr_server_udp():QObject( this, "srv_udp" ) {
  _timer=NULL;
  if ( config.msg_server_tipo==0 ) {
    NEW( _udp, QSocketDevice( QSocketDevice::Datagram ) );
    if ( _udp ) {
      QHostAddress ha;
      _udp->bind( ha, config.server_sync_porta );
    }
  }
  else {
    NEW( _udp, QSocketDevice( QSocketDevice::Stream ) );
    if ( _udp ) {
      QHostAddress ha;
      ha.setAddress( config.msg_server );
      _udp->connect( ha, config.server_sync_porta );
    }
  }

  if ( _udp->error() ) log( QApplication::tr( "Riscontrate difficolta' nella connessione al server comande UDP" ) );
  else {
    _udp->setReceiveBufferSize( 49152 );
    NEW( _timer, QTimer( this ) );
    connect( _timer, SIGNAL( timeout() ), this, SLOT( timer() ) );
    _timer->start( 1000 );
    log( "Porta sincronismo collegata e timer avviato" );
  }

}

tr_server_udp::~tr_server_udp() {
  if ( _timer ) {
    DELETE( _timer );
  }
  if ( _udp ) {
    DELETE( _udp );
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_server_udp::timer() {

  if ( config.lampeggio ) {
    itm_tavolo::lampeggio=!itm_tavolo::lampeggio;
    if ( tavoli.count() ) {
      for ( uint i=0; i<tavoli.count(); i++ ) {
        QCanvasItemList l=tavoli[i]->canvas()->allItems();
        for ( QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it ) {
          itm_tavolo*t=( itm_tavolo* ) ( *it );
          if ( t->stato()&itm_tavolo::proforma_stampata ) t->aggiorna();
        }
      } // for
    }
  }

  // tavolo/orologio

  {
    QDateTime t=QDateTime::currentDateTime();
    QString ts=t.toString( "h:mm" );
    for ( uint i=0; i<tavoli.count(); i++ ) {
      QCanvasItemList l=tavoli[i]->canvas()->allItems();
      for ( QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it ) {
        itm_tavolo*t=( itm_tavolo* ) ( *it );
        if ( t->orologio()&&t->nome()!=ts ) {
          t->set( tr::tavolo_esteso::p_nome, ts );
          t->aggiorna();
        }

      }
    }
  }

  static char buf[1024];

  while ( _udp->bytesAvailable() ) {
    int i=_udp->readBlock( buf, sizeof( buf ) );
    if ( i>0 ) buf[i]='\0';
    QString st=QString::fromUtf8( buf );

    if ( st.find( ';' ) ) st=st.replace( ';', 8 );


    if ( config.msg_server_tipo==0 )
      log( tr( "da UDP:" +st ) );
    if ( config.msg_server_tipo==1 )
      log( tr( "da TCP:" +st ) );

    QStringList ln=QStringList::split( '\n', st, true );
    while ( ln.count() ) {
      QStringList e=QStringList::split( config.cmd_sep, ln.first(), true );
      ln.pop_front();

      if ( e[0]=="POPUP" ) tools.msgbox( e[1] );

      if ( e[0]=="AGGIORNA_VOTI" ) TROWSLI( glb.voti ) {
          if ( glb.voti[i].section( '|', 0, 0 )==e[1] ) {
            glb.voti[i]=ln.first().mid( e[1].length() );
          }
        }

      if ( e[0]=="RICARICA" &&glb.cache_menu_listino.count()&&glb.cache_menu_listino[0].section( '|', 0, 0 )==e[1] )
        glb.cache_menu_listino.clear();


      if ( tavoli.count() ) {
        if ( e[0]=="TAVOLO_MODIFICATO" ) {
          int id=e[1].toInt();
          for ( uint i=0; i<tavoli.count(); i++ ) {
            itm_tavolo*t=tavoli[i]->tavolo( id );
            if ( t ) {
              if ( t->tway() ) e[2]="0";
              t->set( tr::enumeratori::tavolo_esteso::p_stato, e[2] );
              t->card.clear();
              t->aggiorna();
            }
          } // for
        }
        else if ( e[0]=="AGGIORNA_SALA" ) {


          int id_sala=e[2].toInt();

          for ( uint i=0; i<tavoli.count(); i++ ) {
            if ( tavoli[i]->sala().id==id_sala )
              tavoli[i]->carica_tavoli( id_sala, true );
          }
        }
      }

      if ( e[0]=="TAG" ) {
        e[2]=e[2].stripWhiteSpace();
        if ( e[2]=="ANY" ||e[2]==config.sn ) {

          frm_base*fb=( frm_base* ) tools.actWin();

          if ( fb&&e[1].isNull()&&!fb->splash ) { // tolta chiavetta:spalsh screen
            if ( fb ) {
              fb->card.clear();
              QTimer::singleShot( 10, fb, SLOT( card_readed() ) );
            }
          }

          else {

            if ( fb&&e[1].isNull()&&fb->splash ) break;

            srv->silent=true;
            ::cmd( tok_identifica_tag, e[1] );
            if ( srv->last_rcv().length() ) {
              if ( srv->last_rcv().left( 3 )=="-KO" ) {
                srv->eop();
                if ( fb ) {
                  if ( fb->splash ) break;
                  fb->card.clear();
                  fb->card<<"";
                  fb->card<<e[1];
                  QTimer::singleShot( 10, fb, SLOT( card_readed() ) );
                }
              }

              else // if OK

              {

                QString tes=srv->rcv();

                srv->eop();
                srv->silent=false;

                if ( fb ) {
                  fb->card=QStringList::split( '|', tes, true );
                  if ( !fb->splash||( fb->splash&&fb->card[2].toInt()==TRPayCard::famiglia_cameriere ) )
                    QTimer::singleShot( 10, fb, SLOT( card_readed() ) );
                }

              }

              srv->silent=false;

            }

          }

        }
      }
    }
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_server_udp::log( QString st ) {
  if ( config.verbose>0 ) tools.log( st );
}



// Version that run correctly

QString tr_srvcmd::rcv() {

  if ( lines_buf.count()>0 ) {
    _last_rcv=lines_buf.front();
    log( tab_risposta+_last_rcv );
    lines_buf.pop_front();
    if ( _last_rcv==config.cmd_eop ) _eop=true;
    return _last_rcv;
  }

  DBGSRV;
  static char buf[MAX_RX_BUF_SIZE];
  QString rb="";

  bool eop=false, trovato;
  uint x;
  uint n=0;
  while ( !eop ) {
    if ( !wait_data() ) { DBGSRVEND; return "" ;}

    n+=readBlock( buf+n, sizeof( buf )-n-1 );

    for ( uint i=0; i<n-1; i++ ) {
      if ( buf[i]==0 ) {
        buf[i]=' ';
        cerr<<"___NULL___" <<endl;
      }
    }

    // Come sempre l'approccio K.I.S.S. funziona.
    // Il bel vecchio codice che avevo scritto (che, tra l'altro, non funzionava correttamente
    // nei casi limite - MAX_RX_BUF_SIZE=4) viene rimpiazzato con 3 righe di codice
    // di un'idiozia unica.
    // E funziona anche nei casi limite.
    // E piu' veloce del previsto.


    // ---------------------------------------------------------------
    // C O D I C E   C O R R E T T O

    // Concatena alla stringa come se fosse un latin1 e procedi successivamente
    // a deconcatenare il tutto

    buf[n] = 0;
    rb += QString(buf);

    // Azzera l'offset del buffer di lettura

    n=0;

    // EOP condition

    if ( rb.right( 4 )=="EOP\n" ) {
      // Effettua on-the-fly la conversione in UTF8 - l'EOP è sempre EOP

      lines_buf=QStringList::split( '\n', QString::fromUtf8(rb) );
      eop=true;
      _last_rcv=lines_buf.front();
      log( tab_risposta+_last_rcv );
      lines_buf.pop_front();
      // cerr << "EOP found" << endl;
    }
    // else
//	    cerr << "EOP not found" << endl;

} // while

  DBGSRVEND;
  return _last_rcv;

};


