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

#ifndef TR_BARRA_STATO_H
  #define TR_BARRA_STATO_H

  #include <qstring.h>
  #include <qwidget.h>
  #include <qlabel.h>
  #include <qlayout.h>
  #include "tr_label.hpp"
  #include "tr_config.hpp"

// predefinizione per scopi futuri
class tr_barra_titolo:public tr_label {
public:
  tr_barra_titolo( QWidget*parent=0, const char*name=0, WFlags fl=MYFLAGS );

  ~tr_barra_titolo() {
  }

  void setText( QString t, bool solo_t_senza_piva=false );

  QString text() {
    return tr_label::text();
  }
};



class tr_barra_stato:public tr_widget {
  Q_OBJECT;



public:
  tr_barra_stato( QWidget*parent=0, const char*name=0, WFlags fl=MYFLAGS );
  ~tr_barra_stato();

  enum tr_tipo {
    nullo, titolo, stato
  };

private:
  tr_tipo _tipo;
public:
  void tipo( tr_barra_stato::tr_tipo tipo );

  tr_tipo tipo() {
    return _tipo;
  };

  void adatta( bool start=true );
  void aspetto_titolo();

  //void setText( QString  );		// imposta tipo titolo e setta il contenuto
  void setText( QString txt, int pixelSize=0, bool bold=false ); // se pixelSize<0 scala di pixelSize la dim corrente
  void setText( tr_label*l, QString txt ); // imposta e adatta il testo

  QString text() {
    return testo->text();
  }

  void setTavolo( QString s ) {
    setText( tavolo, s );
  }

  void setSala( QString s ) {
    setText( sala, s );
  }

  void setListino( QString s ) {
    setText( listino, s );
  }

  void setCliente( QString s ) {
    setText( cliente, s );
  }

  void setCameriere( QString s ) {
    setText( cameriere, s );
  }

  void setPaletteForegroundColor( QColor c );
  void setPaletteBackgroundColor( QColor c );
  void setPaletteBackgroundPixmap( QPixmap p );
  void show();

  QGridLayout*layout;
  tr_label*testo;

  tr_label*ico_tavolo;
  tr_label*tavolo;
  tr_label*ico_sala;
  tr_label*sala;
  tr_label*ico_cameriere;
  tr_label*cameriere;
  tr_label*ico_listino;
  tr_label*listino;
  tr_label*ico_cliente;
  tr_label*cliente;

signals:
  void tavolo_clicked();
  void ico_tavolo_clicked();
  void sala_clicked();
  void ico_sala_clicked();
  void cliente_clicked();
  void ico_cliente_clicked();
  void cameriere_clicked();
  void ico_cameriere_clicked();
  void listino_clicked();
  void ico_listino_clicked();

};

#endif
