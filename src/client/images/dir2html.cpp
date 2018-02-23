#include <stdio.h>
#include <qstring.h>
#include <qdir.h>

int main(int argc, char **argv)
{
        QDir d;
        d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
        d.setSorting( QDir::Size | QDir::Reversed );

        const QFileInfoList *list = d.entryInfoList();
        QFileInfoListIterator it( *list );
        QFileInfo *fi;
	
        printf( "<table>\n" );
        while ( (fi = it.current()) != 0 ) {
	  if (fi->fileName().right(4)==".png" || fi->fileName().right(4)==".jpg")
      	    printf("<tr><td>%s</td><td><img src=\"%s\"></td><td>description</td></tr>\n", fi->fileName().latin1(), fi->fileName().latin1() );
          ++it;
        }
        printf( "<table>\n" );

        return 0;
}
