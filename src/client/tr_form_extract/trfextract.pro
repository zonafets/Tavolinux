SOURCES	+= main.cpp
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
TEMPLATE	=app
CONFIG	+= qt warn_on debug
LANGUAGE	= C++
