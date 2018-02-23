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

#define VERSION "1.5.05.p00"

#include <qapplication.h>
#include <qcursor.h>
#include <qdatetime.h>
#include <qsocket.h>
#include <qtranslator.h>
#include <qcolor.h>
#include <qt.h>

#include <stdlib.h>
#include <search.h>
#include <execinfo.h>
#include <qstring.h>

#include <qsound.h>

#include "tr_globals.hpp"
#include "tr_config.hpp"
#include "tr_tools.hpp"

#include "tr_forms.hpp"

#ifndef POCKET_VERSION
  #include "frm_main.hpp"
#endif

#include "tr_display.hpp"

#include <csignal>
#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>  /* For fcntl() */


#define HELP QApplication::tr("Modo d'uso\n    tavolinux [-addr:ip/hostname] [-port:n_porta] [-pipe] [-noudp] [-nosetuid] [-display1] [-verbose:0..2|1024] [-lang[:english.ts]] [-border] [-noborder] [-nosf] [-pocket] \n\nle [] indicano argomenti facoltativi\n")

#define MAIN_CPP
#include "tr_srvcmd.hpp"
tr_srvcmd*srv;
tr_config config;
tr_tools tools;
tr_globals glb;
tr_display display_lcd;

extern void frm_pocket();

/* ============================================================== STACK BACKTRACE
============================================================== */

#ifndef MAX_STACK_DEPTH
  #define MAX_STACK_DEPTH (2<<10)
#endif

/* ============================================================== */

void*devnul;

void( *old_signal ) ( int );

void seg_fault( int dato ) {
  // ripristina il precedente
  signal( SIGSEGV, old_signal );

  cout<<"Errore nel segmento di memoria\n";
  if ( tools.actWin() ) cout<<"Ultima finestra attiva:" <<tools.actWin()->name()<<endl;

  // backtrace();

  QFile log_events( config.exe_path+"log_events" );
  cout<<"Registro " <<log_events.name().latin1()<<endl;
  if ( log_events.open( IO_WriteOnly ) ) {
    QString st=QDateTime::currentDateTime().toString();
    log_events.writeBlock( st.latin1(), st.length() );
    log_events.putch( '\n' );
    TROWSLI( tools.log_event_buffer ) {
      log_events.writeBlock( tools.log_event_buffer[i].latin1(), tools.log_event_buffer[i].length() );
      log_events.putch( '\n' );
    }
    log_events.close();
  }
  else
    cout<<"Errore nell'apertura del file\n";

  log_events.setName( config.exe_path+"log_server" );
  cout<<"Registro " <<log_events.name().latin1()<<endl;
  if ( log_events.open( IO_WriteOnly ) ) {
    QString st=QDateTime::currentDateTime().toString();
    log_events.writeBlock( st.latin1(), st.length() );
    log_events.putch( '\n' );
    TROWSLI( srv->log_buffer ) {
      log_events.writeBlock( srv->log_buffer[i].latin1(), srv->log_buffer[i].length() );
      log_events.putch( '\n' );
    }
    log_events.close();
  }
  else
    cout<<"Errore nell'apertura del file\n";


  if ( old_signal ) ( *old_signal ) ( dato );
  else
    exit( EXIT_ERROR );

}

#ifdef _MCD_CHECK
const bool _mcd_check=true;
#else
const bool _mcd_check=false;
#endif

extern void init_hooks_1();
extern tr_tools tools;

QString htm_table( QString x ) {
  return "<br><table width=120 cellspacing=0 cellpadding=0 border=0>" +x+"</table>";
}

QString htm_row( QString x ) {
  return "<tr>" +x+"</tr>";
}

QString htm_data( QString x ) {
  return "<font size=2>%" +x+"%</font>";
}

// htm_dico non funziona correttamente: sballa i fonts e non trova le icone dal mime_source
QString htm_dico( QString ico, QString x ) {
  return "<img name=\"" +ico+".png\" width=14 height=14><font size=3>%" +x+"%</font>";
}

QString htm_cell( QString x ) {
  return "<td align=\"center\">" +x+"</td>";
}

QString htm_cell_span( QString x ) {
  return "<td colspan=2 align=\"center\">" +x+"</td>";
}



