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

#include "tr_menu_dim.hpp"

#include "tr_enumeratori.hpp"
#include "cmdcodicigruppi.hpp"
#include "tr_config.hpp"
#include "tr_tools.hpp"
#include <qimage.h>
#include <cmath>

extern tr_config config;
extern tr_tools tools;

tr_menu_dim* tr_menu_dim::top_owner=NULL;
tr_menu_dim* tr_menu_dim::last_inside=NULL;
tr_menu_button* tr_menu_dim::last_path=NULL;
/******************************************************************************/
tr_menu_button::tr_menu_button(tr_menu_dim * parent, QString name, WFlags f ):
  tr_label((QWidget*)parent,name,f)
{
  tipo=t_nondefinito;idx=-1;
}

/******************************************************************************/

void tr_menu_button::setidx(int i)
{
  tr_menu_dim *p=(tr_menu_dim*)parent();
  idx=i;
  if (p && idx<p->voci.count()) {
    QStringList voce=QStringList::split('|',p->voci[idx],true);
    denu_elenco_menu;
    unsigned int stato=voce[p_stato].toUInt();
    if ((stato & GRUPPO_ARTICOLO_SELEZIONE) && tipo==t_nondefinito) tipo=t_corolla;
    settipo(gettipo());
    // i_id_listino,i_id_istanza,i_id_istanza_padre,i_stato,i_locazione,i_disposizione,i_geometria,s_descrizione,s_descr_default,s_colore,s_icona,s_pixmap
  }
} // setidx(...)

/******************************************************************************/

void tr_menu_button::settipo(tr_tipo tp)
{
  tipo=tp;
  QColor c;
  QFont pfont( font() );
  pfont.setPixelSize( 12 );
  pfont.setFamily( "Helvetica [Urw]" );
  bool bold=false;
  switch (tipo) {
    case t_nondefinito:
      c=config.menu_elementi_colore;
    break;
    case t_corolla:
      c=config.menu_corolla_colore;
      bold=true;
    break;
    case t_elemento:
      c=config.menu_elementi_colore;
    break;
    case t_next_corolla:
      c=config.menu_pulsanti_colore;
      setPixmap( PIXMAP( "az_dx.png" ) );
      setName("az_cor");
    break;
    case t_next_elementi:
      c=config.menu_pulsanti_colore;
      setPixmap( PIXMAP( "az_giu.png" ) );
      setName("az_ele");
    break;
    case t_path_main:
      c=config.menu_percorso_colore.light( 100 );
      setText( "MAIN<br>" /*+QSN( indici[corolla].pagina+1 )+","
           +QSN( indici[elementi].pagina+1 )*/ );
    break;
    case t_path:
      c=config.menu_percorso_colore.light( 100 );
    break;
  }

  pfont.setBold( bold );
  setFont( pfont );

  setPaletteForegroundColor( QColor( 0, 0, 127 ) );
  setPaletteBackgroundColor(c);
} // settipo(...)

/******************************************************************************/

void tr_menu_button::settipo(char t)
{
  switch (t) {
    case '1':
      settipo(tr_menu_button::t_corolla);
      aspetto_bottone();
      break;
    case '2':
      settipo(tr_menu_button::t_next_corolla);
    break;
    case '3':
      settipo(tr_menu_button::t_next_elementi);
    break;
    case '4':
      settipo(tr_menu_button::t_elemento);
    break;
    case '5':
      settipo(tr_menu_button::t_path);
    break;
    case '6':
      settipo(tr_menu_button::t_path_main);
    break;
  } // switch
}

/******************************************************************************/
/*      TR_MENU_DIM                                                           */
/******************************************************************************/

tr_menu_dim::tr_menu_dim( QWidget * parent, const char * name, WFlags f ):
  tr_frame(parent,name,f),bsize(0,0),spessore(2)
{
    cache=false;
    percorso=NULL;
    articoli=NULL;
    if (qstrcmp(parent->className(),"tr_menu_dim")!=0) top_owner=(tr_menu_dim*)this;
} // tr_menu_dim


