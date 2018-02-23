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

#ifndef FRM_CLIENTI_HPP
  #define FRM_CLIENTI_HPP


  #include "frm_ok_exit.hpp"
  #include "frm_scelta.hpp"
  #include "tr_listview.hpp"
  #include "tr_input.hpp"
  #include "tr.hpp"

class frm_clienti:public frm_ok_exit, tr::enumeratori::cliente_esteso {
  Q_OBJECT

  public:frm_clienti( frm_scelta*parent, const char*name );
  ~frm_clienti();

  tr_input*codice;
  tr_input*ragione;
  tr_input*pi;
  tr_input*cf;
  tr_input*via;
  tr_input*citta;
  tr_input*cap;
  tr_input*telefono;
  tr_input*fax;
  tr_input*data_ultima_fattura;
  tr_input*aiva;
  tr_input*diva;
  tr_listview*pagamenti;

public:
  bool modifica( QString id );

  bool modificato() {
    return _modificato;
  }

  void modificato( bool v ) {
    _modificato=v;
  }

  public slots:void show();
  void ok_clicked();

private:
  bool _modificato;
  bool _modifica;
  frm_scelta*form_scelta;
};

#endif
