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


#include "tr_mem_chk.hpp"
#include "tr_common.hpp"
#include <qobject.h>
#include <iostream>


using namespace std;

tr_mem_chk mem_chk;

tr_mem_chk::tr_mem_chk()
{
    allocazioni.setAutoDelete(true);
    _show=false;
    _group=0;
}

tr_mem_elem* tr_mem_chk::find(void *p)
{
    tr_mem_elem *e=NULL;
    QPtrListIterator<tr_mem_elem> it( allocazioni );
    while ( (e = it.current()) != 0 ) {
        ++it;
        if (e && e->p==p) return e;
    }
    return NULL;
}

tr_mem_elem* tr_mem_chk::find(int gruppo)
{
    tr_mem_elem *e=NULL;
    QPtrListIterator<tr_mem_elem> it( allocazioni );
    while ( (e = it.current()) != 0 ) {
        ++it;
        if (e && e->group==gruppo) return e;
    }
    return NULL;
}

bool tr_mem_chk::ins(void *p, const char *fname, const int line)
{
    tr_mem_elem *e;
    // cerca se è già stato allocato lo stesso puntatore
    if (p!=0) {
      e=find(p);
      if (e) {
          cout << "doppia allocazione " << p << " in " << fname << " (" << line << ")" << endl;
          return false;
      }
    }

    if (_show) cout << "+ " << p << " in " << fname << " (" << line << ")" << endl;
    e = new tr_mem_elem( _group, p, fname, line);
    allocazioni.append(e);
    return true;
}

bool tr_mem_chk::del(void *p, const char *fname, const int line)
{
    if (_show) cout << "- " << p << " in " << fname << " (" << line << ")" << endl;
    bool r=false;

    tr_mem_elem *e=find(p);
    if (e) r = allocazioni.remove(e);

    /*
    if (!r) {
        cout << "rimozioni :" << p << " in " << fname << " (" << line << ")" << endl;
    }
    */

    return r;
}

bool tr_mem_chk::ungroup(const char *fname, const int line) {
    tr_mem_elem *e = find(_group);
    if (e) {
        cout << "Disallineamento nel gruppo in " << fname << " (" << line << ") :" << endl;
        while (e) {
            cout << "-- :" << e->p << " in " << e->fname << " (" << e->line << ")" << endl;
            allocazioni.remove(e);
            e=find(_group);
        }
    }

    _group--;
    if (_group<0) { cout << "disallineamento tra group e ungroup\n"; return false; }

    return true;
}

bool tr_mem_chk::stat() {
    if (allocazioni.count()>0) {
        cout << "Allocazioni rimaste (" << allocazioni.count() << "):\n";
        QPtrListIterator<tr_mem_elem> it( allocazioni );
        tr_mem_elem *a;
        while ( (a = it.current()) != 0 ) {
            ++it;
            if (a) {
                cout << a->fname << " (" << a->line << ")\n";
                // QObject *p = (QObject*)a->p;
                // cout << "  " << p->name() << endl;
            }
        }
        return true;
    }
    return false;
}

/*

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <search.h>
#include <execinfo.h>
#include <string.h>

  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  abbandonato perchè la malloc e la free vengono chiamate un sacco di volte di più di quelle

  indicate nell'applicazione. A volte la free viene chiamata di più della malloc
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

*/

/*
// Prototypes for our own hooks hooks.
static void init_hooks (void);
static void *malloc_hook (size_t, const void *);
static void free_hook (void *, const void *);

// file scoped globals
static const char *leak_log_file = "./mtrace.log";
static FILE *leak_stream = 0;
static int first_call = 1;

// pointers in which to save the org malloc and free functions
static __malloc_ptr_t
  (*org_malloc_hook) __MALLOC_PMT ((size_t __size, __const __malloc_ptr_t));

static void
  (*org_free_hook)
__MALLOC_PMT ((__malloc_ptr_t __ptr, __const __malloc_ptr_t));

// this overrides malloc initializing hook from the glibc library.
void (*__malloc_initialize_hook) (void) = init_hooks;


static void
init_hooks (void)
{
  org_malloc_hook = __malloc_hook;
  org_free_hook = __free_hook;
  __malloc_hook = malloc_hook;
  __free_hook = free_hook;

}

static void *
malloc_hook (size_t size, const void *caller)
{
  static char **fnames = NULL;
  int ntraced;
  void *callers[MAX_STACK_DEPTH];
  void *ptr;

  // Restore all org hooks to avoid infinite recursion via calls to malloc
  __malloc_hook = org_malloc_hook;
  __free_hook = org_free_hook;

  ntraced = backtrace (callers, MAX_STACK_DEPTH);
  fnames = backtrace_symbols ((void *const *) &callers[ntraced-1], 1);
  // if (strncmp(fnames[0],"tavolinux",9)==0)   cout << "+ " << fnames[0] << " | " << callers[ntraced-1] << "|" << callers[0] << "|" << caller << endl;

    // init some stuff the first go round
  if (first_call)
  {
      first_call = 0;
      if (!leak_stream)
          leak_stream = fopen (leak_log_file, "w+");
  }

  if (strncmp(fnames[0],"tavolinux",9)==0) fprintf (leak_stream, "+ [%8.8p] frame %s\n",
                 (callers[ntraced-1]), (fnames [0]));

  if (strncmp(fnames[0],"tavolinux",9)==0) mem_chk.count++; // else cout << "malloc fuori da tavolinux\n";

  if (fnames)
    {
      free (fnames);
      fnames = NULL;
    }


  // Call *not* recursively - tricky
  ptr = malloc (size);

  // Restore our own hooks
  __malloc_hook = malloc_hook;
  __free_hook = free_hook;

  return ptr;
}



static void
free_hook (void *ptr, const void *caller)
{
  static char **fnames = NULL;
  int ntraced;
  void *callers[MAX_STACK_DEPTH];

  // restore all org hooks to prevent recursion
  __malloc_hook = org_malloc_hook;
  __free_hook = org_free_hook;

  free (ptr);

  ntraced = backtrace (callers, MAX_STACK_DEPTH);
  fnames = backtrace_symbols ((void *const *) &callers[ntraced-1],1);
  // if (strncmp(fnames[0],"tavolinux",9)==0) cout << "- " << fnames[0] << endl;

    // init some stuff the first go round
  if (first_call)
  {
      first_call = 0;
      if (!leak_stream)
          leak_stream = fopen (leak_log_file, "w+");
  }

  if (strncmp(fnames[0],"tavolinux",9)==0) fprintf (leak_stream, "- [%8.8p] frame %s\n",
                 (callers[ntraced-1]), (fnames [0]));

  if (strncmp(fnames[0],"tavolinux",9)==0) mem_chk.count--; // else cout << "malloc fuori da tavolinux\n";

  if (fnames)
    {
      free (fnames);
      fnames = NULL;
    }



  // Restore our own hooks
  __malloc_hook = malloc_hook;
  __free_hook = free_hook;
}
*/

