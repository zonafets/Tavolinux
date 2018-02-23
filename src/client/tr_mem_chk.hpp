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

#ifndef TR_MEM_MGR_H
#define TR_MEM_MGR_H

#include <iostream>
using namespace std;

#include <malloc.h>
#include <qintdict.h>
#include <qptrlist.h>

#define _MCD_CHECK

// TR memory manager
// memory check

class tr_mem_elem {
public:
    tr_mem_elem( short _group, void *_p, const char *_fname, const int _line)
        : group(_group), p(_p), fname(_fname), line(_line) { /*qstrncpy(fname, _fname, 64); fname[64]='\0'; */}
    short group;
    void *p;
    const char *fname;
    const int line;
};


class tr_mem_chk {
public:

    QPtrList<tr_mem_elem> allocazioni;

    tr_mem_chk();

    tr_mem_elem* find(void *p);
    tr_mem_elem* find(int gruppo);
    bool ins(void*p, const char *fname , const int line );
    bool del(void*p, const char *fname, const int line);
    void group() {_group++;}		// inizia un sotto gruppo
    bool ungroup(const char *fname, const int line);	// chiude un sotto gruppo di allocazioni e controlla x differenze
    bool stat();					// stampa statistiche differenze
    void show( bool s) {_show=s;}

private:
    bool _show;
    short _group;

};

//extern static tr_mem_chk mem_chk

/* vecchia versione lasciata qui per questioni di codice già scritto e funzionante
   e per usi futuri

#define NALLOC 100000

class tr_mem_array{
public:
    tr_mem_array() : count(0), eliminato(-1) {}
    class tr_chunk {
    public:
        void *p;
        size_t sz;
        bool eliminato;
    } chunks[NALLOC];

    int count ;
    int eliminato;
    void ins(void *p, size_t sz);

    void del( void *p) ;

};

extern tr_mem_array mem_array;

extern const bool _mcd_check;

inline void tr_mem_array::ins(void *p, size_t sz) {
        if (count<NALLOC) {
            int i;
            // if (eliminato!=-1) i=eliminato; else i=count;
            // cout << "metto: " << p << "," << sz << " in " << i << endl;
            i = count;
            chunks[ i ].p=p;
            chunks[ i ].sz=sz;
            chunks[ i ].eliminato=false;
            // if (eliminato!=-1) eliminato=-1; else count++;
            count++;
        } else cout << "raggiunte 30000 allocazioeni\n";
        if (!(count % 5000)) cout << "+count=" << count << endl;
    }

void tr_mem_array::del( void *p) {
    bool trovato=false;
        if (count < 100) cout << "elimino " << p << endl;
        // QTime t; t.start();
        int i=count-1;
        for (i=count-1;i>=0;i--) if (chunks[i].eliminato==false && chunks[i].p==p) {
            chunks[i].eliminato=true;
            trovato=true;
            // eliminato=i;
            if (i==count-1) count--; else if (i>count-10)
            {
                count--;
                for (int j=i;j<count;j++) {
                    chunks[j] = chunks[j+1];
                }
            }
            break;
        }
        if (!(count % 100)) cout << "-count=" << count << endl;
        // if (t.elapsed()>100) cout <<
        if (!trovato && count<NALLOC)
        {
            cout << "non ho trovato il puntatore da deallocare " << p << " ";
            cout << (char*)p << endl;
        }
    }

*/

#endif