/******************************************************************************/

/******************************************************************************/
// struttura il widget secondo struttura e restituisce il puntatore alla
// eventuale sezione interna
tr_menu_dim* tr_menu_dim::build(QString struttura)
{

  if (layout())
    delete layout();  // se e' una ricostruzione per cambio struttura
  // dovrebbe cancellare tutto il contenuto

  QStringList righe=QStringList::split('\n',struttura,true);
  uint cols=0; int rows=righe.count();

  QVariant pana_xb,pana_yb,pana_xe=0,pana_ye=0;

  // determina dimensione in rihe e colonne e
  // occupazione del pannello articoli
  bool there_is_inside=false;
  int path_len=0;
  int path_pos=-1;
  enum {hor,hor_again,vert} path_dir=hor;
  TROWSLI(righe) {
    QString riga=righe[i];
    if (riga.length()) {
      if (cols<riga.length()) cols=riga.length();
      for (uint j=0;j<riga.length();j++) {
        bool inside=false;
        if (riga[j]=='i') {inside=true;there_is_inside=true;}
        if (riga[j]=='5') {
          path_len++;
          if (path_pos==-1 && path_dir==hor) path_pos=j;
          if (path_len>1 && path_dir==hor_again) {
            LOG(tr("Struttura menu' non regolare. Percorso sparso."));
            return NULL;
          }
          if (path_dir==hor_again) path_dir=vert;
          if (path_pos==-1 && path_dir==vert) path_pos=i;
        }
        if (inside && pana_yb.isNull()) pana_yb=i;
        if (inside && pana_xb.isNull()) pana_xb=j;
        if (inside && pana_xe.asUInt()<j) pana_xe=j;
        if (inside && pana_ye.asUInt()<i) pana_ye=i;
      }
      if (path_len) path_dir=hor_again;
    } // if riga.length
  } // TROWSLI

  // N.B. dovra' essere specializzato anche per sezioni orizzontali
  QGridLayout*lyt=NULL;
  QVBoxLayout*vlyt=NULL;
  QHBoxLayout*hlyt=NULL;
  tr_menu_dim *area;
  if (path_len && there_is_inside) {
    if (path_dir==hor_again) vlyt=new QVBoxLayout(this);
    if (path_dir==vert) hlyt=new QHBoxLayout(this);
    area=new tr_menu_dim(this,"path");
    if (path_dir==hor_again) {
      if (path_pos==0) vlyt->addWidget(area,0,Qt::AlignTop);
      else vlyt->addWidget(area,0,Qt::AlignBottom);
      lyt=new QGridLayout(area, path_pos,path_len);
    }
    if (path_dir==vert) {
      if (path_pos==0) hlyt->addWidget(area,0,Qt::AlignLeft);
      else hlyt->addWidget(area,0,Qt::AlignRight);
      lyt=new QGridLayout(area, path_len,path_pos);
    }
  }
  else
  {
    area=this;
    lyt=new QGridLayout(this, rows,cols);
  }

  area->page_cor_size=0;
  area->page_ele_size=0;
  int n_adiacenti=0;
  TROWSLJ(righe) {
    QString riga=righe[j];
    QSize bcor; // usato nel dimensionamento dei pulsanti del percorso
    for (uint i=0;i<riga.length();i++) {
      bool inside=false;
      if (riga[i]=='i') inside=true;
      if ( !inside ) {
        // tr_menu_button *b=new tr_menu_button(this,QSN(contatore_puls_corolla));
        QString nome;
        if (riga[i]=='1') nome="cor_"+QSN(area->page_cor_size++);
        if (riga[i]=='-') n_adiacenti++;
        if (riga[i]=='4') nome="ele_"+QSN(area->page_cor_size++);
        if (riga[i]=='5') {
          nome="per_"+QSN(area->page_cor_size++);
          if (bcor.isEmpty()) {
            tr_menu_button *b=(tr_menu_button*)parent()->child("cor_0");
            if (b) bcor=b->size();
          }
        } // riga=5
        if (!n_adiacenti || !nome.isNull()) {
          tr_menu_button *b=new tr_menu_button(area,nome);
          connect(b,SIGNAL(clicked()),(QObject*)top_owner,SLOT(button_clicked()));
          if (!bsize.isEmpty() && riga[i]!='4')
            b->setFixedSize(bsize);
          if (!bcor.isEmpty() && riga[i]=='5')
            b->setFixedSize(bcor);
          if (!n_adiacenti) lyt->addWidget(b,j,i);
          else {
            lyt->addMultiCellWidget(b,j,j,i-n_adiacenti,i);
          }
          QChar c=riga[i];
          b->settipo(c);
          if ( spessore && config.stile_menu==2 )
            b->aspetto_bottone( spessore );
          else
            b->aspetto_bottone();
          b->pulsante_animato();
          if (riga[i]=='5' && i==0) b->setText( "MAIN<br>(1,1)" );
          palette_dark(b);
          b->show();
          n_adiacenti=0;
        }
      } // if
    } // for
  } //  TROWSLJ
  if (pana_xb.isValid() && pana_yb.isValid() && pana_xe.isValid() && pana_ye.isValid()) {
    QString nome="con_";
    nome+=QSN(QString(name()).mid(4).toInt()+1);
    last_inside=new tr_menu_dim(this,nome);
    if (path_len && there_is_inside) {
      if (!path_pos) vlyt->addWidget(last_inside);
      else vlyt->insertWidget(0,last_inside);
    }
    else
      lyt->addMultiCellWidget(last_inside,pana_yb.asInt(),pana_ye.asInt(),pana_xb.asInt(),pana_xe.asInt());
    return last_inside;
  }
  return NULL;
} // build(...)

