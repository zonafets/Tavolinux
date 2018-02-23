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

#ifndef FRM_WAIT_H
  #define FRM_WAIT_H

  #include <qobject.h>
  #include "frm_base.hpp"
  #include "tr_label.hpp"
  #include <qlabel.h>

class frm_wait:public frm_base {
  Q_OBJECT

  public:frm_wait( QWidget*parent=0, const char*name=0, bool modal=true, WFlags fl=MYFLAGS );
  ~frm_wait();

  enum {
    clessidra, computer, caffe
  }
  tipo;

  QLabel*icona;
  tr_label*testo;

  void avanza();

private:
  int n_icona;
  int n_icone;
  QString base;
};

#endif
