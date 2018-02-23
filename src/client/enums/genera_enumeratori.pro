SOURCES	+= genera_enumeratori.cpp
HEADERS	+= cmdlangtoken.hpp \
	moduli.hpp
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
QMAKE_CXXFLAGS_DEBUG = -ggdb3 -MD -fno-default-inline

TEMPLATE	=app
CONFIG	+= qt warn_on release debug
LANGUAGE	= C++