class tr_splash_screen:public QWidget {
public:
  tr_splash_screen():QWidget( 0, "splash", MYFLAGS ) {
    setBackgroundMode( Qt::FixedColor );
    setPaletteBackgroundColor( Qt::white );
    setPaletteForegroundColor( Qt::darkBlue );
    QPixmap tavolinux;
    if ( config.splash[0]!='<' ) {
      tavolinux=PIXMAP( config.splash.section( '|', 0, 0 ) );
      setPaletteBackgroundPixmap( tavolinux );
      showFullScreen();
      _msg=NULL;
    }
    else {
      QGridLayout*l=new QGridLayout( this );
      QFrame*frame=new QFrame( this, "frame" );
      l->addWidget( frame, 0, 0 );
      l->setMargin( 2 );
      frame->setMargin( 2 );
      frame->setLineWidth( 1 );
      frame->setMidLineWidth( 1 );
      frame->setFrameShape( QFrame::GroupBoxPanel );
      frame->setFrameShadow( QFrame::Sunken );
      QVBoxLayout*lv=new QVBoxLayout( frame );
      lv->setMargin( 2 );
      QLabel*label=new QLabel( frame, "textbox" );
      label->setMargin( 3 );
      label->setText( config.splash.section( '|', 0, 0 ) );
      lv->addWidget( label );
      _msg=new QLabel( frame, "messagebox" );
      QFont f( _msg->font() );
      f.setBold( true );
      _msg->setFont( f );
      _msg->setAlignment( Qt::AlignCenter );
      lv->addWidget( _msg );

      QSize w( this->sizeHint() );
      setFixedSize( w );
      move( ( QApplication::desktop()->width()-w.width() )/2, ( QApplication::desktop()->height()-w.height() )/2 );
      show();
    }
    quit_to_term=false;
  }

  void message( QString msg ) {
    if ( _msg ) {
      _msg->setPaletteForegroundColor( Qt::darkBlue );
      _msg->setText( msg );
      qApp->processEvents();
    }
  }

  void error( QString msg ) {
    if ( _msg ) {
      _msg->setPaletteForegroundColor( Qt::red );
      _msg->setText( msg+TR( " - TOCCA O CLICCA PER CHIUDERE" ) );
      qApp->exec();
    }
  }

  void advance() {
    if ( _msg ) {
      _msg->setText( "* " +_msg->text()+" *" );
      qApp->processEvents();
    }
  }

  void finish( QWidget* ) {
  }

  bool event( QEvent*e ) {
    if ( e->type()==QEvent::MouseButtonPress ) qApp->quit();
    if ( e->type()==QEvent::KeyPress ) {
      QKeyEvent*ke=( QKeyEvent* ) e;
      if ( ( ke->stateAfter()&Qt::ShiftButton )==QEvent::ShiftButton ) {
        QApplication::beep();
        quit_to_term=true;
        qApp->quit();
      }
      return true;
    }
    return QWidget::event( e );
  }

  bool quit_to_term;

private:
  QLabel*_msg;
};

// ----------------------------------------------------------------------------
#include <X11/Xlib.h>
#define XC(x) {x,#x},
struct {
  int id;
  QString name;
}
XEventsName

[]= {
  XC( KeyPress                )
  XC( KeyRelease              )
  XC( ButtonPress             )
  XC( ButtonRelease           )
  XC( MotionNotify            )
  XC( EnterNotify             )
  XC( LeaveNotify             )
  XC( FocusIn                 )
  XC( FocusOut                )
  XC( KeymapNotify            )
  XC( Expose                  )
  XC( GraphicsExpose          )
  XC( NoExpose                )
  XC( VisibilityNotify        )
  XC( CreateNotify            )
  XC( DestroyNotify           )
  XC( UnmapNotify             )
  XC( MapNotify               )
  XC( MapRequest              )
  XC( ReparentNotify          )
  XC( ConfigureNotify         )
  XC( ConfigureRequest        )
  XC( GravityNotify           )
  XC( ResizeRequest           )
  XC( CirculateNotify         )
  XC( CirculateRequest        )
  XC( PropertyNotify          )
  XC( SelectionClear          )
  XC( SelectionRequest        )
  XC( SelectionNotify         )
  XC( ColormapNotify          )
  XC( ClientMessage           )
  XC( MappingNotify           )
  XC( LASTEvent               )
};

#undef XC

