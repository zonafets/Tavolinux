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

#ifndef TR_TOOLS_CPP
  #define TR_TOOLS_CPP

/* HISTORY: 23-09-04 aggiunta sostituzione \n con <br> in msgbox e aggiunto <br>
finale perche' pare che le qt calcolino male l'altezza */

  #include <qcolor.h>
  #include <qregexp.h>
  #include <qdatetime.h>
  #include <qlineedit.h>
  #include <qwidget.h>
  #include <qdialog.h>
  #include <qdeepcopy.h>
  #include "frm_base.hpp"
  #include "tr_display.hpp"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  #include "tr.hpp"
  #include "tr_tools.hpp"
  #include "frm_calendario.hpp"
  #include "frm_calcolatrice.hpp"
  #include "tr_dmacro.hpp"
  #include "msgs.hpp"
  #include "cmddiritti.hpp"

  #include "tr_config.hpp"
extern tr_config config;

  #include "tr_globals.hpp"
extern tr_globals glb;

extern tr_tools tools;

extern tr_display display_lcd;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_palette::tr_palette() {
  pulsante=config.pulsante_barra;
  titolo.bm="sfondo_titolo.jpg";
  titolo.bg=config.colore_sfondo;
  titolo.fg=config.colore_testo;
  stato.bm=""; // "white_marble.jpg";	// privilegia il colore
  stato.bg=config.colore_sfondo_stato;
  stato.fg=config.colore_sfondo;
  stato.sfg=config.colore_sfondo.light();
  area.bm="";
  area.bg=config.colore_sfondo;
  area.fg=config.colore_testo;
  selezione=config.colore_selezione;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* QPixmap tr_tools::tr_image_mixer::unione() { if (!lista.first()) return QPixmap(); tr_tools::tr_image_mixer::tr_obj *o;
o = (tr_tools::tr_image_mixer::tr_obj*)lista.current(); QPixmap p( o->name() );	// pixmap di base
QPainter pa( &p );		// crea un painter sul pixmap di base while (lista.next()) {		// sovrappone gli altri
o = (tr_tools::tr_image_mixer::tr_obj*)lista.current(); switch (o->tipo) { case pixmap: QPixmap p( o->name() );
pa.drawPixmap( x, y, p ); break; case testo: pa.setFont( o->font ); pa.drawText( x, y, o->testo ); break; } } return p; } */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* QDate tr_tools::data( QString s) { if (s.isEmpty()) return QDate(); QStringList data_ora=QStringList::split('T',s,true);
if (data_ora.count()>1 && data_ora[1]=="00:00:00") data_ora.pop_back(); QStringList e;
if (s.find('/')!=-1) e = QStringList::split("/",data_ora[0]); if (s.find('-')!=-1) e = QStringList::split("-",data_ora[0]);

if (!e.count()) e= QStringList::split("-",s); // int anno, mese, giorno; int anno=e[0].toInt(); int mese=e[1].toInt();
int giorno=e[2].toInt(); if (anno>1000) { if (QDate::isValid(anno,mese, giorno)) return QDate(anno, mese, giorno); } else
if (QDate::isValid(giorno,mese,anno)) return QDate(giorno, mese, anno); return QDate(); } */

QString tr_tools::conforma_data( QString s ) {
  if ( s.isEmpty() ) return s;
  QStringList data_ora=QStringList::split( 'T', s, true );
  if ( data_ora.count()>1&&data_ora[1]=="00:00:00" ) data_ora.pop_back();
  QStringList e;
  if ( s.find( '/' )!=-1 ) e=QStringList::split( "/", data_ora[0] );
  if ( s.find( '-' )!=-1 ) e=QStringList::split( "-", data_ora[0] );
  int anno=e[0].toInt();
  int mese=e[1].toInt();
  int giorno=e[2].toInt();
  s="";
  for ( uint i=0; i<config.formato_data.length(); i++ ) {
    if ( s!="" ) s+='/';
    if ( config.formato_data[i]=='Y' ) s+=QString::number( anno ).rightJustify( 4, '0' );
    if ( config.formato_data[i]=='M' ) s+=QString::number( mese ).rightJustify( 2, '0' );
    if ( config.formato_data[i]=='D' ) s+=QString::number( giorno ).rightJustify( 2, '0' );
  }
  if ( data_ora.count()>1 ) return s+"T" +data_ora[1];
  else
    return s;
}

bool tr_tools::data_valida( QString s, QDate*d ) {
  if ( s.isEmpty() ) return true;
  QStringList e=QStringList::split( "/", s );
  int anno, mese, giorno;
  for ( uint i=0; i<config.formato_data.length(); i++ ) {
    if ( config.formato_data[i]=='Y' ) anno=e[i].toInt();
    if ( config.formato_data[i]=='M' ) mese=e[i].toInt();
    if ( config.formato_data[i]=='D' ) giorno=e[i].toInt();
  }
  if ( !QDate::isValid( anno, mese, giorno ) ) return false;

  if ( d )*d=QDate( anno, mese, giorno );
  return true;
}

bool tr_tools::ora_valida( QString s, QTime*t ) {
  if ( s.isEmpty() ) return true;
  QVariant v( s );
  if ( !v.canCast( QVariant::Time )||!v.asTime().isValid() ) return false;
  if ( t )*t=v.toTime();
  return true;
}

