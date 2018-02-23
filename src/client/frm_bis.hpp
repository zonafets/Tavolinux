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

#ifndef FRM_BIS_H
#define FRM_BIS_H

#include "frm_base.hpp"
#include "frm_main.hpp"
#include "tr_label.hpp"

/* presenta la pulsantiera di men per altre opzioni */

class frm_bis: public frm_base {
    Q_OBJECT

    public:
    frm_bis( frm_main* parent = 0, const char* name = 0, bool modal = true, WFlags fl = MYFLAGS );
    ~frm_bis();

    tr_label *conto;
    tr_label *comanda;
    tr_label *incassa;
    tr_label *svuota;
    tr_label *sblocca;
    tr_label *sposta;
    tr_label *exit;

    public slots:

    void conto_clicked();
    void comanda_clicked();
    void incassa_clicked();
    void numero_clicked();
    void exit_clicked();
    void sposta_clicked();
    void svuota_clicked();
    void sblocca_clicked();

    protected:
    void init();

    private:
    tr_label* _selezionato;
    tr_label* primo;
    int id;
    int bis;
    int stato;
    frm_main *form_main;

    void stato_pulsanti();
};

#endif
