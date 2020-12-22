/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waMono.h>


WaMono::WaMono(WaSkinModell* waSkinModell,QWidget* parent,char* name):
QWidget(parent,name){
  this->waSkinModell=waSkinModell;
  connect(waSkinModell,SIGNAL(event_WA_SKIN_MONOSTER_MONO_FALSE()),
	  this,SLOT(pixmapChange()));
  monoPixmap=new QPixmap();
  noMonoPixmap=new QPixmap();
  lMono=false;
  QSize size=sizeHint();
  monoPixmap->resize(size.width(),size.height());
  noMonoPixmap->resize(size.width(),size.height());

  WaSkinDesc* waSkinDesc=
    waSkinModell->getWaSkinMapDesc(_WA_MAPPING_MONOSTER_MONO);
  setGeometry((*waSkinDesc->getGeometry()));
  pixmapChange();
}

WaMono::~WaMono() {
  delete noMonoPixmap;
  delete monoPixmap;
}




QSize WaMono::sizeHint() const {
  WaSkinDesc* waSkinDesc=
    waSkinModell->getWaSkinMapDesc(_WA_MAPPING_MONOSTER_MONO);
  return QSize(waSkinDesc->getWidgetWidth(),waSkinDesc->getWidgetHeight());
}


void WaMono::paintEvent ( QPaintEvent * paintEvent ) {

  QPainter paint;

  int mono=getMono();
  if (mono == true) {
    paint.begin( this );
    paint.drawPixmap(0,0,*monoPixmap);
    paint.end();  
  } else {
    paint.begin( this );
    paint.drawPixmap(0,0,*noMonoPixmap);
    paint.end();  
  }

}


void WaMono::setMono(int lMono) {
  this->lMono=lMono;
  pixmapChange();
}


int WaMono::getMono() {
  return lMono;
}


void WaMono::pixmapChange() {

  monoPixmap=waSkinModell->get(_WA_SKIN_MONOSTER_MONO_TRUE);
  noMonoPixmap=waSkinModell->get(_WA_SKIN_MONOSTER_MONO_FALSE);

  repaint(false);
}
     
#include "waMono.moc"
