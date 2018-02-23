#ifndef FRM_POCKET_CPP
#define FRM_POCKET_CPP

#include "frm_base.hpp"
#include "tr_listview.hpp"
#include "tr_comanda.hpp"
#include "tr_srvcmd.hpp"
#include "tr_menu.hpp"

void frm_pocket()
{
    
  config.elenchi_font_size=14;
  config.elenchi_abc_font_size=10;
  config.elenchi_button_size=32;
  config.altezza_riga=16;
  config.comanda_font_size=14;
  config.comanda_child_font_size=12;
  config.comanda_altezza_riga=16;
  config.comanda_totale_altezza_riga=22;
  config.comanda_totale_font_size=18;

  // config.cursor=false;
  config.stile_menu=3;
  config.menu_button_size_x=34;
  config.menu_button_size_y=30;
  config.menu_button_font_size=8;
  config.menu_testo_bold=false;
  config.finestra_larghezza_menu=32*7;
  config.finestra_altezza_centrale=30*9;
  config.finestra_altezza_stato=26; // pocket status=26
  
  config.icon_scale_factor=0.75;
  
#define LOGIN "1|0|6|MANAGER"
  CMD(tok_login,QString(LOGIN));

#define TAV "39500" 
  CMD(tok_sblocca_tavolo,QString(TAV)); // 14
  CMD(tok_apri_tavolo,QString(TAV)); // 14

  QString res="240|320";
  if (!config.pocket.section('|',1,1).isEmpty()) res=config.pocket.section('|',1,1).replace('x','|');

  FRM_PARAMS;

  QString str_mnu=
  "minmax=%res%\n"
  "margine=0\n"
  "status=Menu\n"
  "extra=menu_old";

  QString str_cmd=
  "minmax=%res%\n"
  "margine=0\n"
  "status=Menu\n"
  "extra=comanda";

  QString str_ffm=
  "minmax=%res%\n"
  "margine=0\n"
  "listview=(tp:%tp%,abc,onclick)";

  FRM_PARAM( "%tp%", QSN( tr_listview::camerieri ) );
  FRM_PARAM( "%res%", res );


  bFORM( ffm, str_ffm, NULL );
  // ffm->move(0,20);
  CTRL_FRM ( ffm );
  CTRL(listview);

  bFORM( cmd, str_cmd, NULL );
  cmd->show();
  cmd->hide();

  bFORM( mnu, str_mnu, NULL );
  // cmd->move(0,20);
  tr_comanda*comanda=( tr_comanda* ) cmd->pointer( "comanda" );
  tr_menu*menu=(tr_menu*)mnu->pointer("menu_old");
  if (menu) {
    menu->uso();
    menu->cambia(1);
    mnu->show();
    mnu->hide();
  }

  ffm->interazione();
  ffm->hide();

  if (listview.value()=="7") {
    cmd->show();
    if (comanda) {
      comanda->aggiorna();
      comanda->show_levels();
    }
    cmd->interazione();
  }
  if (listview.value()=="8") {
    mnu->show();
    mnu->interazione();
  }


  xFORM( cmd, str_cmd );

  xFORM( mnu, str_mnu );

  xFORM( ffm, str_ffm );

  CMD(tok_chiudi_tavolo);

}

#endif
