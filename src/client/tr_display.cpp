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

#include "tr_display.hpp"

// carica la configurazione

#include "tr_config.hpp"
extern tr_config config;

// termios per impostazione hardware della seriale

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

// Codici escape del display epson/firich

#define HT      "\\x09"
#define BS      "\\x08"
#define USLF    "\\x1F\\x0A"
#define LF      "\\x0A"
#define USCR    "\\x1F\\x0D"
#define CR      "\\x0D"
#define HOME    "\\x0B"
#define CLR     "\\x0C"
#define CAN     "\\x18"
#define LOCATE	"\\x1F\\x24"
#define SCROLLH	"\\x1F\\x03"
#define SCROLLV "\\x1F\\x02"
#define RESET	"\\x1B\\x40"
#define TIME	"\\x1F\\x54"


// Debugging

//#define DEBUG

// Costruttore del display

tr_display::tr_display ()
{

	struct definizione_display {
		enum tr_contesto_display contesto;
		char formato[254];
	};

	struct {
		enum tr_tipo_display tipo;
		struct definizione_display def_disp[tr_contesto_nullo + 1];
	} tipi_standard[] = {

	     // Display standard EPSON

	     {
		display_epson,
                {{ tr_inserisci_articolo,
                        CLR "%quantita% X %descrizione%" LOCATE "\\x09\\x02%prezzo% EU" } ,
                { tr_modifica_prezzo,
                        CLR "PREZZO %descrizione%" CR LF LOCATE "\\x09\\x02%prezzo% EU" },
                { tr_modifica_quantita,
                        CLR "MODIFICA QUANTITA" CR LF "%quantita% X %descrizione%" },
                { tr_totale_conto,
                        CLR "TOTALE" LOCATE "\\x09\\x02%prezzo% EU" },
                { tr_storna_articolo,
                        CLR "STORNA" CR LF "%quantita% X %descrizione%" },
				{ tr_inserisci_pagamento,
					CLR "%descrizione%" USCR "%prezzo% EU"},
				{ tr_inizializza_display,
					RESET CLR "TAVOLINUX - %data% %ora%" },
				{ tr_dividi_conto,
					CLR "DIVIDI CONTO x %quantita%"},
				{ tr_annulla_conto,
					CLR "ANNULLA CONTO"},
        { tr_resto_conto,
          CLR "RESTO" USCR "%prezzo% EU"},
        { tr_contesto_nullo, "" }
		  }
             }
        };


	// Determina se devo fare qualcosa o no

	init_ok = false;

	// Guarda se siamo definiti o no

	if ( !config.cfg_display.tipo)
		return;

	// In caso contrario procedi all'inizializzazione

	fd.setName (config.cfg_display.porta);

	if (!fd.open (IO_WriteOnly | IO_Raw))
		return;

	// Imposta i patterns

	bool patterns_loaded = false;
	for (int i=0;i < display_ultimo;i++)
	{
		// Cerca il display tra i tanti disponibili

		if (config.cfg_display.tipo ==
			(tipi_standard[i].tipo+1))
		{
			patterns_loaded = true;
#ifdef DEBUG
			cerr << "Display " << config.cfg_display.tipo << " trovato" << endl;
#endif
			for (int x=0;tipi_standard[i].def_disp[x].contesto < tr_contesto_nullo;x++)
			{
				// Copia il pattern "x"	dal modello standard

				patterns[tipi_standard[i].def_disp[x].contesto] =
						tipi_standard[i].def_disp[x].formato;

				// Aggiorna eventuali valori
			}

			// Loop di lettura parametri standard

			for (int x=0;x < tr_contesto_nullo;x++)
				if ( config.cfg_display.formati[x] != "default")
				{
					if ( config.cfg_display.formati[x] == "null")
						patterns[x] = QString::null;
					else
						patterns[x] = config.cfg_display.formati[x];
				}
		}
	}

	// Imposta le lunghezze standard

	lunghezze.descrizione = config.cfg_display.lunghezze.descrizione;
	lunghezze.quantita = config.cfg_display.lunghezze.quantita;
	lunghezze.prezzo = config.cfg_display.lunghezze.prezzo;

	// Imposta le sequenze di selezione/deselezione

	seq_seleziona = (config.cfg_display.sequenze.seleziona != "null") ?
						config.cfg_display.sequenze.seleziona :
						QString::null;
	seq_deseleziona = (config.cfg_display.sequenze.deseleziona != "null") ?
						config.cfg_display.sequenze.deseleziona :
						QString::null;

	// Se necessario imposta la seriale

	if (patterns_loaded)
	{
		bool _x = true;
		if (config.cfg_display.parametri_porta == "default")
			_x = imposta_seriale ( "9600,8,n,1,RTSCTS");
		else if (config.cfg_display.parametri_porta != "null")
			_x = imposta_seriale ( config.cfg_display.parametri_porta );

		if (!_x)
			cerr << "Errore impostazione parametri porta: " << endl;
	}

	// Alla fine setta il flag ed inizializza il display

	if (patterns_loaded)
	{
		init_ok = true;
#ifdef DEBUG
		cerr << "LCD display init OK" << endl;
#endif
		aggiorna ( tr_inizializza_display, QString::null);
	}
}

