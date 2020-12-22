/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waInfo.h>


WaInfo::WaInfo(WaSkinModell* waSkinModell,QWidget* parent,char* name):
QWidget(parent,name){
  this->waSkinModell=waSkinModell;
  connect(waSkinModell,SIGNAL(event_WA_SKIN_TEXT()),
	  this,SLOT(pixmapChange()));
  completePixmap=new QPixmap();
  buildPixmap=new QPixmap();

  QSize size=sizeHint();
  completePixmap->resize(size.width(),size.height());
  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_INFO);
  setGeometry((*waSkinDesc->getGeometry()));


  timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(timeEvent()));
  
  pixmapChange();
}

WaInfo::~WaInfo() {
  delete completePixmap;
  delete buildPixmap;
}


void WaInfo::timeEvent() {
  repaint(false);
}


void WaInfo::scrollerSetup() {
  xScrollPos=0;
  xScrollDirection=0;
  timer->stop();
  QSize size=sizeHint();
  if (completePixmap->width() > size.width()) {
    xScrollDirection=1;
    timer->start(50);
  }
}




QSize WaInfo::sizeHint() const {
  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_INFO);
  return QSize(waSkinDesc->getWidgetWidth(),waSkinDesc->getWidgetHeight());
}


void WaInfo::paintEvent ( QPaintEvent * paintEvent ) {

  QPainter paint;
  QSize size=sizeHint();
  if (completePixmap->width() <= size.width()) {
    paint.begin( this );
    paint.drawPixmap(0,0,*completePixmap);
    paint.end();  
    return;
  }
  
  // pixmap widther than window:
  int xDrawWidth;
  int xRestWidth;

  xDrawWidth=completePixmap->width()-xScrollPos;
  if (xDrawWidth > size.width()) {
    xDrawWidth=size.width();
  }
  paint.begin( this );
  paint.drawPixmap(0,0,*completePixmap,xScrollPos,0,xDrawWidth);
  if (xDrawWidth < size.width()) {
    xRestWidth=size.width()-xDrawWidth;
    paint.drawPixmap(xDrawWidth,0,*completePixmap,0,0,xRestWidth);
  }
  paint.end();  
  
  xScrollPos+=xScrollDirection;
  if (abs(xScrollPos) >completePixmap->width() ) {
    xScrollPos=0;
  }
}


void WaInfo::setSong(QString song) {
  text=song;
  pixmapChange();
}

QString WaInfo::getSong() {
  return text;
}


void WaInfo::pixmapChange() {
  int i;
  const char* infoString=getSong().latin1();

  int x=0;
  int n=strlen(infoString);
  QSize size=sizeHint();

  completePixmap->resize(0,0);
  
  for(i=0;i<n;i++) {
    waSkinModell->getNumber(_WA_SKIN_TEXT,infoString[i],buildPixmap);
    if (i == 0) {
      int w=buildPixmap->width();
      int h=buildPixmap->height();
      int textWidth=w*n;
      if (textWidth < size.width()) {
	textWidth=size.width();
      }
      completePixmap->resize(textWidth,h);
    }

    bitBlt(completePixmap,x,0,buildPixmap);
    x=x+buildPixmap->width();
  }
  // if the size is now smaller than the with of this widget, we
  // fill the pixmap with spaces
  if (x < size.width()) {
    while (x < size.width()) {
      waSkinModell->getNumber(_WA_SKIN_TEXT,' ',buildPixmap);
      bitBlt(completePixmap,x,0,buildPixmap);
      x=x+buildPixmap->width();
    }
  }


  scrollerSetup();
  repaint(false);
}
     



#include "waInfo.moc"
