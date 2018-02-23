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

// file generato automaticamente

#ifndef TR_ENUMERATORI_HPP
#define TR_ENUMERATORI_HPP


#define denu_login\
    enum enu_login {\
    p_diritti_cameriere=0,p_nome}
#define denu_login_terminal_esteso\
    enum enu_login_terminal_esteso {\
    p_licenza=0,p_ragione_sociale,p_piva}
#define denu_apri_tavolo\
    enum enu_apri_tavolo {\
    p_nome_tavolo=0,p_comanda_parcheggiata,p_cod_listino,p_descr_listino,p_tavolata,p_articoli_p_nullo,p_coperti_presenti,p_nome_prenotatore}
#define denu_apri_tavolo_diretto\
    enum enu_apri_tavolo_diretto {\
    p_id_tavolo=0,\
    p_nome_tavolo=0,p_comanda_parcheggiata,p_cod_listino,p_descr_listino,p_tavolata,p_articoli_p_nullo,p_coperti_presenti,p_nome_prenotatore}
#define denu_apri_tavolo_tag\
    enum enu_apri_tavolo_tag {\
    p_nome_tavolo=0,p_comanda_parcheggiata,p_cod_listino,p_descr_listino,p_tavolata,p_articoli_p_nullo,p_coperti_presenti,p_nome_prenotatore}
#define denu_controlla_tavolo\
    enum enu_controlla_tavolo {\
    p_stato_tavolo=0,p_stato_esteso,p_elenco_tipi_conto,\
    p_id_tavolo_bis=0,p_stato_tavolo_bis}
#define denu_chiudi_tavolo\
    enum enu_chiudi_tavolo {\
    p_stato_tavolo=0}
#define denu_chiudi_tavolo_tag\
    enum enu_chiudi_tavolo_tag {\
    p_stato_tavolo=0}
#define denu_inserisci_tavolo\
    enum enu_inserisci_tavolo {\
    p_nome_tavolo=0,p_id_sala,p_tavolo_x,p_tavolo_y,p_tipo,p_n_posti,p_stato,p_ruotato,p_ordine,p_id_tavolo,p_id_prima_comanda,p_id_tavolata,p_id_listino,p_pixmap,p_id_gruppo_listino,p_elenco_tipi_conto,p_id_prenotazione}
#define denu_inserisci_tavolo_fittizio\
    enum enu_inserisci_tavolo_fittizio {\
    p_id_tavolo_fittizio=0}
#define denu_inserisci_cliente\
    enum enu_inserisci_cliente {\
    p_codice_cliente=0}
#define denu_crea_tavolata\
    enum enu_crea_tavolata {\
    p_nome_tavolo=0,p_id_sala,p_tavolo_x,p_tavolo_y,p_tipo,p_n_posti,p_stato,p_ruotato,p_ordine,p_id_tavolo,p_id_prima_comanda,p_id_tavolata,p_id_listino,p_pixmap,p_id_gruppo_listino,p_elenco_tipi_conto,p_id_prenotazione}
#define denu_aggiungi_tavolo_tavolata\
    enum enu_aggiungi_tavolo_tavolata {\
    p_nome_tavolo=0,p_id_sala,p_tavolo_x,p_tavolo_y,p_tipo,p_n_posti,p_stato,p_ruotato,p_ordine,p_id_tavolo,p_id_prima_comanda,p_id_tavolata,p_id_listino,p_pixmap,p_id_gruppo_listino,p_elenco_tipi_conto,p_id_prenotazione}
#define denu_sciogli_tavolata\
    enum enu_sciogli_tavolata {\
    p_nome_tavolo=0,p_id_sala,p_tavolo_x,p_tavolo_y,p_tipo,p_n_posti,p_stato,p_ruotato,p_ordine,p_id_tavolo,p_id_prima_comanda,p_id_tavolata,p_id_listino,p_pixmap,p_id_gruppo_listino,p_elenco_tipi_conto,p_id_prenotazione}
