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

#ifndef TR_FORMS_HPP
#define TR_FORMS_HPP

#include <qstring.h>
#include <qdict.h>

#include "frm_base.hpp"
#include "tr_common.hpp"

class tr_forms:public frm_base {
public:
    tr_forms(QWidget *parent,QString nome);
                                /* interpreta ed esegue la form nome
				   la sua immediata esecuzione obbliga
				   il programmatore a catalogare e organizzare
				   la struttura delle forms prima in una funzione
				   generica all'avvio dell'applicazione */
    ~tr_forms();

    /* aggiunge al dizionario delle forms una nuova form. Il nome deve essere composto
       da una prima parte di una form esistente (vedere costruttore)  piu' il "punto" piu'
       il nome della form derivata.
       I parametri sono specializzatori e modificatori della form così:
       <parametro>=valore;<parametro>=valore;...
      -<parametro> elimina il parametro dalla form con l'eventuale ,(virgola) prima
                               serve per togliere pulsanti o sottoparametri di troppo
     */
    static void add_form(QString parent_dot_name_form, QString params);

protected:
    static tr_Dict<QString> forms;

};

#endif
