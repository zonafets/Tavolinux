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

#ifndef TR_DMACRO_H
#define TR_DMACRO_H

#include <qvariant.h>
#include <qstring.h>

// se viene incluso questo file viene attivata la gestione della memoria
// per la release version, commentare la seguente define

#define QSNull QString::null

#define LOGOUT "logout"
#define JPG(x) x##".jpg"

#define CONNECT(x) connect(x,SIGNAL(clicked()), this, SLOT(x##_clicked()))
#define LINK( widget, slot ) connect( widget, SIGNAL(clicked()), this, SLOT( slot ))

// memory check
#include "tr_mem_chk.hpp"

extern tr_mem_chk mem_chk;

#include <new> // Size_t definition
#include <iostream>

inline bool ins(void*p, const char *fname=__FILE__ , const int line=__LINE__ ) {
    return mem_chk.ins(p,fname,line);
}

inline bool del(void*p, const char *fname=__FILE__ , const int line=__LINE__ ) {
    return mem_chk.del(p,fname,line);
}

//#define NEW(variabile, classe_e_parametri) variabile = new classe_e_parametri;mem_chk.ins(variabile, PF,__LINE__)
#define NEW(variabile, classe_e_parametri) variabile = new classe_e_parametri;\
ins(variabile,__FILE__,__LINE__)
#define DELETE(variabile) delete variabile;\
del(variabile,__FILE__,__LINE__);
#define DELETE_ARRAY(variabile) delete [] variabile;\
del(variabile,__FILE__,__LINE__);

using namespace std;

#include <qstring.h>
#include <qevent.h>


inline int simpleMemStats() {return 0;}
inline void showMemStats() {}

#define mem_check_begin() \
mem_chk.group();

#define mem_check_end(classe) \
mem_chk.ungroup(__FILE__,__LINE__);

// macro comando di comodita'
#define COUNT(x)	((int)(sizeof(x)/sizeof(x[0])))
#define FORI(limite) for (int i=0;i<limite;i++)
#define FORUI(limite) for (uint i=0;i<limite;i++)
#define FORJ(limite) for (int j=0;j<limite;j++)
#define FORUJ(limite) for (uint j=0;j<limite;j++)
#define FORK(limite) for (int k=0;k<limite;k++)
#define FORUK(limite) for (uint k=0;k<limite;k++)
#define FORX(limite) for (int x=0;x<limite;x++)
#define FORUX(limite) for (uint x=0;x<limite;x++)
#define FORY(limite) for (int y=0;y<limite;y++)
#define FORUY(limite) for (uint y=0;y<limite;y++)
#define FORYX(limiteJ, limiteI) \
  for (int y=0;y<limiteJ;y++) \
  for (int x=0;x<limiteI;x++)
#define FORUYX(limiteJ, limiteI) \
  for (uint y=0;y<limiteJ;y++) \
  for (uint x=0;x<limiteI;x++)
#define FORJI(limiteJ, limiteI) \
  for (int j=0;j<limiteJ;j++) \
  for (int i=0;i<limiteI;i++)
#define FORUJI(limiteJ, limiteI) \
  for (uint j=0;j<limiteJ;j++) \
  for (uint i=0;i<limiteI;i++)
#define TROWI(array) for (uint i=0;i<COUNT(array);i++)
#define TROWJ(array) for (uint j=0;j<COUNT(array);j++)
#define TROWk(array) for (uint k=0;k<COUNT(array);k++)
#define TROWSLI(stringlist) for (uint i=0;i<stringlist.size();i++)
#define TROWSLJ(stringlist) for (uint j=0;j<stringlist.size();j++)
#define TROWSLK(stringlist) for (uint k=0;k<stringlist.size();k++)
#define TROWSLX(stringlist) for (uint x=0;x<stringlist.size();x++)
#define TROWSLY(stringlist) for (uint y=0;y<stringlist.size();y++)
#define TROW(array, idx) for (uint idx=0;idx<COUNT(array);idx++)

inline QString value_of(const QStringList &s, QString key)
{
  // key=key.mid(1,key.length()-2);
  // QD(key);
  TROWSLJ(s) {
    // QD(s[j].left(key.length()));
    if (s[j].left(key.length())==key) return s[j].mid(key.length());
  }
  return "=N.R.";
}

class tr_event: public QEvent {
public:
    tr_event(QEvent *e):QEvent(e->type()) {}
    QString name() {
        Type num_evs[] =  { None , Timer , MouseButtonPress , MouseButtonRelease ,
                            MouseButtonDblClick , MouseMove , KeyPress ,
                            KeyRelease , FocusIn , FocusOut , Enter , Leave , Paint ,
                            Move , Resize , Create , Destroy , Show , Hide , Close ,
                            Quit , Reparent , ShowMinimized , ShowNormal , WindowActivate ,
                            WindowDeactivate , ShowToParent , HideToParent , ShowMaximized ,
                            ShowFullScreen , Accel , Wheel , AccelAvailable , CaptionChange ,
                            IconChange , ParentFontChange , ApplicationFontChange , ParentPaletteChange ,
                            ApplicationPaletteChange , PaletteChange , Clipboard , Speech , SockAct ,
                            AccelOverride , DeferredDelete , DragEnter , DragMove , DragLeave , Drop ,
                            DragResponse , ChildInserted , ChildRemoved , LayoutHint , ShowWindowRequest ,
                            ActivateControl , DeactivateControl , ContextMenu , IMStart , IMCompose ,
                            IMEnd , Accessibility , TabletMove , LocaleChange , LanguageChange ,
                            LayoutDirectionChange  , Style , TabletPress , TabletRelease ,
                            User , MaxUser  };
        QString ev_names[] = { "None", "Timer" , "MouseButtonPress" , "MouseButtonRelease" ,
                               "MouseButtonDblClick" , "MouseMove" , "KeyPress" ,
                               "KeyRelease" , "FocusIn" , "FocusOut" , "Enter" , "Leave" ,
                               "Paint" ,
                               "Move" , "Resize" , "Create" , "Destroy" , "Show" , "Hide" , "Close" ,
                               "Quit" , "Reparent" , "ShowMinimized" , "ShowNormal" , "WindowActivate" ,
                               "WindowDeactivate" , "ShowToParent" , "HideToParent" , "ShowMaximized" ,
                               "ShowFullScreen" , "Accel" , "Wheel" , "AccelAvailable" , "CaptionChange" ,
                               "IconChange" , "ParentFontChange" , "ApplicationFontChange" , "ParentPaletteChange" ,
                               "ApplicationPaletteChange" , "PaletteChange" , "Clipboard" , "Speech" , "SockAct" ,
                               "AccelOverride" , "DeferredDelete" , "DragEnter" , "DragMove" , "DragLeave" ,
                               "Drop" ,
                               "DragResponse" , "ChildInserted" , "ChildRemoved" , "LayoutHint" , "ShowWindowRequest" ,
                               "ActivateControl" , "DeactivateControl" , "ContextMenu" , "IMStart" , "IMCompose" ,
                               "IMEnd" , "Accessibility" , "TabletMove" , "LocaleChange" , "LanguageChange" ,
                               "LayoutDirectionChange"  , "Style" , "TabletPress" , "TabletRelease" ,
                               "User" , "MaxUser"  };
        TROWI(num_evs) if (num_evs[i]==type()) return ev_names[i];
        return "???";
    }
};



#endif
