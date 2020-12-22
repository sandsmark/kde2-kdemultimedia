/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waStatus.h>


WaStatus::WaStatus(WaSkinModell* waSkinModell,QWidget* parent,char* name):
QWidget(parent,name){
  this->waSkinModell=waSkinModell;
  connect(waSkinModell,SIGNAL(event_WA_SKIN_PLAYPAUS_PLAY()),
	  this,SLOT(pixmapChange()));
  playPixmap=new QPixmap();
  pausePixmap=new QPixmap();
  stopPixmap=new QPixmap();

  QSize size=sizeHint();
  nStatus=_STATUS_STOPPED;

  playPixmap->resize(size.width(),size.height());
  pausePixmap->resize(size.width(),size.height());
  stopPixmap->resize(size.width(),size.height());

  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_PLAYPAUS);
  setGeometry((*waSkinDesc->getGeometry()));
  pixmapChange();
}

WaStatus::~WaStatus() {
  delete playPixmap;
  delete pausePixmap;
  delete stopPixmap;
}




QSize WaStatus::sizeHint() const {
  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_PLAYPAUS);
  return QSize(waSkinDesc->getWidgetWidth(),waSkinDesc->getWidgetHeight());
}

void WaStatus::setStatus(int status) {
  this->nStatus=status;
  pixmapChange();
}


int WaStatus::getStatus() {
  return nStatus;
}


void WaStatus::paintEvent ( QPaintEvent * paintEvent ) {

  QPainter paint;

  int status=getStatus();
  if (status == _STATUS_PLAYING) {
    paint.begin( this );
    paint.drawPixmap(0,0,*playPixmap);
    paint.end();  
  }
  if (status == _STATUS_STOPPED) {
    paint.begin( this );
    paint.drawPixmap(0,0,*stopPixmap);
    paint.end();  
  }
  if (status == _STATUS_PAUSED) {
    paint.begin( this );
    paint.drawPixmap(0,0,*pausePixmap);
    paint.end();  
  }

}




void WaStatus::pixmapChange() {

  playPixmap=waSkinModell->get(_WA_SKIN_PLAYPAUS_PLAY);
  pausePixmap=waSkinModell->get(_WA_SKIN_PLAYPAUS_PAUSE);
  stopPixmap=waSkinModell->get(_WA_SKIN_PLAYPAUS_STOP);
  

  repaint(false);
}
     
#include "waStatus.moc"
