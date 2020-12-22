/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waFreq.h>


WaFreq::WaFreq(WaSkinModell* waSkinModell,QWidget* parent,char* name):
QWidget(parent,name){
  this->waSkinModell=waSkinModell;
  connect(waSkinModell,SIGNAL(event_WA_SKIN_TEXT()),
	  this,SLOT(pixmapChange()));
  completePixmap=new QPixmap();
  buildPixmap=new QPixmap();

  QSize size=sizeHint();
  completePixmap->resize(size.width(),size.height());
  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_FREQ);
  setGeometry((*waSkinDesc->getGeometry()));
  connect(this,SIGNAL(setFreqEvent()),this,SLOT(pixmapChange()));
  pixmapChange();
}

WaFreq::~WaFreq() {
  delete completePixmap;
  delete buildPixmap;
}



void WaFreq::setSongEvent() {
  pixmapChange();
}


QSize WaFreq::sizeHint() const {
  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_FREQ);
  return QSize(waSkinDesc->getWidgetWidth(),waSkinDesc->getWidgetHeight());
}


void WaFreq::paintEvent ( QPaintEvent * paintEvent ) {


  QPainter paint;
  paint.begin( this );
  paint.drawPixmap(0,0,*completePixmap);
  paint.end();  



}

void WaFreq::setFreq(int val) {
  val=val/1000;
  QString tmp=QString::number(val);
  freq=tmp.rightJustify(2,' ');
  pixmapChange();
}

int WaFreq::getFreq() {
  return freq.toInt()*1000;
}


void WaFreq::pixmapChange() {
  int i;
  
  int x=0;
  int n=freq.length();

  for(i=0;i<n;i++) {
    waSkinModell->getNumber(_WA_SKIN_TEXT,freq.latin1()[i],buildPixmap);
    bitBlt(completePixmap,x,0,buildPixmap);
    x=x+buildPixmap->width();
  }


  repaint(false);
}
     

#include "waFreq.moc"
