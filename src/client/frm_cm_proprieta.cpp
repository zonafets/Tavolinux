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

#include <iostream>
#include <qdatetime.h>
#include <qapplication.h>
#include <qregexp.h>
#include <qfile.h>
#include <qdatastream.h>

#include "cmdcodicigruppi.hpp"
#include "frm_scelta.hpp"
#include "tr_menu.hpp"
#include "frm_cm.hpp"
#include "tr_tools.hpp"
#include "tr_globals.hpp"

#include "tr_enumeratori.hpp"

#include <qurl.h>

#define USA_PRN_PADRE tr("Usa stampanti del padre")

extern tr_tools tools;
extern tr_globals glb;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FRM_CM_MENU_PROPRIETA
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

frm_cm_proprieta::frm_cm_proprieta( frm_cm*parent, const char*name, bool modal, WFlags fl ):
   frm_ok_exit( parent, name, modal, fl ) {

     titolo->setText( tr( "proprieta' tasto menu" ) );

     base_alza();

     barra_titolo->hide();

     barra_stato->setText( tr( "Proprieta' pulsante del menu" ) );

     NEW( descrizione, tr_input( area, "descrizione", tr( "Descrizione" ), QString() ) );
     descrizione->setReadOnly( true );

     NEW( desc1, tr_input( area, "descrizione1", tr( "1" ), QString(), "desc(9)", true ) );
     NEW( desc2, tr_input( area, "descrizione2", tr( "2" ), QString(), "desc(9)", true ) );
     NEW( desc3, tr_input( area, "descrizione3", tr( "3" ), QString(), "desc(9)", true ) );

     NEW( colori, tr_colori( area, "colori" ) );

     QDir d( config.exe_path+"icone/" );
     QStringList elenco_icone=d.entryList( "*.png" );

     NEW( icone, tr_iconviewer( area, "icone", elenco_icone, 6 ) );

     QWidget*ws[]= {
    descrizione, descrizione, descrizione, descrizione, descrizione, descrizione, 0,
    desc1, desc1, desc1, icone, icone, icone, 0,
    desc2, desc2, desc2, icone, icone, icone, 0,
    desc3, desc3, desc3, icone, icone, icone, 0,
    colori, colori, colori, colori, colori, colori, 0,0};

     build( ws );

     QWidget*w1[]= {desc2,desc3,icone,colori, NULL};
     link( desc1, w1 );

     resize( minimumSizeHint() );

}


frm_cm_proprieta::~frm_cm_proprieta() {
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm_proprieta::no_colori() {
  disabilita( colori );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_cm_proprieta::modifica( QString descr, QString descr1, QString descr2, QString descr3,
   QColor colore, QString icona ) {
     descrizione->setText( descr );
     desc1->setText( descr1 );
     desc2->setText( descr2 );
     desc3->setText( descr3 );
     descrizione->setSelection( 0, 0 );
     desc1->setSelection( 0, 0 );
     desc2->setSelection( 0, 0 );
     desc3->setSelection( 0, 0 );

     colori->select( colore );
     icone->visualizza( icona );
     icone->select( icona );
}
