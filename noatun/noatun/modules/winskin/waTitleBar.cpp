/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waTitleBar.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <kstddirs.h>
#include <kglobal.h>

WaTitleBar::WaTitleBar(WaSkinModell* waSkinModell,QWidget* parent,char* name):
QWidget(parent,name){
        moving = false;
  this->waSkinModell=waSkinModell;
  waParent = parent;
  titlePixmap=new QPixmap();
  QSize size=sizeHint();
  titlePixmap->resize(size.width(),size.height());
  connect(waSkinModell,SIGNAL(event_WA_SKIN_TITLE()),
	  this,SLOT(pixmapChange()));
  WaSkinDesc* waSkinDesc=
    waSkinModell->getWaSkinMapDesc(_WA_MAPPING_TITLE);
  setGeometry((*waSkinDesc->getGeometry()));
  pixmapChange();
}

WaTitleBar::~WaTitleBar() {
}




QSize WaTitleBar::sizeHint() const {
  WaSkinDesc* waSkinDesc=
    waSkinModell->getWaSkinMapDesc(_WA_MAPPING_TITLE);
  return QSize(waSkinDesc->getWidgetWidth(),waSkinDesc->getWidgetHeight());
}


void WaTitleBar::paintEvent ( QPaintEvent * paintEvent ) {
        QPainter paint;
        paint.begin( this );
        paint.drawPixmap(0,0,*titlePixmap);
        paint.end();
}

void WaTitleBar::pixmapChange() {

  titlePixmap=waSkinModell->get(_WA_SKIN_TITLE);
  repaint(false);
}

void WaTitleBar::mousePressEvent(QMouseEvent *e)
{
        if (e->x() >= 255 && e->x() < 263 && e->y() >= 3 && e->y() < 12)
	{
/*	        cout << "\nShade Clicked\n";
		QPainter p;
	        QStringList rd = KGlobal::dirs()->findDirs("data", "noatun/winskin/winamp");
                QStringList::Iterator it = rd.begin();
                QString filename = (*it).latin1();
                filename.append("TITLEBAR.BMP");
                QPixmap titlebarPixmap(filename, QPixmap::imageFormat(filename));
		p.begin(this);
	        p.drawPixmap(255,3, titlebarPixmap, 9, 9, 8, 8);
		p.end();
*/
	}
        if (e->button()!=RightButton)
	{
    		if (!moving) {
                        moving=true;
        		mDragStart = e->pos();
      		  	mLastPos = e->globalPos();
    		}
		return;

	}
}

void WaTitleBar::mouseReleaseEvent(QMouseEvent *e)
{
        if (e->button()!=RightButton)
	{
		moving = false;
		return;
	}
}

void WaTitleBar::mouseMoveEvent(QMouseEvent *e)
{
    QPoint diff = e->globalPos() - mLastPos;
    if ( abs(diff.x()) > 10 || abs(diff.y()) > 10) {
        // Moving starts only, when passing a drag border
        moving = true;
    }

    if ( moving == true ) {
        waParent->move( e->globalPos() - mDragStart );
    }
}