// ----------------------------------------------------------------------------
int main( int argc, char**argv ) {
  cout<<TR( "-- Benvenuto in Tavolinux ---------------------------------------------------" )<<endl;

  /* x test date PP(tools.dataiso_data("2005-12-23T12:20")); PP(tools.dataiso_data("2005-12-23",true));
  PP(tools.data_dataiso("23/12/05")); PP(tools.data_dataiso("23/12/05",true)); PP(tool s.data_dataiso("23/12/05 12:20")); */


  QStringList arguments;
  if ( argc>1 )
    for ( int i=1; i<argc; i++ ) arguments<<argv[i];

  QDir d( argv[0] );
  d.convertToAbs();
  config.exe_path=d.path();
  config.exe_path.truncate( config.exe_path.findRev( '/' )+1 );
  if ( config.exe_path.isEmpty()||( config.exe_path[0]=='.' &&config.exe_path[1]=='/' ) ) {
    d.setPath( d.currentDirPath() );
    d.convertToAbs();
    config.exe_path=d.path();
  }
  if ( config.exe_path.right( 1 )!=QString( QChar( '/' ) ) )
    config.exe_path+=QString( QChar( '/' ) );
  cout<<( "Esecuzione a partire da:" )<<config.exe_path.latin1()<<endl;


  /*

  // generazione dei formati html per lo stato del tavolo

  QString tbl_ncop_tot_hap_hns= htm_table( htm_row(htm_cell(htm_data("coperti"))+htm_cell(htm_data("tot_tav")))+
  htm_row(htm_cell(htm_data("dpc"))+htm_cell(htm_data("duc")))); QString tbl_ncop_tot_cam=htm_table(
  htm_row(htm_cell(htm_data("coperti"))+htm_cell(htm_data("tot_tav")))+ htm_row(htm_cell_span(htm_data("cam"))));

  QString formati[2]; formati[0]=tbl_ncop_tot_hap_hns; formati[1]=tbl_ncop_tot_cam;

  for (int i=0;i<COUNT(formati);i++) { QFile fmt; fmt.setName(config.exe_path+"formats/table_state_"+QSN(i+1)+".html"); //
  split orizzontale if (!fmt.exists()) if (fmt.open(IO_WriteOnly)) { fmt.writeBlock(formati[i].ascii(),formati[i].length());
  // if (config.format_table_state==i) config.format_table_state_html=formati[i]; // gestiro in carica_extra, qui di seguito }
  else PP(fmt.String()); } // for

  // fine generazione dei formati html per lo stato del tavolo

  */

  /* manuale pag. 762 SIGABRT errore terminazione SEGFPE	err vig mob SIGILL	istr errata SIGINT	CTRL+C
  SIGSEGV	acc illeg a mem SIGTERM	progr terminato */

  old_signal=signal( SIGSEGV, &seg_fault );
  uint r=( uint )old_signal;
  // ritorna puntatore a prec funz.
  if ( r!=0 ) cout<<QApplication::tr( "un prec segnal vfunc era registrato\n" ).latin1();
  if ( r==( uint )SIG_ERR ) cout<<QApplication::tr( "Non ho potuto installare la funzione di segnale\n" ).latin1();

  short porta=config.server_comande_porta;
  bool udp=true;
  bool unix_socket;
  bool p_setuid=true;

  if ( config.tipo_socket==tr_config::unix_socket ) unix_socket=true;
  else unix_socket=false;

  QApplication a( argc, argv );

  QFont new_font( config.font_applicazione.section( '|', 0, 0 ) );
  if ( !config.font_applicazione.section( '|', 1, 1 ).isEmpty() )
    new_font.setPixelSize( config.font_applicazione.section( '|', 1, 1 ).toInt() );
  a.setFont( new_font );

#ifndef POCKET_VERSION
  tr_splash_screen*splash=new tr_splash_screen;
  splash->message( ( "TAVOLINUX STA PARTENDO" ) );
  #define AVANZA splash->advance();
  // a.setMainWidget(&splash);
  QTimer::singleShot( config.splash.section( '|', 1, 1 ).toInt(), &a, SLOT( quit() ) );
  a.exec();
  if ( splash->quit_to_term ) {
    QProcess p;
    QString modulo;
    modulo=config.default_terminal;
    p.addArgument( modulo );
    p.start();
    delete splash;
    return -1;
  }
  a.processEvents();
#else
#define AVANZA
#endif

  QTranslator translator( 0 );

  if ( QApplication::desktop()->width()!=config.finestra_larghezza&&QApplication::desktop()->height()!=config.finestra_altezza )
    MYFLAGS=WDBGFLG;

  if ( !config.arguments.isEmpty() ) arguments=QStringList::split( ' ', config.arguments );
  if ( !config.language.isEmpty() ) arguments<<"-lang";

  QString login="";

  AVANZA

  bool nolang=false;

  if ( arguments.size() ) {
    TROWSLI( arguments ) {
      if ( arguments[i]=="-nosf" ) signal( SIGSEGV, old_signal );
      if ( arguments[i].left( 7 )=="-border" ) MYFLAGS=WDBGFLG;
      if ( arguments[i].left( 7 )=="-pocket" ) {
        config.pocket="1";
        if ( arguments[i].left( 8 )=="-pocket:" ) config.pocket+=( "|" +arguments[i].mid( 8 ) );
      }
      if ( arguments[i].left( 9 )=="-noborder" ) MYFLAGS=WRUNFLG;
      if ( arguments[i].left( 6 )=="-login" ) login=arguments[i].section( ':', 1, 1 );
      if ( arguments[i].left( 5 )=="-addr" ) config.server_addr=arguments[i].section( ':', 1, 1 );
      if ( arguments[i].left( 5 )=="-port" ) {
        bool ok;
        porta=arguments[i].section( ':', 1, 1 ).toInt( &ok );
        if ( !ok ) {
          cout<<HELP.latin1();
          return EXIT_ERROR;
        }
        else {
          QString st=QApplication::tr( "connessione al server sulla porta alternativa:" );
          st+=porta;
          cout<<st.latin1()<<endl;
        }
      }
      if ( arguments[i]=="-nosetuid" ) p_setuid=false;
      if ( arguments[i]=="-display1" ) {
        setenv( "DISPLAY", /* config.server_addr + */ "localhost:0.1", 1 );
      }
      if ( arguments[i]=="-nolang" ) nolang=true;
      if ( arguments[i].left( 5 )=="-lang" && !nolang) {
        glb.lang=arguments[i].section( ':', 1, 1 );
        if ( glb.lang.isEmpty()&&!config.language.isEmpty() ) glb.lang=config.language;
        if ( glb.lang.isEmpty() ) glb.lang="english";
        if ( glb.lang.find( '\\' )==-1&&glb.lang.find( '/' )==-1 ) glb.lang.prepend( config.exe_path );
        if ( !translator.load( glb.lang, "." ) )
          cout<<TR( "Dizionario esterno '" )+glb.lang+TR( "' non trovato\n" );
        else {
          a.installTranslator( &translator );
          cout<<TR( "Trovato e installato linguaggio: " )<<glb.lang<<endl;
        }
      }
      if ( arguments[i]=="-noudp" ) udp=false;
      if ( arguments[i].left( 9 )=="-verbose:" ) {
        config.verbose=arguments[i].section( ':', 1, 1 ).toInt();
        cout<<"Verbose impostato su " <<config.verbose<<endl;
      }
      if ( arguments[i]=="-pipe" ) unix_socket=true;
      if ( arguments[i]=="-?" ||arguments[i]=="-h" ||arguments[i]=="--help" ) {
        cout<<HELP.latin1();
        return EXIT_OK;
      }
    }
  }

  //init_hooks_1();

#ifdef _MCD_CHECK
  cout<<"MemStats in apertura MAIN\n";
  showMemStats();
#endif

  AVANZA

  if ( QSound::isAvailable() )
    cout<<QApplication::tr( "Interfaccia sonora disponibile\n" ).latin1();

  // prende solo la prima parola (nome dell'applicazione) e aggiunge la versione
  config.nome_applicazione=config.nome_applicazione.section( ' ', 0, 0 )+" " +VERSION;
  cout<<config.nome_applicazione.latin1()<<endl;

  if ( p_setuid ) {

    struct passwd*pwd_entry=NULL;

    // Individua l'utente Tavolinux (se esiste)

    pwd_entry=getpwnam( "tavolinux" );

    if ( !pwd_entry||( setuid( pwd_entry->pw_uid )==-1 ) ) {
      cout<<QApplication::tr( "Set utente tavolinux non accettato\n" ).latin1();
      return EXIT_ERROR;
    }

    // Da questo momento in poi proseguo come utente Tavolinux
  }

  AVANZA

  NEW( srv, tr_srvcmd( porta, udp, unix_socket ) );
  glb.init_modules();
  if ( !srv->stato()&&::cmd( tok_login_terminal_esteso, config.tipo_terminale, config.sn ) ) {
    denu_login_terminal_esteso;
    QString st=srv->rcv();
    glb.moduli=st.section( '|', p_licenza, p_licenza );
    glb.ragione_sociale=st.section( '|', p_ragione_sociale, p_ragione_sociale );
    glb.piva=st.section( '|', p_piva, p_piva );
    srv->eop();

    AVANZA

    if ( ::cmd( tok_elenco_menu_listino, QString( "1" ) ) ) {
      if ( config.verbose<2 ) srv->nolog=true;
      glb.cache_menu_listino=srv->rcv_list();
    }
    srv->eop();
    srv->nolog=false;
  }
  else {
#ifndef POCKET_VERSION
      splash->error( TR( "IL SERVER NON RISPONDE O NON E' COLLEGATO" ) );
#else      
      cerr << TR( "IL SERVER NON RISPONDE O NON E' COLLEGATO" ) << endl;
#endif
      DELETE( srv );
    return EXIT_ERROR;
  }

  AVANZA

  // carica le specifiche dei pixmap
  if ( ::cmd( tok_elenco_pixmap, LEAVE ) ) {
    glb.pixmaps=srv->rcv_list();
    srv->eop();
  }
  else {
#ifndef POCKET_VERSION
    splash->error( TR( "IL SERVER NON RISPONDE O NON E' COLLEGATO" ) );
#else      
      cerr << TR( "IL SERVER NON RISPONDE O NON E' COLLEGATO" ) << endl;
#endif
    return EXIT_ERROR;
  }

  AVANZA

  // carica le specifiche dei pixmap
  if ( ::cmd( tok_elenco_voti ) ) {
    glb.voti=srv->rcv_list();
    srv->eop();
  }
  else {
#ifndef POCKET_VERSION
    splash->error( TR( "IL SERVER NON RISPONDE O NON E' COLLEGATO" ) );
#else      
      cerr << TR( "IL SERVER NON RISPONDE O NON E' COLLEGATO" ) << endl;
#endif
    return EXIT_ERROR;
  }

  AVANZA

  config.carica_extra();

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef POCKET_VERSION
  if ( config.pocket.isEmpty() ) {
    frm_main*frm_sala;
    NEW( frm_sala, frm_main( 0, "frm_main" ) );
    frm_sala->setCaption( config.nome_applicazione );

    // installa i log eventi
    qApp->installEventFilter( frm_sala );

    tools.actWin( frm_sala ); // registra la finestra corrente come attiva

    a.setMainWidget( frm_sala );

    int x=( QApplication::desktop()->width()-config.finestra_larghezza )/2;
    int y=( QApplication::desktop()->height()/config.finestra_altezza )/2;

    frm_sala->move( x, y );

    if ( MYFLAGS==WDBGFLG ) {
      frm_sala->show();
    }
    else {
      frm_sala->showMaximized();
    }

    splash->finish( frm_sala );
    delete splash;

    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );

    // identificazione turno. Vedere anche frm_main::first_login()
    if ( glb.stato[tr_globals::p_turno_id].isEmpty()&&config.turns_ask_NoStartLogin==1 ) {
      QString descrizione="";
      if ( !tools.scelta( tr_listview::turni, glb.stato[tr_globals::p_turno_id],
         glb.stato[tr_globals::p_turno_descrizione] ) ) {
           glb.stato[tr_globals::p_turno_id]="";
           glb.stato[tr_globals::p_turno_descrizione]="";
      }
    }

    if ( !login.isEmpty() ) {
      frm_sala->first_login( login, QString::null, true );
    }
    else if ( !config.auto_login.isEmpty() ) {
      frm_sala->first_login( config.auto_login, QString::null, true );
    }

    // MYFLAGS=WRUNFLG;
    r=a.exec();

    DELETE( frm_sala );
  }
  else {
    delete splash;
    frm_pocket();
  }