bool tr_tools::dataora_valida( QString s, QDateTime*dt ) {
  if ( s.isEmpty() ) return true;
  QVariant v( s );
  if ( !v.canCast( QVariant::DateTime )||!v.asDateTime().isValid() ) return false;
  else
    return true;
  if ( dt )*dt=v.toDateTime();
  return true;
}

QString tr_tools::data_dataiso( QString data, bool yeszerotime ) {
  if ( data.isEmpty() ) return data;
  QStringList data_ora;
  if ( data.find( 'T' )!=-1 ) data_ora=QStringList::split( 'T', data, true );
  else
    data_ora=QStringList::split( ' ', data, true );
  if ( data_ora.count()>1&&data_ora[1]=="00:00:00" ) data_ora.pop_back();
  QStringList e;
  if ( data.find( '/' )!=-1 ) e=QStringList::split( "/", data_ora[0] );
  else if ( data.find( '-' )!=-1 ) e=QStringList::split( "-", data_ora[0] );
  int p_anno=config.formato_data_locale.find( 'y' );
  int p_mese=config.formato_data_locale.find( 'm' );
  int p_giorno=config.formato_data_locale.find( 'd' );
  if ( p_giorno<p_mese&&p_mese<p_anno ) {
    p_giorno=0;
    p_mese=1;
    p_anno=2;
    goto a;
  } // gma
  if ( p_mese<p_giorno&&p_giorno<p_anno ) {
    p_giorno=1;
    p_mese=0;
    p_anno=2;
    goto a;
  } // mga
  if ( p_anno<p_mese&&p_mese<p_giorno ) {
    p_giorno=2;
    p_mese=1;
    p_anno=0;
    goto a;
  } // amg
  if ( p_anno<p_giorno&&p_giorno<p_mese ) {
    p_giorno=1;
    p_mese=2;
    p_anno=0;
    goto a;
  } // agm
a:

  int anno=e[p_anno].toInt();
  int mese=e[p_mese].toInt();
  int giorno=e[p_giorno].toInt();
  QDate d;
  if ( anno<100 ) d.setYMD( 2000+anno, mese, giorno );
  else
    d.setYMD( anno, mese, giorno );
  QTime t;
  if ( data_ora.count()>1 ) t=QTime::fromString( data_ora[1] );
  else if ( yeszerotime ) t.setHMS( 0, 0, 0 );
  if ( data_ora.count()>1||yeszerotime ) return QDateTime( d, t ).toString( Qt::ISODate );
  else
    return QDate( d ).toString( Qt::ISODate );
}

QString tr_tools::dataiso_data( QString dataiso, bool yeszerotime ) {
  QString&data=dataiso;
  if ( dataiso.isEmpty() ) return dataiso;
  QStringList data_ora=QStringList::split( 'T', dataiso, true );
  if ( data_ora.count()>1&&data_ora[1]=="00:00:00" ) data_ora.pop_back();
  QStringList e;
  if ( dataiso.find( '/' )!=-1 ) e=QStringList::split( '/', data_ora[0] );
  if ( dataiso.find( '-' )!=-1 ) e=QStringList::split( '-', data_ora[0] );
  int p_anno=0;
  int p_mese=1;
  int p_giorno=2;
  QString fdata=config.formato_data_locale;
  fdata=fdata.replace( "m", "M" );

  QString fora=config.formato_ora_locale;
  fora=fora.replace( "hh24", "hh" );
  fora=fora.replace( "mi", "mm" );

  QDate d( e[p_anno].toInt(), e[p_mese].toInt(), e[p_giorno].toInt() );
  QTime t( 0, 0, 0 );
  if ( data_ora.count()>1 ) {
    QStringList e=QStringList::split( ':', data_ora[1], true );
    t.setHMS( e[0].toInt(), e[1].toInt(), e[2].toInt() );
  }
  data=d.toString( fdata );
  if ( data_ora.count()>1||yeszerotime ) dataiso+=( " " +t.toString( fora ) );
  return dataiso;
}

