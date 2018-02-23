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

#define QT_NO_ASCII_CAST
#include <qstring.h>
#include <qstringlist.h>
#include <stdio.h>
#include <string.h>
#include <qdict.h>

#define BREAKPOINT __asm__ __volatile__ ("int $3")

unsigned long crc_table[256];

QDict<int> ripetuti;

// Genera la tabella per il

void generaTabellaCRC() {
  // Generazione della tabella del polinomio

  unsigned long crc, poly = 0xEDB88320L;

  for ( int i = 0; i < 256; i++ ) {
    crc = i;
    for ( int j = 8; j > 0; j-- ) {
      if ( crc & 1 )
        crc = ( crc >> 1 ) ^ poly;
      else
        crc >>= 1;
    }
    crc_table[i] = crc;
  }

}

// CRC a 32 BIT del codice

QString calcolaCRC( QString stringa ) {

  // Calcolo del CRC a 32 BIT

  unsigned long crc = 0xFFFFFFFF;
  int ch;

  for ( int i = 0; i < ( int )stringa.length(); i++ ) {
    QChar c = stringa[i];
    ch = c;
    crc = ( crc >> 8 ) ^ crc_table[( crc ^ ch ) & 0xFF];
  }

   crc ^= 0xFFFFFFFF;
   QString st=QString::number(crc,16).upper();
   return st;
}

inline QString make_instr(QString st)
{
  st=st.stripWhiteSpace();
  if (st.isEmpty()) return "";
  QString st1="const char *msg";
  st1+=calcolaCRC(st)+"=QApplication::tr(\"" + st +"\");\n";
  return st1;
}

inline QString strip(QString instr)
{
  QString st=instr.stripWhiteSpace();
  int pos;
  if (st[0]=='!') st=st.section(':',1,1);
  if (st[0]=='?') st=st.section(':',1,1);
  pos=st.find('=');
  if (pos!=-1) st=st.mid(pos+1);
  pos=st.find('(');
  if (pos!=-1) st.truncate(pos);
  pos=st.find('#');
  if (pos!=-1) st.truncate(pos);
  pos=st.find("\\\\");
  if (pos!=-1) st=st.mid(pos+2);
  return st;
}

void parse( FILE * out, QString blocco ) {

  blocco.replace( "\\n", "\n" );
  printf("\nPROCESSO ** FORM **\n%s\n",blocco.ascii());

  QString cmd_list=
    "links|palette|minmax|margine|validatore|hole|events.ignore|"
    "barcodectrls|events|readonly|#|spazio|debug|remove|conds";
  QStringList cmds=QStringList::split('|',cmd_list);

  printf("----------------------- tokens\n");

  int fine_commenti=0;
  int inizio_commenti=blocco.find( "/*" );
  while ( inizio_commenti!=-1 ) {
    fine_commenti=blocco.find( "*/", inizio_commenti );
    if ( fine_commenti==-1 ) {
      printf("Errore nei commenti della form\n");
      break;
    }
    blocco.remove( inizio_commenti, fine_commenti-inizio_commenti+2 );
    inizio_commenti=blocco.find( "/*" );
  }

  QStringList instrs;
  QStringList righe = QStringList::split( '\n', blocco );
  if (righe.count()==1 && righe[0].find('|')!=-1 && righe[0].left(8)!="buttons=") righe[0].prepend("buttons=");
  for (unsigned int j=0;j<righe.count();j++) {
    QStringList colonne=QStringList::split(';',righe[j]);
    QString last_token="";
    printf("riga:%s\n",righe[j].ascii());
    for (unsigned int i=0;i<colonne.count();i++) {
      QString instr=colonne[i];
      if (instr.find("=")>-1) {
        last_token=instr.section('=',0,0);
        if (last_token[0]=='?') last_token=last_token.section(':',1,1);
        if (last_token[0]=='!') last_token=last_token.section(':',1,1);
      } else instr.prepend(last_token+"=");
      if (instr[0]=='?') instr=instr.mid(instr.find(':')+1);
      bool skip=false;
      for (unsigned int k=0;k<cmds.count();k++) if (last_token==cmds[k]) {skip=true;break;}
      if (!skip && !instr.isNull()) {
        printf("::%s\n",instr.ascii());
        QStringList sentences;
        if (instr.left(8)=="buttons=")
          sentences=QStringList::split('|',instr.section('=',1,1));
        else {
          sentences<<instr;
        }
        if (instr.left(9)=="listview=" && instr.find("header:")>-1) {
          QString st=instr.section("header:",1,1);
          int pos=st.find(',');
          if (pos==-1) pos=st.find(')');
          st.truncate(pos);
          QStringList hs=QStringList::split('|',st);
          for (unsigned int k=0;k<hs.count();k++) hs[k]=hs[k].section('!',0,0);
          sentences+=hs;
        }
        if (instr.left(6)=="label=" && instr.find("radio:")>-1) {
          QString st=instr.section("radio:",1,1);
          int pos=st.find(',');
          if (pos==-1) pos=st.find(')');
          st.truncate(pos);
          QStringList hs=QStringList::split('|',st);
          for (unsigned int k=0;k<hs.count();k++) if (hs[k][0]=='*') hs[k]=hs[k].mid(1);
          sentences+=hs;
        }
        for (unsigned int k=0;k<sentences.count();k++) {
          sentences[k]=strip(sentences[k]);
          if (!sentences[k].isEmpty()) {
            bool salta=false;
            if (sentences[k][0]=='%' &&
                sentences[k][sentences[k].length()-1]=='%') salta=true;

            if (ripetuti.find(sentences[k].stripWhiteSpace())==NULL && !salta) {
              ripetuti.insert(sentences[k].stripWhiteSpace(),(int*)-1);
              if (sentences[k]=="buttons") BREAKPOINT;
              QString st=make_instr(sentences[k]);
              if (!st.isEmpty()) fputs(st.ascii(),out);
            }
          }
        }
      }
    }
  }
}

