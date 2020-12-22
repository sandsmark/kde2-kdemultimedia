/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waBPS.h>


WaBPS::WaBPS(WaSkinModell* waSkinModell,QWidget* parent,char* name):
QWidget(parent,name){
  this->waSkinModell=waSkinModell;
  connect(waSkinModell,SIGNAL(event_WA_SKIN_TEXT()),
	  this,SLOT(pixmapChange()));
  completePixmap=new QPixmap();
  buildPixmap=new QPixmap();
  QSize size=sizeHint();
  completePixmap->resize(size.width(),size.height());
  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_BPS);
  setGeometry((*waSkinDesc->getGeometry()));
  connect(this,SIGNAL(setSongEvent()),this,SLOT(pixmapChange()));
  pixmapChange();
}

WaBPS::~WaBPS() {
  delete completePixmap;
  delete buildPixmap;
}



void WaBPS::setSongEvent() {
  pixmapChange();
}


QSize WaBPS::sizeHint() const {
  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_BPS);
  return QSize(waSkinDesc->getWidgetWidth(),waSkinDesc->getWidgetHeight());
}


void WaBPS::paintEvent ( QPaintEvent * paintEvent ) {


  QPainter paint;
  paint.begin( this );
  paint.drawPixmap(0,0,*completePixmap);
  paint.end();  



}

void WaBPS::setBPS(int val) {
  QString tmp=QString::number(val);
  bps=tmp.rightJustify(3,' ');
  pixmapChange();
}

int WaBPS::getBPS() { 
  return bps.toInt(); 
}




void WaBPS::pixmapChange() {
  int i;
  
  int x=0;
  int n=bps.length();

  for(i=0;i<n;i++) {
    waSkinModell->getNumber(_WA_SKIN_TEXT,bps.latin1()[i],buildPixmap);
    bitBlt(completePixmap,x,0,buildPixmap);
    x=x+buildPixmap->width();
  }


  repaint(false);
}
     
#include "waBPS.moc"