/******************************************************************************/

bool tr_menu_dim::carica(QString id_listino)
{
  denu_elenco_menu_listino;
  tr::mse menu(tok_elenco_menu_listino,id_listino);
  voci=menu.elenco;

  page_cor=0;
  page_ele=0;

  tr_menu_button *mb;
  int idx=0;
  while (idx<voci.count()) {
    int locazione=voci[idx].section('|',p_locazione,p_locazione).toInt();
    if ( locazione==tr::menu::voce::loc_elementi ) {idx++;continue;}
    int disposizione=voci[idx].section('|',p_disposizione,p_disposizione).toInt();
    disposizione-=page_cor_size*page_cor;
    if (disposizione>=page_cor_size) break;
    QString nome="cor_" + QSN(disposizione);
    tr_menu_button *mb=(tr_menu_button*)this->child(nome,"tr_menu_button",false);
    if (!mb) {
      cout << "Non ho trovato corrispondenza per " << voci[idx] << endl;
    }
    else
    {
      QString icona=voci[idx].section('|',p_icona,p_icona);
      QColor c(voci[idx].section('|',p_colore,p_colore));
      QString pixmap=voci[idx].section('|',p_pixmap,p_pixmap);
      uint tipo=voci[idx].section('|',p_stato,p_stato).toUInt();
      if ( c.isValid() && c!=QColor(0,0,0) && pixmap.isEmpty() ) {
        if (tipo & (GRUPPO_ARTICOLO_VARIANTE|GRUPPO_ARTICOLO_COMMENTO)
          == (GRUPPO_ARTICOLO_VARIANTE|GRUPPO_ARTICOLO_COMMENTO) ) {
            c.dark(130);
        }
        mb->setPaletteBackgroundColor(c);
      }
      QString st=voci[idx].section('|',p_descrizione,p_descrizione);

      if ( !icona.isEmpty() ) {
        st=st.upper();
        st=tools.solo_testo( st );
        st.truncate( 8 );
        if (QMimeSourceFactory::defaultFactory() &&
            QMimeSourceFactory::defaultFactory()->data(icona))
        // if ( voce->icona.left( 3 )=="az_" )
          st.prepend( "<img src=\"" +icona+"\" width=\"32\" height=\"32\"><br><small>" );
        else {
          st.prepend( "<img src=\"" +config.exe_path+"icone/" +icona+"\" width=\"32\" height=\"32\"><br><small>" );
        }
        st+="</small>";
      } // if icona

      if (pixmap.isEmpty()) pixmap="sfondo_pulsante_menu_c.jpg";

      if (!pixmap.isEmpty()) {
        QImage im;
        im=PIXMAP( pixmap ).convertToImage();
        QSize s=mb->size();
        QImage ims=im.smoothScale(s);
        QPixmap pix(ims); // .convertFromImage(ims);
        mb->setPaletteBackgroundPixmap( pix );
      }

      mb->setText(st);
      mb->idx=idx;

    } // if mb
    idx++;
  }

} // carica(...)

