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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      TR_LABEL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TR_LABEL_CPP
  #define TR_LABEL_CPP

  #include <qregexp.h>
  #include <qimage.h>
  #include <qobjectlist.h>

  #include "tr_label.hpp"
  #include "tr_dmacro.hpp"
  #include "tr_dmacro2.hpp"

  #ifndef TR_CONFIG_CPP
    #include "tr_config.hpp"
extern tr_config config;
  #endif

  #ifndef TR_TOOLS_CPP
    #include "tr_tools.hpp"
extern tr_tools tools;

  #endif


tr_label::tr_label( QWidget*parent, const QString name, WFlags ):tr_widget( parent, name.latin1() ) {
  indice=0;
  _su_piu_righe=false;
  ripetizione( false );
  darkable( false );
  _oneline=false;
  onlinks=false;
  label=NULL;
  _color_cycling=QString::null;

  setBackgroundOrigin( QWidget::WindowOrigin );

  //setBackgroundOrigin( TIPO_BASE::WindowOrigin );
  //setBackgroundMode( Qt::PaletteBase );
  //setBackgroundOrigin( tr_widget::ParentOrigin );

  QGridLayout*_layout;
  NEW( _layout, QGridLayout( this ) );
  _layout->setResizeMode( QLayout::Auto );

  NEW( tabs, tr_tab_widget( this, "tabs" ) );
  tabs->setBackgroundOrigin( QWidget::WindowOrigin );
  _layout->addWidget( tabs, 0, 0 );
  // _layout->addWidget( tabs, 0, 0, Qt::AlignHCenter|Qt::AlignLeft );

  NEW( base, QFrame( tabs, "base" ) );
  base->setBackgroundOrigin( QWidget::WindowOrigin );
  base->setFrameShape( QFrame::NoFrame );
  base->setFrameShadow( QFrame::Plain );
  base->setMidLineWidth( 0 );
  base->setMargin( 0 );
  tabs->addWidget( base );
  bases[0]=base;
  bases[1]=NULL;

  NEW( layout, QGridLayout( base ) );
  layout->setResizeMode( QLayout::Auto );

  //layout()->setMargin(5);
  NEW( label, QLabel( base, "etichetta_di_tr_label" ) );
  layout->addWidget( label, 0, 1 );
  // layout->addWidget( label, 0, 1, Qt::AlignVCenter|Qt::AlignLeft );

  //label->setBackgroundOrigin( tr_widget::ParentOrigin );
  label->setAlignment( int( QLabel::WordBreak|QLabel::AlignCenter ) );
  label->setScaledContents( false );
  _aspetto=piatto;
  label->setTextFormat( Qt::RichText );

  icona=NULL;

  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  setPaletteForegroundColor( config.colore_selezione );

  if ( parent ) {
    setPaletteBackgroundColor( parent->paletteBackgroundColor() ); // QColor( 0,117,175 ) );
    setMaximumSize( parent->maximumSize() );
    setMinimumSize( parent->minimumWidth(), config.altezza_riga );
  }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_label::~tr_label() {
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::aspetto_piatto() {
  label->setFrameShape( QLabel::NoFrame );
  label->setFrameShadow( QLabel::Plain );
  label->setLineWidth( 0 );
  //setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ));
  _aspetto=piatto;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::alza( bool alza ) {
  if ( alza ) {
    label->setFrameShape( QLabel::Panel );
    label->setFrameShadow( QLabel::Raised );
    label->setLineWidth( _spessore );
  }
  else {
    label->setFrameShape( QLabel::Panel );
    label->setFrameShadow( QLabel::Sunken );
    label->setLineWidth( _spessore );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::aspetto_bottone( int profondita ) {
  /* if (profondita==0) { label->setFrameShape( QLabel::Box ); label->setFrameShadow( QLabel::Plain );
  profondita=1; } else { */
  label->setFrameShape( QLabel::Panel );
  label->setFrameShadow( QLabel::Raised );
  // }
  label->setLineWidth( profondita );
  //setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ));
  _aspetto=rialzato;
  _spessore=profondita;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QColor tr_label::aspetto_bottone( QString normale, QString premuto ) {
  // label->setBackgroundOrigin( TIPO_BASE::WidgetOrigin );
  _aspetto=pulsante;
  _normale=normale;
  if ( !premuto.isEmpty() )
    _premuto=premuto;
  else
    _premuto=normale.left( normale.findRev( '.' ) )+"_premuto" +normale.mid( normale.findRev( '.' ) );
  stato( tr_label::normale );

  const QPixmap*p=label->paletteBackgroundPixmap();
  QColor sfondoXY( config.colore_sfondo ); // , sfondoX1Y1;
  if ( p ) {
    if ( p->isNull() ) {
      cout<<"pixmap nullo\n";
    }
    QImage i=p->convertToImage();
    sfondoXY=i.pixel( 0, 0 ); // preleva il colore di sfondo dal primo pixel
  }
  return sfondoXY;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::aspetto_check( bool attivo ) {
  label->setFrameShape( QLabel::NoFrame );
  label->setFrameShadow( QLabel::Plain );
  label->setLineWidth( 0 );
  _aspetto=check;
  NEW( icona, tr_label( base, "icona" ) );
  icona->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
  connect( icona, SIGNAL( clicked() ), this, SIGNAL( clicked() ) );
  layout->addWidget( icona, 0, 0, Qt::AlignCenter );
  label->setAlignment( int( QLabel::WordBreak|QLabel::AlignLeft|QLabel::AlignVCenter ) );

  if ( attivo ) stato( on );
  else
    stato( off );
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::aspetto_titolo() {
  label->setFrameShape( QLabel::NoFrame );
  label->setFrameShadow( QLabel::Plain );
  label->setLineWidth( 0 );
  label->setTextFormat( Qt::RichText );
  label->setAlignment( int( QLabel::SingleLine|QLabel::AlignCenter ) );
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int tr_label::number() {
  if ( _aspetto==radio ) return _radio;
  if ( _aspetto==check ) return isOn();
  return label->text().toInt();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::number( int n ) {
  if ( _aspetto==radio ) {
    tr_label*ico;
    if ( _radio!=-1 ) {
      ico=( tr_label* ) child( "ico_" +QString::number( _radio ) );
      ico->setPixmap( PIXMAP( "radio_off.png" ) );
    }
    ico=( tr_label* ) child( "ico_" +QString::number( n ) );
    if ( ico ) {
      ico->setPixmap( PIXMAP( "radio_on.png" ) );
      _radio=n;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::aspetto_radio( const QStringList etichette, int start ) {
  aspetto_radio( etichette, true, start );
}

void tr_label::aspetto_radio( const QStringList etichette, bool verticale, int start ) {

  _aspetto=radio;

  int pos=0;

  int i=0;
  while ( i<etichette.count() ) {

    tr_label*ico;
    NEW( ico, tr_label( base, "ico_" +QString::number( i ) ) );
    connect( ico, SIGNAL( clicked() ), this, SIGNAL( clicked() ) );
    if ( start==-1&&etichette[i] [0]=='*' ) start=i;
    if ( i==start ) ico->setPixmap( PIXMAP( "radio_on.png" ) );
    else
      ico->setPixmap( PIXMAP( "radio_off.png" ) );
    if ( !i ) {
      icona=ico;
      layout->addWidget( icona, 0, 0, Qt::AlignCenter );
    }
    QLabel*lab;
    if ( !i ) lab=label;
    else
      NEW( lab, QLabel( base, "lab_" +QString::number( i ) ) );

    if ( i==start&&etichette[i] [0]=='*' ) lab->setText( TR( etichette[i].mid( 1 ) ) );
    else
      lab->setText( TR( etichette[i] ) );
    ico->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
    if ( _oneline ) {
      lab->setAlignment( int( QLabel::SingleLine|QLabel::AlignLeft ) );
      ico->setAlignment( int( QLabel::SingleLine|QLabel::AlignVCenter ) );
    }
    else {
      lab->setAlignment( int( QLabel::WordBreak|QLabel::AlignLeft ) );
      ico->setAlignment( int( QLabel::AlignVCenter ) );
    }
    lab->setScaledContents( false );
    lab->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

    if ( i>0 ) {
      if ( verticale ) {
        layout->addWidget( ico, pos, 0, Qt::AlignCenter );
        layout->addWidget( lab, pos, 1, Qt::AlignLeft );
      }
      else {
        layout->addWidget( ico, 0, pos++, Qt::AlignCenter );
        layout->addWidget( lab, 0, pos++, Qt::AlignLeft );
      }
    }
    if ( !i&&!verticale ) pos++;

    pos++;
    i++;
  } // while

  _radio=start;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::stato( tr_stato s ) {
  _stato=s;
  if ( _aspetto!=radio ) {
    if ( _aspetto==check ) {
      if ( s==on ) icona->setPixmap( PIXMAP( "chk_on.png" ) );
      else
        icona->setPixmap( PIXMAP( "chk_off.png" ) );
    }
    else {
      if ( _stato==premuto )
        label->setPaletteBackgroundPixmap( PIXMAP( _premuto ) );
      if ( _stato==normale ) label->setPaletteBackgroundPixmap( PIXMAP( _normale ) );
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::premi() {
  if ( _animato ) {
    if ( _aspetto==pulsante )
      stato( premuto );
    if ( _aspetto==rialzato ) alza( false );
  }

  if ( !_color_cycling.isEmpty() ) {
    QStringList colors=QStringList::split( '|', _color_cycling );
    TROWSLI( colors ) {
      QColor c( colors[i] );
      if ( c==paletteBackgroundColor() ) {
        setPaletteBackgroundColor( QColor( colors[i+1] ) );
        break;
      }
    }
  }

  timeIdAnimazione=startTimer( config.repeat_ms );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::rilascia() {
  if ( _animato ) {
    if ( _aspetto==pulsante )
      stato( normale );
    if ( _aspetto==rialzato ) alza( true );
  }
  killTimer( timeIdAnimazione );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_label::event( QEvent*e ) {

  bool vai_col_click=false;
  bool risult=false;
  static int timeId=-1;
  bool finto_mouse_press=false;
  if ( stato()==premuto ) {
    if ( e->type()==QEvent::Timer ) {
      QTimerEvent*t=( QTimerEvent* ) e;
      if ( t->timerId()==timeIdAnimazione ) rilascia();
    }
  }

  if ( _ripetizione ) {
    if ( e->type()==QEvent::Timer ) {
      QTimerEvent*t=( QTimerEvent* ) e;
      if ( t->timerId()==timeId ) {
        finto_mouse_press=true;
        risult=true;
      }
    }

    if ( finto_mouse_press&&isEnabled() ) {
      risult=true;
      emit clicked();
    }
  }
  if ( e->type()==QEvent::MouseButtonPress||e->type()==QEvent::MouseButtonDblClick ) {

    if ( isEnabled() ) {

      // tools.log_event(name(),e);

      premi();

      QMouseEvent*me=( QMouseEvent* ) e;
      QWidget*w=childAt( me->pos() );

      if ( w ) {


        if ( _aspetto==radio ) {

          tr_label*ico=NULL;
          QLabel*lab=NULL;

          if ( !qstrncmp( w->name(), "ico_", 4 ) ) ico=( tr_label* ) w;
          if ( !qstrncmp( w->name(), "lab_", 4 ) ) lab=( QLabel* ) w;
          if ( w==label ) lab=label;
          if ( ico||lab ) {
            int pos;
            QString st;
            if ( lab!=label ) {
              st=QString( w->name()+4 );
              pos=QString( st ).toInt();
            }
            else {
              pos=0;
              st="0";
            }
            if ( _radio!=-1&&_radio!=pos ) {
              tr_label*ico=( tr_label* ) child( "ico_" +QString::number( _radio ) );
              ico->setPixmap( PIXMAP( "radio_off.png" ) );
            }
            if ( !ico ) ico=( tr_label* ) child( "ico_" +st );
            //if (!lab) lab = (QLabel*)child("lab_" + st );
            ico->setPixmap( PIXMAP( "radio_on.png" ) );
            _radio=pos;
          }
        }

        if ( _aspetto==check ) {
          not_stato();
        }

      } // if (w)

      if ( _ripetizione ) {
        timeId=startTimer( config.repeat_ms );
      }
      risult=true;
      vai_col_click=true;
    }
  }

  if ( _ripetizione ) {
    if ( e->type()==QEvent::MouseButtonRelease&&timeId>-1 ) {
      killTimer( timeId );
      timeId=-1;
      risult=true;
    }
  }

  if ( isEnabled()&&e->type()==QEvent::MouseButtonRelease ) {
    rilascia();
    risult=true;
  }


  if ( risult ) {
    if ( vai_col_click ) {
      emit clicked();
    }
    return true;
  }
  else
    return tr_widget::event( e );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::cambia_testo( QString st ) {
  bool bold=false;
  if ( text().find( "<b>" )!=-1 ) bold=true;
  QRegExp rx( "<[^>]*>" );
  rx.search( text() );
  QStringList s=rx.capturedTexts();
  if ( s.size()==0 ) setText( st, 0, bold );
  else
    setText( s[0]+"<br>" +st, 0, bold );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::cambia_icona( QString st ) {
  if ( !ha_immagine() ) {
    if ( immagine_fissa() ) setText( P32ICON( st )+text() );
    else
      setText( PICON( st )+text() );
  }
  else {
    QString _st=solo_testo();
    if ( immagine_fissa() ) setText( P32ICON( st )+_st );
    else
      setText( PICON( st )+_st );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool tr_label::ha_immagine() {
  if ( text().find( "img name" )!=-1 ) return true;
  else
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool tr_label::immagine_fissa() {
  if ( ha_immagine()&&text().find( "width=" )!=-1 ) return true;
  else
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_label::ha_pixmap() {
  if ( label->pixmap()==NULL ) return false;
  else
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_label::oneline( bool o ) {
  _oneline=o;
  if ( _oneline ) {
    if ( label ) {
      int a=label->alignment()&!QLabel::WordBreak;
      label->setAlignment( int( a|QLabel::SingleLine ) );
    }
  }
  else if ( label ) {
    int a=label->alignment()&!QLabel::SingleLine;
    label->setAlignment( int( a|QLabel::WordBreak ) );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::setEnabled( bool stato ) {

  tr_widget::setEnabled( stato );
  if ( _darkable )
    if ( backgroundMode()!=Qt::NoBackground ) {
      if ( _aspetto!=check&&_aspetto!=radio ) {
        if ( stato ) {
          label->setPaletteBackgroundColor( palette().active().background() );
          if ( icona ) icona->setPaletteBackgroundColor( palette().active().background() );
        }
        if ( !stato ) {
          label->setPaletteBackgroundColor( palette().disabled().background() );
          if ( icona ) icona->setPaletteBackgroundColor( palette().disabled().background() );
        }
      }
      //   }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::setPaletteBackgroundPixmap( QPixmap p ) {
  tr_widget::setPaletteBackgroundPixmap( p );
  if ( label ) label->setPaletteBackgroundPixmap( p );
  if ( icona ) icona->setPaletteBackgroundPixmap( p );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::setPaletteBackgroundColor( QColor c ) {
  tr_widget::setPaletteBackgroundColor( c );
  if ( darkable() ) palette_dark_background( this );
  tabs->setPaletteBackgroundColor( c );
  base->setPaletteBackgroundColor( c );
  label->setPaletteBackgroundColor( c );
  if ( darkable() ) palette_dark_background( label );
  if ( icona ) {
    icona->setPaletteBackgroundColor( c );
    if ( darkable() ) palette_dark_background( icona );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_label::setPaletteForegroundColor( QColor c ) {
  tr_widget::setPaletteForegroundColor( c );
  label->setPaletteForegroundColor( c );
  if ( darkable() ) palette_dark( label );
  if ( icona ) {
    icona->setPaletteForegroundColor( c );
    if ( darkable() ) palette_dark( icona );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_label::setMinimumSize( int w, int h ) {
  tr_widget::setMinimumSize( w, h );
  QSize sl( w, h );
  if ( _aspetto==check||_aspetto==radio ) {
    sl.setWidth( sl.width()-icona->width() );
  }
  label->setMinimumSize( sl );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_label::setMaximumSize( int w, int h ) {
  tr_widget::setMaximumSize( w, h );
  QSize sl( w, h );
  if ( _aspetto==check||_aspetto==radio ) {
    sl.setWidth( sl.width()-icona->width() );
  }
  label->setMaximumSize( sl );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_label::setMinimumWidth( int w ) {
  tr_widget::setMinimumWidth( w );
  if ( _aspetto==check||_aspetto==radio ) {
    w-=icona->width();
  }
  label->setMinimumWidth( w );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_label::setMinimumHeight( int h ) {
  tr_widget::setMinimumHeight( h );
  label->setMinimumHeight( h );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_label::setMaximumWidth( int w ) {
  tr_widget::setMaximumWidth( w );
  if ( _aspetto==check||_aspetto==radio ) {
    w-=icona->width();
  }
  label->setMaximumWidth( w );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_label::setMaximumHeight( int h ) {
  tr_widget::setMaximumHeight( h );
  label->setMaximumHeight( h );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_label::setSizePolicy( QSizePolicy sp ) {
  tr_widget::setSizePolicy( sp );
  if ( tabs ) tabs->setSizePolicy( sp );
  if ( label ) label->setSizePolicy( sp );
  int i=0;
  while ( bases[i]!=NULL ) bases[i++]->setSizePolicy( sp );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_label::setSizePolicy( QSizePolicy::SizeType hor, QSizePolicy::SizeType ver, bool hfw ) {
  tr_widget::setSizePolicy( hor, ver, hfw );
  if ( tabs ) tabs->setSizePolicy( hor, ver, hfw );
  if ( label ) label->setSizePolicy( hor, ver, hfw );
  int i=0;
  while ( bases[i]!=NULL ) bases[i++]->setSizePolicy( hor, ver, hfw );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::setSplit( QString format, QString data ) {
  QStringList var_val=QStringList::split( ';', data );

  if ( bases[1] ) delete bases[1];
  bases[1]=new QFrame( this, format );
  bases[1]->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  bases[1]->setBackgroundOrigin( QFrame::WindowOrigin );
  bases[1]->setFrameShape( QFrame::NoFrame );
  bases[1]->setFrameShadow( QFrame::Plain );
  bases[1]->setMidLineWidth( 0 );
  bases[1]->setMargin( 0 );
  tabs->addWidget( bases[1] );
  bases[2]=NULL;

  QLayout*l=bases[1]->layout();
  int n_righe=format.contains( ';' )+1;
  int n_colonne=0;
  QStringList righe=QStringList::split( ';', format );
  int altezza_font=bases[1]->height()/n_righe-2;


  QVBoxLayout*vb;
  if ( l ) vb=new QVBoxLayout( l, -1, "main_vbox" );
  else
    vb=new QVBoxLayout( bases[1], 0, -1, "main_vbox" );
  FORJ( n_righe ) {
    QStringList var_val=QStringList::split( ';', data );
    QString riga=righe[j];
    int n_cols=righe[j].contains( ',' )+1;
    QHBoxLayout*hb=new QHBoxLayout( vb, -1, QSN( j ) );
    QStringList cells=QStringList::split( ',', riga );
    int x=0;
    TROWSLI( cells ) {
      QString c=cells[i];
      int p_key=c.find( QRegExp( "%([^%)]*)%" ) );
      int p_key_end=-1;
      if ( p_key>-1 ) p_key_end=c.find( '%', p_key+1 );
      QString key=c.mid( p_key+1, p_key_end-p_key-1 );
      QLabel*txt=new QLabel( bases[1], key );
      txt->setBackgroundOrigin( QFrame::WindowOrigin );
      QFont f( txt->font() );
      f.setPixelSize( altezza_font );
      txt->setFont( f );
      hb->addWidget( txt );
      QString value;
      value=value_of( var_val, key ).section( '=', 1, 1 );
      c.replace( "%" +key+"%", value );
      txt->setAlignment( Qt::AlignCenter );
      txt->setText( c );
      x++;
    }
  }
  tabs->raiseWidget( bases[1] );
} // setSplit

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::setText( QString icona, QString txt ) {
  tabs->raiseWidget( base );
  cambia_icona( icona );
  cambia_testo( txt );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::setText( QString txt, int pixelSize, bool bold ) {
  tabs->raiseWidget( base );
  if ( pixelSize>0 ) {
    QFont f( font() );
    f.setPixelSize( pixelSize );
    if ( bold ) f.setBold( true );
    setFont( f );
    label->setFont( f );
  }
  if ( pixelSize<0 ) {
    QFont f( font() );
    f.setPixelSize( f.pixelSize()+pixelSize );
    if ( bold ) f.setBold( true );
    setFont( f );
    label->setFont( f );
  }
  if ( pixelSize==0 ) {
    QFont f( font() );
    if ( bold ) f.setBold( true );
    setFont( f );
    label->setFont( f );
  }
  label->setText( txt );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::adatta( bool su_piu_righe ) {
  _su_piu_righe=su_piu_righe;
  return;
  if ( !label ) return;
  int w=label->alignment();
  QString testo_originale=label->text();
  label->setText( solo_testo() );
  if ( !label->sizeHint().isValid() ) cout<<"minsizhint �invalido\n";
  if ( !label->contentsRect().size().isValid() ) cout<<"Content rect �invalidp\n";
  while ( ( label->sizeHint().width()>label->contentsRect().size().width()||
     label->sizeHint().height()>label->contentsRect().size().height() ) ) {
       QFont f( label->font() );
       if ( f.pixelSize()<9 ) break;
       f.setPixelSize( f.pixelSize()-1 );
       label->setFont( f );
       setFont( f );
  }
  label->setText( testo_originale );
  label->setAlignment( w );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_label::solo_testo() {
  return tools.solo_testo( text() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_label::text() {
  return label->text();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::light() {
  colore_sfondo=paletteBackgroundColor();
  setPaletteBackgroundColor( colore_sfondo.light( 200 ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_label::dark() {
  setPaletteBackgroundColor( colore_sfondo );
}

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      TR_LISTVIEW
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TR_LISTVIEW_CPP
  #define TR_LISTVIEW_CPP

/* HISTORY: 28-09-2004 migliorato ::adatta() */

  #include <typeinfo>
  #include <qregexp.h>
  #include <qimage.h>
  #include <qheader.h>
  #include <qpainter.h>

/* #include <qfontmetrics.h> #include <qpainter.h> #include <qlistview.h> #include <qptrvector.h> #include <qstyle.h> */

  #include "tr_listview.hpp"
  #include "tr_label.hpp"
  #include "tr_common.hpp"
  #include "tr_srvcmd.hpp"
  #include "tr_globals.hpp"
  #include "tr_config.hpp"
  #include "tavolo_tipi.hpp"
  #include "tr_dmacro.hpp"
  #include "tr_tools.hpp"


// #define debug_esclusioni

extern tr_globals glb;
extern tr_config config;
extern tr_tools tools;

const QString tr_listview::e_next="-3";
const QString tr_listview::e_last="-2";
const QString tr_listview::e_none="-1";
const QString tr_listview::e_first="0";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      TR_LISTVIEW
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void QListViewItem::paintFocus( QPainter*, const QColorGroup&, const QRect& ) {
}

void QListViewItem::setHeight( int height ) {
  if ( listView() ) {
    QObject*p=listView()->parent();
    if ( p&&qstrcmp( p->className(), "tr_listview" )==0 ) {
      tr_listview*lv=( tr_listview* ) p;
      if ( height!=0 ) height=lv->altezza_riga();
    }
  }

  // else  QListViewItem::setHeight( height );

  if ( ownHeight!=height ) {
    if ( visible )
      ownHeight=height;
    else
      ownHeight=0;
    invalidateHeight();
  }
}


///////////////////////////////////////////////////////////////////

tr_listview::tr_listview( QWidget*parent, const QString name ):tr_widget( parent, name ) {

  elenco=NULL;
  rollone=false;
  last_opened=NULL;
  unselectable( true );

  NEW( layout, QGridLayout( this, 2, 2, 0, 0, "layout" ) );
  layout->setResizeMode( QLayout::Auto );

  setPaletteBackgroundColor( parent->paletteBackgroundColor() );
  int maxdx, maxdy;

  parent?maxdx=parent->maximumWidth():32767;
  parent?maxdy=parent->maximumHeight():32767;
  if ( parent->layout() ) {
    int m=( parent->layout()->margin()*2 );
    maxdx-=m;
    maxdy-=m;
  }
  setMinMax( 0, 0, maxdx, maxdy );

  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

  if ( darkable() ) palette_dark( this );

  NEW( titolo, QLabel( this, "titolo" ) );
  titolo->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  titolo->setMinimumHeight( 0 );
  titolo->setMaximumHeight( config.altezza_riga );
  maxdy-=config.altezza_riga;
  titolo->setMaximumWidth( maxdx );
  titolo->setPaletteBackgroundColor( paletteBackgroundColor() );
  titolo->setPaletteForegroundColor( config.colore_selezione );
  titolo->hide();

  NEW( elenco, QListView( this, "elenco" ) );

  elenco->header()->setClickEnabled( false );
  elenco->header()->setResizeEnabled( false );
  elenco->header()->setTracking( false );
  elenco->header()->setStretchEnabled( false );
  elenco->header()->setMovingEnabled( false );
  // elenco->header()->setFixedHeight(config.elenchi_font_size);
  disconnect( elenco->header(), 0, 0, 0 );

  nascondi( e_header );
  elenco->setSorting( -1 );
  elenco->setVScrollBarMode( QScrollView::AlwaysOff );
  elenco->setHScrollBarMode( QScrollView::AlwaysOff );
  int dy=config.elenchi_altezza_riga*2+config.elenchi_altezza_riga/2;
  if ( dy<config.elenchi_button_size*2 ) dy=config.elenchi_button_size*2; // altezza freccia su +altezza freccia gi
  elenco->setMinimumSize( QSize( 0, dy ) );
  elenco->setMaximumSize( QSize( maxdx-config.elenchi_button_size, maxdy ) );
  elenco->setPaletteForegroundColor( config.elenchi_color_fg );
  elenco->setPaletteBackgroundColor( config.elenchi_color_bg );
  elenco->header()->setPaletteBackgroundColor( config.elenchi_color_bg.dark( 120 ) );
  elenco->setFrameShape( QFrame::NoFrame );
  elenco->setFrameShadow( QFrame::Plain );
  elenco->verticalScrollBar()->setSteps( config.elenchi_altezza_riga, config.elenchi_altezza_riga );
  elenco->horizontalScrollBar()->setSteps( config.elenchi_altezza_riga, config.elenchi_altezza_riga );
  elenco->setSelectionMode( QListView::Single );
  QFont f=elenco->font();
  f.setPixelSize( config.elenchi_font_size );
  elenco->setFont( f );
  elenco->setSortOrder( Qt::Ascending );

  QPalette pal=elenco->palette();
  QColorGroup cg=pal.active();
  cg.setColor( QColorGroup::Highlight, config.colore_selezione );
  cg.setColor( QColorGroup::HighlightedText, QColor( config.elenchi_color_sel ) );
  pal.setActive( cg );
  elenco->setPalette( pal );

  // questo bisogna lasciarlo sen�al setDisabled non si capisce proprio se �disabilitato
  palette_dark( elenco );
  elenco->setAllColumnsShowFocus( true );

  elenco->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

  connect( elenco, SIGNAL( mouseButtonClicked( int, QListViewItem*, const QPoint&, int ) ),
     this, SLOT( elenco_mouseButtonClicked( int, QListViewItem*, const QPoint&, int ) ) );
  connect( elenco, SIGNAL( clicked( QListViewItem*, const QPoint&, int ) ),
     this, SLOT( elenco_clicked( QListViewItem*, const QPoint&, int ) ) );

  // connect( elenco, SIGNAL( doubleClicked( QListViewItem*) ),
  //    this, SLOT( elenco_clicked( QListViewItem*, QPoint(), 0 ) ) );
  connect( elenco, SIGNAL( selectionChanged( QListViewItem* ) ), this, SLOT( elenco_selectionChanged( QListViewItem* ) ) );

  // barra di sinistra

  NEW( barra, QFrame( this, "barra" ) );
  barra->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding ) );
  barra->setMinimumSize( QSize( config.elenchi_button_size, 0 ) );
  barra->setMaximumSize( QSize( config.elenchi_button_size, maxdy ) );
  barra->setPaletteBackgroundColor( paletteBackgroundColor() );
  NEW( barra_layout, QVBoxLayout( barra, 0, 0, "puls_layout" ) );

  NEW( abc, QFrame( barra, "abc" ) );
  abc->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding ) );
  abc->setMinimumSize( QSize( config.elenchi_button_size, 0 ) );
  abc->setMaximumSize( QSize( config.elenchi_button_size, maxdy ) );
  abc->setFrameShape( QFrame::NoFrame );
  abc->setFrameShadow( QFrame::Plain );
  abc->setPaletteBackgroundColor( paletteBackgroundColor() );

  NEW( abc_layout, QVBoxLayout( abc, 0, 0, "abc_layout" ) );

  char*nomi[]= { "abc_1", "abc_2", "abc_3", "abc_4", "abc_5", "abc_6", "abc_7", "abc_8", NULL };
  char*etichette[]= { "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz", NULL };
  int i=0;
  while ( etichette[i]!=NULL ) {
    tr_label*p;
    NEW( p, tr_label( abc, nomi[i] ) );
    p->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding ) );
    QFont f( p->font() );
    f.setPixelSize( config.elenchi_abc_font_size );
    if ( config.pocket.isEmpty() ) f.setBold( true );
    p->setFont( f );
    p->setPaletteForegroundColor( config.colore_selezione );
    p->setMinimumSize( QSize( config.elenchi_button_size, config.elenchi_altezza_riga ) );
    p->setMaximumSize( QSize( config.elenchi_button_size, maxdy ) );
    p->setText( QString( etichette[i] ).upper() );
    p->setBackgroundMode( Qt::NoBackground );
    abc_layout->addWidget( p );
    connect( p, SIGNAL( clicked() ), this, SLOT( abc_clicked() ) );
    i++;
  }


  NEW( su, tr_label( barra, "su" ) );
  su->ripetizione( true );

  su->setFixedSize( config.elenchi_button_size, config.elenchi_button_size );
  su->setBackgroundOrigin( QLabel::ParentOrigin );
  su->setBackgroundMode( Qt::NoBackground );
  su->setPixmap( PIXMAP( "az_su.png" ) );
  su->setAlignment( int( QLabel::AlignCenter ) );
  connect( su, SIGNAL( clicked() ), this, SLOT( su_clicked() ) );

  NEW( giu, tr_label( barra, "giu" ) );
  giu->ripetizione( true );
  giu->setFixedSize( config.elenchi_button_size, config.elenchi_button_size );
  giu->setBackgroundOrigin( QLabel::ParentOrigin );
  giu->setBackgroundMode( Qt::NoBackground );
  giu->setPixmap( PIXMAP( "az_giu.png" ) );
  giu->setAlignment( int( QLabel::AlignCenter ) );
  connect( giu, SIGNAL( clicked() ), this, SLOT( giu_clicked() ) );

  NEW( destra, tr_label( barra, "destra" ) );
  destra->ripetizione( true );

  destra->setFixedSize( config.elenchi_button_size, config.elenchi_button_size );
  destra->setBackgroundOrigin( QLabel::ParentOrigin );
  destra->setBackgroundMode( Qt::NoBackground );
  destra->setPixmap( PIXMAP( "az_dx.png" ) );
  destra->setAlignment( int( QLabel::AlignCenter ) );
  connect( destra, SIGNAL( clicked() ), this, SLOT( destra_clicked() ) );
  destra->hide();

  NEW( sinistra, tr_label( barra, "sinistra" ) );
  sinistra->ripetizione( true );

  sinistra->setFixedSize( config.elenchi_button_size, config.elenchi_button_size );
  sinistra->setBackgroundOrigin( QLabel::ParentOrigin );
  sinistra->setBackgroundMode( Qt::NoBackground );
  sinistra->setPixmap( PIXMAP( "az_sx.png" ) );
  sinistra->setAlignment( int( QLabel::AlignCenter ) );
  connect( sinistra, SIGNAL( clicked() ), this, SLOT( sinistra_clicked() ) );
  sinistra->hide();

  NEW( space_above, tr_label( barra, "space_above" ) );
  space_above->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding ) );
  space_above->hide();

  NEW( more, tr_label( barra, "more" ) );
  // more->ripetizione( true );
  more->setFixedSize( config.elenchi_button_size, config.elenchi_button_size );
  more->setBackgroundOrigin( QLabel::ParentOrigin );
  more->setBackgroundMode( Qt::NoBackground );
  more->setPixmap( PIXMAP( "az_piu.png" ) );
  more->setAlignment( int( QLabel::AlignCenter ) );
  connect( more, SIGNAL( clicked() ), this, SLOT( more_clicked() ) );
  more->hide();

  NEW( exit, tr_label( barra, "exit" ) );
  exit->setPixmap( PIXMAP( "az_exit.png" ) );
  CONNECT( exit );
  exit->hide();

  NEW( less, tr_label( barra, "less" ) );
  // less->ripetizione( true );
  less->setFixedSize( config.elenchi_button_size, config.elenchi_button_size );
  less->setBackgroundOrigin( QLabel::ParentOrigin );
  less->setBackgroundMode( Qt::NoBackground );
  less->setPixmap( PIXMAP( "az_meno.png" ) );
  less->setAlignment( int( QLabel::AlignCenter ) );
  connect( less, SIGNAL( clicked() ), this, SLOT( less_clicked() ) );
  less->hide();


  NEW( space_below, tr_label( barra, "space_below" ) );
  space_below->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding ) );
  space_below->hide();

  QWidget*w[]= { su, abc, space_above, more, destra, exit, sinistra, less, space_below, giu };
  TROWI( w )barra_layout->addWidget( w[i] );

  layout->addMultiCellWidget( titolo, 0, 0, 0, 1 );
  layout->addWidget( elenco, 1, 0 );
  layout->addWidget( barra, 1, 1 );

  altezza_riga( config.elenchi_altezza_riga );

  _tipo=indefinito;
  dati.dati.tid=tok_endoftokens;
  grp_len=-1;
  _is_grp_varianti="";
  _abc_clic_effect=true;

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_listview::~tr_listview() {

  clear();


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_listview::clear() {

  dati().clear();

  int w=elenco->columnWidth( 0 );

  QListViewItem*lit=elenco->firstChild();
  while ( lit ) {
    lit->setOpen( true );
    lit=lit->itemBelow();
  }

  lit=elenco->lastItem();
  while ( lit ) {
    rimuovi( lit );
    lit=elenco->lastItem();
  }

  elenco->setColumnWidth( 0, w );
  while ( elenco->columns() ) elenco->removeColumn( 0 );

  col_sort.clear();
  col_grp.clear();
  col_nome.clear();
  col_chiave.clear();
  col_max_size.clear();
  colonne.clear();

  indici.clear();

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_listview::chiave( QListViewItem*lit ) {
  tr_key*k=key( lit );
  if ( k ) return QString( k->key );
  else
    return QString( "" );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QListViewItem*tr_listview::chiave( QString chiave ) {
  tr_key*k=key( chiave );
  if ( k ) return k->it;
  else
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_listview::tr_key*tr_listview::key( QString chiave ) {
  QPtrDictIterator<tr_key>it( indici );
  for ( ; it.current(); ++it ) {
    tr_key*k=it.current();
    if ( qstrcmp( k->key, chiave.latin1() )==0 ) return k;
  }
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_listview::tr_key*tr_listview::aggiungi( QString chiave, int idx, QListViewItem*lit ) {

  tr_key*k=key( lit );
  if ( k!=NULL ) cout<<"Inserimento puntatore doppia " <<chiave<<endl;
  NEW( k, tr_key() );
  qstrncpy( k->key, chiave.latin1(), sizeof( k->key ) );
  k->idx=idx;
  k->it=lit;
  indici.insert( lit, k );
  return k;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_listview::rimuovi( QListViewItem*lit ) {

  if ( lit->parent()==( QListViewItem* ) elenco||!lit->parent() )
    elenco->takeItem( lit );
  else
    lit->parent()->takeItem( lit );

  tr_key*chiave=key( lit );

  bool r=indici.remove( lit );

  if ( chiave ) {
    DELETE( chiave );
  }
  DELETE( lit );

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_listview::escludi( QString key ) {
  if ( key.isNull() ) esclusioni.clear();
  else
    esclusioni<<key;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_listview::setText( QString _titolo ) {
  titolo->setText( _titolo );
  titolo->show();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void tr_listview::adatta( QString maschera ) {

  if ( maschera.isNull() ) maschera=maschera_di_visualizzazione;

  if ( dati.header.find( '!' )!=-1 ) {
    return;
  }

  QFontMetrics f( elenco->font() );
  QString st;
  int w;
  if ( sizePolicy().horData()==QSizePolicy::Fixed ) {
    elenco->setFixedSize( elenco->size() );
    elenco->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  }

  if ( maschera.isNull() ) elenco->setColumnWidth( 0, elenco->width()-2 );
  else {
    int ws=0;
    QStringList campi=QStringList::split( '|', maschera );
    int colonna_dinamica=-1;
    int dim_colonne[campi.count()];
    TROWSLI( campi ) {
      // elenco->setColumnWidth(i,0);
      if ( campi[i]=="*" ) {
        if ( colonna_dinamica==-1 ) colonna_dinamica=i;
        st.fill( 'H', col_max_size[i] );
        w=f.width( st );
      } // if (campi
      if ( campi[i]=="^" ) {
        colonna_dinamica=i;
        st.fill( 'H', col_max_size[i] );
        w=f.width( st );
      } // if (campi
      if ( campi[i]=="#" ) {
        st.fill( '0', col_max_size[i] );
        w=( int )( f.width( st )*1.50 );
      } // if (campi
      if ( w<24 ) w=24;
      dim_colonne[i]=w;
      ws+=w;
    } // TROWSLI

    TROWI( dim_colonne ) {
      elenco->setColumnWidth( i, dim_colonne[i] );
    }

    int ww=elenco->width();

    if ( colonna_dinamica!=-1 ) {

      if ( ws>ww ) { // se i contenuti superrano la dimensione del contenitore...
        if ( sizePolicy().horData()!=QSizePolicy::Fixed ) { // ... e non è fisso
          int old_ww=ww;
          ww=ws; // nuova dimensione
          ww+=abc->width();
          if ( ww>config.finestra_larghezza*0.95 ) { // riporta a limite fisso
            old_ww=ww-abc->width();
            ww=( int )( config.finestra_larghezza*0.95 );
          }
          else
            ww=config.finestra_larghezza*0.95-abc->width()-4;
          setFixedWidth( ww );
          ww-=abc->width();
          elenco->setFixedWidth( ww );

          double rapporto=double( ww+abc->width() )/double( old_ww );
          ws-=( ( int )( dim_colonne[colonna_dinamica]*rapporto ) );
        }
        else
          ws-=( dim_colonne[colonna_dinamica] );
      }
      else
        ws-=( dim_colonne[colonna_dinamica] );

      if ( ws<0 ) ws=0;
      if ( ww-ws-4>0 ) dim_colonne[colonna_dinamica]=ww-ws-4;
      else {
        double wsd=0;
        TROWI( dim_colonne )wsd+=dim_colonne[i];
        double wwd=ww;
        double rap=( wwd/wsd );
        TROWI( dim_colonne ) {
          double d=dim_colonne[i]*rap;
          dim_colonne[i]=( int )d;;
        }
      } // else
    } // if (colonna_din...

    TROWI( dim_colonne ) {
      elenco->setColumnWidth( i, dim_colonne[i] );
    }

  }


  if ( sizePolicy().horData()==QSizePolicy::Fixed )
    elenco->setMaximumSize( elenco->size() );

  elenco->triggerUpdate();


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_listview::selezionato() {
  QListViewItem*lit=elenco->selectedItem();
  return chiave( lit );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_listview::deseleziona( QListViewItem*lit ) {
  tr_key*k=key( lit );
  k->stato=QCheckListItem::Off;
  elenco->setSelected( k->it, false );

  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_listview::deseleziona( QString chiave ) {
  tr_key*k=key( chiave );
  k->stato=QCheckListItem::Off;
  elenco->setSelected( k->it, false );
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_listview::seleziona( QListViewItem*lit ) {
  if ( !lit ) return false;
  elenco->setCurrentItem( lit );
  elenco->ensureItemVisible( lit );
  if ( lit->rtti()==1 ) ( ( QCheckListItem* ) lit )->setOn( true );
  else
    elenco->setSelected( lit, true );
  tr_key*k=key( lit );
  k->stato=QCheckListItem::On;
  return true;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_listview::seleziona( QString chiave ) {
  if ( chiave.isEmpty() ) return seleziona( e_none );

  tr_key*k=NULL;
  if ( chiave==e_none ) {
    clearSelection();
    return false;
  }
  if ( chiave==e_next ) {
    QListViewItem*lit=elenco->selectedItem();
    if ( lit ) lit=lit->itemBelow();
    if ( !lit ) k=key( elenco->firstChild() );
    else
      k=key( lit );
  }
  if ( chiave==e_first ) k=key( elenco->firstChild() );
  if ( chiave==e_last ) k=key( elenco->lastItem() );
  if ( !k ) k=key( chiave );
  if ( !k ) return false;

  QListViewItem*lit;
  lit=k->it;
  seleziona( lit );
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_listview::aggiorna_stati() {

  QPtrDictIterator<tr_key>it( indici );
  for ( ; it.current(); ++it ) {
    tr_key*k=it.current();
    if ( k->it ) {
      if ( k->it->rtti()==1 ) k->stato=( ( QCheckListItem* ) k->it )->state();
      else if ( k->it->isSelected() ) k->stato=QCheckListItem::On;
      else
        k->stato=QCheckListItem::Off;
    } // if ( k->it )
  } // for

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_listview::seleziona( QStringList chiavi ) {
  if ( elenco->selectionMode()!=QListView::Multi ) elenco->setSelectionMode( QListView::Multi );
  clearSelection();

  TROWSLI( chiavi ) {
    QListViewItem*lit;
    tr_key*k=key( chiavi[i] );
    if ( !k ) return false;
    lit=k->it;
    seleziona( lit );
  } // TROWSLI
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_listview::selezionati( bool tutti ) {
  elenco->setSelectionMode( QListView::Multi );
  elenco->selectAll( tutti );
  aggiorna_stati();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QStringList tr_listview::selezionati() {

  QStringList ma;
  QListViewItemIterator it( elenco );

  while ( it.current() ) {

    bool ok=false;

    if ( typeid( *it.current() )==typeid( QCheckListItem ) ) {
      if ( !it.current()->childCount() ) {
        QCheckListItem*cit=( QCheckListItem* ) it.current();
        if ( cit->isOn() ) ok=true;
      }
    }

    else if ( it.current()->isSelected() ) ok=true;

    if ( ok ) {
      tr_key*i=key( it.current() );
      if ( i ) ma<<i->key;
    }

    ++it;
  }

  return ma;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_listview::id() {
  return selezionato();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_listview::record( int prel ) {
  QString rec="";
  if ( !prel ) rec=record( selezionato() ); // fino a qui e' giusto
  if ( dati().count() ) {
    if ( prel<0 ) {
      tr_key*k=key( selezionato() );
      if ( k&&k->idx>0 ) rec=dati() [k->idx-1];
    }
    if ( prel>0 ) {
      tr_key*k=key( selezionato() );
      if ( k&&k->idx<dati().count()-1 ) rec=dati() [k->idx+1];
    }
  }
  return rec;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_listview::descrizione() {
  if ( selezionato().isEmpty() ) return "";
  QString rec=record();
  QString key( "" );
  for ( int j=col_nome.count()-1; j>=0; j-- ) {
    int cn=col_nome[j];
    key=rec.section( '|', cn, cn );
    if ( !key.isNull() ) break;
  }
  return key;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_listview::record( QString chiave ) {
  tr_key*k=key( chiave );
  if ( k ) {
    QString st=dati() [k->idx];
    return st;
  }
  return "";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_listview::tr_dati::carica() {
  lv->clear();

  dati.elenco.clear();
  dati.carica( dati.tid, params );
  if ( maschera.isEmpty() ) maschera=dati.maschera;
  QString id_selezione=lv->selezionato();
  lv->carica( dati.elenco, dati.struttura, check_box, icone, maschera );
  if ( !lv->seleziona( id_selezione ) ) {
    lv->elenco->setCurrentItem( NULL );
    lv->clicked( NULL );
  }

  if ( !lv->dati.header.isEmpty() ) {
    QStringList headers=QStringList::split( '|', lv->dati.header, true );
    QHeader*h=lv->elenco->header();
    if ( headers.count()>0 /* ==h->count() */ ) {
      lv->scopri( tr_listview::e_header );
      TROWSLI( headers ) {
        h->setLabel( i, TR( headers[i].section( '!', 0, 0 ) ) );
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_listview::aggiorna() {

  if ( dati.dati.tid!=tok_endoftokens )
    tipo_elenco( dati.dati.tid, dati.params );
  else
    tipo_fittizio( dati.dati.struttura );

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void tr_listview::cancella( QString record ) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_listview::aggiungi( QString record ) {
  dati.dati.elenco<<record;
}

void tr_listview::aggiungi( QStringList records ) {
  if ( records.count() ) dati.dati.elenco+=records;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_listview::tipo_fittizio( QString struttura, QStringList records ) {
  if ( records.count() ) dati.dati.elenco=records;
  dati.check_box=false;
  dati.espandi=false;
  dati.icone=false;
  dati.maschera=QString::null;
  dati.lv=this;
  clear();

  if ( dati.maschera.isEmpty() ) {
    QStringList datas=QStringList::split( '|', struttura, true );
    bool sep=false;
    TROWSLI( datas ) {
      QString dato=datas[i];
      QChar ch=dato[0];
      if ( ch!='-' &&ch!='@' ) {
        if ( sep ) dati.maschera+='|';
        switch ( ch ) {
          case '0':
          case '#':
            dati.maschera+='#';
          break;
          case '^':
            dati.maschera+='^';
          break;
          default:
            dati.maschera+='*';
          break;
        } // switch
        sep=true;
      }
    }
  }

  if ( !dati.lv->dati.header.isEmpty() ) {
    QStringList headers=QStringList::split( '|', dati.lv->dati.header, true );
    QHeader*h=dati.lv->elenco->header();
    if ( headers.count()>0 /* ==h->count() */ ) {
      scopri( tr_listview::e_header );
      TROWSLI( headers ) {
        h->setLabel( i, headers[i].section( '!', 0, 0 ) );
      }
    }
  }

  QString id_selezione=dati.lv->selezionato();
  carica( dati(), struttura, dati.check_box, dati.icone, dati.maschera );
  if ( !id_selezione.isNull()&&!dati.lv->seleziona( id_selezione ) )
    emit clicked( elenco->selectedItem() );
  else {
    dati.lv->elenco->setCurrentItem( NULL );
    dati.lv->clicked( NULL );
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_listview::tipo_elenco( TRTokens token, QString params ) {

  dati.check_box=false;
  dati.espandi=false;
  dati.icone=false;
  dati.maschera=QString::null;
  dati.dati.tid=token;
  dati.params=params;
  dati.lv=this;
  dati.carica();

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_listview::tipo( tr_tipo tipo ) {

  switch ( tipo ) {
    case stampanti:
    case tipo_tavoli:
    case tipo_oggetti:
    case tipo_pavimenti:
      nascondi( e_abc );
    break;
    case selezione_pagamenti:
      elenco->setSelectionMode( QListView::NoSelection );
  } // switch

  _tipo=tipo;
  if ( elenco->childCount()>0 ) clear();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_listview::carica( QString id_selezione, QString id_gruppo ) // id da selezionare
{
  if ( _tipo!=indefinito ) {

    clear();

    bool check_box=false;
    bool espandi=false;
    bool icone=false;
    QString maschera=QString::null;

    switch ( _tipo ) {

      case gruppi:
        espandi=true;
        dati.gruppi.carica( id_gruppo.toInt(), true );
        dati.dati.struttura=dati.gruppi.struttura;
        dati.dati.elenco=dati.gruppi.elenco;
      break;

      case articoli:
        dati.gruppi.carica( id_gruppo.toInt(), espandi, true );
        dati.dati.struttura=dati.gruppi.struttura;
        dati.dati.elenco=dati.gruppi.elenco;
        maschera="*|#";
      break;

      case selezione_pagamenti:
        check_box=true;

      case pagamenti:
        // icone=true;
        dati.gruppi.carica( config.id_grp_pagamenti, true );
        if ( dati.gruppi.elenco.count()>14||config.pagamenti_con_barra ) {
          scopri( tr_listview::e_barra );
          QFont f( elenco->font() );
          f.setPointSize( 18 );
          f.setBold( true );
          elenco->setFont( f );
        }
        dati.dati.struttura=dati.gruppi.struttura;
        dati.dati.elenco=dati.gruppi.elenco;
      break;

      case sale:
        dati.dati.carica( tok_elenco_sale_esteso, "" );
      break;

      case tavoli:
        dati.tavoli.carica( tr::tavoli::ordine_sala_tavolo_inverso );
        dati.dati.struttura=dati.tavoli.struttura;
        dati.dati.elenco=dati.tavoli.elenco;
      break;

      case tavoli_semplici:
        dati.tavoli.carica( tr::tavoli::ordine_sala_tavolo_inverso );
        dati.dati.struttura=dati.tavoli.struttura;
        dati.dati.elenco=dati.tavoli.elenco;
        escludi( "|||tavolo_bar.png" );
        escludi( "|||tavolo_as.png" );
      break;

      case bis:
        dati.dati.elenco.clear();
        dati.dati.struttura="@nome";
        dati.dati.elenco<<"9" <<"8" <<"7" <<"6" <<"5" <<"4" <<"3" <<"2" <<"1";
      break;

      case listini:
      case listini_modifica:
        dati.dati.carica( tok_elenco_listini );
      break;

      case camerieri:
        dati.dati.carica( tok_elenco_camerieri );
      break;

      case turni:
        dati.dati.carica( tok_elenco_turni );
      break;

      case stampanti:
        dati.dati.carica( tok_elenco_stampanti );
      break;

      case selezione_clienti:
      case clienti:
      case clienti_rapidi:
        dati.dati.carica( tok_elenco_clienti );
      break;

      case clienti_esteso:
        dati.dati.carica( tok_elenco_clienti_esteso, "" );
      break;

      case messaggi:
        espandi=true;
        dati.dati.carica( tok_elenco_messaggi, id_gruppo );
        maschera="#|*|*";
      break;

      case prenotazioni:
        espandi=true;
        dati.dati.carica( tok_elenco_prenotazioni, id_gruppo );
        maschera="*|*|#";
      break;

      case conti_ieri:
        dati.dati.carica( tok_elenco_conti, "0" );
        maschera=dati.dati.maschera; //"*|#|";
      break;

      case conti_ultimi:
        dati.dati.carica( tok_elenco_conti, "1" );
        maschera=dati.dati.maschera; //"*|#|";
      break;

      case tipo_tavoli:
        icone=true;
        dati.dati.elenco.clear();
        dati.dati.struttura="@pixmap|$nome|-flags";
        dati.dati.elenco=glb.pixmaps;
        escludi( "||&" +QSN( T_TAVOLO_PIANTA ) );
        escludi( "||&" +QSN( T_TAVOLO_PAVIMENTO ) );
      break;

      case tipo_oggetti:
        icone=true;
        dati.dati.elenco.clear();
        dati.dati.struttura="@pixmap|$nome|-flags";
        dati.dati.elenco=glb.pixmaps.grep( "|2" );
      break;

      case tipo_pavimenti:
        icone=true;
        dati.dati.elenco.clear();
        dati.dati.struttura="@pixmap|$nome|-flags";
        dati.dati.elenco=glb.pixmaps;
        escludi( "||0" ); // tavolo
        escludi( "||&83" ); // bis+bar+piante
        escludi( "||&" +QSN( T_TAVOLO_OROLOGIO ) );
      break;

      case log_com: {
          dati.dati.elenco.clear();
          dati.dati.struttura="log";
          TROWSLI( srv->log_buffer ) dati.dati.elenco.prepend( srv->log_buffer[i] );
        }
      break;

      default:
        return false;
    }

    carica( dati.dati.elenco, dati.dati.struttura, check_box, icone, maschera );
  }

  else // se tipo==indefinito

    dati.carica();

  if ( !seleziona( id_selezione ) ) elenco->setCurrentItem( NULL );


  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_listview::carica_struttura( QString struttura, QString riga0 ) {

  bool ordina=false;

  if ( !struttura.isEmpty() ) {

    QString codici( "@-$^!0#" );
    QStringList e=QStringList::split( "|", struttura, true );

    TROWSLI( e ) {

      int sort_column=-1;

      QChar ch=e[i].at( 0 );

      switch ( ch.latin1() ) {
        case '-':
        break;
        case '@':
          col_chiave.append( i );
        break;
        case '+':
          col_chiave.append( i );
          chiave_numerica=true;
        break;
        case '$':
          col_nome.append( i );
          if ( col_nome.count()==1 ) {
            colonne.prepend( i );
            sort_column=0;
          }
        break;
        case '!':
          col_grp.append( i );
        break;
        case '.':
          col_grps.append( i );
        break;
        case '#':
        case '0':
          col_num.append( i );
          colonne.append( i );
        break;
        default:
          if ( e[i] [0]!='-' ) colonne.append( i );
        break;

      } // switch su primo carattere

      ch=e[i].at( e[i].length()-1 );
      switch ( ch ) {
        case '<':
          if ( sort_column==-1 ) sort_column=colonne.count()-1;
          elenco->setSortOrder( Qt::Ascending );
          elenco->setSortColumn( sort_column );
          ordina=true;

        break;

        case '>':
          if ( sort_column==-1 ) sort_column=colonne.count()-1;
          elenco->setSortOrder( Qt::Descending );
          elenco->setSortColumn( sort_column );
          ordina=true;
        break;

      }

    } // TROWSLI

  }

  else {
    QStringList e=QStringList::split( "|", riga0, true );
    TROWSLI( e )col_chiave.append( i );
  }

  return ordina;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_listview::carica( QStringList dati, QString struttura, bool chk_box, bool icone, QString maschera ) {

  if ( !this->dati().count() ) {
    this->dati.dati.elenco=dati;
    this->dati.dati.struttura=struttura;
  }

  bool ordina=false;

  chiave_numerica=false;

  if ( !tr_listview::dati.header.isEmpty()&&tr_listview::dati.header.find( '!' )==-1 ) {
    QStringList e=QStringList::split( '|', tr_listview::dati.header, true );
    while ( col_max_size.count()<e.count() ) col_max_size.append( 1 );
    for ( int n=0; n<col_max_size.count(); n++ ) col_max_size[n]=e[n].length()+1;
  }


  ordina=carica_struttura( struttura, dati[0] );

  if ( !col_nome.count()&&col_chiave.count() )
    col_nome=col_chiave;
  if ( !col_nome.count()&&col_grp.count() )
    col_nome=col_grp;
  bool colonna_unica=false;
  if ( !col_chiave.count() ) {
    if ( !col_nome.count() ) colonna_unica=true;
    for ( int i=0; i<=struttura.contains( '|' ); i++ )
      col_chiave.append( i );
  }
  if ( !colonne.count()&&col_chiave.count() ) colonne=col_chiave;

  if ( col_chiave.count()>1&&chiave_numerica ) {
    chiave_numerica=false;
  }

  if ( !tr_listview::dati.header.isEmpty()&&tr_listview::dati.header.find( '!' )!=-1 ) {
    QString h=tr_listview::dati.header;
    if ( h.contains( '!' )==h.contains( '|' )+1 ) {
      QStringList hs=QStringList::split( '|', h, true );
      int wide=0;
      TROWSLI( hs ) {

        elenco->addColumn( hs[i].section( '!', 0, 0 ), hs[i].section( '!', 1, 1 ).toInt() );
        wide+=hs[i].section( '!', 1, 1 ).toInt();

        if ( col_num.findIndex( colonne[i] )!=-1 ) elenco->setColumnAlignment( i, Qt::AlignRight );

      } // TROWSLI
    }
  } // if lv

  for ( uint i=elenco->columns(); i<colonne.count(); i++ ) {

    elenco->addColumn( QSN( i ), 0 );

    if ( col_num.findIndex( colonne[i] )!=-1 ) elenco->setColumnAlignment( i, Qt::AlignRight );

  } // for

  while ( col_max_size.count()<colonne.count() ) {
    col_max_size.append( 1 );
    col_max_size[col_max_size.count()-1]=0;
  }

  if ( dati.count()<1 ) {
    return false;
  }

  tr_listviewitem*lit=NULL;


  _is_grp_varianti="";

  TROWSLI( dati ) {

    QStringList e=QStringList::split( '|', dati[i], true );

    if ( _tipo==gruppi||_tipo==articoli ) {
      tr::tipo_articolo stato( e[tr::gruppi::gruppo::p_tipo_articolo].toInt() );
      if ( stato.gasv() ) {
        _is_grp_varianti=e[tr::gruppi::gruppo::p_id_istanza];
        continue;
      }
    }

    QString key;
    if ( !colonna_unica ) key=estrapola( e, col_chiave, e_chiave );
    else
      key=dati[i];

    bool escludi=false;
    TROWSLJ( esclusioni ) {
      QString st;
      if ( esclusioni[j].contains( '|' )>0 ) {
        QStringList ee=QStringList::split( '|', esclusioni[j], true );
        TROWSLK( ee ) {
          st=ee[k];
          if ( !st.isEmpty() ) {
            enum {
              minore, maggiore, and_logico
            }
            comp;
            bool uguale=true;
            bool regexp=false;
            bool exp=false;
            QRegExp re;
            if ( st[0]==QChar( '<' ) ) {
              st=st.mid( 1 );
              exp=true;
              comp=minore;
            }
            if ( st[0]==QChar( '>' ) ) {
              st=st.mid( 1 );
              exp=true;
              comp=maggiore;
            }
            if ( st[0]==QChar( '&' ) ) {
              st=st.mid( 1 );
              exp=true;
              comp=and_logico;
            }

            if ( st[0]==QChar( '!' ) ) {
              st=st.mid( 1 );
              uguale=false;
            }
            if ( st[0]==QChar( '{' )&&st.right( 1 )=="}" ) {
              regexp=true;
              re.setPattern( st.mid( 1, st.length()-2 ) );
            }
            if ( !exp&&!regexp&&uguale&&st==e[k] )
              escludi=true;
            if ( !exp&&!regexp&&!uguale&&st!=e[k] )
              escludi=true;
            if ( !exp&&regexp&&uguale&&e[k].find( re )==-1 )
              escludi=true;
            if ( !exp&&regexp&&!uguale&&e[k].find( re )!=-1 )
              escludi=true;
            if ( exp&&comp==minore&&e[k].toInt()<st.toInt() )
              escludi=true;
            if ( exp&&comp==maggiore&&e[k].toInt()>st.toInt() )
              escludi=true;
            if ( exp&&comp==and_logico&&( e[k].toInt()&st.toInt() ) )
              escludi=true;
            if ( escludi ) break;
          } // if (!st.isEmpty())
        } // TROWSLK
      } // if escludi.contains(|)
      else {
        bool uguale=true;
        if ( esclusioni[j] [0]==QChar( '!' ) ) {
          uguale=false;
          st=esclusioni[j].mid( 1 );
        }
        else
          st=esclusioni[j];
        if ( uguale&&st==key ) escludi=true;
        if ( !uguale&&st!=key ) escludi=true;
      }
      if ( escludi ) break;
    }
    if ( escludi ) continue;

    QString descrizione;
    if ( !colonna_unica ) {
      descrizione=estrapola( e, col_nome, e_descrizione );
      descrizione=descrizione.stripWhiteSpace()+" ";
    }
    else {
      descrizione=dati[i];
      descrizione=descrizione.stripWhiteSpace();
    }

    if ( col_grps.count()>0 ) {

      QString grp_key;
      QString last_key;

      grp_key=e[col_grps[0]];
      if ( lit ) {
        last_key=lit->parent()->text( 0 );
        if ( grp_key!=last_key ) lit=NULL;
        else {
          NEW( lit, tr_listviewitem( ( tr_listviewitem* ) lit->parent(), descrizione ) );
        }
      }
      if ( !lit ) {
        NEW( lit, tr_listviewitem( elenco, grp_key ) );
        NEW( lit, tr_listviewitem( lit, descrizione ) );
      }

    }


    if ( col_grp.count()>0 ) {

      if ( lit ) {

        QString grp_key;
        QString last_key;

        grp_key=estrapola( e, col_grp, e_chiave );
        last_key=chiave( lit );

        if ( grp_key!=last_key ) {
          lit=( tr_listviewitem* ) lit->parent();
          while ( lit ) {
            last_key=chiave( lit );
            if ( grp_key==last_key ) break;
            lit=( tr_listviewitem* ) lit->parent();
          } // while (lit)
        } // if (grp_key!=last_key)

        if ( lit ) {
          if ( chk_box ) {
            QCheckListItem*cit;
            NEW( cit, QCheckListItem( lit, descrizione, QCheckListItem::CheckBoxController ) );
            lit=( tr_listviewitem* ) cit;
          }
          else {
            NEW( lit, tr_listviewitem( lit, descrizione ) );
          }
        } // if (lit)
      } // if (lit)
      else {
        if ( chk_box ) {
          QCheckListItem*cit;
          NEW( cit, QCheckListItem( elenco, descrizione, QCheckListItem::CheckBoxController ) );
          lit=( tr_listviewitem* ) cit;
        }
      }

      if ( !lit ) {
        if ( chk_box ) {
          QCheckListItem*cit;
          NEW( cit, QCheckListItem( elenco, descrizione, QCheckListItem::CheckBoxController ) );
          lit=( tr_listviewitem* ) cit;
        }
        else {
          NEW( lit, tr_listviewitem( elenco, descrizione ) );
        }
      }

    } // if (col_grp.count()>0)

    if ( !col_grp.count()&&!col_grps.count() ) {
      NEW( lit, tr_listviewitem( elenco, descrizione ) );
    }

    if ( icone&&lit ) {
      QPixmap p=PIXMAP( e[col_chiave[0]] );
      if ( p.width()>32||p.height()>32 ) {
        QImage i=p.convertToImage();
        i=i.scale( 32, 32, QImage::ScaleMin );
        p.convertFromImage( i );
      }
      lit->setPixmap( 0, p );
    }

    if ( col_max_size.count()<=0 ) col_max_size.append( 1 );
    if ( col_max_size[0]<descrizione.length() )
      col_max_size[0]=descrizione.length();

    tr_key*pkey=aggiungi( key, i, lit );

    if ( highlights.count()>0 ) {
      TROWSLI( highlights ) {
        QStringList tokens=QStringList::split( ' ', highlights[i], true );
        if ( tokens.count()==3 ) {
          QString val=tokens[2].section( '?', 0, 0 );
          int col=tokens[0].toInt();
          bool t=false;
          if ( tokens[1]=="eq" &&e[col]==val ) t=true;
          if ( tokens[1]=="and" &&( e[col].toInt()&val.toInt() ) ) t=true;
          if ( t ) {
            if ( tokens[2].right( 4 )==".png" )
              pkey->estensione=tokens[0]+"|" +tokens[2].section( '?', 1, 1 );
            else
              pkey->estensione=tokens[2].section( '?', 1, 1 );
          } // if (t)
        } // tokens.ok
      } // trowsli
    } // if highlights


    if ( !colonna_unica ) {
      for ( uint j=1; j<colonne.count(); j++ ) {
        QString st=e[colonne[j]].stripWhiteSpace();
        if ( j!=colonne.count()-1 ) st+=" ";
        if ( col_num.findIndex( colonne[j] )!=-1&&( st=="0" ||st=="0.00" ) ) st="";
        lit->setText( j, st );
        if ( col_max_size.count()<=j ) col_max_size.append( 1 );
        if ( col_max_size[j]<st.length() )
          col_max_size[j]=st.length();
      } // for
    } // if

  } // trowsli(dati)

  maschera_di_visualizzazione=maschera;

  if ( ordina ) elenco->sort();

  if ( elenco->isVisible() )
    adatta();


  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

tr::gruppi::gruppo tr_listview::articolo( QString chiave ) {
  if ( chiave==QString::null ) chiave=selezionato();
  tr::gruppi::gruppo r;
  r.parse( record( chiave ) );
  return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

tr::mse::riga tr_listview::dato( QString chiave ) {
  if ( !dati().count() ) return tr::mse::riga( "" );
  if ( chiave==QString::null ) chiave=selezionato();
  return tr::mse::riga( record( chiave ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

tr::gruppi::gruppo tr_listview::pagamento( QString chiave ) {
  if ( chiave.isNull() ) chiave=selezionato();
  tr::gruppi::gruppo r;
  if ( !chiave.isNull() ) r.parse( record( chiave ) );
  return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7
tr::ordine::riga tr_listview::portata_comanda( QString chiave ) {
  if ( chiave==QString::null ) chiave=selezionato();
  tr::ordine::riga r;
  r.parse( record( chiave ) );
  return r;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////7
tr::tavoli::tavolo tr_listview::tavolo( QString chiave ) {
  if ( chiave==QString::null ) chiave=selezionato();
  tr::tavoli::tavolo r;
  r.parse( record( chiave ) );
  return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listview::clearSelection() {
  elenco->clearSelection();

  if ( elenco->firstChild()&&elenco->firstChild()->rtti()==1 ) {
    QListViewItemIterator it( elenco );
    while ( it.current() ) {
      if ( it.current()->rtti()==1 )
        ( ( QCheckListItem* ) it.current() )->setOn( false );
      ++it;
    } // while
  } // if

  aggiorna_stati();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listview::header_clicked( int section ) {
  QHeader*h=( QHeader* ) sender();
  Qt::SortOrder dir=Qt::Ascending;
  if ( h->sortIndicatorSection()==section )
    dir=h->sortIndicatorOrder()==Qt::Descending?Qt::Ascending:Qt::Descending;
  elenco->setSorting( section, dir );
  elenco->sort();
  h->setSortIndicator( section, dir );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listview::elenco_mouseButtonClicked( int button, QListViewItem*it, const QPoint&pos, int c ) {
  if ( !it ) it=elenco->itemAt( pos );
  if ( !it ) emit clicked( it );
  emit mouseButtonClicked( button, it, pos, c );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listview::elenco_selectionChanged( QListViewItem*it ) {
  // emit clicked( it );
  emit selectionChanged( it );
}

void tr_listview::elenco_clicked( QListViewItem*it, const QPoint&, int ) {

  if ( it ) {

    // se è un gruppo
    int n=it->childCount();
    if ( n>0 ) {

      if ( !elenco->isOpen( it ) ) { // non aperto,
        it->setOpen( true ); // lo apre
        if ( rollone ) {
          if ( last_opened&&last_opened!=it->parent() ) {
            last_opened->setOpen( false );
          }
          if ( ( void* ) it->parent()==0 ) last_opened=it;
        }
        elenco->setCurrentItem( NULL ); //
        QListViewItem*itc=it->firstChild();
        qApp->processEvents();
        int n=0;
        while ( itc->itemBelow() ) {
          itc=itc->itemBelow();
          n++;
        }
        int h=elenco->viewport()->height();
        if ( h>=n*config.elenchi_altezza_riga )
          elenco->ensureItemVisible( itc );
        else {
          n=it->childCount();
          if ( h>=n*config.elenchi_altezza_riga ) {
            itc=it;
            while ( itc&&n ) itc=itc->itemBelow();
            elenco->ensureItemVisible( itc );
          }
        }
        elenco->ensureItemVisible( it );
      }
      else { // se aperto
        it->setOpen( false ); // lo chiude
        elenco->setCurrentItem( NULL ); //
      }

      if ( elenco->selectionMode()==QListView::Single ) {
        if ( typeid( *it )==typeid( QCheckListItem ) ) {
          QCheckListItem*cit=( QCheckListItem* ) it;
          if ( cit->isOn() ) clearSelection();
        }
      }

      aggiorna_stati();

    }

    else // se non è un gruppo

    {

      tr_key*k=key( it );

      if ( typeid( *it )==typeid( QCheckListItem ) ) {

        QCheckListItem*cit=( QCheckListItem* ) it;
        it->setSelected( false );
        if ( elenco->selectionMode()==QListView::Single ) clearSelection();
        cit->setOn( !cit->isOn() );
        if ( k->stato!=cit->state() ) {
          // cit->setOn( !cit->isOn() );
          k->stato=cit->state();
          if ( it->childCount()>0 ) aggiorna_stati();
        }

      } // if id=chk_item

      else {

        if ( !it->childCount() ) { // se non e' un gruppo
          if ( !elenco->selectionMode()==QListView::Multi&&!_onoff ) {
            if ( !it->isSelected() ) it->setSelected( true );
          }
          else {
            if ( k->stato==QCheckListItem::On&&unselectable() ) {
              elenco->setSelected( it, false );
              // it->setSelected( false );
              k->stato=QCheckListItem::Off;
              update();
            }
            else {
              elenco->setSelected( it, true );
              // it->setSelected( true );
              k->stato=QCheckListItem::On;
            }
          }

        } // if (!it->childCount() )

      }
    }

    emit clicked( it );
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listview::abc_clicked() {
  tr_label*l=( tr_label* ) sender();
  QString name=l->name();

  const QString abcs[8]= { "ABC", "DEF", "GHI", "JKL", "MNO", "PQRS", "TUV", "WXYZ" };
  static uint cerca=0;
  static uint pos=0;

  uint i=name.mid( 4, 1 ).toUInt()-1;
  if ( cerca!=i ) {
    cerca=i;
    pos=0;
  }
  else {
    pos++;
    if ( pos>=abcs[cerca].length() ) pos=0;
  }

  QListViewItem*it=NULL;
  while ( !it&&pos<abcs[cerca].length() ) {
    QString p=abcs[cerca] [pos];
    it=elenco->findItem( p, 0, Qt::BeginsWith );
    if ( it ) {
      seleziona( it );
      elenco->ensureItemVisible( it );
      if ( _abc_clic_effect ) {
        emit clicked( it );
      }
      break;
    }
    pos++;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listview::more_clicked() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listview::less_clicked() {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listview::destra_clicked() {
  elenco->horizontalScrollBar()->setValue( elenco->horizontalScrollBar()->value()+config.elenchi_altezza_riga );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listview::sinistra_clicked() {
  elenco->horizontalScrollBar()->setValue( elenco->horizontalScrollBar()->value()-config.elenchi_altezza_riga );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listview::su_clicked() {
  elenco->verticalScrollBar()->setValue( elenco->verticalScrollBar()->value()-config.elenchi_altezza_riga );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listview::giu_clicked() {
  elenco->verticalScrollBar()->setValue( elenco->verticalScrollBar()->value()+config.elenchi_altezza_riga );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listview::scopri( tr_elemento e ) {
  switch ( e ) {
    case e_abc:
      space_above->hide();
      more->hide();
      exit->hide();
      less->hide();
      space_below->hide();
      abc->show();
    break;
    case e_moreless:
      abc->hide();
      space_above->show();
      more->show();
      exit->hide();
      less->show();
      space_below->show();
    break;
    case e_dxsx:
      abc->hide();
      space_above->show();
      destra->show();
      exit->hide();
      sinistra->show();
      space_below->show();
    break;
    case e_exit:
      space_above->show();
      more->hide();
      exit->show();
      less->hide();
      space_below->show();
    break;
    case e_barra:
      barra->show();
    break;
    case e_header:
      elenco->header()->show();
    break;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listview::nascondi( tr_elemento e ) {
  switch ( e ) {

    case e_abc:
      exit->hide();
      space_below->hide();
      abc->hide();
      space_above->show();
    break;

    case e_barra:
      barra->hide();
    break;

    case e_exit:
      space_above->show();
      exit->hide();
      space_below->hide();
    break;

    case e_header:
      elenco->header()->hide();
    break;

  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

bool tr_listview::event( QEvent*e ) {
  if ( e->type()==EV_UPDATE ) {
    aggiorna();
    return true;
  }
  return QWidget::event( e );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7
//             TR_LISTVIEWITEM
/////////////////////////////////////////////////////////////////////////////////////////////////////7

tr_listviewitem::tr_listviewitem( QListView*parent, QString col0 ):QListViewItem( parent, col0 ) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

tr_listviewitem::tr_listviewitem( tr_listviewitem*parent, QString col0 ):QListViewItem( parent, col0 ) {
}

/////////////////////////////////////////////////////////////////////////////////////////////////////7

void tr_listviewitem::paintCell( QPainter*p, const QColorGroup&cg, int column, int width, int align ) {
  QListView*lv=listView();
  tr_listview*trlv=( tr_listview* ) lv->parent();
  tr_listview::tr_key*k=trlv->key( ( QListViewItem* ) this );
  QColorGroup new_cg( cg );
  if ( !k->estensione.isNull() ) {

    QFont f( lv->font() );
    QFont f_original=f;
    p->setFont( f );

    if ( k->estensione.right( 4 )==".png" ) {
      int col=k->estensione.section( '|', 0, 0 ).toInt();
      if ( col==column )
        this->setPixmap( 0, PIXMAP( k->estensione.section( '|', 1, 1 ) ) );
    }
    if ( k->estensione[0]=='#' ) {
      QString color;
      if ( k->estensione[1]=='#' ) {
        QFont f( lv->font() );
        f.setBold( true );
        p->setFont( f );
        if ( k->estensione.length()>2 ) color=k->estensione.mid( 1 );
      }
      else
        color=k->estensione;
      if ( !color.isNull() ) {
        QColor c( color );
        new_cg.setColor( QColorGroup::Text, c );
      }
    }
    QListViewItem::paintCell( p, new_cg, column, width, align );
    p->setFont( f_original );
  } // if estensione
  else {
    QFont f( lv->font() );
    p->setFont( f );
    QListViewItem::paintCell( p, cg, column, width, align );
  }
}

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      TR_INPUT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TR_INPUT_CPP
  #define TR_INPUT_CPP

tr_input::tr_input( QWidget*parent, const QString&name, QString etichetta, QString valore, QString validatore, bool alato ):
   tr_widget( parent, name ) {

     #define POL QSizePolicy::Preferred
     // #define POL QSizePolicy::MinimumExpanding

     QVBoxLayout*layout_v;
     QHBoxLayout*layout_h;

     barcode=false;
     onkey=false;
     onupdate=false;
     into_keyboard=false;
     clear_on_link=false;

     QWidget*p=parent;
     setPaletteBackgroundColor( p->paletteBackgroundColor() );
     setSizePolicy( QSizePolicy( POL, QSizePolicy::Fixed ) );
     if ( alato ) setMaximumHeight( config.altezza_riga );
     else
       setMaximumHeight( config.altezza_riga*2 );
     layout_v=new QVBoxLayout( this, margine, spazio, "vl" );

     NEW( label, tr_label( this, "label" ) );
     if ( alato ) label->setMaximumHeight( config.altezza_riga );
     else {
       label->setMaximumHeight( config.altezza_riga*2 );
     }

     if ( !etichetta.isEmpty() ) {
       if ( alato&&etichetta.contains( ' ' )>0 ) {
         label->setAlignment( int( QLabel::WordBreak|QLabel::AlignLeft|QLabel::AlignVCenter ) );
       }
       else {
         label->setAlignment( int( QLabel::SingleLine|QLabel::AlignLeft|QLabel::AlignTop ) );
       }
       label->setText( etichetta );
       label->setPaletteForegroundColor( config.colore_selezione );
     #ifdef evidenzia
       label->setPaletteBackgroundColor( QColor( 0, 200, 0 ) );
     #endif
     }
     else
       label->hide();

     if ( !alato ) layout_v->addWidget( label );

     layout_h=new QHBoxLayout( NULL, margine, spazio, "hl" );
     layout_v->addLayout( layout_h );

     if ( alato ) layout_h->addWidget( label, Qt::AlignLeft );

     NEW( edit, tr_lineedit( this, "edit" ) );
     edit->setPaletteBackgroundColor( config.input_color_bg );
     edit->setLineWidth( 1 );
     edit->setSizePolicy( QSizePolicy( POL, QSizePolicy::Fixed ) );

     connect( edit, SIGNAL( textChanged( const QString& ) ), SLOT( edit_textChanged( const QString& ) ) );
     connect( edit, SIGNAL( returnPressed() ), SLOT( edit_textUpdated() ) );

     NEW( icona, tr_label( this, "icona" ) );
     // icona->setMinimumSize( 50, config.altezza_riga );
     // icona->setMaximumSize( 50, config.altezza_riga );
     icona->setFixedSize( config.elenchi_button_size, config.altezza_riga );
     // icona->setBackgroundMode( Qt::NoBackground );
     // icona->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
     CONNECT( icona );

     layout_h->addWidget( edit, Qt::AlignCenter );
     layout_h->addWidget( icona, Qt::AlignRight );

     icona->setPixmap( PIXMAP( "az_tastiera.png" ) );
     _tipo=tp_stringa;

     setValidatore( validatore );

     /*

     ??? disabilitato per problemi di visualizzazione ??? del cursore

     switch (_tipo) { case tp_data: case tp_ora: case tp_numero: edit->setAlignment( Qt::AlignRight ); break; default:
     edit->setAlignment( Qt::AlignLeft ); } */

     if ( !validatore.isEmpty() ) {
       if ( valore.isEmpty() ) valore=tools.segna_posto( validatore );
     }


     if ( valore.left( 2 )!="##" ) setText( valore );
     else {
       QFontMetrics fm( edit->font() );
       edit->setBaseSize( fm.width( valore ), config.altezza_riga );
     }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void tr_input::setValidatore( QString validatore ) {
  if ( !validatore.isNull() ) {
    QStringList validatori=QStringList::split( '|', "date|time|int|real|real2|real3|desc" );
    QString icone="calendario|tastiera|calc|calc|calc|calc|tastiera";
    tr_tipo tipi[]= {tp_data,tp_ora,tp_numero,tp_numero,tp_numero,tp_numero,tp_stringa};

    _validatore=validatore;
    bool trovato=false;
    TROWSLI( validatori )
    if ( validatori[i]==validatore.left( validatori[i].length() ) ) {
      validatore=tools.validatore( validatore );
      icona->setPixmap( PIXMAP( "az_" +icone.section( '|', i, i )+".png" ) );
      _tipo=tipi[i];
      trovato=true;
      break;
    }
    if ( !trovato ) {
      icona->setPixmap( PIXMAP( "az_tastiera.png" ) );
      _tipo=tp_stringa;
    }
  }
  else {
    icona->setPixmap( PIXMAP( "az_tastiera.png" ) );
    _tipo=tp_stringa;
  }

  if ( !validatore.isEmpty() ) {
    edit->setRegExp( validatore );
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_input::show() {
  QWidget::show();
  if ( !isHidden() ) adatta();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_input&tr_input::operator=( const tr_input&in ) {
  setText( in.edit->text() );
  _elenco=in._elenco;
  barcode=in.barcode;
  onkey=in.onkey;
  _tipo=in._tipo;
  tr_validator*v;
  edit->setValidator( in.edit->validator() );
  return*this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_input::edit_textUpdated() {
  if ( edit->keypressed||_originale!=text() ) {
    emit textUpdated();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_input::edit_textChanged( const QString&t ) {

  const int p_descrizione=2;
  const int p_id_articolo=0;

  QStringList cfg;
  bool barcode1=false; // barcode all'interno di elenco
  enum {
    from, start, end, len, regexp
  };

  if ( barcode ) cfg=QStringList::split( '|', config.barcode, true );

  if ( t.isEmpty() ) id( "" );

  if ( !_elenco.token.isEmpty()&&t.length()==_elenco.nkeys&&!_elenco.elenco.count() ) {
    bool salta=false;
    int n=t.length();
    if ( barcode&&cfg[start].length() ) {
      if ( cfg[start].length()<n ) n=cfg[start].length();
      if ( cfg[start].left( n )==t.left( n ) ) {
        salta=true;
        barcode1=true;
      }
    }
    if ( !salta ) {
      TRTokens _tok=( TRTokens )_elenco.token.section( '|', 0, 0 ).toInt();
      tr::mse elenco( _tok );
      elenco.carica( t );
      _elenco.elenco=elenco.elenco;
      if ( elenco.elenco.count() ) {
        icona->setPixmap( PIXMAP( "az_elenco.png" ) );
        _tipo=tp_elenco;
        if ( icona->isHidden() ) icona->show();
      }
      else {
        icona->setPixmap( PIXMAP( "az_tastiera.png" ) );
        _tipo=tp_stringa;
        if ( icona->isHidden()&&!into_keyboard ) icona->show();
        _elenco.id="";
      }
    }
  }
  else
    _elenco.id="";
  if ( t.length()<_elenco.nkeys ) {
    icona->setPixmap( PIXMAP( "az_tastiera.png" ) );
    _elenco.id="";
    _tipo=tp_stringa;
    if ( _elenco.elenco.count() ) _elenco.elenco.clear();
    if ( icona->isHidden()&&!into_keyboard ) icona->show();
  }

  if ( barcode||barcode1 ) {

    if ( cfg[start].isNull()&&cfg[end].isNull()&&cfg[len].isNull() ) return;

    int n=cfg[start].length();
    int m=cfg[end].length();
    int l=t.length();
    if ( l>n+m ) {
      l-=n;
      l-=m;
      if ( t.left( n )==cfg[start]&&t.right( m )==cfg[end] ) {
        if ( !barcode1 ) {
          setText( t.mid( n, l ) );
          emit barcodeReaded();
          if ( onkey ) emit textChanged( t );
        }
        else // se barcode1
        {
          if ( ::cmd( tok_egm, "|" +t.mid( n, l ) ) ) {
            QString st=srv->rcv();
            _elenco.id=st.section( '|', p_id_articolo, p_id_articolo );
            edit->disconnect();
            edit->setText( st.section( '|', p_descrizione, p_descrizione ) );
            edit->setSelection( 0, st.length() );
            connect( edit, SIGNAL( textChanged( const QString& ) ), SLOT( edit_textChanged( const QString& ) ) );
            srv->eop();
          }
          else
            srv->eop();
        } // else barcode1
      } // se riconosciuti start e end
    } // se lunghezza maggiore di start+end
  } // if barcode || barcode1
  else {
    if ( _elenco.elenco.count()&&edit->lastkey.category()!=QChar::Other_Control ) {
      TROWSLI( _elenco.elenco ) {
        QString data=_elenco.elenco[i].section( '|', p_descrizione, p_descrizione );
        if ( t.length()<=data.length()&&data.left( t.length() )==t ) {
          _elenco.id=_elenco.elenco[i].section( '|', p_id_articolo, p_id_articolo );
          edit->disconnect();
          edit->setText( data );
          edit->setSelection( t.length(), _elenco.elenco[i].length()-t.length() );
          connect( edit, SIGNAL( textChanged( const QString& ) ), SLOT( edit_textChanged( const QString& ) ) );
          break;
        }
        else
          _elenco.id="";
      }
    }

    frm_base*base=tools.find_base( this );
    if ( !isHidden() ) adatta();
    if ( base&&!base->isHidden() ) emit textChanged( t );
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void tr_input::elenco( const QStringList e, const QString struttura ) {
  if ( e.count()>0 ) {
    _elenco.elenco=e;
    _elenco.struttura=struttura;
    icona->setPixmap( PIXMAP( "az_elenco.png" ) );
    _tipo=tp_elenco;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_input::tok( TRTokens _tok, QString parametri, int nkeys ) {

  tr::mse elenco( _tok );
  if ( !nkeys ) {
    elenco.carica( parametri );
    _elenco.elenco=elenco.elenco;
  }
  if ( !elenco.struttura.isEmpty() ) _elenco.struttura=elenco.struttura;
  _elenco.token=QSN( _tok );
  _elenco.nkeys=nkeys;
  if ( !nkeys ) {
    icona->setPixmap( PIXMAP( "az_elenco.png" ) );
    _tipo=tp_elenco;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_input::icona_clicked() {
  switch ( _tipo ) {
    case tp_data:
      tools.calendario( edit );
    break;
    case tp_ora:
    case tp_stringa:
      tools.tastiera( 0, this );
    break;
    case tp_elenco:
      tools.elenco( this );
    break;
    case tp_numero:
      tools.calcolatrice( this, label->text(), _validatore );
    break;
  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QString tr_input::text() {
  if ( _tipo==tp_numero ) return config.num_interno( edit->text() );
  return edit->text();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//QDate tr_input::data() { return QDate(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//QTime tr_input::ora() { return QVariant( text() ).toTime(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QString tr_input::stringa() {
  return text();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QString tr_input::dataiso( bool yeszerotime ) {
  return tools.data_dataiso( text(), yeszerotime );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_input::setLabel( QString v ) {
  label->setMinimumSize( label->size() );
  label->setText( v );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_input::setText( QString v ) {
  if ( _tipo==tp_numero&&!v.isEmpty() ) v=v.replace( '.', config.segno_decimale );
  edit->setText( v );
  // if (v!=_originale) changed=true; else changed=false;
  _originale=text();
  emit textChanged( _originale );
  emit textUpdated();
  edit->keypressed=false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_input::setText( QDate v ) {
  setText( tools.data( v ) );
  _originale=text();
  // emit textChanged( text() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_input::setText( QTime v ) {
  setText( QVariant( v ).toString() );
  _originale=text();
  // emit textChanged( text() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_input::setText( int v ) {
  setText( QVariant( v ).toString() );
  _originale=text();
  // emit textChanged( text() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_input::setText( double v ) {
  setText( QVariant( v ).toString() );
  _originale=text();
  // emit textChanged( text() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_input::setReadOnly( bool v ) {
  edit->setReadOnly( v );
  if ( v ) icona->hide();
  else
    icona->show();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool tr_input::modificato() {
  return ( _originale!=text() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_input::setCheck( bool stato ) {
  if ( !edit->isReadOnly() ) {
    label->aspetto_check( stato );
    connect( label, SIGNAL( clicked() ), this, SLOT( checked() ) );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_input::checked() {
  if ( label->isOn() ) {
    edit->setEnabled( true );
    icona->setEnabled( true );
  }
  else {
    edit->setEnabled( false );
    icona->setEnabled( false );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_input::setEnabled( bool stato ) {
  tr_widget::setEnabled( stato );
  if ( label&&label->aspetto()==tr_label::check ) checked();
}

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 		      TR_CANVAS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TR_CANVAS_CPP
  #define TR_CANVAS_CPP

  #include <qimage.h>
  #include <qpicture.h>
  #include <qwmatrix.h>
  #include "tr_canvas.hpp"

  #define CDFX 3
  #define CDFY 2
  #define CDX 240
  #define CDY 64

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_canvas::tr_canvas( tr_widget*parent, const char*name ):tr_widget( parent, name, MYFLAGS ) {
  draw();

  last_pos=QPoint( 0, 0 );
  setPaletteBackgroundColor( Qt::white );

  pict.setOptimization( QPixmap::NoOptim );
  setFixedSize( CDX*CDFX, CDY*CDFY );
  pict.resize( CDX*CDFX, CDY*CDFY );

  pict.fill();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_canvas::paintEvent( QPaintEvent*pe ) {
  static bool first=true;

  QPainter paint( this );

  // paint.setClipRect(pe->rect());

  paint.drawPixmap( 0, 0, pict );

  paint.end();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_canvas::mousePressEvent( QMouseEvent*e ) {
  last_pos=e->pos();
  QPainter paint( &pict );
  int x=e->pos().x();
  int y=e->pos().y();
  QPen p;
  if ( _mode==m_draw ) {
    p.setColor( Qt::black );
    p.setWidth( CDFX );
  }
  else {
    p.setColor( Qt::white );
    p.setWidth( CDFX*2 );
  }
  paint.setPen( p );
  paint.drawLine( x, y, x, y );
  paint.end();
  update( x-CDFX, y-CDFX, x+CDFX, y+CDFX );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_canvas::mouseMoveEvent( QMouseEvent*e ) {
  QPainter paint( &pict );
  int _x=last_pos.x();
  int _y=last_pos.y();
  last_pos=e->pos();
  int x=last_pos.x();
  int y=last_pos.y();
  QPen p;
  if ( _mode==m_draw ) {
    p.setColor( Qt::black );
    p.setWidth( CDFX );
  }
  else {
    p.setColor( Qt::white );
    p.setWidth( CDFX*2 );
  }

  paint.setPen( p );
  paint.drawLine( _x, _y, x, y );
  paint.end();
  if ( _x>x ) swap( _x, x );
  if ( _y>y ) swap( _y, y );
  update( _x-CDFX, _y-CDFX, x+CDFX, y+CDFX );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_canvas::leaveEvent( QEvent* ) {
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_canvas::enterEvent( QEvent* ) {
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_canvas::data( QString d ) {
  pict.fill( Qt::white );
  const QString hex="0123456789ABCDEF";
  QByteArray b( d.length()/2 );
  for ( int n=0; n<b.size(); n++ ) {
    uchar ch=hex.find( d[n*2+0] );
    uchar cl=hex.find( d[n*2+1] );
    uchar c=ch;
    c<<=4;
    c|=cl;
    b[n]=c;
  } // for
  QImage i;
  i.loadFromData( b, "PNG" );
  if ( !i.isNull() ) {
    i=i.scale( CDX*CDFX, CDY*CDFY );
    pict.convertFromImage( i, QPixmap::Color );
  }

  repaint();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QString tr_canvas::data() {
  const QString hex="0123456789ABCDEF";
  QImage i=pict.convertToImage();
  i=i.convertDepth( 1, Qt::MonoOnly );

  pict.fill( Qt::white );

  QBuffer b;
  b.open( IO_WriteOnly );
  i.save( &b, "PNG" );

  QString st;
  st.setLength( b.buffer().size()*2 );

  for ( int n=0; n<b.buffer().size(); n++ ) {
    uchar cc=b.buffer() [n];
    uchar ch=hex[cc>>4];
    uchar cl=hex[cc&0x0f];
    st[n*2+0]=ch;
    st[n*2+1]=cl;
  }
  b.close();
  return st;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_canvas::clear() {
  pict.fill();
  repaint();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