#define denu_cerca_tavolo\
    enum enu_cerca_tavolo {\
    p_id_tavolo=0}
#define denu_elenco_gruppi\
    enum enu_elenco_gruppi {\
    p_id_istanza=0,p_codice,p_prezzo_base,p_descrizione,p_ha_figli,p_tipo_gruppo,p_tipo_raggruppamento,p_id_articolo,p_id_padre,p_id_listino,p_iva,p_reparto,p_gruppo_stampa,p_numero_porzioni}
#define denu_elenco_gruppi_ordinati\
    enum enu_elenco_gruppi_ordinati {\
    p_id_istanza=0,p_codice,p_prezzo_base,p_descrizione,p_ha_figli,p_tipo_gruppo,p_tipo_raggruppamento,p_id_articolo,p_id_padre,p_id_listino,p_iva,p_reparto,p_gruppo_stampa,p_numero_porzioni}
#define denu_elenco_gruppi_esteso\
    enum enu_elenco_gruppi_esteso {\
    p_id_istanza=0,p_codice,p_prezzo_base,p_descrizione,p_ha_figli,p_tipo_gruppo,p_tipo_raggruppamento,p_id_articolo,p_id_padre,p_id_listino,p_iva,p_reparto,p_gruppo_stampa,p_numero_porzioni}
#define denu_elenco_gruppi_disponibilita\
    enum enu_elenco_gruppi_disponibilita {\
    p_id_istanza=0,p_codice,p_prezzo_base,p_descrizione,p_tipo_gruppo,p_tipo_raggruppamento,p_descr_alternativa,p_id_articolo,p_giacenza_finale,p_iva,p_reparto,p_gruppo_stampa}
#define denu_elenco_coperti\
    enum enu_elenco_coperti {\
    p_id_istanza=0,p_codice,p_prezzo_base,p_descrizione,p_ha_figli,p_tipo_gruppo,p_tipo_raggruppamento,p_id_articolo,p_id_padre,p_id_listino,p_iva,p_reparto}
#define denu_inserisci_articolo_comanda\
    enum enu_inserisci_articolo_comanda {\
    p_id_riga=0,p_cod_articolo_padre,p_descrizione,p_prezzo_calcolato,p_subtotale_comanda}
#define denu_pesa_articolo\
    enum enu_pesa_articolo {\
    p_peso=0}
#define denu_pesa_articolo_trasmesso\
    enum enu_pesa_articolo_trasmesso {\
    p_peso=0}
#define denu_storna\
    enum enu_storna {\
    p_totale_comanda=0}
#define denu_modifica_gruppo\
    enum enu_modifica_gruppo {\
    p_id_istanza=0,p_id_articolo}
#define denu_validatore\
    enum enu_validatore {\
    p_regexp_validatore=0}
#define denu_elenco_portate\
    enum enu_elenco_portate {\
    p_totale_tavolo=0,\
    p_id_comanda=0,p_id_cameriere,p_num_righe,p_calcolata,\
    p_id_riga=0,p_id_istanza,p_livello_articolo,p_cod_alfa,p_quantita,p_descrizione,p_prezzo_base,p_prezzo,p_tipo_gruppo,p_offerto,p_id_listino}
#define denu_elenco_tavoli\
    enum enu_elenco_tavoli {\
    p_id_tavolo=0,p_nome_tavolo,p_id_sala,p_pixmap,p_nome_sala}
#define denu_elenco_tavoli_esteso\
    enum enu_elenco_tavoli_esteso {\
    p_nome_tavolo=0,p_id_sala,p_tav_x,p_tav_y,p_tpo,p_n_posti,p_stato,p_ruotato,p_ordine,p_id_tavolo,p_id_prima_comanda,p_id_tavolata,p_id_listino,p_pixmap,p_id_gruppo_listino,p_elenco_tipi_conto,p_id_prenotazione,p_voto}
