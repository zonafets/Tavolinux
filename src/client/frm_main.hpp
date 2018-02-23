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

#ifndef FRM_MAIN_H
#define FRM_MAIN_H

/*
  form principale di gestione delle sale, tavoli conti e comande
   */

#include "frm_base.hpp"
#include "tr_tavoli_view.hpp"
#include "tr_label.hpp"
#include "tr.hpp"
#include "tr_enumeratori.hpp"
#include "cmdpaycard.hpp"

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>


class frm_main:
    public frm_base,public tr::enumeratori,
    TRPayCard
{
    Q_OBJECT;

    public:
    frm_main( QWidget* parent = 0, const char* name = 0, bool modal = true, WFlags fl = MYFLAGS );
    ~frm_main();

    tr_label* ristampa;
    tr_label* sposta;
    tr_label* scelta_sala;
    tr_label* tavolata;
    tr_label* conto;
    tr_label* bis;
    tr_label* comanda;
    tr_label* incassa;
    tr_label* gestione;
    tr_label* exit;
    tr_tavoli_view* tavoli;

    void frm_fornitori(bool con_magazzino=false);
    void frm_tessere();
    void frm_ffm();
    void frm_voti();
    void frm_turni();
    void frm_params();

    int chk_tavolo ( int id );
    int chk_tavolo ( int id, int stato );
    void carica_sala( int id, bool vista );

    // aggiunge una prenotazione
    QString prenota(
      frm_base *f,itm_tavolo *tavolo,
      QString dettaglio=QString::null,
      bool per_asporto=false);

    void stato_pulsanti();

    bool eventFilter(QObject *o, QEvent *e);


public slots:
    virtual void ristampa_clicked();
    virtual void sposta_clicked();
    virtual void scelta_sala_clicked();
    virtual void ico_cameriere_clicked();
    virtual void ico_sala_clicked();
    virtual void ico_tavolo_clicked();
    virtual void cameriere_clicked();
    virtual void tavolata_clicked();
    virtual void conto_clicked();
    virtual void bis_clicked();
    virtual void comanda_clicked();
    virtual void incassa_clicked();
    virtual void gestione_clicked();
    virtual void exit_clicked();
    virtual void tavoli_clicked(QMouseEvent *);


protected slots:
    virtual void init();
    virtual void resizeEvent( QResizeEvent * ev );


private:
    int blink_timer_id;
    QString id_utente;				// id cameriere loggato o ""
    bool solo_logout;

public:
    bool first_login(
      QString id=QString::null, QString descrizione=QString::null,
      bool da_tessera=false
    );
    void login(QString id, QString nome, QString pwd);

public slots:
    void logout();
    void card_readed();

public:
    void prenotazioni();
    void stato_sale();
    void apri_conto();

    void tests();

private:
    int bw();
};


#endif

