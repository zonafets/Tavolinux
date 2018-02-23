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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "tr_comanda.hpp"
#include "cmdcodicigruppi.hpp"
#include "tr_tools.hpp"
#include "tr_srvcmd.hpp"
#include <qlayout.h>
#include <qpalette.h>
#include "tr_widget.hpp"

bool tr_comanda::_conto_aperto;
extern tr_tools tools;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_comanda::tr_comanda( QWidget*parent, const char*name ):tr_widget( parent, name, Qt::WRepaintNoErase ) {
  setPaletteBackgroundColor( parent->paletteBackgroundColor() );
  // setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ));

  setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding ) );

  QGridLayout*layout;
  NEW( layout, QGridLayout( this, 2, 3, 0, 0, "comanda_layout" ) );

  NEW( tabella, tr_table( this, "comandatabella" ) );
  tabella->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding ) );
  tabella->setPaletteForegroundColor( QColor( 0, 0, 127 ) );
  tabella->setPaletteBackgroundColor( QColor( 174, 228, 255 ) );
  QFont comanda_font( tabella->font() );
  comanda_font.setFamily( config.comanda_font.section('|',0,0) );
  comanda_font.setBold( TRUE );
  tabella->setFont( comanda_font );
  tabella->verticalScrollBar()->setSteps( config.comanda_altezza_riga, config.comanda_altezza_riga );
  tabella->setSorting( 0 );
  tabella->setFrameShape( QLabel::Panel );
  tabella->setFrameShadow( QLabel::Sunken );
  tabella->setLineWidth( 1 );
  tabella->altezza_riga=config.comanda_altezza_riga;

  connect( tabella, SIGNAL( pressed( int, int, int, const QPoint& ) ),
     this, SLOT( tabella_pressed( int, int, int, const QPoint& ) ) );
  //connect( tabella, SIGNAL(clicked(int,int,int,const QPoint&)), this, SLOT(tabella_pressed(int,int,int,const QPoint&)));
  connect( tabella, SIGNAL( doubleClicked( int, int, int, const QPoint& ) ),
     this, SLOT( tabella_pressed( int, int, int, const QPoint& ) ) );
  //connect( tabella, SIGNAL(currentChanged(int,int)), this, SLOT(tabella_currentChanged(int,int)));
  //connect( tabella, SIGNAL(selectionChanged()), this, SLOT(tabella_selectionChanged()));
  connect( tabella, SIGNAL( valueChanged( int, int ) ),
     this, SLOT( tabella_valueChanged( int, int ) ) );

  layout->addMultiCellWidget( tabella, 0, 0, 0, 1 );

  QVBoxLayout*layout_sb;
  layout_sb=new QVBoxLayout( 0, 0, 0, "layout_sb" );

  // crea controlli di barra a lato, usando macro speciale
  TR_LABEL( su );
  su->ripetizione( true );
  su->setMinMax( config.elenchi_button_size, config.elenchi_button_size );
  //su->noBackground();
  CONNECT( su );

  TR_WIDGET( spazio );

  tr_label*liv;

  TR_LABEL( liv0 );
  liv0->setMinMax( config.elenchi_button_size, config.comanda_altezza_riga );
  liv=liv0;
  CONNECT( liv );

  TR_LABEL( liv1 );
  liv1->setMinMax( config.elenchi_button_size, config.comanda_altezza_riga );
  liv=liv1;
  CONNECT( liv );

  TR_WIDGET( spazio_1 );

  TR_LABEL( piu );
  // piu->setMinMax( config.elenchi_button_size, config.elenchi_button_size );
  CONNECT( piu );

  TR_LABEL( meno );
  // meno->setMinMax( config.elenchi_button_size, config.elenchi_button_size );
  CONNECT( meno );

  TR_WIDGET( spazio_2 );

  TR_LABEL( liv2 );
  liv2->setMinMax( config.elenchi_button_size, config.comanda_altezza_riga );
  liv=liv2;
  CONNECT( liv );

  TR_LABEL( liv3 );
  liv3->setMinMax( config.elenchi_button_size, config.comanda_altezza_riga );
  liv=liv3;
  CONNECT( liv );

  tr_label*livs[]= {liv0,liv1,liv2,liv3};
  TROWI( livs ) {
    livs[i]->setPaletteForegroundColor( config.comanda_livelli_colore );
    livs[i]->bold();
    if (config.comanda_livelli_nomi.section('|',i,i).isEmpty())
       livs[i]->setText( QSN( i ), config.comanda_font_size, true );
    else
       livs[i]->setText( tr(config.comanda_livelli_nomi.section('|',i,i)) );
  }

  TR_WIDGET( spazio_3 );

  TR_LABEL( giu );
  giu->setMinMax( config.elenchi_button_size, config.elenchi_button_size );
  giu->ripetizione( true );
  //giu->noBackground();
  CONNECT( giu );

  // imposta ile icone dei pulsanti

  tr_label*wp[]= { su, piu, meno, giu };
  QString pixmap[]= {"su", "piu", "meno","giu"};
  TROW( wp, i ) wp[i]->setPixmap( PIXMAP( "az_" +pixmap[i]+".png" ) );

  // mette nel layout della barra

  QWidget*w_barra[]= {
      su, spazio,
      liv0,liv1,
      spazio_1,
      piu, meno,
      spazio_2,
      liv2,liv3,
      spazio_3, giu };

  TROW( w_barra, i ) {
    int strech_factor=0;
    int align=Qt::AlignTop;
    if ( w_barra[i]==spazio_1||w_barra[i]==spazio_2 ) strech_factor=2;
    if ( w_barra[i]==piu||w_barra[i]==meno ) align=Qt::AlignCenter;
    if ( i>6 ) align=Qt::AlignBottom;
    layout_sb->addWidget( w_barra[i], strech_factor, align );
  }

  layout->addMultiCellLayout( layout_sb, 0, 1, 2, 2 );

  tr_widget*spazi[]= {spazio,spazio_1,spazio_2,spazio_3};
  TROWI( spazi ) {
    tr_widget*spazio=spazi[i];
    spazio->setMinimumWidth( config.elenchi_button_size );
    spazio->setMinimumHeight( 0 );
    spazio->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ) );
    // spazio->setPaletteBackgroundColor(QColor(200,0,0));
  }

  /* piu->setPaletteBackgroundColor(QColor(10,50,50)); meno->setPaletteBackgroundColor(QColor(10,50,50)); */

  TR_LABEL( tot );
  tot->setMinMax( config.comanda_totale_larghezza_min, config.comanda_totale_altezza_riga, 32767, config.comanda_totale_altezza_riga );
  tot->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ) );
  tot->setPaletteForegroundColor( config.comanda_totale_colore_fg );
  tot->bold();
  tot->setText( tr("TOT") );

  layout->addWidget( tot, 1, 0 );

  TR_LABEL( totale );
  totale->setMinMax( 0, config.comanda_totale_altezza_riga, 32767, config.comanda_totale_altezza_riga);
  totale->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
  totale->setPaletteForegroundColor( QColor( 0, 0, 127 ) );
  totale->setPaletteBackgroundColor( QColor( 174, 228, 255 ) );
  totale->setAlignment( int( QLabel::AlignRight|QLabel::AlignVCenter ) );
  totale->label->setMargin( 2 );
  connect( totale, SIGNAL( clicked() ), SIGNAL( totale_clicked() ) );

  layout->addWidget( totale, 1, 1 );

  // scurisce la palette dei componenti per il disabilitato

  QWidget*w[]= {
      tabella, su, giu, spazio, piu, meno, spazio_1, tot,
      totale, totale->label,spazio_2,spazio_3,
      liv0,liv1,liv2,liv3,
      NULL };
  palette_dark( w );

  TROWI( livs )livs[i]->hide();

  modificato( false );
  _registrata=false;
  modalita( m_comanda );
  _conto_aperto=false;

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_comanda::darkable( bool d ) {
  _darkable=d;
  // tr_widget* w[] = { su, spazio, piu, meno, spazio_1, giu, tot, totale };
  tr_widget*w[]= { this,
      su, piu, meno, giu, liv0, liv1, liv2, liv3, tot, totale,
      spazio, spazio_1, spazio_2, spazio_3 };
  for ( uint i=0; i<COUNT( w ); i++ ) {
    w[i]->darkable( d );
    palette_dark( w[i] );
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_comanda::setMinMax( int mw, int mh, int MW, int MH ) {
  if ( MW==-1 ) MW=mw;
  if ( MH==-1 ) MH=mh;
  tr_widget::setMinMax( mw, mh, MW, MH );
  tabella->setMinMax(
    mw-config.elenchi_button_size, mh-config.comanda_totale_altezza_riga,
    MW-config.elenchi_button_size, MH-config.comanda_totale_altezza_riga );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_comanda::cambia_sfondo( const QColor&c ) {
  QWidget*w[]= { su, giu, spazio, piu, meno, spazio_1,spazio_2,spazio_3, tot, NULL };
  int i=0;
  while ( w[i]!=NULL ) w[i++]->setPaletteBackgroundColor( c );
  //palette_dark ( w );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_comanda::cambia_sfondo( const QColor&colore, const QColor&coloretabellatotale ) {
  tabella->setPaletteBackgroundColor( coloretabellatotale );
  palette_dark( tabella );
  totale->setPaletteBackgroundColor( coloretabellatotale );
  palette_dark_background( totale );
  cambia_sfondo( colore );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define UPDATES

bool tr_comanda::aggiorna( bool tieni_selezione ) {
  bool r=false;
  tr_selezione*riga=selezionato();
  switch ( _modalita ) {
    case m_comanda:
      dati.carica( tr::ordine::t_ordine );
    break;
    case m_conto:
      dati.carica( tr::ordine::t_conto );
    break;
    case m_separata:
      tieni_selezione=false;
      if ( !_conto_aperto ) {
        if ( righe() ) tabella->elimina_righe();
        return false;
      }
      dati.carica( tr::ordine::t_separata );
    break;
    case m_pagamento:
      dati.carica( tr::ordine::t_pagamenti );
    break;
    case m_conti:
      dati.carica( tr::ordine::t_conti );
    break;
  }
  tabella->clearSelection();
  tabella->aggiorna( dati.elenco, false );
  if ( _modalita!=m_pagamento )
    totale->setText( dati.totale );
  else {
    if ( righe()>0 ) {
      if ( dati.totale.toDouble()!=0 )
        totale->setText( dati.resto+"/" +dati.totale );
      else
        totale->setText( dati.resto );
    }
    else
      totale->setText( "" );
  }
  if ( riga&&tieni_selezione ) seleziona( riga->riga );
  else visualizza_ultimo();
  return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_comanda::su_clicked() {
  scroll_su();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_comanda::giu_clicked() {
  scroll_giu();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_comanda::scroll_su() {
  tabella->verticalScrollBar()->setValue( tabella->verticalScrollBar()->value()-tabella->altezza_riga );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_comanda::scroll_giu() {
  tabella->verticalScrollBar()->setValue( tabella->verticalScrollBar()->value()+tabella->altezza_riga );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::event( QEvent*e ) {
  bool risult;
  static int timeId=-1;
  static QWidget*ultimo_premuto=NULL;
  bool finto_mouse_press=false;

  if ( e->type()==QEvent::Timer ) {
    QTimerEvent*t=( QTimerEvent* ) e;
    if ( t->timerId()==timeId ) {
      finto_mouse_press=true;
      risult=true;
    }
  }
  if ( finto_mouse_press&&ultimo_premuto&&ultimo_premuto->isEnabled() ) {
    if ( ultimo_premuto==su ) {
      scroll_su();
      risult=true;
    }
    if ( ultimo_premuto==giu ) {
      scroll_giu();
      risult=true;
    }

  }
  else {
    if ( e->type()==QEvent::MouseButtonPress||e->type()==QEvent::MouseButtonDblClick ) {
      QMouseEvent*me=( QMouseEvent* ) e;
      QWidget*w=childAt( me->pos() );
      if ( w&&w->isEnabled() ) {
        bool t=false;
        if ( w==su ) {
          scroll_su();;
          t=true;
          risult=true;
        }
        if ( w==giu ) {
          scroll_giu();;
          t=true;
          risult=true;
        }
        if ( w&&t ) {
          timeId=startTimer( config.scroll_ms );
          ultimo_premuto=w;
        }
      }
    }
  }
  if ( e->type()==QEvent::MouseButtonRelease&&timeId>-1 ) {
    killTimer( timeId );
    timeId=-1;
    ultimo_premuto=NULL;
    risult=true;
  }
  // qDebug("evento in frm_main : " + QVariant(e->type()).toString());

  if ( risult ) {
    QWidget::event( e );
    return true;
  }
  else return QWidget::event( e );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cambia la qta della riga corrente (selezionasto()) a qta
bool tr_comanda::cambia_qta( double qta ) {
  if ( !selezionato() ) BEEPERF;

  double valore=selezionato()->qta.toDouble();
  bool portata=selezionato()->portata;
  int id_riga=selezionato()->id_riga;
  int riga=selezionato()->riga;
  bool peso=selezionato()->stato.gpeso();
  QString desc=selezionato()->descrizione;
  TRTokens token_usato;

  bool rsp;

  if ( portata ) {
    if ( peso ) token_usato=tok_modifica_peso;
    else
      token_usato=tok_modifica_quantita;
  }
  else {
    if ( peso ) token_usato=tok_modifica_peso_trasmesso;
    else {
      if ( qta>valore ) {
        tools.msgbox( this, tr( "Per le comande gia' trasmesse<br>e' possibile solo<br>stornare la quantita'" ) );
        return false;
      }
      token_usato=tok_storna;
    }
  }

  rsp=CMD( token_usato, QSN( id_riga ), QSN( qta ) );

  if ( rsp ) {

    // Aggiorna il dato di transito per il display

    _ultimo_dato.aggiorna( token_usato, QSN( qta ), desc );

    // Prosegui

    selezionato()->qta=QSN( qta );
    modificato( true );
    int y=tabella->verticalScrollBar()->value();
    aggiorna();
    tabella->verticalScrollBar()->setValue( y );
    emit qta_changed( riga );
    // seleziona( riga );
    return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool tr_comanda::cambia_qta( int row ) {
  //  cambio   QTA
  const int col=0;

  if ( tabella->text( row, col )!="" ) {

    tr_selezione*riga=selezionato();
    if ( !riga||riga->riga!=row ) {
      seleziona( row, 2 );
      riga=selezionato();
    }

    // esclude articoli che non hanno qta
    if ( riga->stato.gapm() ) BEEPERF;
    if ( riga->stato.gavf() ) BEEPERF;
    if ( riga->stato.gapm() ) BEEPERF;
    if ( riga->stato.gam() ) BEEPERF;


    QString valore;

    if ( riga->stato.gpeso() ) {
      bool rsp;
      if ( riga->portata ) rsp=::cmd( tok_pesa_articolo, QSN( riga->id_riga ) );
      else
        rsp=::cmd( tok_pesa_articolo_trasmesso, QSN( riga->id_riga ) );
      if ( rsp ) {
        valore=srv->rcv();
      }
    }
    else
      valore=riga->qta;

    QString validatore=QString::null;
    int p=riga->qta.find( '#' );

    QString titolo;
    if ( riga->stato.gpeso() ) {
      validatore="real3";
      titolo=tr( "PESO" );
    }
    else {
      validatore="int";
      titolo=tr( "QUANTITA'" );
    }

    valore=tools.calcolatrice( this, tabella->cellGeometry( row, col ), titolo, valore, validatore, riga->stato.gpeso() );

    valore.replace( ',', '.' );
    if ( !valore.isNull() ) cambia_qta( valore.toDouble() );
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool tr_comanda::cambia_prz( int row ) {

  const int col=2;

  if ( tabella->text( row, col )!="" ) {

    tr_selezione*riga=selezionato();
    if ( !riga||riga->riga!=row ) {
      seleziona( row, 2 );
      riga=selezionato();
    }

    // esclude articoli che non hanno qta
    if ( riga->stato.gapm() ) BEEPERF;
    if ( riga->stato.gavf() ) BEEPERF;
    if ( riga->stato.gam() ) BEEPERF;

    QString valore=riga->tot_riga;
    bool portata=riga->portata;
    int id_riga=riga->id_riga;
    QString desc=riga->descrizione;

    QString validatore="real2";
    int p=riga->qta.find( '#' );

    valore=tools.calcolatrice( this, tabella->cellGeometry( row, col ), tr( "Prezzo" ),
       valore, validatore, config.calcolatrice_dec_fisso );

    if ( valore.isNull() ) return false;

    seleziona( row, 2 );

    bool rsp;
    if ( modalita()==m_separata )
      rsp=CMD( tok_modifica_prezzo_separato, QSN( id_riga ), valore );
    else {
      if ( portata ) rsp=CMD( tok_modifica_prezzo, QSN( id_riga ), valore );
      else
        rsp=CMD( tok_modifica_prezzo_trasmesso, QSN( id_riga ), valore );
    }
    _selezionato.prezzo=valore.toDouble();

    if ( rsp ) {

      // Aggiorna il dato di transito per il display

      _ultimo_dato.aggiorna( tok_modifica_prezzo, QString::null, desc, valore );

      // se il numero di decimali specificato supera quello letto, allarga le colonne

      modificato( true );
      int y=tabella->verticalScrollBar()->value();
      aggiorna();
      tabella->verticalScrollBar()->setValue( y );
      emit prz_changed( row );
      return true;
    }
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_comanda::tabella_valueChanged( int row, int col)
{
  enum {tcol_date,tcol_rif,tcol_vat,tcol_total};
  QString dato=tabella->text(row,col);
  if (modalita()==m_ffm) {
    QDate dd;
    bool ok;
    double d;
    switch (col) {
      case tcol_date:
      if (!tools.data_valida(dato,&dd)) tools.msgbox(TR("Data non valida"));
      break;
      case tcol_vat: {
        QStringList elenco=QStringList::split(';',config.ffm_vats);
        bool trovato=false;
        TROWSLI(elenco) if (elenco[i].section('|',1,1)==dato) {trovato=true;break;}
        if (!trovato) tools.msgbox(TR("IVA non valida"));
      }
      break;
      case tcol_total:
      d=dato.toDouble(&ok);
      if (!ok) tools.msgbox(TR("Totale non valido"));
    } // switch
  } // if ffm

} // tabella_valueChanged

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tr_comanda::tabella_pressed( int row, int col, int, const QPoint&p ) {

  if (modalita()==m_ffm) {
    enum {tcol_date,tcol_rif,tcol_vat,tcol_total};
    QString dato="";
    if (col==tcol_date) {
      QDate d;
      if (tools.calendario(tools.actWin(),&d)) dato=tools.data(d);
    }
    if (col==tcol_vat) {
      tr_tools::tr_elenco item;
      QStringList elenco=QStringList::split(';',config.ffm_vats);
      item=tools.elenco(elenco,TR("IVA"),"@id|$desc");
      if (!item.id.isEmpty()) dato=item.descrizione;
    }
    if (!dato.isEmpty()) tabella->setText(row,col,dato);
    tabella->setCurrentCell( row, col );
    return;
  }

  if ( tabella->rowAt( p.y() )==-1 ) {
    tabella->setCurrentCell( -1, -1 );
    tabella->clearSelection( true );
    emit articolo_selezionato( -1 );
    return;
  }

  /* un click in quest' area pu�significare: 1. cambiare il valore della qta (colonna 0)
  2. cambiare il valore del prezzo (colonna2) 3. operare con articolo (colonna 1) a. cancellare: abilita cestino
  i) abilita annulla dopo cancellazione b. spostamento x separa conto: abilita art_sep1 i) abilita annulla dopo spostamento */

  // un testo vuoto significa cella non operativa o insignificante
  //if ( (QString  testo_originale = tabella->text( row, col ))  ==  "" ) return;
  QString testo_originale=tabella->text( row, col );
  if ( testo_originale=="" ) return;

  _selezionato=selezione( row );

  switch ( tipo_colonna(col) ) {
    case tcol_qta:
      emit qta_clicked( row );
    break;

    case tcol_desc:
      seleziona( row );
      emit articolo_selezionato( row );
    break;

    case tcol_prz:
      emit prz_clicked( row );
    break;

  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tr_comanda::tr_selezione*tr_comanda::selezionato() {
  if ( tabella->currentRow()>=0&&tabella->numSelections()>0 ) {
    _selezionato=selezione( tabella->currentRow() );
    return&_selezionato;
  }
  else
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// restituisce i dati della riga ma non la imposta
tr_comanda::tr_selezione tr_comanda::selezione( int row ) {
  tr_comanda::tr_selezione selezionato;
  selezionato.riga=row;
  selezionato.colonna=1;
  selezionato.qta=tabella->text( row, com_qta );
  if ( tabella->text( row, com_qta )=="+" ) selezionato.qta=1;
  if ( tabella->text( row, com_qta )=="-" ) selezionato.qta=-1;
  selezionato.descrizione=tabella->text( row, com_descrizione ).simplifyWhiteSpace();
  selezionato.tot_riga=tabella->text( row, com_tot_riga );
  selezionato.livello=tabella->text( row, com_livello ).toInt();
  selezionato.id_riga=tabella->text( row, com_id_riga ).toInt();
  selezionato.id_istanza=tabella->text( row, com_id_istanza ).toInt();
  selezionato.id_articolo=tabella->text( row, com_id_art ).toInt();
  selezionato.prezzo=tabella->text( row, com_prezzo ).toDouble();
  selezionato.stato=tabella->text( row, com_stato ).toInt();
  ( tabella->text( row, com_offerto )=="1" )?selezionato.offerto=true:selezionato.offerto=false;
  ( tabella->text( row, com_portata )=="1" )?selezionato.portata=true:selezionato.portata=false;
  return selezionato;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int tr_comanda::riga_da_id_riga( int id_riga ) {
  int i=0;
  while ( i<righe() ) {
    if ( tabella->text( i, com_id_riga ).toInt()==id_riga ) return i;
    i++;
  }
  return -1;
}

void tr_comanda::seleziona() {
  tabella->setCurrentCell( -1, -1 );
}

void tr_comanda::seleziona( int row ) {
  if ( tabella->isRowSelected( row ) ) tabella->clearSelection();
  else {
    tabella->setCurrentCell( row, -1 );
    _selezionato=selezione( row );
  }
}

void tr_comanda::seleziona( int row, int ) {
  seleziona( row );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::cancella() {

  bool rsp;
  if ( selezionato()->portata ) {
    rsp=CMD( tok_rimuovi_articolo_comanda, QSN( selezionato()->id_riga ) );
  }
  else {
    rsp=CMD( tok_storna, QSN( selezionato()->id_riga ), QSN( 0 ) );
  }

  if ( !rsp ) return false;

  modificato( true );

  seleziona(); // ormai scomparso
  aggiorna( true );

  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::offri() {
  bool rsp;
  rsp=CMD( tok_offri_articolo, QSN( selezionato()->id_riga ), selezionato()->offerto?"0" :"1" );

  if ( !rsp ) return false;

  modificato( true );

  aggiorna();

  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::annulla() {
  bool r;

  switch ( modalita() ) {

    case m_comanda:
    case m_separata:
    case m_pagamento:
    case m_conto:
      annulla_conto();
      totale_comanda( "0.00" );

    default:
      return false;
  }

  return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::modifica_descrizione( QString nuova ) {
  if ( CMD( tok_modifica_descrizione_trasmessa, QSN( selezionato()->id_riga ), nuova ) ) {
    modificato( true );
    return true;
  }
  return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// cerca a ritroso dall'ultimoarticolo, escludendo le varianti
// articolo
// --variante
// ---variante
tr_comanda::tr_selezione tr_comanda::ultimo_articolo() {
  const uint gav=GRUPPO_ARTICOLO_VARIANTE;

  tr_comanda::tr_selezione selezionato;
  int row=tabella->numRows()-1;
  int stato=tabella->text( row, com_stato ).toInt();
  while ( ( stato&gav )==gav&&row>0 ) {
    row--;
    stato=tabella->text( row, com_stato ).toInt();
  }

  selezionato.riga=row;
  selezionato.colonna=1;
  selezionato.qta=tabella->text( row, com_qta );
  if ( tabella->text( row, com_qta )=="+" ) selezionato.qta=1;
  if ( tabella->text( row, com_qta )=="-" ) selezionato.qta=-1;
  selezionato.descrizione=tabella->text( row, com_descrizione ).simplifyWhiteSpace();
  selezionato.tot_riga=tabella->text( row, com_tot_riga );
  selezionato.livello=tabella->text( row, com_livello ).toInt();
  selezionato.id_riga=tabella->text( row, com_id_riga ).toInt();
  selezionato.id_istanza=tabella->text( row, com_id_istanza ).toInt();
  selezionato.id_articolo=tabella->text( row, com_id_art ).toInt();
  selezionato.prezzo=tabella->text( row, com_prezzo ).toDouble();
  selezionato.stato=tabella->text( row, com_stato ).toInt();
  ( tabella->text( row, com_offerto )=="1" )?selezionato.offerto=true:selezionato.offerto=false;
  ( tabella->text( row, com_portata )=="1" )?selezionato.portata=true:selezionato.portata=false;
  return selezionato;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::acquisisci_portata( int id_riga, double qta ) {
  bool rsp=false;

  if ( modalita()==m_comanda||modalita()==m_conti||modalita()==m_conto )
    rsp=CMD( tok_sposta_portata_per_separa, QSN( da_conto ), QSN( id_riga ), QSN( qta ) );

  if ( modalita()==m_separata ) {
    if ( !conto_aperto() ) tools.msgbox( tr( "Conto non aperto durante la separazione" ) );
    else
      rsp=CMD( tok_sposta_portata_per_separa, QSN( a_conto ), QSN( id_riga ), QSN( qta ) );
  }

  if ( rsp ) modificato( true );
  return rsp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::inserisci_pagamento( int id_istanza, QString prz ) {

  bool r=::cmd( tok_inserisci_articolo_comanda, QSN( 1 ), QSN( id_istanza ), LEAVE, prz, LEAVE, LEAVE );
  if ( r ) {

    // aggiorna il dato di transito

    denu_inserisci_articolo_comanda;
    QStringList d=QStringList::split( CMDLANG_SEPARATOR, srv->rcv() );
    _ultimo_dato.aggiorna( tok_inserisci_articolo_comanda, "1", d[p_descrizione], d[p_prezzo_calcolato] );
    srv->eop();

    // procedi come prima

    modificato( true );
    aggiorna();
    tabella->ensureCellVisible( tabella->numRows()-1, 0 );
  }
  else {
    // chiudi la comunicazione
    srv->eop();
  }

  // esci

  return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::inserisci_conto( double prezzo ) {
  bool r=CMD( tok_inserisci_articolo_comanda, QSN( 1 ), QSN( tr::gruppi::conto_diviso ), "", QSN( prezzo ), LEAVE, LEAVE );
  if ( r ) modificato( true );
  return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::inserisci_articolo( int is ) {
  tr::tipo_articolo stato=0;
  switch ( is ) {
    case tr::gruppi::insieme_a:
    case tr::gruppi::segue:
      stato( GRUPPO_ARTICOLO_CONSOLIDANTE );

    break;
  }
  return inserisci_articolo( is, 0, stato, true );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::inserisci_articolo(
  int is, int /* is_padre */, tr::tipo_articolo stato, bool var_metti,
   double*p_qta, QString prz, QString descrizione ) {

     /* lo stato della comanda può essere: a. nessun selezionato b. un elem selezionato Le azioni possono essere:
     1. accodamento a fine comanda 2. affiliamento 3. inserimento L'accodamento avviene:
     i) se nessun elemento è selezionato e loggetto da accodare non è una variante L'affiliamento avviene:
     i) se l'ultimo elem è un gap e l'accodante è un gav L'inserimento avviene se: i) c'è un elemento selezionato */

     bool inserimento=false;
     int id_riga;
     bool affiliato=false;
     int id_riga_padre;

     tr_comanda::tr_selezione*s=selezionato();
     tr_comanda::tr_selezione*su=&ultimo_articolo();

     double qta_default=1;
     if ( p_qta!=NULL ) qta_default=*p_qta;
     double qta;
     if ( var_metti||!stato.gav() ) qta=qta_default;
     else qta=-qta_default;
     if ( stato.gac()||stato.gam() ) qta=0;

     if ( s ) {

       if ( stato.gav()&&s->stato.gap() ) { // se affiliazione
         affiliato=true;
         id_riga_padre=s->id_riga;
       }
       else // se accodamento
       {
         int riga=s->riga; // senza -1 perche' parte gia' da zero
         if ( riga>0 ) {
           tr_comanda::tr_selezione sp=selezione( riga-1 );
           id_riga=sp.id_riga;
           while ( sp.livello&&riga>0 ) {
             riga--;
             sp=selezione( riga );
           }
           if ( riga ) id_riga=sp.id_riga;
         }
         if ( !riga ) id_riga=-1;
         inserimento=true;
       }
     }
     else // se nessuno seleziona
     {
       if ( stato.gav()&&su&&su->stato.gap()&&!su->stato.gac() ) { // se affiliazione
         affiliato=true;
         id_riga_padre=su->id_riga;
       }
       else if ( !stato.gap()&&!stato.gac()&&!stato.gam() ) {
         BEEPERF;
       }
     }

     /* tr_comando inserisci_articolo_comanda ( double quantita, int codice_istanza, QString descrizione=QString::null,
     bool prezzo_imposto=false, double prezzo=0, bool affiliato=false, int id_riga_padre=0,
     bool posizione_forzata=false, int posizione=0 ); */
     bool r=::cmd( tok_inserisci_articolo_comanda, QSN( qta ), QSN( is ),
        descrizione, prz, affiliato?QSN( id_riga_padre ):LEAVE, inserimento?QSN( id_riga ):LEAVE );
     if ( r ) {

       // Leggi i dati dell'ultimo inserimento e ponili nella struttura
       // di passaggio

       denu_inserisci_articolo_comanda;
       QStringList d=QStringList::split( CMDLANG_SEPARATOR, srv->rcv() );
       _ultimo_dato.aggiorna( tok_inserisci_articolo_comanda, QSN( qta ), d[p_descrizione], d[p_prezzo_calcolato] );

       // Prosegui come di consueto

       modificato( true );
       int vp;
       if ( affiliato||inserimento ) vp=tabella->verticalScrollBar()->value();
       if ( !affiliato ) seleziona(); // se affiliato non deseleziona
       aggiorna();
       if ( affiliato||inserimento )
         tabella->verticalScrollBar()->setValue( vp );
       else
         visualizza_ultimo();
     }
     else
       BEEP;

     // Comunque sia svuota la coda

     srv->eop();

     return r;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::inserisci_articolo( QString barcode, double qta ) {
  bool esito=true;
  if ( ::cmd( tok_inserisci_articolo_barcode, barcode, QSN( qta ) ) ) {

    // Aggiorna il dato di transito

    denu_inserisci_articolo_comanda;
    QStringList d=QStringList::split( CMDLANG_SEPARATOR, srv->rcv() );
    _ultimo_dato.aggiorna( tok_inserisci_articolo_barcode, QSN( qta ), d[p_descrizione], d[p_prezzo_calcolato] );

    // prosegui

    modificato( true );
    aggiorna();
  }
  else
    esito=false;

  // Svuota la coda dei comandi

  srv->eop();

  return esito;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::registra() {
  if ( modificato() ) {
    _registrata=CMD( tok_registra_comanda );
    if ( _registrata ) {
      modificato( false );
    }
    return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::stampa() {
  bool r=CMD( tok_stampa_comanda );
  // CMD(tok_nuova_comanda);
  return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::nuova() {
  return CMD( tok_nuova_comanda );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_comanda::modalita( t_modalita m ) {
  _modalita=m;
  switch ( m ) {
    case m_comanda:
    case m_conto:
      tabella->altezza_riga=config.comanda_altezza_riga;
      piu->show();
      meno->show();
      tot->setText( "TOT" );
    break;
    case m_ffm: {
      tabella->as_original=true;
      tabella->setMaximumSize(32767,config.finestra_altezza-220);
      tabella->insertColumns( 0, 4 );
      tabella->insertRows( 0,999 );
      tabella->altezza_riga=config.altezza_riga;
      tabella->setTopMargin( tabella->altezza_riga );
      tabella->setLeftMargin( tabella->altezza_riga*1.5 );
      QHeader *h=tabella->verticalHeader();
      h->show();
      h->setClickEnabled(false);
      h->setResizeEnabled(false);

      h=tabella->horizontalHeader();
      h->show();
      h->setLabel(0,TR("SCONTRINO"),160);
      h->setLabel(1,TR("RIFERIMENTO"),160);
      h->setLabel(2,TR("IVA"),80);
      h->setLabel(3,TR("TOTALE"),120);
      // h->setPaletteBackgroundColor( QColor( 208, 210, 255 ).dark( 120 ) );
      h->setClickEnabled(false);
      h->setResizeEnabled(false);
      // tabella->setFocusStyle( QTable::SpreadSheet );
      // tabella->setSelectionMode( QTable::Single );
      for (int i=0;i<tabella->numRows();i++) tabella->setRowHeight(i,tabella->altezza_riga);
      // questo stricca for (int i=0;i<tabella->numCols();i++) tabella->adjustColumn(i);
      // tabella->updateHeaderStates();
      tabella->setCurrentCell(0,0);
    }
    case m_separata:
      tabella->altezza_riga=config.comanda_altezza_riga;
      piu->hide();
      meno->hide();
      tot->setText( "TOT" );
    break;
    case m_pagamento:
      tabella->altezza_riga=config.comanda_altezza_riga;
      piu->hide();
      meno->hide();
      tot->setText( "RES" );
    break;
    case m_conti:
      tabella->altezza_riga=config.comanda_altezza_riga;
      piu->hide();
      meno->hide();
      tot->setText( "TOT" );
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_comanda::meno_clicked() {
  if ( !selezionato() ) BEEPER;
  if ( selezionato()->stato.gavf() ) BEEPER;
  if ( selezionato()->stato.gpeso() ) BEEPER;
  double qta=selezionato()->qta.toDouble();
  if ( qta==1.0 ) {
    if ( selezionato()->stato.gav() ) qta=-1.0;
    else {
      QString msg=tr( "Vuoi cancellare l'articolo " )+selezionato()->descrizione+" ?";
      if ( tools.msgbox( msg, tr( "Attenzione!!!" ), SINO )==SI ) qta=0;
    }
  }
  else
    qta-=1.0;
  cambia_qta( qta );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_comanda::piu_clicked() {
  if ( !selezionato() ) BEEPER;
  if ( selezionato()->stato.gavf() ) BEEPER;
  if ( selezionato()->stato.gpeso() ) BEEPER;
  double qta=selezionato()->qta.toDouble();
  if ( qta==-1.0 ) qta=1.0;
  else qta+=1.0;
  cambia_qta( qta );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::apri_conto( tr_comanda::conti tipo ) {
  _conto_aperto=CMD( tok_crea_conto, QSN( tipo ) );
  return _conto_aperto;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::chiudi_conto( tr_comanda::conti_chiusi tipo ) {
  if ( !_conto_aperto ) return false;
  if ( CMD( tok_chiudi_conto, QSN( tipo ) ) ) {
    _conto_aperto=false;
    return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool tr_comanda::annulla_conto() {
  if ( !_conto_aperto ) return false;
  if ( CMD( tok_annulla_conto ) ) {
    _conto_aperto=false;
    return true;
  }
  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int tr_comanda::righe_comanda() {
  int c=0;
  for ( int i=0; i<righe(); i++ )
    if ( tabella->text( i, com_portata )!="1" ) c++;
  return c;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int tr_comanda::righe_portate() {
  int c=0;
  for ( int i=0; i<righe(); i++ )
    if ( tabella->text( i, com_portata )=="1" ) c++;
  return c;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double tr_comanda::totale_comanda() {
  return totale->text().toDouble();
};

double tr_comanda::residuo() {
  return ( modalita()!=m_pagamento?0:dati.totale.toDouble() );
}

void tr_comanda::totale_comanda( QString t ) {
  totale->setText( t );
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_comanda::liv_clicked() {
  QString liv;
  if ( sender()==liv0 ) liv="imposto livello 0";
  if ( sender()==liv1 ) liv="imposto livello 1";
  if ( sender()==liv2 ) liv="imposto livello 2";
  if ( sender()==liv3 ) liv="imposto livello 3";
  tools.msgbox( liv );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void tr_comanda::show_levels() {
  if ( config.multi_comanda ) {
    tr_label*livs[]= {liv0,liv1,liv2,liv3};
    TROWI( livs )livs[i]->show();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tr_comanda::tr_tipo_col tr_comanda::tipo_colonna(int n_col)
{
  if (modalita()!=m_ffm) {
    if (n_col==0) return tcol_qta;
    if (n_col==1) return tcol_desc;
    if (n_col==2) return tcol_prz;
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