QString tr_tools::tastiera( QWidget*parent, QString titolo, QString valore_iniziale, QString regexp, bool pwd ) {
  QRegExp rx;
  if ( regexp.isEmpty() ) rx.setPattern( validatore( "desc(64)" ) );
  else
    rx.setPattern( regexp );

  return tastiera( parent, titolo, valore_iniziale, rx, pwd );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_tools::tastiera( QWidget*parent, QString titolo, QString valore_iniziale, QRegExp rx, bool password,
   tr_input::tr_elenco elenco ) {
     if ( !parent ) parent=actWin();
     frm_tastiera f( parent, "frm_tastiera", true );

     if ( password ) f.pannello->edit->setEchoMode( QLineEdit::Password );

     tr_validator*v;
     NEW( v, tr_validator( rx, f.pannello->edit ) );
     f.pannello->edit->setValidator( v );

     if ( titolo!="" ) f.barra_stato->setText( titolo );

     if ( valore_iniziale!="" ) f.pannello->setText( valore_iniziale );
     if ( f.exec()==QDialog::Accepted ) return f.pannello->text();
     return valore_iniziale;
}


QString tr_tools::tastiera( QWidget*parent, tr_input*le /* , QString titolo */ ) {

  if ( !parent ) parent=actWin();
  frm_tastiera f( parent, "frm_tastiera", true );

  if ( le->label->text() ) f.barra_stato->setText( tr( "Inserimento " )+le->label->text() );

  f.pannello->edit->setLineWidth( 0 );

  *f.pannello=*le;
  f.pannello->into_keyboard=true;

  if ( f.exec()==QDialog::Accepted )
    le->setText( f.pannello->text() );

  return le->text();

};


QString tr_tools::tastiera( QString titolo ) {
  return tastiera( 0, "", titolo );
};


QString tr_tools::tastiera( QString titolo, QRegExp rx ) {
  return tastiera( 0, "", titolo, rx );
};



QString tr_tools::input_descrizione( QWidget*parent, QString titolo, QString valore_iniziale, bool pwd ) {
  return tastiera( parent, titolo, valore_iniziale, QString::null, pwd ).simplifyWhiteSpace();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



bool tr_tools::calendario( QWidget*parent, QDate*data ) {
  if ( !parent ) parent=actWin();

  #ifdef _MCD_CHECK
  mem_check_begin();
  #endif


  frm_calendario*f;
  NEW( f, frm_calendario( parent, "frm_calendario" ) );

  bool r=false;
  if ( f->accepted() ) {
    if ( data ) {
      if ( !f->cancellato() )*data=f->data();
      else
        *data=QDate();
    }
    r=true;
  }

  DELETE( f );

  #ifdef _MCD_CHECK
  mem_check_end();
  #endif

  return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_tools::calendario( QLineEdit*le ) {
  QDate d;
  bool b=calendario( 0, &d );
  if ( b ) {
    if ( d.isNull() ) le->setText( "" );
    else
      le->setText( data( d ) );
  }
  return b;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_tools::calcolatrice( QWidget*parent, const QString&titolo, QString valore, QString maschera, bool virgola_mobile ) {
  return calcolatrice( parent, QRect( 0, 0, 0, 0 ), titolo, valore, maschera, virgola_mobile );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_tools::calcolatrice( QWidget*parent, const QRect&pos, const QString&titolo, QString valore,
   QString validatore, bool virgola_mobile ) {

     #ifdef _MCD_CHECK
     mem_check_begin();
     #endif

     frm_calcolatrice*f;
     NEW( f, frm_calcolatrice( parent, "frm_calcolatrice" ) );
     f->impostazioni( titolo, valore, validatore, virgola_mobile==false?tr_calcolatrice::decimale_fisso:
        tr_calcolatrice::decimale_mobile );
     if ( pos!=QRect( 0, 0, 0, 0 ) ) {

       QPoint p( QPoint( pos.topRight().x()+2, pos.y() ) );

       int x=p.x();
       int y=p.y()+16;
       int x2=x+f->width();
       int y2=y+f->height();
       int dy=config.finestra_altezza;
       int dx=config.finestra_larghezza;
       if ( MYFLAGS==WDBGFLG ) {
         dx=QApplication::desktop()->width();
         dy=QApplication::desktop()->height();
       }

       if ( y2>dy ) y=dy-f->height();
       if ( x2>dx ) x=p.x()-f->width();

       p=f->mapFromGlobal( QPoint( x, y ) );
       p=f->mapToParent( p );

       f->centered=false;
       f->move( p );

     } // if pos!=(0,0)
     int r=f->exec();
     if ( r==QDialog::Accepted ) valore=f->text();
     else
       valore=QString::null;

     DELETE( f );

     #ifdef _MCD_CHECK
     mem_check_end();
     #endif

     return valore;
}



QString tr_tools::calcolatrice( tr_input*input, const QString&titolo, QString maschera, bool virgola_mobile ) {
  QPoint p1=actWin()->mapToGlobal( input->edit->geometry().topLeft() );
  QPoint p2=actWin()->mapToGlobal( input->edit->geometry().bottomRight() );
  QRect rect( p1, p2 );
  QString r=calcolatrice( actWin(), rect, titolo, input->text(), maschera, virgola_mobile );
  if ( !r.isNull() )
    input->setText( r );
  return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_wait*tr_tools::wait( QWidget*parent, QString ) {
  if ( !parent ) parent=actWin();
  frm_wait*w;
  NEW( w, frm_wait( parent, "frm_wait" ) );
  w->show();
  qApp->processEvents();
  return w;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_tools::wait( int millisecondi ) {
  QTime t;
  frm_wait*_wait=NULL;
  t.start();
  while ( t.elapsed()<millisecondi ) {
    if ( !_wait&&t.elapsed()>2000 )
      _wait=wait();
    if ( _wait&&( ( t.elapsed()%250 )==0 ) ) _wait->avanza();
  }
  if ( _wait ) {
    DELETE( _wait );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_tools::msgbox( QWidget*parent, QString testo, QString titolo, QString pulsanti, int wait_seconds ) {
  if ( !wait_seconds ) {
    QString bt;
    if ( !pulsanti.isEmpty() )
      bt=pulsanti.section( '|', 0, 0 ).section( '#', 1, 1 ).section( '(', 0, 0 );
    if ( bt.isEmpty() ) return "ok";
    return bt;
  }

  frm_base*base;
  if ( parent&&parent->inherits( "frm_base" ) ) base=( frm_base* ) parent;
  else
    base=NULL;
  if ( base==NULL ) {
    base=find_right_base_parent();
  }

  QString frm_msgbox="events.ignore=cards\n""margine=2\n""status=";
  if ( !titolo.isEmpty() ) frm_msgbox+=titolo;
  else
    frm_msgbox+=tr( "Avviso..." );
  frm_msgbox+="\n";

  testo=testo.replace( '\n', "<br>" );
  if ( testo.right( 4 ).lower()!="<br>" ) testo+="<br>";

  frm_msgbox+=( "label=???#txt(notr,bg:#fff7bb,fg:#000abe)\n" );

  if ( !pulsanti.isEmpty() ) {
    frm_msgbox+=( "buttons=" +pulsanti );
  }
  else {
    frm_msgbox+=tr( "buttons=ok#ok" );
  }

  if ( wait_seconds>0 )
    frm_msgbox.prepend( "events.timers=" +QSN( wait_seconds )+"\n" );

  FRM_PARAMS;
  NEW_FRM( f, frm_msgbox, base );
  CTRL_FRM( f );
  CTRL( txt );
  txt.value( testo );
  FRM_CTRL iterato=FRM_ITER(f);
  QDeepCopy<QString>nome;
  if ( iterato ) nome=iterato->name();
  DEL_FRM( f, frm_msgbox );

  return nome;
}


QString tr_tools::validatore( QString tabella_campo ) {
  if ( tabella_campo.left( 5 )=="desc(" ) {
    int start=tabella_campo.find( '(' );
    int stop=tabella_campo.find( ')' );
    return ".{0," +tabella_campo.mid( start+1, stop-start-1 )+"}";
  }
  if (tabella_campo=="int") return "^(\\d|-)?(\\d)*$";
  if ( cmd( tok_validatore, tabella_campo ) )
    return srv->rcv();
  return ".{0," +QString::number( config.max_descrizione )+"}";;
}

QString tr_tools::segna_posto( QString validatore ) {
  QStringList s=QStringList::split( QRegExp( "[{|}|,]" ), validatore );
  int n=0;
  for ( uint i=0; i<s.size(); i++ ) {
    bool b;
    int nn=s[i].toInt( &b );
    if ( b&&n<nn ) n=nn;
  }
  return QString().fill( '#', n );
}


void tr_tools::setPalette( frm_base*form ) {
  tr_palette p=palette( form );
  form->setPaletteBackgroundColor( p.area.bg );
  form->setPaletteForegroundColor( p.area.fg );
  form->barra_titolo->setPaletteBackgroundPixmap( PIXMAP( p.titolo.bm ) );
  form->barra_titolo->setPaletteForegroundColor( p.titolo.fg );

  form->barra_stato->setPaletteForegroundColor( p.stato.fg );
  if ( !p.stato.bm.isEmpty() ) form->barra_stato->setPaletteBackgroundPixmap( PIXMAP( p.stato.bm ) );
  else {
    form->barra_stato->setPaletteBackgroundColor( p.stato.bg );
  }

}


tr_palette tr_tools::palette( frm_base*form ) {
  tr_palette p;
  if ( !form ) return p;

  bool b=false;
  if ( !qstrcmp( form->name(), "frm_comanda" ) ) b=true;
  if ( !qstrcmp( form->name(), "frm_conto" ) ) b=true;
  if ( !qstrcmp( form->name(), "frm_base" ) ) b=true;
  if ( !qstrcmp( form->name(), "frm_main" ) ) b=true;
  if ( !qstrcmp( form->name(), "frm_sale" ) ) b=true;
  if ( !qstrcmp( form->name(), "frm_cm" ) ) b=true;

  if ( b ) {
    p.stato.bm="white_marble.jpg";
    p.pulsante=config.pulsante_barra;
  }
  else {
    if ( !qstrcmp( form->name(), "tr_hvmenu" ) ) p.pulsante=config.pulsante_menu;
    else
      p.pulsante=config.pulsante_menu;
    QColor c=QColor( 0, 10, 190 );
    if ( !qstrcmp( form->name(), "frm_scelta" ) ) c=QColor( 85, 170, 255 );
    if ( !qstrncmp( form->name(), "scelta.", 7 ) ) c=QColor( 85, 170, 255 );
    if ( !qstrcmp( form->name(), "frm_calendario" ) ) {
      c=QColor( 80, 200, 80 );
      p.pulsante=config.pulsante_dialog;
    }
    if ( !qstrcmp( form->name(), "frm_tastiera" ) ) c=QColor( 85, 170, 255 );
    if ( !qstrcmp( form->name(), "frm_ok_exit" ) ) c=QColor( 0, 100, 190 );
    if ( !qstrcmp( form->name(), "frm_bis" ) ) {
      c=QColor( 80, 200, 80 );
      p.pulsante=config.pulsante_dialog;
    }

    p.stato.bg=c;
    p.stato.bm="";
    p.stato.fg=WHITE;

  }
  return p;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_palette tr_tools::palette( int livello ) {
  tr_palette p;

  if ( !livello ) {
    p.stato.bm="white_marble.jpg";
    p.pulsante=config.pulsante_barra;
  }
  else {
    p.pulsante=config.pulsante_menu;
    p.stato.fg=WHITE;
    QColor c=QColor( 0, 10, 190 );
    if ( livello==1 ) c=QColor( 85, 170, 255 );
    if ( livello==2 ) c=QColor( 0, 100, 190 );
    if ( livello>=3 ) {
      c=QColor( 80, 200, 80 );
      p.pulsante=config.pulsante_dialog;
      p.area.bg=c.dark( 150 );
      p.stato.fg=WHITE;
    }
    p.stato.bg=c;
    p.stato.bm="";

  }
  return p;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


QString tr_tools::hvmenu( tr_label*pulsante, QString str_menu, TRDiritti::TRDirittiUtente diritti[] ) {
  QString idx="";

  str_menu.replace( ';', '|' );
  QStringList voci=QStringList::split( '|', str_menu );

  int base_diritti=0;
  int prima_sub=-1;
  int voci_sub=0;
  int voci_sub_tolte=0;
  QString radice="";
  if ( diritti ) {
    TROWSLI( voci ) {
      if ( voci[i].find( '\\' )!=-1||i+1==voci.count() ) {
        if ( !radice.isEmpty()&&voci_sub==voci_sub_tolte ) {
          for ( uint j=0; j<prima_sub; j++ ) {
            QString nome=voci[j];
            if ( nome.find( '#' )!=-1 ) nome=nome.mid( nome.find( '#' )+1 );
            if ( nome==radice ) {
              diritti[j]=TRDiritti::p_null;
              break;
            }
          }
        }
        radice=voci[i].left( voci[i].find( '\\' ) );
        if ( prima_sub==-1 ) prima_sub=voci_sub;
        voci_sub=voci_sub_tolte=0;
      }
      voci_sub++;
      if ( !flags( diritti[i] ) ) voci_sub_tolte++;
    }
  }

  tr_label*iterato=pulsante;
  int indice=0;
  QValueList<frm_base*>menus;
  bool trovato;
  radice=QString::null;

  do {

    QString menu="";
    while ( indice<voci.count() ) {
      int pos_voce=voci[indice].findRev( '\\' );
      if ( pos_voce!=-1&&voci[indice].left( pos_voce+1 )!=radice ) break;
      bool add=true;
      if ( diritti&&!flags( diritti[indice] ) ) add=false;
      if ( add ) {
        if ( !menu.isEmpty() ) menu+='|';
        if ( !radice.isNull()&&pos_voce==-1 ) menu+=radice;
        menu+=voci[indice];
      }
      indice++;
    }

    if ( menu.isEmpty() ) return idx;
    if ( radice.isNull() ) indice=0;

    frm_base*f;
    NEW( f, frm_base( iterato, menu.section( '|', 0, 0 ) /* "tr_hvmenu" */, true, MYFLAGS|WType_Popup ) );
    menus.append( f );

    f->pulsanti( menu, !indice, NULL, diritti );

    f->events_ignore_cards=true;

    iterato=( tr_label* ) f->interazione();

    if ( iterato ) {
      trovato=false;
      QString submenu=iterato->name();
      submenu+='\\';
      TROWSLI( voci ) {
        if ( voci[i].left( submenu.length() )==submenu ) {
          trovato=true;
          indice=i;
          radice=submenu;
          break;
        } // if
      }
    }

  }
  while ( iterato!=NULL&&trovato==true );

  if ( iterato ) idx=iterato->name();

  while ( menus.count() ) {
    frm_base*f=menus.back();

    menus.pop_back();
    DELETE( f );
  }

  return idx;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_tools::scelta( tr_listview::tr_tipo tipo, QString&id, QString&descrizione, QString&record,
   QString gruppo, bool imposta ) {
     frm_scelta*scelta;

     NEW( scelta, frm_scelta( actWin(), "frm_scelta" ) );
     scelta->mostra_imposta=imposta;

     scelta->tipo( tipo, id, gruppo );
     int r;
     if ( r=scelta->accepted() ) {
       id=scelta->id();
       descrizione=scelta->descrizione();
       if ( !record.isEmpty() ) {
         QString rec=scelta->record();
         record=rec;
       }
     }
     DELETE( scelta );
     return ( r==QDialog::Accepted );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_tools::removeWin( QWidget*set ) {
  if ( win_queue.top()==set ) {
    QObject*parent=set->parent();
    if ( parent&&parent->inherits( "frm_base" ) ) {
      frm_base*f=( frm_base* ) parent;
      if ( f->barcode_enabled() ) f->grabKeyboard();
    }
    return win_queue.remove();
  }
}

void tr_tools::actWin( QWidget*set ) {
  if ( win_queue.top()!=set ) {
    win_queue.push( set );
    QObject*parent=set->parent();
    if ( parent&&parent->inherits( "frm_base" ) ) {
      frm_base*f=( frm_base* ) parent;
      if ( f->barcode_enabled() ) f->releaseKeyboard();
    }
  }
}


QWidget*tr_tools::actWin() {
  if ( !win_queue.count() ) return NULL;
  if ( !win_queue.top()->isHidden() ) return win_queue.top();
  else {
    QWidget*p=win_queue.pop();
    QWidget*r=win_queue.top();
    win_queue.push( p );
    return r;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_tools::log( QString st ) {
  if ( st.isNull() ) cout<<"null o 0\n";
  else
    cout<<st<<endl;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_tools::label_to_name( QString etichetta ) {
  QString nome=etichetta.lower();
  //nome.replace(" ","_");
  nome.replace( QRegExp( "[,|;|:|.|/|\\| ]" ), "_" );
  nome.replace( QRegExp( "[è|é]" ), "e" );
  nome.replace( "à", "a" );
  nome.replace( "ò", "o" );
  nome.replace( "ì", "i" );
  nome.replace( "ù", "u" );
  return nome;
}


bool tr_tools::sillaba( QChar corrente, QChar succ ) {
  QString aiueo="aiueo";
  if ( corrente==' ' ) return true;
  if ( succ==' ' ) return true;
  if ( corrente==succ ) return true;
  if ( ( corrente.upper()=='N' )&&aiueo.find( succ, 0, false )<0 ) return true;
  if ( ( corrente.upper()=='A' )&&( succ.upper()=='N' ) ) return false;
  if ( aiueo.find( corrente, 0, false )>=0&&aiueo.find( succ, 0, false )<0 ) return true;
  return false;
}


QString tr_tools::solo_testo( QString st ) {
  st.remove( QRegExp( "<[^>]*>" ) );
  return st;
}


QString tr_tools::stringa( QByteArray t ) {
  QString st="";
  for ( uint i=0; i<t.count(); i++ ) {
    st+="\\";
    unsigned char c=t[i];
    st+=QSN( c ).rightJustify( 3, '0' );
  }
  return st;
}


QByteArray tr_tools::ba( QString st ) {
  int dim=st.length()/2;
  if ( dim<=0 ) dim=36;
  QByteArray b( dim );

  uint i;
  for ( i=0; i<36; i++ ) b[i]=0;

  if ( st.length()>0 ) {
    bool ok;
    QStringList e=QStringList::split( '\\', st );
    for ( i=0; i<e.count(); i++ ) b[i]=e[i].toInt( &ok );

  }

  return b;
}

  #include <qobjectlist.h>
void tr_tools::activateAllLayouts( QWidget*w ) {
  if ( w->layout() ) {
    w->layout()->activate();
  }

  if ( w->children() ) {
    QObjectList*l=( QObjectList* ) w->children(); // Silence please
    for ( uint i=0; i<l->count(); i++ ) {
      QObject*o=l->at( i );
      if ( o->isWidgetType() ) {
        activateAllLayouts( ( QWidget* ) o );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_tools::conto_clicked( tr_label*pulsante, tr_tavolo*t, QStringList stato_tavolo, double totale ) {
  QStringList tc;

  if ( t->stato()&itm_tavolo::conto_diviso ) {
    return true;
  }

  if ( t->bs()&&!config.conto_rapido_bis ) return true;

  if ( !t->tipi_conto().isEmpty() ) {
    tc=QStringList::split( ',', t->tipi_conto(), true );
  }

  bool ho_aperto_tavolo=false;

  if ( !tc.count() ) return true;
  else {
    denu_apri_tavolo;
    if ( !stato_tavolo.count() ) {

      if ( !::cmd( tok_apri_tavolo, QSN( t->id() ) ) ) return false;

      QString stato=srv->rcv();
      srv->eop();

      stato_tavolo=QStringList::split( config.cmd_sep, stato, true );
      ho_aperto_tavolo=true;
    }

    if ( stato_tavolo[p_articoli_p_nullo]=="1" &&config.chiedi_su_articoli_p_nullo ) {
      if ( msgbox( MSG_1, SINO )!=SI ) {
        if ( ho_aperto_tavolo )::cmd( tok_chiudi_tavolo );
        return false;
      }
    }

  }

  if ( tc.count()==1||( tc.count()==2&&t->tipi_conto().find( "-2" )!=-1 ) ) {
    QString conto;
    denu_elenco_tipi_conto;
    tr::mse etc( tok_elenco_tipi_conto );
    etc.carica();
    TROWSLI( etc )
    if ( etc[i].section( '|', p_id_documento, p_id_documento )==tc[0] ) {
      conto=etc[i];
      break;
    }
    if ( !conto.isEmpty() ) print( t, conto );
  }

  if ( tc.count()>1 ) {
    QString nuovo_totale=QString::null;
    denu_elenco_tipi_conto;
    QString etichette;
    tr::mse etc( tok_elenco_tipi_conto );
    etc.carica();
    QStringList etc_filtrati;
    QStringList indice;
    TROWSLI( etc ) {
      TROWSLJ( tc )
      if ( etc[i].section( '|', p_id_documento, p_id_documento )==tc[j] ) {
        if ( !etichette.isEmpty() ) etichette+='|';
        QString stampante=etc[i].section( '|', p_nome_stampante, p_nome_stampante ).lower();
        etichette+=( TR( stampante )+"#" +stampante );
        etc_filtrati<<etc[i];
        indice<<stampante;
      }
    }

    bool conto_normale=false;
    if ( t->tipi_conto().find( "-1" )!=-1 ) {
      if ( !etichette.isEmpty() ) etichette+='|';
      etichette+=( TR( "conto" )+"#conto" );
      conto_normale=true;
    }

    if ( conto_normale&&etichette==( TR( "conto" )+"#conto" ) ) {
      if ( ho_aperto_tavolo )::cmd( tok_chiudi_tavolo );
      return true;
    }

    QString n=hvmenu( pulsante, etichette );
    if ( n.isEmpty() ) {
      if ( ho_aperto_tavolo )::cmd( tok_chiudi_tavolo );
      return false;
    }
    if ( conto_normale&&n=="conto" ) {
      if ( ho_aperto_tavolo )::cmd( tok_chiudi_tavolo );
      return true;
    }
    else {
      QString valore=config.formatta( totale, 2 );
      if ( t->tipi_conto().find( "-2" )!=-1&&totale>=0 ) {
        if ( !totale&&!config.conto_zero_nsq ) BEEPERF;
        if ( !totale&&config.conto_zero_nsq==2 ) {
          if ( tools.msgbox( TR( "Totale a ZERO.<br>Si vuole stamparlo comunque?" ), SINO )!=SI ) return false;
        }
        if ( totale>0 ) {
          while ( 1 ) {
            valore=calcolatrice( pulsante, TR( "TOTALE VERSATO" ), valore, "real2" );
            if ( valore.isEmpty() ) return false;
            valore.replace( ',', '.' );
            if ( valore.toDouble()-totale<0 ) {
              BEEP;
              valore=config.formatta( totale, 2 );
              continue;
            }
            if ( valore.toDouble()-totale>=0&&totale>0 ) {
              if ( valore.toDouble()-totale!=0 ) nuovo_totale=valore;
              break;
            }
          }
        }
      }

      print( t, etc_filtrati[indice.findIndex( n )], nuovo_totale );

      if ( !nuovo_totale.isNull() ) {
        QString msg;
        nuovo_totale.replace( ',', '.' );
        double resto=nuovo_totale.toDouble()-totale;
        if ( resto ) msg=TR( "  RESTO: " )+config.formatta( resto, 2 )+"  ";
        else
          msg=TR( "  RESTO: nessuno  " );
        if ( display_lcd.inizializzato() ) {
          display_lcd.aggiorna( tr_display::tr_resto_conto, QString::null, QString::null, config.formatta( resto, 2 ) );
        } // if display
        tools.msgbox( msg, config.conto_resto );
      } // if

    }
  }

  if ( ho_aperto_tavolo )::cmd( tok_chiudi_tavolo );
  return false;

}



bool tr_tools::print( tr_tavolo* /* t */, QString tipo_conto, QString nuovo_totale ) {

  bool r=false;

  denu_elenco_tipi_conto;
  QString chiusura=tipo_conto.section( '|', p_tipo_chiusura, p_tipo_chiusura );
  QString stampante=tipo_conto.section( '|', p_nome_stampante, p_nome_stampante );
  QString tipo_doc=tipo_conto.section( '|', p_id_documento, p_id_documento );
  if ( CMD( tok_crea_conto, QString( "1" ) ) ) {

    int is=config.pg_contanti;

    QString prz;
    if ( nuovo_totale.isNull() ) prz=LEAVE;
    else prz=nuovo_totale;
    r=CMD( tok_inserisci_articolo_comanda, "1", QSN( is ), LEAVE, prz, LEAVE, LEAVE );
    if ( r ) r=CMD( tok_chiudi_conto, chiusura );
    if ( r ) r=CMD( tok_stampa_conto, tipo_doc, "0", stampante );
    if ( r ) r=CMD( tok_nuova_comanda );
    if ( !r ) CMD( tok_annulla_conto );

  } // crea conto

  return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_tools::barcode( QString txt ) {

  static QString barcode="";
  if ( txt.isNull() ) return barcode;
  if ( txt.isEmpty() ) return QString::null;

  QStringList cfg=QStringList::split( '|', config.barcode, true );

  enum {
    from, start, end, len, regexp
  };

  if ( cfg[start].isNull()&&cfg[end].isNull()&&cfg[len].isNull() ) return QString::null;

  barcode+=txt;

  if ( !cfg[start].isNull()&&barcode.length()>0 ) {
    int pos=barcode.find( cfg[start] );
    if ( pos>-1 ) barcode=barcode.mid( pos );
  }

  if ( cfg[len].isEmpty()||barcode.length()>=cfg[len].toInt() ) {
    bool ok=false;
    bool ok1=false;
    if ( cfg[start].isEmpty() ) ok=true;
    if ( cfg[end].isEmpty() ) ok1=true;
    if ( !ok&&barcode.left( cfg[start].length() )==cfg[start] ) ok=true;
    if ( !ok1&&barcode.right( cfg[end].length() )==cfg[end] ) ok1=true;
    if ( ok&&ok1 ) {
      BEEP;
      int len=barcode.length()-cfg[start].length()-cfg[end].length();
      barcode=barcode.mid( cfg[start].length(), len );
      QString st=barcode;
      barcode="";
      return st;
    }
    if ( cfg[len].toInt()>0 )
      barcode="";
  }

  return QString::null;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_tools::tr_elenco tr_tools::elenco( const QStringList elenco, QString titolo, const QString struttura ) {

  tr_elenco el;

  frm_base*base;
  base=( frm_base* ) actWin();

  if ( titolo.isEmpty() ) titolo=tr( "voce" );
  QString str_elenco=TRFORM( (
  // "minmax=70%|70%|90%|80%\n"
  "margine=0\n""status=Scegliere un/a %titolo%\n""listview=elenco#lv(abc,onclick)\n""buttons=uscita#exit\n" ) );

  FRM_PARAMS;
  FRM_PARAM( "%titolo%", titolo );

  bFORM( frm_elenco, str_elenco, base );

  CTRL_FRM( frm_elenco );

  CTRL( lv );
  lv.values( elenco );
  lv.listview->tipo_fittizio( struttura );
  CTRL( cancel );

  tr_widget*iterato=frm_elenco->interazione();

  QString risul;

  if ( iterato==lv ) {
    el.descrizione=lv.listview->descrizione();
    el.id=lv.listview->id();
    el.record=lv.listview->record();
  }

  xFORM( frm_elenco, str_elenco );

  return el;
}


QString tr_tools::elenco( tr_input*in ) {

  tr_elenco el;

  if ( in->_elenco.tp!=tr_listview::indefinito ) {
    QString record;
    scelta( in->_elenco.tp, el.id, el.descrizione, record );
    if ( !el.descrizione.isNull() ) {
      in->setText( el.descrizione );
      in->_elenco.id=el.id;
    }
  }
  else {

    QStringList elenco_filtrato;

    if ( in->_elenco.nkeys>0 ) {
      QString radice;
      int start, end;
      in->edit->getSelection( &start, &end );
      if ( in->edit->hasSelectedText() ) radice=in->edit->text().left( start );
      else
        radice=in->text();
      TROWSLI( in->_elenco.elenco ) {
        const int p_descrizione=2;
        QString descrizione=in->_elenco.elenco[i].section( '|', p_descrizione, p_descrizione );
        if ( descrizione.left( radice.length() )==radice )
          elenco_filtrato<<in->_elenco.elenco[i];
      }
    }
    else
      elenco_filtrato=in->_elenco.elenco;

    el=elenco( elenco_filtrato, in->label->text(), in->_elenco.struttura );

    if ( !el.descrizione.isNull() ) {
      in->setText( el.descrizione );
      in->_elenco.id=el.id;
    }
  }

  return el.descrizione;

}

QPixmap PIXMAP( QString nome ) {
  if ( config.pocket.isEmpty() ) return QPixmap::fromMimeSource( nome );
  else {
    QImage img_giu;
    img_giu=QPixmap::fromMimeSource( nome ).convertToImage();
    img_giu=img_giu.smoothScale( 24, 24 );
    QPixmap p;
    p.convertFromImage( img_giu );
    return p;
  }
}

frm_base*tr_tools::find_base( tr_widget*w ) {
  QObject*parent=w->parent();
  while ( parent&&!parent->inherits( "frm_base" ) ) parent=parent->parent();
  return ( frm_base* ) parent;
}

frm_base*tr_tools::find_right_base_parent( frm_base*new_window ) {
  frm_base*parent=( frm_base* ) actWin();
  if ( !parent||( new_window&&new_window->isPopup() ) ) return parent;
  if ( !parent->isPopup() ) return parent;
  while ( parent&&parent->isPopup() ) {
    if ( parent->inherits( "frm_base" ) ) parent=( frm_base* ) parent->parent();
    while ( parent&&!parent->inherits( "frm_base" ) ) {
      parent=( frm_base* ) parent->parent();
    }
  }
  return parent;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  #undef CMDSRV_LANGTOKEN

  #define CMDSRV_LANGUAGE
  #define CMDSRV_DEFONLY
  #include "cmdlangtoken.hpp"

extern TRIstruzioneLinguaggio comandi[];

QString tr_tools::get_cmd_struct( TRTokens tok, int pos_desc, int pos_key ) {
  QString struttura;
  for ( int i=0; ::comandi[i].token!=tok_endoftokens; i++ )
    if ( ::comandi[i].token==tok ) {
      struttura=::comandi[i].output.section( ",e,", 1, 1 );
      break;
    }
  QStringList cols=QStringList::split( ',', struttura );
  TROWSLI( cols ) {
    if ( cols[i].left( 2 )=="i_" ||cols[i].left( 2 )=="d_" ) cols[i]='#' +cols[i].mid( 2 );
    if ( cols[i].left( 2 )=="s_" &&( pos_desc==-1||pos_desc==i ) ) {
      cols[i]='$' +cols[i].mid( 2 );
      pos_desc=-i;
    }
    if ( pos_key==i ) cols[i]='@' +cols[i].mid( 2 );
  }
  return cols.join( "|" );
}

void tr_tools::sleep(int ms)
{
  QTime t; t.start();
  while (t.elapsed()<ms) qApp->processEvents();
}

#endif