#define denu_elenco_tavolo_esteso\
    enum enu_elenco_tavolo_esteso {\
    p_nome_tavolo=0,p_id_sala,p_tav_x,p_tav_y,p_tipo,p_n_posti,p_stato,p_ruotato,p_ordine,p_id_tavolo,p_id_prima_comanda,p_id_tavolata,p_id_listino,p_pixmap,p_id_gruppo_listino,p_elenco_tipi_conto,p_id_prenotazione,p_voto}
#define denu_elenco_tavoli_ordinati\
    enum enu_elenco_tavoli_ordinati {\
    p_id_tavolo=0,p_nome_tavolo,p_id_sala,p_pixmap,p_nome_sala}
#define denu_elenco_tavoli_aperti\
    enum enu_elenco_tavoli_aperti {\
    p_id_tavolo=0,p_nome_tavolo,p_nome_sala}
#define denu_elenco_sale\
    enum enu_elenco_sale {\
    p_id_sala=0,p_nome_sala,p_numero_tavoli}
#define denu_elenco_sale_esteso\
    enum enu_elenco_sale_esteso {\
    p_id_sala=0,p_nome_sala,p_pixmap}
#define denu_elenco_comande\
    enum enu_elenco_comande {\
    p_totale_comanda=0,\
    p_id_comanda=0,p_id_cameriere,p_num_righe,p_calcolata,p_coperti_presenti,p_articoli_p_nullo,\
    p_id_riga=0,p_id_istanza,p_livello_articolo,p_cod_alfa,p_quantita,p_descrizione,p_prezzo_base,p_prezzo,p_tipo_gruppo,p_offerto,p_id_listino}
#define denu_elenco_comande_separate\
    enum enu_elenco_comande_separate {\
    p_totale_comanda=0,\
    p_id_comanda=0,p_id_cameriere,p_num_righe,p_calcolata,\
    p_id_riga=0,p_id_istanza,p_livello_articolo,p_cod_alfa,p_quantita,p_descrizione,p_prezzo_base,p_prezzo,p_tipo_gruppo,p_offerto,p_id_listino}
#define denu_elenco_camerieri\
    enum enu_elenco_camerieri {\
    p_id_cameriere=0,p_nome_cameriere}
#define denu_elenco_camerieri_esteso\
    enum enu_elenco_camerieri_esteso {\
    p_id_cameriere=0,p_nome_cameriere,p_codice,p_password}
#define denu_elenco_clienti\
    enum enu_elenco_clienti {\
    p_id_cliente=0,p_cod_cliente,p_ragione_sociale}
#define denu_elenco_clienti_esteso\
    enum enu_elenco_clienti_esteso {\
    p_id_cliente=0,p_cod_cliente,p_ragione_sociale,p_via,p_citta,p_cap,p_telefono,p_fax,p_p_iva,p_c_fiscale,p_data_inserimento,p_data_modifica,p_data_ultima_fattura,p_pagamenti_preferiti,p_aliquota_iva_specifica,p_ragione_aliquota_iva_specifica}
#define denu_elenco_clienti_ricevute_credito\
    enum enu_elenco_clienti_ricevute_credito {\
    p_id_cliente=0,p_cod_cliente,p_ragione_sociale,p_via,p_citta,p_cap,p_telefono,p_fax,p_p_iva,p_c_fiscale,p_data_inserimento,p_data_modifica,p_data_ultima_fattura,p_pagamenti_preferiti,p_aliquota_iva_specifica,p_ragione_aliquota_iva_specifica,p_num_documenti,p_totale_documenti}
#define denu_elenco_stampanti\
    enum enu_elenco_stampanti {\
    p_nome_stampante=0}
#define denu_elenco_stampanti_gruppo\
    enum enu_elenco_stampanti_gruppo {\
    p_nome_stampante=0}
#define denu_elenco_stampanti_grafiche\
    enum enu_elenco_stampanti_grafiche {\
    p_nome_stampante=0}
