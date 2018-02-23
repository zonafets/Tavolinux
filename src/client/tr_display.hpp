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

#ifndef TR_DISPLAY

#define TR_DISPLAY

#include <qstring.h>
#include <qfile.h>
#include <qmap.h>




class tr_display {

	public:

	// ATTENZIONE !!
	// In "tr_config.hpp" e' definito un array di QString (nella
	// struttura "cfg_display") contenente i formati definiti.
	// Di base è dimensionato staticamente a 16 formati. Se il
	// numero dei formati definiti qui SUPERA i 16, il sistema
	// potrebbe fare dei disastri. Correggere il numero 16
	// in funzione dei formati definiti e anche l'array delle
	// descrizioni dei formati


	static QString descrizione_formato(int x) {
			char *descrizioni_formati[] = {
				"inserimento_articolo",
				"modifica_prezzo",
				"modifica_quantita",
				"totale_conto",
				"storno_articolo",
				"inserimento_pagamento",
				"inizializzazione",
				"divisione_conto",
				"annullamento_conto",
        "resto_conto",
				NULL};
			return QString (descrizioni_formati[x]);
			}

	enum tr_contesto_display {
                tr_inserisci_articolo = 0,
                tr_modifica_prezzo,
                tr_modifica_quantita,
                tr_totale_conto,
                tr_storna_articolo,
				tr_inserisci_pagamento,
				tr_inizializza_display,
				tr_dividi_conto,
				tr_annulla_conto,
        tr_resto_conto,
                tr_contesto_nullo };

	// pattern dei tipi di display supportati

	enum tr_tipo_display {
        	display_epson = 0,
	        display_ultimo
	};



	tr_display ();

	void aggiorna ( enum tr_contesto_display,
			QString = QString::null, // quantita articolo o altro
			QString = QString::null, // descrizione
			QString = QString::null // prezzo
			);

	// overload della precedente

	void aggiorna ( enum tr_contesto_display c,
			double q,
			QString d,
			QString p = QString::null)
	{
		return aggiorna (c,QString::number (q,'f',2),d,p);
	}

	bool inizializzato() { return init_ok; }

	private:

	QString seq_seleziona, seq_deseleziona;
	struct {
		int descrizione, quantita, prezzo;
	} lunghezze;
	QFile fd;
	bool init_ok;

	// buffer di output al display

	QString data_output;
	char linea_output[256];
	int l_linea_output;

	// Struttura per la definizione delle righe del display
	// Siccome siamo dei fighi queste le carichiamo dal file
	// di configurazione direttamente, così smettono di romperci
	// l'anima sulle personalizzazioni delle visualizzazioni

	QMap < int, QString > patterns;

	// Display standard

	// Decodifica HEX

	bool decHex();

	// Setta i parametri di porta

	bool imposta_seriale ( QString );

};

#endif
