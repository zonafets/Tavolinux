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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "tr_globals.hpp"
#include "tr_tools.hpp"
#include "tr_menu.hpp"

#include <qapplication.h>

extern tr_globals glb;
extern tr_tools tools;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int tr_menu::tr_key_list::posizione( tr_key*k ) {
  if ( k ) {
    for ( uint i=0; i<count(); i++ ) {
      tr_key c=operator[] ( i );
      if ( c.pulsante==k->pulsante&&c.is==k->is )
        return i+pagina*count();
    }
  }
  return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int tr_menu::tr_key_list::posizione( tr_menu_pulsante*p ) {
  if ( p ) {
    tr_key*k=chiave( p );
    return posizione( k );
  }
  return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int tr_menu::tr_key_list::posizione() {
  // asltrimenti cerca il primo pulsante senza voce
  for ( uint i=0; i<count(); i++ ) {
    tr_key*k=&operator[] ( i );
    if ( !k->is ) return i+pagina*count();
  }

  return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::tr_key_list::clear( QLayout*l ) {
  while ( count()>0 ) {
    QWidget*w=( QWidget* ) last().pulsante;
    if ( l ) l->remove( w );
    DELETE( w ); // il distruttore di QObject lo rimuove anche dal parent
    pop_back();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::tr_key_list::append( tr_menu_pulsante*p, tr::menu::voce*v ) {
  tr_key k;
  k.pulsante=p;
  if ( v ) k.is=v->is;
  else k.is=0;
  append( k );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_menu::tr_key*tr_menu::tr_key_list::chiave( tr_menu_pulsante*p ) {
  for ( uint i=0; i<count(); i++ ) {
    tr_key*k=&operator[] ( i );
    if ( k->pulsante==p ) return k;
  }
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_menu::tr_key*tr_menu::tr_key_list::chiave( tr::menu::voce*v ) { return&( *this ) [v->disposizione%count()]; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_menu::tr_key*tr_menu::tr_key_list::chiave( int id ) {
  for ( uint i=0; i<count(); i++ ) {
    tr_key*k=&operator[] ( i );
    if ( k->is&&k->is==id ) return k;
  }
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::tr_key_list::pagina_clicked() {
  int id_padre=0;
  tr_menu::tr_key*k=menu->cd();
  if ( k ) {
    if ( locazione==tr::menu::voce::loc_corolla ) id_padre=menu->voce( k->is )->is_padre;
    if ( locazione==tr::menu::voce::loc_elementi ) id_padre=menu->voce( k->is )->is;
  }

  // in modalita' design puo' andare su una pagina vuota e poi torna
  // alla prima
  if ( menu->modalita==tr_menu::m_design )
    if ( pagina<=ultima_pagina( id_padre ) ) pagina++;
    else
      pagina=0;

  // in modalita' uso, se la pag successiva è vuota torna alla prima
  if ( menu->modalita==tr_menu::m_uso )
    if ( pagina<ultima_pagina( id_padre ) ) pagina++;
    else
      pagina=0;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// con pag==-1 verifica se i pulsanti correnti sono vuoti
// con pag!=-1 verifica se ci sono voci nella pagina idicata
int tr_menu::tr_key_list::ultima_pagina( int id_padre ) {
  int d=0;
  QIntDictIterator<tr::menu::voce>it( menu->menu );
  for ( ; it.current(); ++it ) {
    tr::menu::voce*v=it.current();
    bool b=true;
    if ( locazione!=tr::menu::voce::loc_corolla&&v->is_padre!=id_padre ) b=false;
    if ( b&&v->locazione==locazione&&v->disposizione>d ) d=v->disposizione;
  }
  return d/count();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::tr_key_list::vuota() {
  for ( uint i=0; i<count(); i++ ) {
    tr_key*k=&operator[] ( i );
    k->is=0;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::tr_key_list::ridisegna() {

  static tr_key*old_cd=NULL;

  // se indice di pagina è cambiato rispetto a quello visualizzato
  int disp_base;
  disp_base=pagina*count();
  bool ridis=false;
  int is_padre=0;

  /* if (menu->voci()>0 && locazione==tr::menu::voce::loc_corolla && pagina!=prec_pagina) {
  prec_pagina=pagina; ridis=true; } */
  ridis=true;

  if ( locazione==tr::menu::voce::loc_indefinita ) ridis=false; // percorso

  tr_menu::tr_key*cd=menu->cd();
  if ( locazione==tr::menu::voce::loc_elementi ) {
    if ( old_cd!=cd ) ridis=true;
    if ( old_cd&&cd&&old_cd==cd&&old_cd->is!=cd->is ) ridis=true;
    old_cd=cd;
  }

  if ( cd&&cd->is==IS_VARIANTI ) is_padre=IS_VARIANTI;
  else {
    if ( cd&&locazione==tr::menu::voce::loc_corolla ) is_padre=menu->voce( cd->is )->is_padre;
    if ( cd&&locazione==tr::menu::voce::loc_elementi ) is_padre=menu->voce( cd->is )->is;
  }

  for ( uint i=0; i<count(); i++ ) {
    tr_key*k=&operator[] ( i );
    if ( ridis ) {
      if ( is_padre>IS_VARIANTI||locazione==tr::menu::voce::loc_elementi ) {
        tr::menu::voce*v=menu->voce( locazione, is_padre, i+disp_base );
        if ( v )
          k->is=v->is;
        else
          k->is=0;
      }
    }
    k->ridisegna( menu );

    /* if (is_padre==IS_VARIANTI && locazione==tr::menu::voce::loc_elementi) { menu->adatta_testo(k->pulsante); } */

  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_menu_pulsante::tr_menu_pulsante( QWidget*parent ):tr_label( parent, 0, 0 ) {
  darkable( true );
  setFixedSize( config.menu_button_size_x, config.menu_button_size_y );
  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

  setPaletteForegroundColor( QColor( 0, 0, 127 ) );

  setBackgroundOrigin( tr_label::WidgetOrigin );

  QFont pfont( font() );
  pfont.setStyleHint(QFont::Helvetica, QFont::PreferAntialias);
  pfont.setPixelSize( config.menu_button_font_size );
  pfont.setFamily( config.menu_font );
  pfont.setBold( false );
  setFont( pfont );
  if ( config.stile_menu<2 ) aspetto_bottone();
  if ( config.stile_menu==2 ) aspetto_bottone( 2 );
  if ( config.stile_menu==3 ) {
    aspetto_bottone( 1 );

  }
  // if ( config.stile_menu==3 ) tasto->aspetto_bottone( 0 );
  pulsante_animato();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu_pulsante::evidenzia() {
  setPaletteBackgroundColor( paletteBackgroundColor().light() );
  palette_dark( this );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu_pulsante::sfondo( QColor c, QString testo, QString pixmap ) {
  if ( !pixmap.isEmpty() ) setPaletteBackgroundPixmap( PIXMAP( pixmap ) );
  else
    setPaletteBackgroundColor( c );
  setText( testo, 0, config.menu_testo_bold );
  palette_dark( this );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu_pulsante::sfondo( tr::menu::voce*voce ) {
  if ( voce ) {
    QColor c=voce->colore;
    if ( !c.isValid() ) c=colore();
    QString st=voce->descrizione;

    setName( voce->descrizione_originale.latin1() );

    if ( !voce->icona.isEmpty() ) {
      st=st.upper();
      st=tools.solo_testo( st );
      st.truncate( 8 );
      if (config.pocket.isEmpty()) {
        if (QMimeSourceFactory::defaultFactory() &&
            QMimeSourceFactory::defaultFactory()->data(voce->icona))
        // if ( voce->icona.left( 3 )=="az_" )
          st.prepend( "<img src=\"" +voce->icona+"\" width=\"32\" height=\"32\"><br><small>" );
        else {
          st.prepend( "<img src=\"" +config.exe_path+"icone/" +voce->icona+"\" width=\"32\" height=\"32\"><br><small>" );
        }
        st+="</small>";
      }
      else // se pocket
      {

	  QFont f;
	  f.setPointSize(config.pocket.section('|',4,4).toInt());
 	  f.setFamily(config.pocket.section('|',5,5));
	  setFont(f);
        if (QMimeSourceFactory::defaultFactory() &&
            QMimeSourceFactory::defaultFactory()->data(voce->icona))
        // if ( voce->icona.left( 3 )=="az_" )
        st.prepend( "<img src=\"" +voce->icona+"\" width=\"20\" height=\"20\"><br><small><small>" );
        else {
          st.prepend( "<img src=\"" +config.exe_path+"icone/" +voce->icona+"\" width=\"20\" height=\"20\"><br><small><small>" );
        }
        st+="</small>";
      }
    }

    QString pixmap=voce->pixmap;
    if ( config.stile_menu==1&&pixmap.isEmpty()&&voce->tipo_articolo.gas() ) {
      if ( _tipo==t_corolla ) pixmap="sfondo_pulsante_menu_c.jpg";
      if ( _tipo==t_elementi ) pixmap="sfondo_pulsante_menu.jpg";
    }

    if ( voce->tipo_articolo.gavc() )
      c=c.dark( 130 );

    sfondo( c, st, pixmap );

  }
  else
    sfondo();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu_pulsante::sfondo() {
  sfondo( _tipo );
  setName( "unnamed" );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::tr_key::ridisegna( tr_menu*m ) {
  tr::menu::voce*v=m->voce( is );
  pulsante->sfondo( v );
  if ( v&&v->is_padre==IS_VARIANTI&&v->locazione==tr::menu::voce::loc_elementi ) m->adatta_testo( pulsante );
  pulsante->updateGeometry();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QColor tr_menu_pulsante::colore() {

  QColor c=config.menu_selezionato_colore;

  switch ( _tipo ) {
    case t_corolla:
      c=config.menu_corolla_colore;
    break;
    case t_main_percorso:
      c=config.menu_percorso_colore.light( 100 );
    break;
    case t_percorso:
      c=config.menu_percorso_colore;
    break;
    case t_elementi:
      c=config.menu_elementi_colore;
    break;
    case t_pulsanti:
      c=config.menu_pulsanti_colore;
    break;
  }
  return c;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu_pulsante::sfondo( tr_menu_pulsante::tr_tipo tipo ) {

  _tipo=tipo;
  sfondo( colore(), "" );

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                class                         tr_menu
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// la seguente abilita la modalit�di mostra/noascondi per i pulsanti speciali invece di abilita/disabilita
// #define ART_SHOW_HIDE

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::adatta_testo( tr_label*l ) {
  // l->setText( descrizione );  dovrebbe già essere stato fatto
  QString descrizione=l->text();

  // controllo dimensione e trasformazione della stringa contenuta
  if ( ( l->sizeHint().width()>l->contentsRect().size().width()||l->sizeHint().height()>l->contentsRect().size().height() ) ) {

    // controlla se c'�spazio per una riga in pi
    uint i=0;
    QString t; // t conterra' i tag x l'immagine
    QString s=tools.solo_testo( descrizione ); // .remove(QRegExp("/<(.*)>/"));
    QString parola;
    QStringList ss;
    int lung_parola=0;
    // separa le parole secondo spazi, punteggiatura (e in futuro sillabe)
    while ( i<s.length() ) {
      QChar c=s[i++];
      QChar cs=s[i];
      lung_parola++;
      if ( ( c==' ' )||( c=='.' )||( c==':' )||( c=='\'' )||( c==',' )||( c==';' )||( c=='/' )||i==s.length()
         ||( tools.sillaba( c, cs )&&lung_parola>2 ) ) {
           parola+=c;
           ss<<parola;
           parola="";
      }
      else
        parola+=c;
      if ( ( c==' ' )||( c=='.' )||( c==':' )||( c=='\'' )||( c==',' )||( c==';' ) ) {
        // cout << "lung_parola=" << lung_parola << endl;
        lung_parola=0;
      }
    }

    // riprende le parole e le accorpa fino a riempire una stringa di 9 caratteri

    // interrompe alla terza riga !!!!!!!!!!!!!!!!!!!

    i=0;
    int riga=0;
    s="";
    descrizione="";
    while ( i<ss.count() ) {
      s+=ss[i++];
      if ( s.simplifyWhiteSpace().length()+ss[i].simplifyWhiteSpace().length()>9 ) { //
        riga++;
        if ( riga==3 ) break;
        descrizione+=( s+"<br>" );
        s="";
      }
    }
    descrizione+=s;
    descrizione.prepend( t );
    l->setText( descrizione );

    // se dopo le spezzature, il testo deborda ancora, allora riduce il carattere
    /* if  ((l->sizeHint().width() > l->contentsRect().size().width() ||
    l->sizeHint().height() > l->contentsRect().size().height())) { int cc = 4;
    while ((l->sizeHint().width() > l->contentsRect().size().width() ||
    l->sizeHint().height() > l->contentsRect().size().height()) && cc) { // riduce il font fino a quando ci sta
    QFont f(l->font()); f.setPixelSize(f.pointSize()-1); l->setFont(f); cc--; } } */
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_menu::tr_menu( QWidget*parent, const char*name, WFlags f ):tr_widget( parent, name, f ), dimensione( 0, 0 ) {

  indici[corolla].init( this, tr::menu::voce::loc_corolla );
  indici[elementi].init( this, tr::menu::voce::loc_elementi );
  indici[percorso].init( this );

  setMinimumSize( QSize( config.finestra_larghezza_menu, config.finestra_altezza_centrale ) );
  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  setPaletteBackgroundColor( parent->paletteBackgroundColor() );

  menu.resize( N_VOCI_MENU );
  menu.setAutoDelete( true );

  modificato=false;

  modalita=m_non_specificato;

  frame_percorso=NULL;
  frame_elementi=NULL;
  frame_contenitore=NULL;
  voce_annullata=NULL;
  pagina_corolla=NULL;
  pagina_elementi=NULL;

  id=-1;
  _corrente=NULL;

  // dimensione 0,0 specifica a geometria di inizializzare il menu da 0
  geometria();

  // setMaximumSize( sizeHint() );

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// questa funzione crea la struttura e ridisegna. Il chiamante si deve occupare
// di non chiamarla inutilmente
void tr_menu::geometria( int n_elementi, int n_corolla ) {

  QGridLayout*layout_corolla=( QGridLayout* ) layout();
  QGridLayout*layout_percorso=NULL;
  if ( frame_percorso ) layout_percorso=( QGridLayout* ) frame_percorso->layout();
  QGridLayout*layout_elementi=NULL;
  if ( frame_elementi ) layout_elementi=( QGridLayout* ) frame_elementi->layout();
  QGridLayout*layout_contenitore=NULL;
  if ( frame_contenitore ) layout_contenitore=( QGridLayout* ) frame_contenitore->layout();

  if ( n_elementi==-1&&n_corolla==-1 ) {
    DELETE( pagina_corolla );
    DELETE( pagina_elementi );
    indici[corolla].clear( layout_corolla );
    indici[percorso].clear( layout_percorso );
    indici[elementi].clear( layout_elementi );
    dimensione.setWidth( 0 );
    dimensione.setHeight( 0 );
  }

  if ( !dimensione.corolla()&&!dimensione.elementi() ) { dimensione=tr_size( INIT_DIM_COROLLA, INIT_DIM_ELEMENTI ); }
  else {

    if ( n_corolla>0&&dimensione.corolla()!=n_corolla ) {
      _corrente=NULL;
      DELETE( pagina_corolla );
      DELETE( pagina_elementi );
      indici[corolla].clear( layout_corolla );
      indici[percorso].clear( layout_percorso );
      dimensione=tr_size( n_corolla, dimensione.elementi() );
    }

    if ( n_elementi>0&&dimensione.elementi()!=n_elementi ) {
      _corrente=NULL;
      indici[elementi].clear( layout_elementi );
      dimensione=tr_size( dimensione.corolla(), n_elementi );
    }

  }

  short n_dx; // numero di elementi orizzontali in corolla + 1 (pulsante)
  short n_dy; // numero di elementi verticali in corolla
  int dx, dy;

  /* senza tanti giri strani, poichè comunque dipendenti da combinazioni dipendenti da risoluzione, dotpitch, dimensione fonts
  e ampiezza schermo reale */
  switch ( dimensione.corolla() ) {
    case INIT_DIM_COROLLA:
      n_dx=7;
      n_dy=9;
      dx=minimumSize().width();
      dy=minimumSize().height();
    break;
  }

  if ( !layout_corolla ) {
    NEW( layout_corolla, QGridLayout( this, n_dy, n_dx ) );
    layout_corolla->setMargin( 3 );
  }

  // riferimento di percorso per contenuto elementi
  int id_padre=-1;
  if ( cd() ) id_padre=cd()->is;


  // COROLLA

  if ( !indici[corolla].count() ) { // se non esiste la costruisce

    short x=0; short y=0;

    for ( int i=0; i<dimensione.corolla()+2; i++ ) {

      tr::menu::voce*v=NULL;

      int disp_base=indici[corolla].pagina*dimensione.corolla();
      if ( id_padre!=-1 ) v=voce( tr::menu::voce::loc_corolla, id_padre, i+disp_base );

      enum { p_corolla, p_pag_corolla, p_pag_elementi }
      posizione=p_corolla;
      if ( QPoint( x, y )==QPoint( n_dx-1, 0 ) ) posizione=p_pag_corolla;
      if ( QPoint( x, y )==QPoint( n_dx-1, n_dy-1 ) ) posizione=p_pag_elementi;

      tr_menu_pulsante*tasto;
      NEW( tasto, tr_menu_pulsante( this ) );
      layout_corolla->addWidget( tasto, y, x );
      tasto->show();

      if ( posizione==p_pag_corolla ) {
        pagina_corolla=tasto;
        connect( tasto, SIGNAL( clicked() ), this, SLOT( pagina_corolla_clicked() ) );
        tasto->sfondo( tr_menu_pulsante::t_pulsanti );
        tasto->setPixmap( PIXMAP( "az_dx.png" ) );
      }
      else if ( posizione==p_pag_elementi ) {
        pagina_elementi=tasto;
        connect( tasto, SIGNAL( clicked() ), this, SLOT( pagina_elementi_clicked() ) );
        tasto->sfondo( tr_menu_pulsante::t_pulsanti );
        tasto->setPixmap( PIXMAP( "az_giu.png" ) );
      }
      else {
        connect( tasto, SIGNAL( clicked() ), this, SLOT( corolla_elementi_clicked() ) );
        indici[corolla].append( tasto, v );
        if ( v ) tasto->sfondo( v );
        else
          tasto->sfondo( tr_menu_pulsante::t_corolla );
      }

      // gestione rotazione nella disposizione dei pulsanti della corolla
      enum { dx, giu, sx, su }
      ramo;
      if ( !x&&!y ) ramo=dx;
      if ( x==n_dx-1&&!y ) { ramo=giu; }
      if ( x==n_dx-1&&y==n_dy-1 ) { ramo=sx; }
      if ( !x&&y==n_dy-1 ) ramo=su;
      switch ( ramo ) {
        case dx:
          x++;
        break;
        case giu:
          y++;
        break;
        case sx:
          x--;
        break;
        case su:
          y--;
      } // switch

    } // for

  } // if pulsanti.corllla.count == 0


  indici[corolla].ridisegna();

  // contenitore PERCORSO e ELEMENTI. Serve per avere il bordo
  // che tanto piace a roby
  if ( !frame_contenitore ) { NEW( frame_contenitore, QFrame( this, "frame_contenitore" ) ); }
  layout_corolla->remove( frame_contenitore );
  layout_corolla->addMultiCellWidget( frame_contenitore, 1, n_dy-2, 1, n_dx-2 );
  QFrame*area=frame_contenitore;
  if ( !layout_contenitore ) {
    NEW( layout_contenitore, QGridLayout( frame_contenitore, 2, 1 ) );
    layout_contenitore->setMargin( 0 );
  }
  // area=this;   // se non si usa il contenitore

  // PERCORSO

  if ( indici[percorso].count()==0 ) { // se non c'è lo crea

    if ( !frame_percorso ) { NEW( frame_percorso, QFrame( area, "frame_percorso" ) ); }
    layout_contenitore->remove( frame_percorso );
    //layout_corolla->addMultiCellWidget( frame_percorso, 1, 1 , 1, n_dx-2);
    layout_contenitore->addMultiCellWidget( frame_percorso, 0, 0, 0, 0 );
    if ( !layout_percorso ) { NEW( layout_percorso, QGridLayout( frame_percorso, 1, n_dx-2 ) ); }

    // le caselle del percoso sono n_dx - 2
    for ( int i=0; i<n_dx-2; i++ ) {
      tr_menu_pulsante*tasto;
      NEW( tasto, tr_menu_pulsante( frame_percorso ) );
      // tasto->aspetto_bottone(0);
      layout_percorso->addWidget( tasto, 0, i );
      indici[percorso].append( tasto );
      tasto->show();

      connect( tasto, SIGNAL( clicked() ), this, SLOT( percorso_clicked() ) );
      tasto->sfondo( tr_menu_pulsante::t_percorso );
      if ( !i ) {
        //tasto->setPaletteBackgroundPixmap(PIXMAP("az_main.png"));
        tasto->sfondo( tr_menu_pulsante::t_main_percorso );
        tasto->setText( "MAIN<br>(1,1)" /* PERCORSO_MAIN */ );
      }
    }

  } // if (pulsanti_percorso.count()==0)

  else // altrimenti ri-inizializza (sfondo e testo)

  {
    indici[percorso].ridisegna();
    indici[percorso] [0].pulsante->setText( "MAIN<br>" +QSN( indici[corolla].pagina+1 )+","
       +QSN( indici[elementi].pagina+1 ) );
  }


  // ELEMENTI

  if ( indici[elementi].count()==0 ) { // se non esistono li crea

    if ( !frame_elementi ) {
      NEW( frame_elementi, QFrame( area, "frame_elementi" ) );
      /* questo codice di merda, ciao ma non perchè di merda lui ma perchè le QT son di merda, non funziona
      frame_elementi->setFrameShape(QFrame::Box); frame_elementi->setLineWidth(3); frame_elementi->setMidLineWidth(3);
      frame_elementi->setMargin(3); */

      // layout_corolla->addMultiCellWidget( frame_elementi, 2, n_dy-2 , 1, n_dx-2);
      layout_contenitore->addMultiCellWidget( frame_elementi, 1, 1, 0, 0 );
    }

    // dimensione menu - (larghezza tasto corolla * 2) diviso geometria

    int divx, divy; // numero di divisioni
    int dx,dy;
    dx=config.menu_button_size_x*5; dy=config.menu_button_size_y*6; // dimensioni fisse usate per necessita'
    switch ( dimensione.elementi() ) {
      case 3:
        divx=1;
        divy=3;
      break;
      case 4:
        divx=2;
        divy=2;
      break;
      case 9:
        divx=3;
        divy=3;
      break;
      case 16:
        divx=4;
        divy=4;
      break;
      case 30:
        divx=5;
        divy=6;
      break;
    }

    dx/=divx;
    dy/=divy;

    // PP(divx);
    // PP(divy);

    int disp_base=indici[corolla].pagina*dimensione.corolla();

    if ( !layout_elementi ) { NEW( layout_elementi, QGridLayout( frame_elementi, divy, divx ) ); }
    for ( int j=0; j<divy; j++ )
      for ( int i=0; i<divx; i++ ) {

        tr::menu::voce*v=NULL;
        if ( id_padre!=-1 ) v=voce( tr::menu::voce::loc_elementi, id_padre, j*divx+i+disp_base );

        tr_menu_pulsante*tasto;
        NEW( tasto, tr_menu_pulsante( frame_elementi ) );
        tasto->setMinMax( dx, dy );
        layout_elementi->addWidget( tasto, j, i );
        tasto->sfondo( tr_menu_pulsante::t_elementi );
        tasto->show();
        indici[elementi].append( tasto, v );

        connect( tasto, SIGNAL( clicked() ), this, SLOT( corolla_elementi_clicked() ) );

      }

  } //  if (pulsanti_elementi.count()==0)

  // disegna contenuto fissando le dimensioni dei pulsanti
  indici[elementi].ridisegna();

  tr_key*k=cd();
  tr::menu::voce*voce=NULL;
  if ( k ) voce=tr_menu::voce( k->is );
  // se è stato apportato un cambiamento esterno alla geometria
  if ( k&&k->is!=-1&&voce&&voce->geometria!=dimensione.elementi()&&modalita==tr_menu::m_design ) {
    voce->geometria=dimensione.elementi();
    voce->salva(); // salva la nuova configurazione
  }
  else {
    // frame_percorso->setEnabled(k);
    // frame_elementi->setEnabled(k);
  }

  if ( _corrente&&_corrente->is )
    _corrente->pulsante->evidenzia();


  // gestione comparsa scomparsa dei pulsanti freccia

  if ( modalita==tr_menu::m_uso ) {
    int id_padre=0;
    tr_menu::tr_key*k=cd();
    if ( k ) id_padre=k->is;

    // in modalita' uso, se la pag successiva è vuota torna alla prima
    if ( indici[corolla].ultima_pagina( 0 )>0 ) {
      pagina_corolla->setPixmap( PIXMAP( "az_dx.png" ) );
      pagina_corolla->setEnabled( true );
    }
    else {
      // pagina_corolla->setPixmap( QPixmap() );
      pagina_corolla->setPixmap( PIXMAP( "az_dx.png" ) );
      pagina_corolla->setEnabled( false );
    }

    if ( indici[elementi].ultima_pagina( id_padre )>0 ) {
      pagina_elementi->setPixmap( PIXMAP( "az_giu.png" ) );
      pagina_elementi->setEnabled( true );
    }
    else {
      // pagina_elementi->setPixmap( QPixmap() );
      pagina_elementi->setPixmap( PIXMAP( "az_giu.png" ) );
      pagina_elementi->setEnabled( false );
    }

  }

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int tr_menu::larghezza() {
  int dx=sizeHint().width();
  if ( dx<0 ) dx=420;
  return dx;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_menu::~tr_menu() {
  if ( voce_annullata ) { DELETE( voce_annullata ); voce_annullata=NULL; }
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::pagina_corolla_clicked() {

  _corrente=NULL;
  cd( main );
  indici[corolla].pagina_clicked();
  indici[elementi].vuota();
  geometria();
  if ( modalita==tr_menu::m_design ) emit clicked();

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::pagina_elementi_clicked() {

  if ( !cd() ) BEEPER;

  _corrente=NULL;
  indici[elementi].pagina_clicked();
  geometria();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::deseleziona() {
  if ( _corrente ) {
    _corrente->ridisegna( this );
    _corrente=NULL;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::corolla_elementi_clicked() {

  tr_menu_pulsante*premuto=( tr_menu_pulsante* ) sender();
  tr_key*k=chiave( premuto );
  tr::menu::voce*v=voce( k->is ); // qui è implicito che k esiste
  tr::menu::voce*cd_v=voce( cd() );

  if ( modalita==m_design ) {

    tr::menu::voce*vp=NULL;
    if ( _corrente ) vp=voce( _corrente->is );

    enum tr_azione { az_aggiunta=1, az_spostamento, az_selezione, az_entra }
    az=( tr_azione )0;
    if ( !v ) az=az_aggiunta;
    if ( v&&vp!=v ) az=az_selezione;
    if ( vp&&!v ) az=az_spostamento;
    // un secondo clic su un elemento che è anche gas = cd
    if ( v&&vp==v&&v->tipo_articolo.gas()&&v->locazione==tr::menu::voce::loc_elementi ) az=az_entra;

    switch ( az ) {
      case az_aggiunta:
        if ( voce_annullata ) { DELETE( voce_annullata ); voce_annullata=NULL; }
        deseleziona();
        _corrente=k;
        // se ne accupa aggiungi(grp)
      break;
      case az_selezione:
        deseleziona();
        if ( v->locazione==tr::menu::voce::loc_corolla ) {
          if ( v->tipo_articolo.gas() ) cd( k->is );
          else
            cd( voce( k->is )->is_padre );
          // nel caso degli elementi, c'è az_entra
        }
        _corrente=k;
        premuto->evidenzia(); // seleziona
      break;
      case az_entra:
        if ( v->locazione==tr::menu::voce::loc_corolla ) _corrente=k;
        else
          _corrente=NULL;
        if ( v->tipo_articolo.gas() ) cd( v->is );
      break;
      case az_spostamento: {
          if ( voce_annullata ) { DELETE( voce_annullata ); voce_annullata=NULL; }
          tr::menu::voce::tr_locazione loc=tr::menu::voce::loc_elementi;
          int pos=indici[corolla].posizione( premuto );
          if ( pos==-1 ) pos=indici[elementi].posizione( premuto );
          else
            loc=tr::menu::voce::loc_corolla;

          // spostamenti permessi:
          // da corolla a corolla
          // da elementi a elementi
          // da elementi a corolla
          // da corolla a elementi se la voce di corolla ha come padre cd()->voce->is
          if ( ( vp->locazione==tr::menu::voce::loc_corolla&&loc==tr::menu::voce::loc_elementi
             &&( ( cd_v&&vp->is_padre!=cd_v->is )||( !cd_v ) ) )||vp->is_padre==IS_VARIANTI ) BEEP;
          else {
            k->is=_corrente->is;
            _corrente->is=0;
            vp->locazione=loc;
            vp->disposizione=pos;
            vp->salva();
            k->ridisegna( this );
            _corrente->ridisegna( this );
            premuto->evidenzia(); // seleziona
          } // else spostabile

          deseleziona();
          _corrente=k;
        break;
        } // case az_spostamento

    } // switch

    emit clicked();
  }
  else { // se modalita m_use
    if ( v ) {
      if ( v->tipo_articolo.gas() ) cd( v->is ); // se è un gruppo selezione: entra
      else {
        _corrente=k;
        emit clicked();
      }
    }
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::percorso_clicked() {
  tr_key*k=indici[percorso].chiave( ( tr_menu_pulsante* ) sender() );
  if ( !k ) BEEPER;
  tr::menu::voce*v=voce( k );
  if ( v&&v->is==IS_VARIANTI ) BEEPER;
  if ( v ) cd( v->is );
  if ( !v&&k==&indici[percorso] [0] ) cd( main );
  emit clicked();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool tr_menu::cambia( int id_listino, bool visualizza, bool mantieni_percorso ) {

  // e dealloca le voci
  menu.clear();

  tr::menu struttura_menu;
  struttura_menu.carica( id_listino );

  id=id_listino;
  return cambia( struttura_menu.elenco, visualizza, mantieni_percorso );

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool tr_menu::cambia( QString id_listino, bool visualizza, bool mantieni_percorso ) {

  if ( glb.cache_menu_listino.count()&&glb.cache_menu_listino[0].section( '|', 0, 0 )==id_listino ) {
    return cambia( glb.cache_menu_listino, visualizza, mantieni_percorso );
  }

  // e dealloca le voci
  menu.clear();

  tr::menu struttura_menu;
  struttura_menu.carica( id_listino.toInt() );
  glb.cache_menu_listino=struttura_menu.elenco;

  id=id_listino.toInt();
  return cambia( struttura_menu.elenco, visualizza, mantieni_percorso );

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// conserva id precedente (id)

bool tr_menu::cambia( QStringList elenco, bool visualizza, bool mantieni_percorso ) {

  bool r;

  // e dealloca le voci

  QTime t; t.start();
  if ( !elenco.count() ) r=false;
  else {

    TROWSLI( elenco ) {
      QTime t; t.start();
      tr::menu::voce*voce;
      NEW( voce, tr::menu::voce );
      voce->parse( elenco[i] );
      menu.insert( voce->is, voce );
    }

    r=true;
  }
  if ( !voci() ) mantieni_percorso=false;

  if ( visualizza ) {
    t.restart();
    if ( !mantieni_percorso ) geometria( -1, -1 );
    else geometria();
  }

  return r;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// entra(visualizza) nel gruppo.
//    Se è presente nel percorso, scala fino a raggiungere questo id_istanza
//    se non è presente, vi entra
//    se is è precedente equivale a cd ..
//    se is è varianti mostra il gruppo varianti globali+gruppo+elemento corrente
//    se is è main, vuota la pagina e il percorso
// riposiziona in pagina 0 gli elementi
// visualizza i figli dell'elemento is

tr_menu::tr_key*tr_menu::cd( int is, int is_extra ) {

  tr_key*k=cd();
  uint i;

  // rimuove
  if ( k&&k->is==IS_VARIANTI ) {

    tr::menu::voce*v=voce( IS_VARIANTI );
    if ( !v ) return k;
    menu.take( v->is );
    DELETE( v );

    v=voce_padre( IS_VARIANTI );
    while ( v ) {
      menu.take( v->is );
      DELETE( v );
      v=voce_padre( IS_VARIANTI );
    }

    if ( is!=precedente ) {
      k=cd();
      k->is=0;
    }
  }

  switch ( is ) {

    case varianti: {

        indici[elementi].pagina=0;

        for ( i=indici[percorso].count()-1; i>0; i-- )
          if ( indici[percorso] [i].is!=0 )
          break;

        indici[percorso] [i+1].is=IS_VARIANTI;

        tr::menu::voce*voce;

        // aggiunge gruppo varianti (globali+gruppo+articolo)
        NEW( voce, tr::menu::voce );
        voce->is=IS_VARIANTI;
        voce->is_padre=IS_VARIANTI; // per ridisegno geometria corolla
        voce->descrizione=tr( "VARIANTI" );
        voce->geometria=30;
        voce->icona="az_varianti.png";
        voce->pixmap="";

        voce->tipo_articolo.stato=GRUPPO_ARTICOLO_SELEZIONE|GRUPPO_ARTICOLO_VARIANTE;
        menu.insert( voce->is, voce );

        tr::varianti varianti;
        if ( corrente() ) is_extra=_corrente->is;
        if ( is_extra!=-1 ) varianti.carica( is_extra );
        else {
          if ( k ) varianti.carica( k->is );
          else
            varianti.carica();
        }

        TROWSLI( varianti.elenco ) {

          tr::menu::voce*voce;
          tr::gruppi::gruppo grp=varianti.parse( i );

          NEW( voce, tr::menu::voce( grp ) );
          voce->is_padre=IS_VARIANTI;
          voce->locazione=tr::menu::voce::loc_elementi;
          voce->disposizione=i;
          voce->descrizione=voce->descrizione.lower();
          voce->descrizione_originale=voce->descrizione;
          menu.insert( voce->is, voce );

        }
        // vengono caricate e inserite le voci varianti

        _corrente=NULL;
        geometria( voce->geometria );
      }

    break;

    case precedente:

      k->is=0; // toglie l'ultimo riferimento
      indici[elementi].pagina=0;
      _corrente=NULL;
      k=cd();
      if ( k ) {
        tr::menu::voce*v=voce( k->is );
        geometria( v->geometria ); // ridisegna il contenuto
      }
      else
        geometria(); // ridisegna il contenuto

    break;

    case main:

      for ( i=0; i<indici[percorso].count(); i++ ) { indici[percorso] [i].is=0; }
      indici[elementi].pagina=0;
      indici[elementi].vuota();
      _corrente=NULL;
      geometria();
      k=&indici[percorso] [0];
    break;

    default:

      indici[elementi].pagina=0;
      i=0;
      tr::menu::voce*v=menu.find( is );
      if ( k&&v ) { // c'è già gualche gruppo presente
        // cerca se è figlio di uno dei presenti
        bool clear=false;
        int j=1;
        for ( i=1; i<indici[percorso].count(); i++ ) {
          if ( clear ) indici[percorso] [i].is=0;
          if ( indici[percorso] [i].is&&voce( indici[percorso] [i].is )->is==v->is_padre ) {
            clear=true;
            j=i;
          }
        } // for

        if ( !clear ) // se non ha cancellato niente non è fglio di presenti
        // quindi cancella tutto
          for ( i=indici[percorso].count()-1; i>0; i-- ) indici[percorso] [i].is=0;
        else
          i=j;
      }
      if ( v ) indici[percorso] [i+1].is=v->is;
      _corrente=NULL;
      if ( v ) geometria( v->geometria );
      k=&indici[percorso] [i+1];
  };


  // frame_percorso->setEnabled(k);
  // frame_elementi->setEnabled(k);

  return k;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// restituisce NULL se
tr_menu::tr_key*tr_menu::cd() {
  tr_key*k=NULL;
  for ( uint i=1; i<indici[percorso].count(); i++ ) {
    if ( indici[percorso] [i].is==0 ) break;
    k=&indici[percorso] [i];
  }
  return k;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// vedere *_clicked
tr_menu::tr_key*tr_menu::corrente() { return modalita==m_design?_corrente:NULL; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr::menu::voce*tr_menu::selezionata() { return _corrente?voce( _corrente ):NULL; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_menu::inserisci( tr::gruppi::gruppo*grp ) {
  // cerca l'articolo piu' lontano nel menu
  int last_disp=0;
  TROWSLI(glb.cache_menu_listino) {
    denu_elenco_menu_listino;
    QString voce=glb.cache_menu_listino[i];
    if (voce.section('|',p_id_istanza_padre,p_id_istanza_padre).toInt()==grp->is_padre) {
      int disp=voce.section('|',p_disposizione,p_disposizione).toInt();
      if (last_disp<disp) {
        last_disp=disp;
      }
    } // if voce.is_padre
  } // TROWSLI

  while (last_disp>0) {
    TROWSLI(glb.cache_menu_listino) {
      denu_elenco_menu_listino;
      QStringList e=QStringList::split('|',glb.cache_menu_listino[i],true);
      int disp=e[p_disposizione].toInt();
      if (disp==last_disp) {
       if (!CMD( tok_modifica_menu, e[p_id_listino], e[p_id_istanza],
               e[p_locazione], QSN(disp+1), e[p_geometria], e[p_descrizione],
               e[p_colore], e[p_icona], e[p_pixmap])) return false;
        break;
      }
    } // TROWSLI
    last_disp--;
  } // while

  return aggiungi(grp);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_menu::aggiungi( tr::gruppi::gruppo*grp ) {

  tr::menu::voce voce( *grp );

  tr_key*k=corrente();
  if ( k ) {
    if ( voce.disposizione==-1 ) {
      // controlla se il pulsante prmuto sta sulla corolla o sugli elementi
      int pos=indici[corolla].posizione( k->pulsante );
      if ( cd()&&pos==-1 ) pos=indici[elementi].posizione( k->pulsante );
      voce.disposizione=pos;
    }
    if ( indici[elementi].chiave( k->pulsante )&&!cd() ) BEEPERF;
    if ( indici[corolla].chiave( k->pulsante ) ) voce.locazione=tr::menu::voce::loc_corolla;
    if ( indici[elementi].chiave( k->pulsante ) ) voce.locazione=tr::menu::voce::loc_elementi;
  }
  else {
    // cerca la prima posizione libera
    int i;
    if ( !tr_menu::voce( grp->is ) ) {
      for ( i=0; i<1000; i++ ) {
        tr::menu::voce*v=tr_menu::voce( tr::menu::voce::loc_elementi, cd()->is, i );

		// --- INIZIO CODICE SPOSTATO DA LUCA

 		if ( i >=( indici[elementi].pagina+1 )*indici[elementi].size() )
      			pagina_elementi_clicked();

		// --- FINE CODICE SPOSTATO DA LUCA

		if ( !v ) break;
      }
    }
    else
      return false;
    voce.disposizione=i;
    voce.locazione=tr::menu::voce::loc_elementi;

	// Patch di Luca del 22/7 - Le righe nel ciclo "for" precedente
	// erano in precedenza qui. Provocavano un fastidioso segfault
	// se l'utente selezionava il disponi e aveva gia' inserito un numero
	// di elementi in pagina del menu sufficienti a riempire esattamente
	// un multiplo intero del numero delle pagine. In pratica il disponi
	// funzionava correttamente solo in caso di pagine degli elementi "vuote".
	// Ora invece la pagina viene avanzata in fase di ricerca della posizione
	// libera (se serve) aggiornando correttamente il contatore apposito

    k=&indici[elementi] [voce.disposizione-indici[elementi].pagina*indici[elementi].size()];
  }

  voce.id_listino=id;
  voce.geometria=dimensione.elementi();

  bool r=true;
  if ( voce.descrizione==voce.descrizione_originale ) {
    k->pulsante->setText( voce.descrizione_originale );
    adatta_testo( k->pulsante );
    voce.descrizione=k->pulsante->text();
  }

  if ( voce.salva() ) {

    if ( voce_annullata ) { DELETE( voce_annullata ); voce_annullata=NULL; }
    modificato=true;
    if ( !_corrente ) {
      // aggiunge manualmente la voce al dizionario del menu
      tr::menu::voce*v;
      NEW( v, tr::menu::voce( voce ) );
      menu.insert( v->is, v );

      return r; // se e' un'aggiunta da disposizione
    }

    _corrente=NULL;
    cambia( id, true, true );

  }
  else {
    r=false;
    k->pulsante->setText( "" );
  }

  return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_menu::rimuovi() {
  if ( CMD( tok_rimuovi_menu, QSN( id ) ) ) return cambia( id );
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rimuove la voce correntemente selezionata
// può essere un elemento
// può essere una voce di corolla che corrisponde a cd()
bool tr_menu::cancella() {

  if ( !corrente() ) return false;

  tr_key*k=corrente();
  tr::menu::voce*v=voce( k->is );
  if ( cd()&&cd()->is==v->is )
    cd( precedente );

  bool r=true;

  NEW( voce_annullata, tr::menu::voce );
  *voce_annullata=*v;

  r=CMD( tok_modifica_menu, QSN( id ), QSN( v->is ), QSN( v->locazione ), QSN( v->disposizione ), QSN( v->geometria ), LEAVE, QColor().name(), "", "" );

  if ( r ) {
    _corrente=NULL;
    k->pulsante->sfondo();
    k->is=0;
    cambia( id );
  }

  return r;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::cancella( int is ) {
  tr_key*k=indici[corolla].chiave( is );
  if ( !k ) k=indici[elementi].chiave( is );
  if ( k ) {
    tr::menu::voce*v=voce( k->is );
    k->is=0;
    k->ridisegna( this );
    // if ( v->is_padre==IS_VARIANTI ) {
      menu.take( v->is );
      DELETE( v );
    // }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_menu::disponi( const QStringList elenco ) {
  tr_key*k;
  if ( !(k=cd()) ) return false;

  tr_key*hold=_corrente;
  cd (k->is);
  _corrente=NULL;
  int righe_da_processare=elenco.count();
  TROWSLI( elenco ) {
    tr::gruppi::gruppo grp;
    grp.parse( elenco[i] );
    if ( !grp.tipo_articolo.gasv() ) { if ( aggiungi( &grp ) ) righe_da_processare--; }
    else
      righe_da_processare--;
  }
  _corrente=hold;

  cambia( id, true, true );

  // aggiorna visualizzazione
  geometria();

  if ( voce_annullata ) {
    DELETE( voce_annullata );
    voce_annullata=NULL;
  }

  return ( righe_da_processare==0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_menu::annulla() {
  voce_annullata->salva();
  voce_annullata=NULL;
  cambia( id, true, true );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_menu::tr_key*tr_menu::chiave( tr_menu_pulsante*p, tr::menu::voce::tr_locazione dove ) {
  if ( dove==tr::menu::voce::loc_elementi ) return indici[elementi].chiave( p );
  if ( dove==tr::menu::voce::loc_corolla ) return indici[corolla].chiave( p );

  // se indefinito esegue una ricerca su tutti
  tr_key*k=indici[elementi].chiave( p );
  if ( !k ) k=indici[corolla].chiave( p );
  if ( !k ) k=indici[percorso].chiave( p );
  return k;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_menu::tr_key*tr_menu::chiave( tr::menu::voce*v ) {
  tr_key*k=NULL;
  if ( v->locazione==tr::menu::voce::loc_elementi ) k=indici[elementi].chiave( v );
  if ( v->locazione==tr::menu::voce::loc_corolla ) k=indici[corolla].chiave( v );
  return k;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr::menu::voce*tr_menu::voce( tr::menu::voce::tr_locazione loc, int id_padre, int disp ) {
  QIntDictIterator<tr::menu::voce>it( menu );
  for ( ; it.current(); ++it ) {
    tr::menu::voce*v=it.current();
    bool b=true;
    if ( loc!=tr::menu::voce::loc_corolla&&v->is_padre!=id_padre ) b=false;
    if ( b&&v->locazione==loc&&v->disposizione==disp ) return v;
  }
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr::menu::voce*tr_menu::voce( tr::menu::voce::tr_locazione loc, int disp ) {
  QIntDictIterator<tr::menu::voce>it( menu );
  for ( ; it.current(); ++it ) {
    tr::menu::voce*v=it.current();
    // if (v->is==is && v->locazione==loc) return v;
    if ( v->disposizione==disp&&v->locazione==loc ) return v;
  }
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr::menu::voce*tr_menu::voce_padre( int is_padre ) {
  QIntDictIterator<tr::menu::voce>it( menu );
  for ( ; it.current(); ++it ) {
    tr::menu::voce*v=it.current();
    // if (v->is==is && v->locazione==loc) return v;
    if ( v->is_padre==is_padre ) return v;
  }
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr::menu::voce*tr_menu::voce( tr_menu::tr_key*k ) {
  if ( k!=NULL ) return voce( k->is );
  k=cd();
  if ( k!=NULL ) return voce( k->is );
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr::menu::voce*tr_menu::voce( int is ) { return menu.find( is ); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// verifica la visiblità dell'elemento nella pagina corrente
bool tr_menu::visibile( tr::menu::voce*voce ) {
  int low, high;

  tr_key*k=cd();

  if ( k&&voce->locazione==tr::menu::voce::loc_elementi&&k->is!=voce->is_padre ) return false;

  if ( !k&&voce->locazione==tr::menu::voce::loc_elementi ) return false;

  if ( voce->locazione==tr::menu::voce::loc_corolla ) {
    low=dimensione.corolla()*indici[corolla].pagina;
    high=low+dimensione.corolla()-1;
  }

  if ( voce->locazione==tr::menu::voce::loc_elementi ) {
    low=dimensione.elementi()*indici[elementi].pagina;
    high=low+dimensione.elementi()-1;
  }

  return ( low<=voce->disposizione&&voce->disposizione<=high )?true:false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