#define denu_elenco_documenti\
    enum enu_elenco_documenti {\
    p_id_doc=0,p_data_ora,p_chiusura,p_modello}
#define denu_elenco_chiusure\
    enum enu_elenco_chiusure {\
    p_id_linea=0,p_data_da,p_data_a}
#define denu_elenco_conti\
    enum enu_elenco_conti {\
    p_id_conto=0,p_descrizione_conto,p_tavolo,p_cameriere,p_data_emissione,p_totale}
#define denu_elenco_ricevute_credito_cliente\
    enum enu_elenco_ricevute_credito_cliente {\
    p_data_ora=0,p_descrizione,p_id_doc_associato,p_importo,p_id_conto}
#define denu_elenco_pixmap\
    enum enu_elenco_pixmap {\
    p_pixmap=0,p_descrizione,p_tipo}
#define denu_visualizza_chiusura\
    enum enu_visualizza_chiusura {\
    p_html_chiusura=0}
#define denu_registra_comanda\
    enum enu_registra_comanda {\
    p_totale=0}
#define denu_parcheggia_comanda\
    enum enu_parcheggia_comanda {\
    p_totale=0}
#define denu_mappa_codice_istanza\
    enum enu_mappa_codice_istanza {\
    p_id_istanza=0}
#define denu_chiusura_fiscale\
    enum enu_chiusura_fiscale {\
    p_elenco_tavoli_aperti=0}
#define denu_elenco_listini\
    enum enu_elenco_listini {\
    p_id_listino=0,p_cod_listino,p_descrizione}
#define denu_elenco_listini_esteso\
    enum enu_elenco_listini_esteso {\
    p_id_listino=0,p_cod_listino,p_descrizione,p_da_data,p_a_data,p_giorni,p_intervalli}
#define denu_ultima_chiusura\
    enum enu_ultima_chiusura {\
    p_da_data=0,p_a_data}
#define denu_hello\
    enum enu_hello {\
    p_hello_string=0}
#define denu_elenco_menu\
    enum enu_elenco_menu {\
    p_id_listino=0,p_id_istanza,p_id_istanza_padre,p_stato,p_locazione,p_disposizione,p_geometria,p_descrizione,p_descr_default,p_colore,p_icona,p_pixmap,p_disponibilita}
#define denu_elenco_menu_listino\
    enum enu_elenco_menu_listino {\
    p_id_listino=0,p_id_istanza,p_id_istanza_padre,p_stato,p_locazione,p_disposizione,p_geometria,p_descrizione,p_descr_default,p_colore,p_icona,p_pixmap,p_disponibilita}
#define denu_modifica_menu\
    enum enu_modifica_menu {\
    p_id_listino=0,p_id_istanza,p_id_istanza_padre,p_stato,p_locazione,p_disposizione,p_geometria,p_descrizione,p_descr_default,p_colore,p_icona,p_pixmap}
#define denu_elenco_varianti\
    enum enu_elenco_varianti {\
    p_id_istanza=0,p_codice,p_prezzo_base,p_descrizione,p_ha_figli,p_tipo_gruppo,p_tipo_raggruppamento,p_id_articolo,p_id_padre,p_id_listino,p_iva,p_reparto}
#define denu_abilita_modulo\
    enum enu_abilita_modulo {\
    p_modulo_abilitato=0}
#define denu_ha_varianti\
    enum enu_ha_varianti {\
    p_id_gruppo_varianti=0}
#define denu_elenco_gruppo\
    enum enu_elenco_gruppo {\
    p_id_istanza=0,p_codice,p_prezzo_base,p_descrizione,p_ha_figli,p_tipo_gruppo,p_tipo_raggruppamento,p_id_articolo,p_id_padre,p_id_listino,p_iva,p_reparto}
