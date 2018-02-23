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

/**********************************************************************
* qlabel_wrapper e' sostanzialmente identico a qlabel tranne che mette
* in pubblico il membro doc
**********************************************************************/

#ifndef QLABEL_H
#define QLABEL_H

#ifndef QT_H
#include "qframe.h"
#endif // QT_H

#ifndef QT_NO_LABEL

class QSimpleRichText;
class QLabelPrivate;

class Q_EXPORT QLabel : public QFrame
{
    Q_OBJECT
    Q_PROPERTY( QString text READ text WRITE setText )
    Q_PROPERTY( TextFormat textFormat READ textFormat WRITE setTextFormat )
    Q_PROPERTY( QPixmap pixmap READ pixmap WRITE setPixmap )
    Q_PROPERTY( bool scaledContents READ hasScaledContents WRITE setScaledContents )
    Q_PROPERTY( Alignment alignment READ alignment WRITE setAlignment )
    Q_PROPERTY( int indent READ indent WRITE setIndent )
    Q_OVERRIDE( BackgroundMode backgroundMode DESIGNABLE true)

public:
    QLabel( QWidget *parent, const char* name=0, WFlags f=0 );
    QLabel( const QString &text, QWidget *parent, const char* name=0,
	    WFlags f=0 );
    QLabel( QWidget *buddy, const QString &,
	    QWidget *parent, const char* name=0, WFlags f=0 );
    ~QLabel();

    QString	 text()		const	{ return ltext; }
    QPixmap     *pixmap()	const	{ return lpixmap; }
#ifndef QT_NO_PICTURE
    QPicture    *picture()	const	{ return lpicture; }
#endif
#ifndef QT_NO_MOVIE
    QMovie      *movie()		const;
#endif

    TextFormat textFormat() const;
    void 	 setTextFormat( TextFormat );

    int		 alignment() const	{ return align; }
    virtual void setAlignment( int );
    int		 indent() const		{ return extraMargin; }
    void 	 setIndent( int );

    bool 	 autoResize() const	{ return autoresize; }
    virtual void setAutoResize( bool );
#ifndef QT_NO_IMAGE_SMOOTHSCALE
    bool 	hasScaledContents() const;
    void 	setScaledContents( bool );
#endif
    QSize	 sizeHint() const;
    QSize	 minimumSizeHint() const;
#ifndef QT_NO_ACCEL
    virtual void setBuddy( QWidget * );
    QWidget     *buddy() const;
#endif
    int		 heightForWidth(int) const;

    void setFont( const QFont &f );

public slots:
    virtual void setText( const QString &);
    virtual void setPixmap( const QPixmap & );
#ifndef QT_NO_PICTURE
    virtual void setPicture( const QPicture & );
#endif
#ifndef QT_NO_MOVIE
    virtual void setMovie( const QMovie & );
#endif
    virtual void setNum( int );
    virtual void setNum( double );
    void	 clear();

protected:
    void	 drawContents( QPainter * );
    void	 fontChange( const QFont & );
    void	 resizeEvent( QResizeEvent* );

private slots:
#ifndef QT_NO_ACCEL
    void	 acceleratorSlot();
    void	 buddyDied();
#endif
#ifndef QT_NO_MOVIE
    void	 movieUpdated(const QRect&);
    void	 movieResized(const QSize&);
#endif

private:
    void	init();
    void	clearContents();
    void	updateLabel( QSize oldSizeHint );
    QSize	sizeForWidth( int w ) const;
    QString	ltext;
    QPixmap    *lpixmap;
#ifndef QT_NO_PICTURE
    QPicture   *lpicture;
#endif
#ifndef QT_NO_MOVIE
    QMovie *	lmovie;
#endif
#ifndef QT_NO_ACCEL
    QWidget *	lbuddy;
#endif
    ushort	align;
    short	extraMargin;
    uint	autoresize:1;
    uint	scaledcontents :1;
    TextFormat textformat;
#ifndef QT_NO_RICHTEXT
    public:
    QSimpleRichText* doc;
    private:
#endif
#ifndef QT_NO_ACCEL
    QAccel *	accel;
#endif
    QLabelPrivate* d;

    friend class QTipLabel;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QLabel( const QLabel & );
    QLabel &operator=( const QLabel & );
#endif
};


#endif // QT_NO_LABEL

#endif // tr_QLABEL_H
