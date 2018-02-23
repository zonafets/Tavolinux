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

#ifndef T_TAVOLI_HPP
  #define T_TAVOLI_HPP

  #include <qcanvas.h>

/* nomenclatura: imt ->  item frm  ->  form */
  #include "tr_config.hpp"
  #include "tavolo_bits.hpp"
  #include "tavolo_tipi.hpp"
  #include "tr.hpp"

extern tr_config config;

class frm_tavoli_view;



//--------------------------------------------------------------------------------------------- class  itm_sala
class itm_sala {
public:
  itm_sala() {
    id=-1;
    nome=QString::null;
  }

  int id;
  QString nome;

  itm_sala&operator+( itm_sala&s ) {
    id=s.id;
    nome=s.nome;
    return*this;
  };
};

//----------------------------------------------------------------------------------------- class  tr_tavolo
class itm_tavolo;



class tr_tavolo:public tr::enumeratori::tavolo_esteso {
public:

  tr_tavolo() {
    _dati.fill( '|', p_last-1 );
    aperto=false;
  }

  enum tr_genere {
    nessuno=-1, tavolo=0, arredo, pavimento, dinamico
  };

  enum {
    apparecchiato=( 1<<TAVOLO_OCCUPATO ), riservato=( 1<<TAVOLO_PRENOTATO ), bloccato=( 1<<TAVOLO_APERTO ), conto=( 1<<TAVOLO_CONTO ),
       conto_diviso=( 1<<TAVOLO_CONTO_DIVISO ), comanda_parcheggiata=( 1<<TAVOLO_COMANDA_PARCHEGGIATA ),
       conto_riesumato=( 1<<TAVOLO_CONTO_RIESUMATO ), proforma_stampata=( 1<<TAVOLO_PROFORMA_STAMPATA ),
       con_bis=( 1<<TAVOLO_CON_BIS )
  };

  void set( tr_posizioni p, QString nuovo_valore );

  QString operator[] ( tr_posizioni p ) {
    if ( !_dati.length() ) {
      cout<<( "dati tavolo assenti\n" );
      return "";
    }
    else
      return _dati.section( '|', p, p );
  }

  int id() {
    return operator[] ( p_id ).toInt();
    cout<<"p_id=" <<p_id<<endl;
  }

  int id_tavolata() {
    return operator[] ( p_id_tavolata ).toInt();
  }

  QString numero() {
    return operator[] ( p_nome ).stripWhiteSpace();
  }

  int stato() {
    return operator[] ( p_stato ).toInt();
  }

  int tipo() {
    return operator[] ( p_tipo ).toInt();
  }

  QString pixmap() {
    return operator[] ( p_pixmap );
  }

  int ordine() {
    return operator[] ( p_ordine ).toInt();
  }

  QString tipi_conto() {
    return operator[] ( p_tipi_conto );
  }

  static int bs( int _id ) {
    return _id%10;
  };

  int bs() {
    return bs( id() );
  }

  int capo_tavolata() {
    return id()-bs();
  };

  static tr_genere genere( QString pixmap );
  static tr_genere genere( int tipo );
  static bool bar( QString pixmap );
  static bool bar( int tipo );
  tr_genere genere();
  bool bar();
  bool tway();
  bool orologio();
  bool prenotato();

  bool nascosto;
  bool selezionato;

  int bis;
  int biss[10];

  void azzera_biss() {
    for ( uint i=0; i<=config.max_tavoli_bis; i++ ) biss[i]=0;
  };

  static QString nome( int id, QString numero );

  QString nome() {
    return nome( id(), numero() );
  };

  tr_tavolo&operator=( const tr_tavolo&t );

  void dati( QString dato ) {
    _dati=dato;
  }

  QString dati() {
    return _dati;
  }

  QStringList card;

  QStringList apri();
  bool chiudi( bool conto=false );
  bool aperto;

protected:
  QString _dati;

};



//----------------------------------------------------------------------------------------- class  itm_tavolo

