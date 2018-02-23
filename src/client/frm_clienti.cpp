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

#include "frm_clienti.hpp"
#include "tr_tools.hpp"

extern tr_tools tools;

frm_clienti::frm_clienti( frm_scelta*parent, const char*name ):frm_ok_exit( parent, name ) {

  _modificato=false;
  _modifica=false;
  form_scelta=parent;

  titolo->setText( tr( "Inserimento/modifica cliente" ) );

  scopri_pulsantiera();

  QString validatori[]= {
    "id_cliente", "ragione_sociale", "p_iva", "c_f", "via", "cap", "citta", "telefono", "fax", "data_ultima_fattura",
       "iva_specifica", "ragione_iva"
  };
  tr_input**campi[]= {
    &codice, &ragione, &pi, &cf, &via, &cap, &citta, &telefono, &fax, &data_ultima_fattura, &aiva, &diva, NULL
  };
  QString etichette[]= {
    tr( "Codice" ), tr( "Ragione sociale" ), tr( "P.IVA" ), tr( "Codice Fiscale" ), tr( "Via" ), tr( "CAP" ), tr( "Citta'" ), tr( "Telefono" ), tr( "Fax" ),
       tr( "Data ult.fatt." ), tr( "Aliquota IVA" ), tr( "Ragione A.IVA" )
  };

  int i=0;
  while ( campi[i]!=NULL ) {
    QString validatore=tools.validatore( "anagrafica_clienti." +validatori[i] );
    QString falso=tools.segna_posto( validatore );
    tr_input*in;
    NEW( in, tr_input( area, tools.label_to_name( etichette[i] ), etichette[i], falso, validatore ) );
    *campi[i]=in;
    i++;
  }


  NEW( pagamenti, tr_listview( area, "pagamenti" ) );
  pagamenti->nascondi( tr_listview::e_barra );
  pagamenti->setMinMax( 190, 0, 32767, 32767 );
  pagamenti->tipo( tr_listview::selezione_pagamenti );
  pagamenti->carica();

  QWidget*ws[]= {
    codice, ragione, ragione, pagamenti, 0, pi, cf, cf, pagamenti, 0, via, via, via, pagamenti, 0, cap, citta, citta,
       pagamenti, 0, telefono, fax, data_ultima_fattura, pagamenti, 0, aiva, diva, diva, pagamenti, 0, 0
  };

  build( ws, 6, 3 );

  data_ultima_fattura->setReadOnly( true );

  QWidget*w[]= { ragione, pagamenti, pi, cf, via, cap, citta, telefono, fax, data_ultima_fattura, aiva, diva, NULL };
  link( codice, w );

  barra_stato->setText( tr( "Inserimento/Modifica Cliente" ) );
  barra_stato->show();

  disconnetti_ok();
  CONNECT( ok );

}

frm_clienti::~frm_clienti() {
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_clienti::show() {
  frm_ok_exit::show();
  pagamenti->adatta();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

void frm_clienti::ok_clicked() {

  if ( ragione->text().isEmpty() ) {
    tools.msgbox( tr( "Specificare almeno la ragione sociale" ) );
    return;
  }
  if ( ( aiva->text().isEmpty()&&!diva->text().isEmpty() )||( !aiva->text().isEmpty()&&diva->text().isEmpty() ) ) {
    tools.msgbox( tr( "Specificare sia aliquota che descrizione per l'IVA" ) );
    return;
  }

  QString elenco_pagamenti;
  QStringList scelti=pagamenti->selezionati();
  if ( scelti.count() ) elenco_pagamenti=scelti.join( "," );
  else
    elenco_pagamenti="NULL";

  if ( !_modifica ) {

#define T(x) x->text()
    if ( CMD( tok_inserisci_cliente, T( codice ), T( ragione ), T( via ), T( citta ), T( cap ), T( telefono ), T( fax ), T( pi ), T( cf ),
       elenco_pagamenti, T( aiva ), T( diva ) ) )

#undef T
   {
         accept();
         modificato( true );
    }
  }

  else {

    QString st;
#define T(x) (x->text())
    if ( CMD( tok_modifica_cliente, form_scelta->id(), T( codice ), T( ragione ), T( via ), T( citta ), T( cap ), T( telefono ), T( fax ), T( pi ), T( cf ),
       elenco_pagamenti, T( aiva ), T( diva ) ) )
#undef T
   {
         accept();
         modificato( true );
   }
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////77

bool frm_clienti::modifica( QString id ) {

  tr_input*ctrls[]= { codice, ragione, via, citta, cap, telefono, fax, pi, cf, data_ultima_fattura, aiva, diva };
  tr_posizioni campi[]= {
    p_codice, p_descrizione, p_via, p_citta, p_cap, p_telefono, p_fax, p_iva, p_cf, p_data_ultima_fattura,
       p_aiva, p_descrizione_aiva
  };

  tr::mse clienti( tok_elenco_clienti_esteso );
  clienti.carica( id );

  if ( clienti.count() ) {

    tr::mse::riga cliente( clienti[0] );

    TROWI( ctrls ) {
      ctrls[i]->setText( cliente[campi[i]] );
    }

    if ( cliente[p_aiva]=="0" )
      aiva->setText( QString::null );

    QStringList elenco_pagamenti=QStringList::split( ",", cliente[p_pagamenti] );
    pagamenti->seleziona( elenco_pagamenti );

    _modifica=true;

    return true;
  }
  else
    return false;
}
