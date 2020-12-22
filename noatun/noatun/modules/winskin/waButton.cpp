/*
  standard Button fo winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waButton.h>




WaButton::WaButton(WaSkinModell* waSkinModell,int mapId,
		   QWidget* parent,char* name):
QPushButton(parent,name) {
  this->waSkinModell=waSkinModell;
  lSelected=false;
  this->mapId=mapId;
  
  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinMapDesc(mapId);
  setGeometry((*waSkinDesc->getGeometry()));
  setCurrentLocation(QPoint(0,0));
  lmove=false;
  connect(this,SIGNAL(toggled(bool)),this,SLOT(doToggle(bool)));
}


WaButton::~WaButton() {
}



void WaButton::setPixmapUp(const char* sig,int pixId) {
  nUpId=pixId;
  connect(waSkinModell,sig,this,SLOT(pixmapChange()));
}


void WaButton::setPixmapDown(const char* signal,int pixId) {
  nDownId=pixId;
  connect(waSkinModell,signal,this,SLOT(pixmapChange()));
}


void WaButton::setPixmapUpSelected(const char* signal,int pixId) {
  nUpIdSelected=pixId;
  connect(waSkinModell,signal,this,SLOT(pixmapChange()));
}


void WaButton::setPixmapDownSelected(const char* signal,int pixId) {
  nDownIdSelected=pixId;
  connect(waSkinModell,signal,this,SLOT(pixmapChange()));
}


int WaButton::getSelected() {
  return lSelected;
}


void WaButton::setSelected(int lSelect) {
  lSelected=lSelect;
  repaint(false);
}

 


void WaButton::pixmapChange() {
  repaint(false);
}


QSize WaButton::sizeHint() const {
  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinMapDesc(mapId);
  return QSize(waSkinDesc->getWidgetWidth(),waSkinDesc->getWidgetHeight());
}


void WaButton::paintEvent (QPaintEvent*) {
  int lDown=isDown();
  int id=nUpId;

  if (lSelected == false) {
    if (lDown) {
      id=nDownId;
    }
  }
  if (lSelected == true) {
    id=nUpIdSelected;
    if (lDown) {
      id=nDownIdSelected;
    }    
  }
  

  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinDesc(id);
  QPixmap* pixmapCurrent=waSkinDesc->getPixmap();
  if (lmove) {
    move(currentLocation.x(),currentLocation.y());
  }
  QPainter paint;
  paint.begin( this );
  paint.drawPixmap (0,0,*pixmapCurrent);
  paint.end();  

}


void WaButton::mouseMoveEvent (QMouseEvent* e ) {
  emit(sliderMoveEvent(e));
}


void WaButton::mousePressEvent(QMouseEvent* e) {
  emit(sliderPressEvent(e));
  QPushButton::mousePressEvent(e);
}


QPoint WaButton::getCurrentLocation() {
  return currentLocation;
}


void WaButton::setCurrentLocation(QPoint point) {
  currentLocation.setX(point.x());
  currentLocation.setY(point.y());
}
 

void WaButton::setMove(int lmove) {
  this->lmove=lmove;
}


int WaButton::getMove() {
  return lmove;
}


void WaButton::setToggleFlag(int flag) {
  setToggleButton(flag);
}

void WaButton::doToggle(bool val) {
  setSelected(val);
  emit(setSelectedEvent(lSelected));
}
#include "waButton.moc"
