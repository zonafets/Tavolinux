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

#include <stdio.h>
#include <qstring.h>
#include <qstringlist.h>

#define CMDSRV_LANGUAGE
#include "cmdlangtoken.hpp"

int main()
{
    FILE *f=fopen("./tr_enumeratori.hpp","w+");
    // fprintf(f,"// file generato automaticamente\n\n#ifndef TR_ENUMERATORI_HPP\n#define TR_ENUMERATORI_HPP\n\nclass enums {\npublic:\n\n");
    fprintf(f,"// file generato automaticamente\n\n#ifndef TR_ENUMERATORI_HPP\n#define TR_ENUMERATORI_HPP\n\n");
	unsigned int i;
    for (i=0;i<tok_endoftokens;i++) {
	printf("Processo istr:%s",comandi[i].istruzione.ascii());
	if (!comandi[i].output.isEmpty() /*&& comandi[i].istruzione.left(6)=="ELENCO"*/) {
	    // fprintf(f,"\n  class enu_%s {\n",comandi[i].istruzione.lower().ascii());
	    // fprintf(f,"    public:\n");
	    fprintf(f,"\n#define denu_%s\\\n    enum enu_%s {\\\n    ",comandi[i].istruzione.lower().ascii(),comandi[i].istruzione.lower().ascii());
	    
	    QStringList os=QStringList::split(',',comandi[i].output,true);
	    bool enum_opened=false;
	    int k=0;
	    for (unsigned int j=0;j<os.count();j++) {
		// printf("%s\n",os[j].ascii());
		if (os[j]=="r" || os[j]=="e") {
		    // if (enum_opened) fprintf(f,"};\\\n");
		    if (enum_opened) fprintf(f,",\\\n    ");
		    enum_opened=true;
		    k=0;
		}
		else {
		    QString st=os[j].mid(2);
		    if (st.right(1)=="$") st.truncate(st.length()-1);
		    if (k==0) fprintf(f,"p_%s=0",st.ascii());
		    else fprintf(f,",p_%s",st.ascii());
		    k++;
		}
	    } // for j
	    if (enum_opened) fprintf(f,"}",comandi[i].istruzione.lower().ascii());
	    // if (enum_opened) fprintf(f,"};\n  }; // cmd_%s\n\n",comandi[i].istruzione.lower().ascii());
	    printf("--- elaborato\n");
	} // if !output
	else
	{
	    printf("--- NO OUTPUT\n");
	}
    } // for i
    
    // fprintf(f,"}; // enums\n#endif\n");
    fprintf(f,"\n#endif\n// enums\n");
    
    fclose(f);
return 0;
}