/******************************************************************************/

void tr_menu_dim::button_clicked()
{
  /* funziona in due modalita':
    1. in modalite' edit se la modalita' edit e' attiva
       Evidenzia il pulsante. Se il pulsante contiene un gruppo ed e' di
       corolla entra subito, altrimenti entra solo al secondo clic
    2. in modalit' uso
      Il pulsante pue' essere un pulsante di corolla, percorso o articolo
      e' si distingue per l'iniziale del nome (cor_,per_,art_,az_)
      L'indice di riferimento della voce di mene' sta in idx
  */
  tr_menu_button *mb=(tr_menu_button*)sender();
  if (mb->tipo==tr_menu_button::t_next_corolla) {page_cor++;aggiorna();return;}
  if (mb->tipo==tr_menu_button::t_next_elementi) {page_ele++;aggiorna();return;}
  cd(mb->idx);
}

/******************************************************************************/

bool tr_menu_dim::cd(int idx)
{
  if (idx>-1) { // se punta a una voce
    denu_elenco_menu_listino;
    uint stato=voci[idx].section('|',p_stato,p_stato).toUInt();
    if (stato & GRUPPO_ARTICOLO_SELEZIONE) {
      tr_menu_dim *path;
      if (last_path) path=(tr_menu_dim*)last_path->parent();
      else path=(tr_menu_dim*)top_owner->child("path");
      if (path) {
        if (!last_path) last_path=(tr_menu_button*)path->child("per_1");
        else {
          QString nome="per_"+QSN(QString(last_path->name()).mid(4).toInt()+1);
          last_path=(tr_menu_button*)path->child(nome);
        }
      }

      if (!last_path) idx=-1;
      else {
        // verifica se la voce e' un sotto gruppo del cruppo corrente
        int n=QString(last_path->name()).mid(4).toInt();
        // if (n>0)
        last_path->setText(voci[idx].section('|',p_descrizione,p_descrizione));
      }

      aggiorna(idx);

    }
  } // if idx>-1
} // funct

/******************************************************************************/
/* prende l'ultimo pulsante in last_path
   Se last_path e' nullo idx varra' probabilmente qualcosa. Si tratta di un
   menu' senza path */
void tr_menu_dim::aggiorna(int idx)
{
  if (idx==-1 && !last_path) {
    LOG(tr("Nessun menu' da visualizzare"));
    return;
  }
  if (idx==-1 && last_path) idx=last_path->idx;

  QStringList e=QStringList::split('|',voci[idx],true);
  denu_elenco_menu_listino;
  uint stato=e[p_stato].toUInt();
  if (stato & GRUPPO_ARTICOLO_SELEZIONE) {
    int geometria=e[p_geometria].toUInt();
    int dx=int(sqrt(double(geometria)));
    int dy=geometria / dx;
    QString str="";
    FORI( dy ) {
      FORK( dx ) str+='4';
      str+="\n";
    }
    last_inside->build(str);
  } // if (stato...
} // aggiorna