//
// Decodifica i caratteri da hex a ok
//

bool tr_display::decHex()
{
	QString hc = "0123456789ABCDEF";
	unsigned char c = 0;
	bool ferr = false;

	enum {
		carattere_normale,
		inizio_escape,
		hex_escape,
		primo_hex,
		secondo_hex
		} step_dec = carattere_normale;

	// Azzera la stringa di output

	bzero (linea_output, sizeof (linea_output));
	l_linea_output = 0;

	// Inizia la scansione

	for (uint i=0;i < data_output.length();i++)
	{
		switch (data_output[i].latin1()) {
			case '\\':
			 if (step_dec == inizio_escape)
				step_dec = carattere_normale; // doppio "\"
			 else
				step_dec = inizio_escape;
			break;
			case 'x':
			 if (step_dec == inizio_escape)
				step_dec = hex_escape;
			break;
			default:
			 switch (step_dec) {
				case hex_escape:
					if (ferr = (hc.find (data_output[i]) == -1))
						step_dec = carattere_normale; //err
					else
					{
						c += hc.find (data_output[i]) << 4;
						step_dec = primo_hex;
					}
				break;
				case primo_hex:
					if (ferr = (hc.find (data_output[i]) == -1))
						step_dec = carattere_normale;
					else
  					{
                                                c += hc.find (data_output[i]);
						linea_output[l_linea_output++] = c;
						c = 0;
						step_dec = secondo_hex;
                                        }
				break;
				case secondo_hex:
					step_dec = carattere_normale;
					break;
				case inizio_escape:
					ferr = 1;
					step_dec = carattere_normale;
					break;
				default:
					break;
				}
		} // switch (data_output)


		if (step_dec == carattere_normale)
		{
			linea_output[l_linea_output++] = data_output[i].latin1();
		}
	} // for



	return ferr;




}

// Aggiornamento del display