#define denu_elenco_messaggi\
    enum enu_elenco_messaggi {\
    p_id_istanza=0,p_codice,p_prezzo_base,p_descrizione,p_ha_figli,p_tipo_gruppo,p_tipo_raggruppamento,p_id_articolo,p_id_padre,p_id_listino,p_iva,p_reparto}
#define denu_elenco_prenotazioni\
    enum enu_elenco_prenotazioni {\
    p_id_prenotazione=0,p_id_tavolo,p_tipo,p_nome_cliente,p_data_ora,p_preavviso,p_note,p_coperti,p_stato,p_libera_dopo,p_id_associato}
#define denu_elenco_tipi_conto\
    enum enu_elenco_tipi_conto {\
    p_id_documento=0,p_nome_stampante,p_tipo_chiusura}
#define denu_inserisci_articolo_barcode\
    enum enu_inserisci_articolo_barcode {\
    p_id_riga=0,p_cod_articolo_padre,p_descrizione,p_prezzo_calcolato,p_subtotale_comanda}
#define denu_elenca_barcode\
    enum enu_elenca_barcode {\
    p_barcode=0}
#define denu_elenco_gruppi_stampa\
    enum enu_elenco_gruppi_stampa {\
    p_id_gruppo=0,p_descrizione_gruppo,p_ordine}
#define denu_modifica_gruppo_stampa\
    enum enu_modifica_gruppo_stampa {\
    p_id_gruppo=0}
#define denu_elenco_associazioni_gruppi_stampa\
    enum enu_elenco_associazioni_gruppi_stampa {\
    p_id_istanza=0,p_codice,p_descrizione,p_ha_figli,p_tipo_gruppo,p_id_articolo,p_id_padre,p_id_gruppo_stampa,p_descr_gruppo_stampa}
#define denu_elenco_immagine_articolo\
    enum enu_elenco_immagine_articolo {\
    p_img_data_hex_encoded=0}
#define denu_identifica_tag\
    enum enu_identifica_tag {\
    p_sn=0,p_rftag,p_famiglia,p_tipo,p_id_p0,p_id_p1,p_id_t0,p_id_t1,p_id_t2,p_pagamenti,p_valore_minimo,p_valore,p_valore_massimo,p_punteggio_minimo,p_punteggio_attuale,p_punteggio_massimo,p_scadenza,p_nota_riferimento,p_sconto_percentuale}
#define denu_elenco_tag\
    enum enu_elenco_tag {\
    p_sn=0,p_rftag,p_famiglia,p_tipo,p_id_p0,p_id_p1,p_id_t0,p_id_t1,p_id_t2,p_pagamenti,p_valore_minimo,p_valore,p_valore_massimo,p_punteggio_minimo,p_punteggio_attuale,p_punteggio_massimo,p_scadenza,p_nota_riferimento,p_ragione_sociale,p_sconto_percentuale}
#define denu_modifica_ricetta\
    enum enu_modifica_ricetta {\
    p_qta=0,p_id_istanza_base,p_um,p_fattore,p_mm,p_iva}
#define denu_modifica_giacenza_magazzino\
    enum enu_modifica_giacenza_magazzino {\
    p_giacenza_iniziale=0,p_soglia_scorta_esaurita,p_soglia_scorta_in_esaurimento}
#define denu_elenco_ricetta\
    enum enu_elenco_ricetta {\
    p_qta=0,p_id_istanza_base,p_descrizione_istanza_base,p_um,p_fattore,p_mm,p_iva}
#define denu_egm\
    enum enu_egm {\
    p_id_articolo=0,p_cod_articolo,p_descrizione}
#define denu_egm_esteso\
    enum enu_egm_esteso {\
    p_id_articolo=0,p_cod_articolo,p_descrizione,p_um,p_mm,p_iva,p_albero}
#define denu_inserisci_fornitore\
    enum enu_inserisci_fornitore {\
    p_codice_fornitore=0}
#define denu_elenco_fornitori\
    enum enu_elenco_fornitori {\
    p_id_fornitore=0,p_cod_fornitore,p_ragione_sociale}
