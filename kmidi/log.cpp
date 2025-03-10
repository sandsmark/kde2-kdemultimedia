/*
 *        kmidi
 *
 * $Id: log.cpp 96743 2001-05-13 01:55:26Z gebauer $
 *            Copyright (C) 1997  Bernd Wuebben
 *                 wuebben@math.cornel.edu 
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
//#include <kapp.h>
#include <klocale.h>

#include "log.moc"


LogWindow::LogWindow(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  setCaption(i18n("Info Window"));

  text_window = new QMultiLineEdit(this,"logwindow");
  text_window->setFocusPolicy ( QWidget::NoFocus );
  text_window->setReadOnly( TRUE );
  text_window->setUndoEnabled( FALSE );
  //text_window->setMaxLineLength( 24 );
  text_window->setMaxLines( 110 ); // text_window->numRows() ??

  stringlist = new QStringList;

  sltimer = new QTimer(this);
  connect(sltimer,SIGNAL(timeout()),this,SLOT(updatewindow()));
  timerset = false;

}


LogWindow::~LogWindow() {
}

void LogWindow::updatewindow(){

  static int line = 0, col = 0;
  timerset = false;



  if (stringlist->count() != 0){

    text_window->setAutoUpdate(FALSE);

    for ( QStringList::Iterator it = stringlist->begin();
          it != stringlist->end();
          ++it )
    {
      /* after a string starting with "~", don't start a new line --gl */
	static int tildaflag = 0;
	int futuretilda, len;
	QString s = *it;
	if (s.at(0) == '~') {
	    futuretilda = 1;
            s = s.remove(0, 1);
	}
	else futuretilda = 0;
	len = s.length();
	if (tildaflag && len) {
	    text_window->insertAt(s, line, col);
	    col += len;
	}
	else {
	    if (line > 100) text_window->removeLine(0);
	    else line++;
	    text_window->insertLine(s,line);
	    col = len;
	}
	tildaflag = futuretilda;
    }
    text_window->setAutoUpdate(TRUE);

    text_window->setCursorPosition(line+1,0,FALSE);
    text_window->repaint(FALSE);

    stringlist->clear();

  }

}

void LogWindow::insertStr(const QString &string){

  //if(string.find("Lyric:",0,TRUE) != -1)
  //  return;
  
  if(string.find("MIDI file",0,TRUE) != -1){
    stringlist->append(" ");
  }
 
  stringlist->append(string);
 
  if(!timerset){
    sltimer->start(10,TRUE); // sinlge shot TRUE
    timerset = true;
  }

}
void LogWindow::clear(){

  if(text_window){
    
    text_window->clear();

  }

}

void LogWindow::resizeEvent(QResizeEvent* ){

  int w = width() ;
  int h = height();

  text_window->resize(w, h);
}
