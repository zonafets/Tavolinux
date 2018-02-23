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

#ifndef FRM_OK_EXIT
  #define FRM_OK_EXIT

  #include "frm_base.hpp"
  #include "tr_input.hpp"

  #include <qwidget.h>
  #include <qstring.h>
  #include <qstringlist.h>


class frm_ok_exit:public frm_base {
  Q_OBJECT;



public:

  frm_ok_exit( frm_base*parent=0, const char*name=0, bool modal=TRUE, WFlags fl=MYFLAGS );

  ~frm_ok_exit() {
  };

  tr_label*ok;
  tr_label*annulla;
  tr_label*exit;

  void disconnetti_ok() {
    disconnect( ok, SIGNAL( clicked() ), this, SLOT( ok_clicked() ) );
  }

  void disconnetti_annulla() {
    disconnect( annulla, SIGNAL( clicked() ), this, SLOT( annulla_clicked() ) );
  }

  void disconnetti_exit() {
    disconnect( exit, SIGNAL( clicked() ), this, SLOT( exit_clicked() ) );
  }

  bool annullato() {
    return _annullato;
  }

  public slots:void ok_clicked() {
    accept();
  }

  void annulla_clicked() {
    _annullato=true;
    reject();
  }

  void exit_clicked() {
    reject();
  }

private:
  bool _annullato;

};

#endif