#define denu_elenco_fornitori_esteso\
    enum enu_elenco_fornitori_esteso {\
    p_id_fornitore=0,p_cod_fornitore,p_ragione_sociale,p_via,p_citta,p_cap,p_telefono,p_fax,p_p_iva,p_c_fiscale,p_data_inserimento,p_data_modifica,p_data_ultima_fattura,p_pagamenti_preferiti}
#define denu_elenco_tipi_doc_magazzino\
    enum enu_elenco_tipi_doc_magazzino {\
    p_id_doc=0,p_descr}
#define denu_elenco_conv\
    enum enu_elenco_conv {\
    p_id_art=0,p_descr,p_fattore}
#define denu_elenco_doc_fornitore\
    enum enu_elenco_doc_fornitore {\
    p_id_fornitore=0,p_id_doc,p_data_doc,p_cod_doc,p_causale_doc,p_num_tipo_doc,p_desc_tipo_doc,p_tipo_doc,p_cameriere,p_totale_doc}
#define denu_elenco_magazzini\
    enum enu_elenco_magazzini {\
    p_id_magazzino=0,p_descr_magazzino}
#define denu_numera_doc_magazzino\
    enum enu_numera_doc_magazzino {\
    p_numero_tipo_doc=0}
#define denu_apri_doc_magazzino\
    enum enu_apri_doc_magazzino {\
    p_id_doc=0}
#define denu_modifica_riga_doc_magazzino\
    enum enu_modifica_riga_doc_magazzino {\
    p_id_riga=0,p_totale_documento}
#define denu_elenco_righe_doc_magazzino\
    enum enu_elenco_righe_doc_magazzino {\
    p_id_riga_doc=0,p_tipo_movimento,p_id_articolo,p_id_magazzino,p_quantita,p_desc_confez_carico,p_fattore_confez,p_cameriere,p_codice,p_descrizione,p_prezzo_unitario,p_prezzo_totale,p_iva}
#define denu_elenco_testa_doc_magazzino\
    enum enu_elenco_testa_doc_magazzino {\
    p_id_forn_cli=0,p_data_doc,p_cod_doc,p_numero_tipo_doc,p_causale_doc,p_desc_tipo_doc,p_tipo_doc,p_cameriere,p_totale_documento}
#define denu_elenco_giacenza_art_magazzino\
    enum enu_elenco_giacenza_art_magazzino {\
    p_id_articolo=0,p_id_magazzino,p_giacenza_iniziale,p_valore_giacenza_iniziale,p_data_giacenza_iniziale,p_giacenza_finale,p_valore_giacenza_finale,p_prezzo_medio_acquisto,p_soglia_scorta_esaurita,p_soglia_scorta_in_esaurimento,p_mm}
#define denu_elenco_giacenze_magazzino\
    enum enu_elenco_giacenze_magazzino {\
    p_gruppo=0,p_id_articolo,p_id_magazzino,p_giacenza_iniziale,p_valore_giacenza_iniziale,p_data_giacenza_iniziale,p_giacenza_finale,p_valore_giacenza_finale,p_prezzo_medio_acquisto,p_soglia_scorta_esaurita,p_soglia_scorta_in_esaurimento,p_mm}
#define denu_elenco_movimenti_art_magazzino\
    enum enu_elenco_movimenti_art_magazzino {\
    p_id_doc=0,p_id_forn_cli,p_rag_soc_forn_cli,p_num_doc_forn,p_num_doc_tipo,p_causale,p_tipo,p_data,p_id_articolo,p_id_magazzino,p_id_riga_doc,p_tipo_movimento,p_quantita_carico,p_quantita_scarico,p_mm,p_descrizione,p_prezzo_unitario,p_prezzo_totale,p_iva,p_operatore}
#define denu_ultimo_prezzo_acquisto\
    enum enu_ultimo_prezzo_acquisto {\
    p_ultimo_prezzo_acquisto=0,p_id_fornitore,p_ragione_sociale,p_data_fornitura,p_quantita_totale}