int main( int argc, char * argv[] ) {
  printf( "TR_Form_EXTRACtor - estrattore testo di traduzione da progetto Qt\nRicerca testo compreso nelle macro TRFORM e TRBUTTONS. Crea un file chiamato tr_forms_text.hpp\n" );

  if ( argc < 2 ) {
    printf( "Usage:  trfextract project[.pro]\n" );
    exit( 1 );
  }

  generaTabellaCRC();

  QString name = argv[1];
  if ( name.right( 4 ).lower() != ".pro" ) name += ".pro";
  QString path = name;
  // printf("%s|slash a=%d\n", path.ascii(), name.findRev( "//" ) );
  path.truncate( name.findRev( "/" ) + 1 );

  printf( "Parser file %s\n", name.ascii() );
  FILE * pro = fopen( name.ascii(), "r" );
  if ( !pro ) {
    printf( "%s non trovato\n", name.ascii() );
    exit( 2 );
  }

#define read fgets( buffer,1024,pro );
  QStringList files;
  char buffer[1024];
  do read
    while ( strncmp( buffer, "SOURCES", 7 ) != 0 );

  bool primo = true;
  while ( buffer[strlen( buffer ) - 2] == '\\' ) {
    buffer[strlen( buffer ) - 2] = '\0';
    files << buffer;
    if ( primo ) {
      files.last() = files.last().section( '=', 1, 1 ); primo = false;
    }
    read;
  }
  files << buffer;

  do read
    while ( strncmp( buffer, "HEADERS", 7 ) != 0 );
  primo = true;
  while ( buffer[strlen( buffer ) - 2] == '\\' ) {
    buffer[strlen( buffer ) - 2] = '\0';
    files << buffer;
    if ( primo ) {
      files.last() = files.last().section( '=', 1, 1 ); primo = false;
    }
    read;
  }
  files << buffer;

  fclose( pro );
#undef read
#define read fgets( buffer,1024,src );

  QString outfile = path + "tr_forms_text.cpp";
  printf( "Creo file %s\n", outfile.ascii() );
  FILE * out = fopen( outfile.ascii(), "w+" );
  if ( !out ) {
    printf( "Non riesco ad aprire %s\n", outfile.ascii() );
    exit( 3 );
  }

  fputs("#include <qapplication.h>\n\n",out);

  unsigned int i = 0;
  while ( i < files.size() ) {
    files[i] = files[i].stripWhiteSpace();
    QString srcfile = path + files[i];
    printf( "Processo %s ---------------------------\n", srcfile.ascii() );
    FILE * src = fopen( srcfile.ascii(), "r" );
    i++;
    if ( !src ) {
      printf( "File %s non trovato\n", files[i].ascii() ); continue;
    }
    QString blocco;
    bool trovato=false;
    while ( !feof( src ) ) {
      read;
      QString st = ( char * ) buffer;
      st = st.stripWhiteSpace();
      st.replace( "\"\"", "" );
      if ( st.left( 2 ) == "//" ) continue;
      if (trovato && st[0]==')' && !blocco.isEmpty()) {
        parse( out, blocco );
        blocco = "";
        trovato=false;
      }
      int pos = 0;
      pos = st.find( "TRFORM" );
      if (st.find("#define")!=-1 && st.find("#define")<pos) pos=-1;
      if (pos>-1) trovato=true; else pos=0;
      if (trovato) {
        pos = st.find( '"', pos );
        if (pos==-1) continue;
        int text_end = st.findRev( '"' );
        if ( text_end == -1 ) continue;
        pos++;
        blocco += st.mid( pos, text_end - pos );
        if (st.find(')',text_end)>-1) {
          parse( out, blocco );
          blocco = "";
          trovato=false;
        }
      }
    }
    fclose( src );

    if ( blocco.length() ) parse( out, blocco );

  }

  fclose( out );
}