#else
  frm_pocket();
#endif

#ifdef _MCD_CHECK
  //QStringList buffer;
  //if (!config.logout_comandi) buffer=cmd_srv->log_buffer;
#endif

  DELETE( srv );

  arguments.clear();

#ifdef _MCD_CHECK
  /* cout << "MemStats in chiusura di MAIN\n"; showMemStats(); if (mem_chk.stat()) { if (!config.logout_comandi) PP(buffer);
  cout << "premere INVIO per proseguire\n"; getchar(); } */
#endif

  return EXIT_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void main_exit() {

  QFile log_events( config.exe_path+"log_server" );
  cout<<"Registro " <<log_events.name().latin1()<<endl;
  if ( log_events.open( IO_WriteOnly ) ) {
    QString st=QDateTime::currentDateTime().toString();
    log_events.writeBlock( st.latin1(), st.length() );
    log_events.putch( '\n' );
    TROWSLI( srv->log_buffer ) {
      log_events.writeBlock( srv->log_buffer[i].latin1(), srv->log_buffer[i].length() );
      log_events.putch( '\n' );
    }
    log_events.close();
  }
  else
    cout<<"Errore nell'apertura del file\n";

  DELETE( srv );

  tools.msgbox( QApplication::tr( "Comunicazione col server interrotta<br>Tavolinux viene riavviato per sicurezza" ) );
  //cout << "USCITA INDESIDERATA\nPremere INVIO per proseguire\n";
  //getchar();

  exit( EXIT_ERROR );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