class itm_tavolo:public QCanvasRectangle, public tr_tavolo {

public:
  itm_tavolo():QCanvasRectangle( NULL ), tr_tavolo() {
  };
  itm_tavolo( QCanvas* );

  int rtti() const;

  enum {
    RTTI_tavolo=984376, RTTI_gadget=984375
  };

  itm_tavolo&operator=( const itm_tavolo&t );

  QPixmap pixmap_from_tipo();

  void aggiorna() {
    canvas()->setChanged( dimensione() );
    canvas()->update();
  }

  bool is_table() {
    return ( ( genere()==tavolo )?true:false );
  };

  bool tavolata() {
    return ( ( id_tavolata()!=-1&&id_tavolata()==id() )?true:false );
  };

  bool aggregato() {
    return ( ( id_tavolata()!=-1&&id_tavolata()!=id() )?true:false );
  };

  QRect dimensione();

  void posiziona( bool mostra=true ) {
    move( operator[] ( p_x ).toInt(), operator[] ( p_y ).toInt() );
    setZ( ordine() );
    if ( mostra ) show();
  }

  static QPixmap componi_tavolo( QString pixmap, QString nome, const QColor&sfondo, bool selezionato, int stato );

  static bool lampeggio;

protected:
  void drawShape( QPainter& );

};



//--------------------------------------------------------------------------------------------------------- class  tr_tavoli_view
class tr_tavoli_view:public QCanvasView, public tr::enumeratori::tavolo_esteso {

  Q_OBJECT;



public:
  tr_tavoli_view( QWidget*parent=0, const char*name=0, WFlags f=0 );

  ~tr_tavoli_view() {
    cancella_items();
  }

  bool cancella_items();
  bool carica_tavoli( int sala_id, bool refresh=false );
  void tavoli_a_griglia( bool stato ); // true pone in griglia, false mantiene normale

  bool vista_a_griglia() {
    return _vista_a_griglia;
  };

  itm_sala&sala() {
    return _sala;
  }

  bool lampeggio;
  void lampeggio_update();

  void inserimento( bool stato ) {
    _inserimento=stato;
  };

  bool inserimento() {
    return _inserimento;
  };

  void tavolata( bool stato );

  bool tavolata() {
    return _tavolata;
  }

  QString tavoli_aggregati();
  QString tavoli_aggregati( int id_capo );

  int n_tavoli_aggregati() {
    return _n_tavoli_aggregati;
  }

  void sciogli_tavolata( int id );

  bool cancella();
  bool aggiungi( QPoint&e, QString pixmap );
  bool sposta( QPoint&e );

  itm_tavolo*seleziona( QMouseEvent* );
  itm_tavolo*seleziona( itm_tavolo*itm=NULL, int*id=NULL );

  itm_tavolo*seleziona( int id ) {
    return seleziona( NULL, &id );
  }

  itm_tavolo*tavolo( int id );

  itm_tavolo*selezionato() {
    return _selezionato;
  };

  QRect selezionato_dimensione() {
    return selezionato()->dimensione();
  };

  void pavimento( QString pixmap );

  QString pavimento() {
    return _pavimento;
  }


signals:
  void toccato( QMouseEvent* );

protected:
  virtual void contentsMousePressEvent( QMouseEvent*e ) {
    if ( tavolata() ) aggrega_tavolo( e );
    else
      seleziona( e );
    emit toccato( e );
    //qApp->notify ((QObject*)parentWidget(), e );
  };


private:
  itm_tavolo*_selezionato;
  itm_sala _sala;

  QString _pavimento;

  int ultimo_ordine( itm_tavolo::tr_genere g );
  int ultimo_bar();

  bool _vista_a_griglia;
  bool _inserimento;
  bool _tavolata;
  QString _tavoli_aggregati;
  int _n_tavoli_aggregati;

  itm_tavolo*quale_toccato( QPoint pos );
  itm_tavolo*aggrega_tavolo( QMouseEvent* );

  bool collisioni( QRect rect );
};

#endif