#define denu_elenco_prezzi_forniture\
    enum enu_elenco_prezzi_forniture {\
    p_prezzo_unitario=0,p_id_fornitore,p_ragione_sociale,p_data_fornitura}
#define denu_elenco_periodi_magazzino\
    enum enu_elenco_periodi_magazzino {\
    p_id_periodo=0,p_descrizione,p_data}
#define denu_elenco_valore_art_magazzino\
    enum enu_elenco_valore_art_magazzino {\
    p_descr_art=0,p_pma,p_qta_tot,p_pmv,p_rv,p_(null),p_rv_perc,p_rv_tot}
#define denu_codifica_dati_stampante_client\
    enum enu_codifica_dati_stampante_client {\
    p_dati=0}
#define denu_inserisci_sottocliente\
    enum enu_inserisci_sottocliente {\
    p_id_sottocliente=0}
#define denu_modifica_sottocliente\
    enum enu_modifica_sottocliente {\
    p_id_cliente_padre=0,p_id_sottocliente,p_nome,p_cognome,p_matricola,p_cf,p_data_nascita}
#define denu_elenco_sottoclienti\
    enum enu_elenco_sottoclienti {\
    p_id_cliente_padre=0,p_id_sottocliente,p_nome,p_cognome,p_matricola,p_cf,p_data_nascita}
#define denu_elenco_sottoclienti_esteso\
    enum enu_elenco_sottoclienti_esteso {\
    p_id_cliente_padre=0,p_id_sottocliente,p_nome,p_cognome,p_matricola,p_cf,p_data_nascita}
#define denu_elenco_descrizioni_articolo\
    enum enu_elenco_descrizioni_articolo {\
    p_id_descrizione=0,p_desc_descrizione,p_id_lingua,p_descrizione}
#define denu_statistiche_sale\
    enum enu_statistiche_sale {\
    p_intestazione=0,p_coperti_ultima_chiusura,p_coperto_medio,p_tavoli_aperti_per_sala,\
    p_voce=0,p_quantita,p_totale_incassato}
#define denu_statistiche_sale_tavoli\
    enum enu_statistiche_sale_tavoli {\
    p_nome_tavolo=0,p_totale_ordinato,p_totale_venduto,p_totale_coperti,p_num_conti,p_sec_apertura}
#define denu_elenco_data_ultima_fattura\
    enum enu_elenco_data_ultima_fattura {\
    p_data_ultima_fattura=0}
#define denu_cerca_articolo_barcode\
    enum enu_cerca_articolo_barcode {\
    p_id_istanza=0}
#define denu_statistiche_tavoli_sala\
    enum enu_statistiche_tavoli_sala {\
    p_tot_odierno=0,p_tot_cop_odierno,p_tot_attuale_ordinato,p_tot_cop_attuale_ordinato,p_cop_med_odierno,p_cop_med_attuale_ordinato,p_cop_med_globale,p_tot_in_cassa,p_tot_coperti,\
    p_tavolo=0,p_aperto_da,p_non_servito_da,p_cameriere,p_voto,p_num_coperti,p_totale_tavolo,p_totale_ultimo_conto_f,p_totale_ultimo_conto_nf,p_totale_odierno_f,p_totale_odierno_nf,p_totale_coperti_f,p_totale_coperti_nf}
#define denu_elenco_voti\
    enum enu_elenco_voti {\
    p_id=0,p_descrizione,p_soglia,p_pixmap}
#define denu_elenco_turni\
    enum enu_elenco_turni {\
    p_id=0,p_descrizione}
#define denu_statistiche_globali_sala\
    enum enu_statistiche_globali_sala {\
    p_id_sala=0,p_nome_sala,p_data,p_id_turno,p_nome_turno,p_cop_tot,p_tot_cop,p_cop_med,p_tot_docs}
#endif
// enums
