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

#ifndef TR_LINEEDIT_H
  #define TR_LINEEDIT_H

  #include <qlineedit.h>
  #include <qevent.h>
  #include <qregexp.h>
  #include <qvalidator.h>

class tr_validator:public QRegExpValidator {
  Q_OBJECT public:tr_validator( QObject*parent, const char*name=0 ):QRegExpValidator( parent, name ) {
  }

  tr_validator( const QRegExp&rx, QObject*parent, const char*name=0 ):QRegExpValidator( rx, parent, name ) {
  }

  // ~tr_validator() {qDebug("Distruggo validatore\n");}
  //~tr_validator()~QRegExpValidator ()
  QValidator::State validate( QString&input, int&pos ) const;
};



class tr_lineedit:public QLineEdit {
  Q_OBJECT

  private:bool _upcase;

public:
  tr_lineedit( QWidget*parent, const char*name=0 );
  tr_lineedit( const QString&contents, QWidget*parent, const char*name=0 );
  ~tr_lineedit();

  bool keypressed;

  void upcase( bool uc ) {
    _upcase=uc;
  }

  bool upcase() {
    return _upcase;
  }

  void setRegExp( QString rx );
  void adatta();
  QChar lastkey;

protected:
  void keyPressEvent( QKeyEvent*e );

  void mouseReleaseEvent( QMouseEvent* ) {
    emit clicked();
  }

signals:
  void clicked();

};

#endif
