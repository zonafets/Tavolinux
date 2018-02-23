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

#ifndef TR_LABEL_HPP

  #define TR_LABEL_HPP

  #include <qlayout.h>
  #include <qsimplerichtext.h>
  #include <qptrlist.h>
  #include <qlabel.h>
  // #include "qlabel_wrapper.hpp"  // mette doc in public
/* implementa un controllo QLabel cliccabile con ripetizione */

  #include "tr_widget.hpp"
  #include "tr_tab_widget.hpp"

  #define TR_LABEL(x) NEW ( x , tr_label( this, #x ) )
  #define QLABEL(x) NEW ( x , QLabel( this, #x ) )

class tr_label;

typedef QPtrList<tr_label*>tr_label_array;



// qui ci va 1 solo * perche e' gia' una PTR list

/* tr_label_array	tr_label 		object [___________]-->	[______]---------->	+-----+ [___________]			|          |
[___________]			|          | [___________]			+-----+ [___________] */

// queste macro definiscono se il pulsante �basato su una label o button
//#define TIPO_BASE QToolButton
//#define TIPO_BASE_QBUTTON

class tr_label:public tr_widget {
  Q_OBJECT

  public:

  tr_label( QWidget*parent, const QString name=QString::null, WFlags f=0 );
  ~tr_label();

  enum tr_stato { normale=0, premuto=1, off=0, on=1 };

  tr_stato stato() { return isEnabled()?_stato:off; }

  bool isOn() { return stato()==on?true:false; }

  void setOn() { stato(on); }

  private: QString _color_cycling; public:
  void set_color_cycling() {_color_cycling="white|green|yellow|magenta|cyan|white";}

  void stato( tr_stato s );

  void not_stato() { stato()==on?stato( off ):stato( on ); }

  enum tr_aspetto { piatto, rialzato, pulsante, check, radio };

  tr_aspetto aspetto() {return _aspetto;}

  void premi();
  void rilascia();

  void alza( bool alza );
  void aspetto_piatto();
  void aspetto_bottone( int profondita=1 ); // mette un bordo che conferisce alla label un aspetto da pulsante
  QColor aspetto_bottone( QString normale, QString premuto=QString::null );

  void pulsante_animato( bool stato=true ) { _animato=stato; }

  void aspetto_check( bool attivo=false );
  void aspetto_radio( const QStringList etichette, bool verticale=true, int start=-1 );
  void aspetto_radio( const QStringList etichette, int start );
  void aspetto_titolo(); // richtext + centrato + singleline

  bool ripetizione() { return _ripetizione; };

  void ripetizione( bool stato ) { _ripetizione=stato; }; // abilita il click a ripetizione default= false

  void cambia_testo( QString );
  void cambia_icona( QString );
  QString solo_testo();

  QString text();

  void setText( QString txt, int pixelSize=0, bool bold=false ); // se pixelSize<0 scala di pixelSize la dim corrente
  void setText( QString icona, QString testo );
  void setSplit( QString format, QString data );


  void setTextPixelSize( int ps ) {
    QFont p_font( font() );
    p_font.setPixelSize( ps );
    //p_font.setFixedPitch( true );
    setFont( p_font );
    if ( label ) label->setFont( p_font );
  }

  void bold( bool stato=true ) {
    QFont p_font( font() );
    p_font.setBold( stato );
    setFont( p_font );
  }

  const QRect contentsRect() { return label->contentsRect(); }

  void setPixmap( const QPixmap&p ) { label->setPixmap( p ); }

  void setAlignment( int a ) { label->setAlignment( a ); }

  //void setBackgroundMode ( BackgroundMode b);
  void noBackground() { setBackgroundMode( Qt::NoBackground ); }

  void setScaledContents( const bool&s ) { label->setScaledContents( s ); }

  operator const QLabel*() { return label; }

  void setMinimumSize( int w, int h );
  void setMaximumSize( int w, int h );

  void setMinimumSize( const QSize&s ) { setMinimumSize( s.width(), s.height() ); }

  void setMaximumSize( const QSize&s ) { setMaximumSize( s.width(), s.height() ); }

  void setMinimumWidth( int minw );
  void setMinimumHeight( int minh );
  void setMaximumWidth( int maxw );
  void setMaximumHeight( int maxh );
  //void setMinMax(int min_x, int min_y, int max_x=-1, int max_y=-1);
  void setSizePolicy( QSizePolicy );
  void setSizePolicy( QSizePolicy::SizeType hor, QSizePolicy::SizeType ver, bool hfw=FALSE );

  int alignment() const { return label->alignment(); }

  void setFrameShape( QFrame::Shape f ) { label->setFrameShape( f ); }

  void setMargin( int m ) { label->setMargin( m ); }

  void setTextFormat( Qt::TextFormat formato ) { label->setTextFormat( formato ); }

  void setPaletteBackgroundColor( QColor c );
  void setPaletteForegroundColor( QColor c );
  void setPaletteBackgroundPixmap( QPixmap p );

  tr_tab_widget *tabs;
  QFrame *base;
  QFrame *bases[10];
  QGridLayout*layout;
  tr_label*icona;
  QLabel*label; // l'etichetta contenuta nel botton

  // indica che ll'etichetta viene forzata su singola linea
  bool oneline() {return _oneline;}
  void oneline(bool o);

  bool ha_immagine(); // restituisce true se nel testo contenuto c'�<img=
  bool immagine_fissa(); // restituisce true se nel testo contenuto c'�<img= e width=
  bool ha_pixmap(); // restituisce true se come sfondo ha un'immagine

  void adatta( bool su_piu_righe=false ); // riduce la dimensione del font contenuto fino a quanto non ci sta

  int number(); // restituisce il valore numerico della stringa o la posizione del radio
  void number(int n); // imposta il numero di radio button

  void light();
  void dark();

  int indice;		// utile per identificazione univoca

  bool onlinks;

private:
  bool _ripetizione;
  int _radio;
  bool _su_piu_righe;
  bool _animato;
  int _spessore;
  int timeIdAnimazione;
  bool _oneline;

  QColor colore_sfondo; // usato in light e dark

protected:
  bool event( QEvent*e );
  tr_stato _stato;
  tr_aspetto _aspetto;
  QString _normale;
  QString _premuto;


public slots:
  void setEnabled( bool );

signals:
  void clicked();

};

#endif
