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

#ifndef TR_TABLE_HPP
#define TR_TABLE_HPP

#include <qtable.h>
//#include <qpainter.h>

#include "tr_config.hpp"
#include "cmdcodicigruppi.hpp"
#include "tr.hpp"

class Q_EXPORT tr_table : public QTable, public tr::enumeratori {
    Q_OBJECT;
public:
    tr_table ( QWidget * parent = 0, const char * name = 0 );
    ~tr_table() {}

    bool as_original;

    void paintFocus ( QPainter *p , const QRect &r  ) {
      if (as_original) return QTable::paintFocus(p,r);
    };	// disabilita il disegno del rettangolo di focus

    void paintCell ( QPainter * p, int row, int col, const QRect & cr,
		     bool selected, const QColorGroup & cg );

    // sposta e seleziona contemporaneamente
    void setCurrentCell( int row, int col=-1);

    bool event ( QEvent * e );

    /* confronta l'attuale _contenuto con contenuto e aggiunge solo le differenze
       accodando o inserendo righe nuove */
    void aggiorna(QStringList contenuto, bool conserva_selezione=false );
    void adatta();
    void elimina_righe();

    void adjustColumn(int col);	// ottimizzate
    void setRowHeight(int r, int h);
    int altezza_riga;

    void setMinMax(int mw, int mh, int MW=-1, int MH=-1);

private:
    QStringList _contenuto;		// copia del contenuto passato

    // rimangono private perch�tutto deve essere gestito da aggiorna()
    void agg_riga( int riga, QStringList &dati );	// aggiorna riga con dati(piped)

protected:
  void activateNextCell();
};

#endif