void tr_display::aggiorna (enum tr_contesto_display contesto,
				QString q,
				QString d,
				QString p)
{
	QString ol;

	// Individua la frase per il contesto

	if (!init_ok)
		return;

	// Trova il contesto richiesto

	ol = patterns[contesto];


	// Vedi se va

	if (ol.isEmpty())
	{
#ifdef DEBUG
	cerr << "Non trovo il pattern di formato per il contesto" << endl;
#endif
		return;
	}

	// Pad dei dati

	QString qp,dp,pp;
	qp = QString ("%1")
		.arg (q.left (lunghezze.quantita), -lunghezze.quantita);
	dp = QString ("%1")
		.arg (d.left (lunghezze.descrizione), -lunghezze.descrizione);
	pp = QString ("%1")
		.arg (p.left (lunghezze.prezzo), lunghezze.prezzo)
		.replace ('.',',');

	// valorizza le variabili

	ol.replace ("%quantita%", qp);
	ol.replace ("%descrizione%",dp);
	ol.replace ("%prezzo%",pp);

	QDateTime ora = QDateTime::currentDateTime();

	ol.replace ("%data%", ora.toString ("d/M/yy"));
	ol.replace ("%ora%",
		QString (TIME "\\x%1\\x%2")
			.arg ( ora.time().hour() + 1, 2, 16)
			.arg ( ora.time().minute(), 2, 16)
			.replace (' ', '0'));

	// Aggiungi le eventuali sequenze di selezione/deselezione

	if (!seq_seleziona.isEmpty())
		ol.prepend (seq_seleziona);

	if (!seq_deseleziona.isEmpty())
		ol.append (seq_deseleziona);

	// Copialo

	data_output = ol;

#ifdef DEBUG

	cerr << "do: " << data_output << endl;

#endif
	// escapalo

	decHex();

	// fuoco !!

	fd.writeBlock (linea_output, l_linea_output);

}


//
// Imposta i paremtri della porta seriale
//

bool tr_display::imposta_seriale ( QString parametri )
{
	struct termios seriale;
	enum {
		e_baudrate = 0,
		e_databit,
		e_parity,
		e_stopbit,
		e_handshake,
		e_end		};

	// Leggi i dati utente

	QStringList p = QStringList::split (',',parametri);
	if (p.count() != 5)
		return false;

	// Leggi le impostazioni attuali della seriale

	tcgetattr ( fd.handle(), &seriale );

	// Processa l'handshake

	seriale.c_cflag &= ~CRTSCTS;
    seriale.c_iflag &= ~(IXON | IXOFF);

	if (p[e_handshake] == "RTSCTS")
    {
    	seriale.c_cflag |= CRTSCTS;
    }
    else if (p[e_handshake] == "XONXOFF")
    {
    	seriale.c_iflag |= IXON | IXOFF;
    }

	// Processa la parita'

	seriale.c_cflag &= ~(PARENB | PARODD);

    if (p[e_parity] == "E")
    {
    	seriale.c_cflag |= PARENB;
    }
    else if (p[e_parity] == "O")
    {
    	seriale.c_cflag |= PARODD;
    }

	// Bit di dati

	seriale.c_cflag &= ~CSIZE;

    if (p[e_databit] == "8")
    {
    	seriale.c_cflag |= CS8;
   	}
    else if (p[e_databit] == "7")
    {
    	seriale.c_cflag |= CS7;
    }
    else if (p[e_databit] == "6")
    {
    	seriale.c_cflag |= CS6;
    }
    else if (p[e_databit] == "5")
    {
    	seriale.c_cflag |= CS5;
    }

    // Stop bits

    if (p[e_stopbit] == "1")
    	seriale.c_cflag &= ~CSTOPB;
    else
    	seriale.c_cflag |= CSTOPB;

	// Baudrate

	if (p[e_baudrate] == "9600")
	    {
    	cfsetispeed (&seriale, B9600);
    	cfsetospeed (&seriale, B9600);
    	}
	else if (p[e_baudrate] == "19200")
		{
		cfsetispeed (&seriale, B19200);
		cfsetospeed (&seriale, B19200);
		}
	else if (p[e_baudrate] == "38400")
	    {
		cfsetispeed (&seriale, B38400);
		cfsetospeed (&seriale, B38400);
	    }
	else if (p[e_baudrate] == "57600")
	    {
		cfsetispeed (&seriale, B57600);
		cfsetospeed (&seriale, B57600);
	    }
	else if (p[e_baudrate] == "115200")
		{
		cfsetispeed (&seriale, B115200);
		cfsetospeed (&seriale, B115200);
		}

	// Rendi effettivi i cambiamenti da ora

	return (tcsetattr (fd.handle(), TCSANOW, &seriale) == 0);

}
