#############################################################################
# Makefile for building: genera_enumeratori
# Generated by qmake (1.06c) (Qt 3.2.0) on: Tue Apr 19 16:08:21 2005
# Project:  genera_enumeratori.pro
# Template: app
# Command: $(QMAKE) -o Makefile genera_enumeratori.pro
#############################################################################

####### Compiler, tools and options

CC       = gcc
CXX      = g++
LEX      = flex
YACC     = yacc
CFLAGS   = -pipe -Wall -W -g  -DQT_SHARED -DQT_NO_DEBUG
CXXFLAGS = -pipe -Wall -W -ggdb3 -MD -fno-default-inline  -DQT_SHARED -DQT_NO_DEBUG
LEXFLAGS = 
YACCFLAGS= -d
INCPATH  = -I/usr/local/qt/mkspecs/default -I. -I$(QTDIR)/include -I.ui/ -I.moc/
LINK     = g++
LFLAGS   =  -Wl,-rpath,$(QTDIR)/lib
LIBS     = $(SUBLIBS) -L$(QTDIR)/lib -L/usr/X11R6/lib -lqt -lXext -lX11 -lm
AR       = ar cqs
RANLIB   = 
MOC      = $(QTDIR)/bin/moc
UIC      = $(QTDIR)/bin/uic
QMAKE    = qmake
TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f
COPY_FILE= $(COPY)
COPY_DIR = $(COPY) -r
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rmdir
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
MKDIR    = mkdir -p

####### Output directory

OBJECTS_DIR = .obj/

####### Files

HEADERS = cmdlangtoken.hpp \
		moduli.hpp
SOURCES = genera_enumeratori.cpp
OBJECTS = .obj/genera_enumeratori.o
FORMS = 
UICDECLS = 
UICIMPLS = 
SRCMOC   = 
OBJMOC = 
DIST	   = genera_enumeratori.pro
QMAKE_TARGET = genera_enumeratori
DESTDIR  = 
TARGET   = genera_enumeratori

first: all
####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(UICDECLS) $(OBJECTS) $(OBJMOC)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS) $(OBJCOMP)

mocables: $(SRCMOC)
uicables: $(UICDECLS) $(UICIMPLS)

$(MOC): 
	( cd $(QTDIR)/src/moc ; $(MAKE) )

Makefile: genera_enumeratori.pro  /usr/local/qt/mkspecs/default/qmake.conf /usr/local/qt/lib/libqt.prl
	$(QMAKE) -o Makefile genera_enumeratori.pro
qmake: 
	@$(QMAKE) -o Makefile genera_enumeratori.pro

dist: 
	@mkdir -p .obj/genera_enumeratori && $(COPY_FILE) --parents $(SOURCES) $(HEADERS) $(FORMS) $(DIST) .obj/genera_enumeratori/ && ( cd `dirname .obj/genera_enumeratori` && $(TAR) genera_enumeratori.tar genera_enumeratori && $(GZIP) genera_enumeratori.tar ) && $(MOVE) `dirname .obj/genera_enumeratori`/genera_enumeratori.tar.gz . && $(DEL_FILE) -r .obj/genera_enumeratori

mocclean:

uiclean:

yaccclean:
lexclean:
clean:
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) $(TARGET)


FORCE:

####### Compile

.obj/genera_enumeratori.o: genera_enumeratori.cpp cmdlangtoken.hpp \
		moduli.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/genera_enumeratori.o genera_enumeratori.cpp

####### Install

install: all 

uninstall: 

