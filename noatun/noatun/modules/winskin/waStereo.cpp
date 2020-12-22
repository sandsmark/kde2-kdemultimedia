/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waStereo.h>


WaStereo::WaStereo(WaSkinModell* waSkinModell,QWidget* parent,char* name):
QWidget(parent,name){
  this->waSkinModell=waSkinModell;
  connect(waSkinModell,SIGNAL(event_WA_SKIN_MONOSTER_STEREO_FALSE()),
	  this,SLOT(pixmapChange()));
  stereoPixmap=new QPixmap();
  noStereoPixmap=new QPixmap();
  lStereo=false;
  QSize size=sizeHint();
  stereoPixmap->resize(size.width(),size.height());
  noStereoPixmap->resize(size.width(),size.height());

  WaSkinDesc* waSkinDesc=
    waSkinModell->getWaSkinMapDesc(_WA_MAPPING_MONOSTER_STEREO);
  setGeometry((*waSkinDesc->getGeometry()));
  pixmapChange();
}

WaStereo::~WaStereo() {
  delete noStereoPixmap;
  delete stereoPixmap;
}




QSize WaStereo::sizeHint() const {
  WaSkinDesc* waSkinDesc=
    waSkinModell->getWaSkinMapDesc(_WA_MAPPING_MONOSTER_STEREO);
  return QSize(waSkinDesc->getWidgetWidth(),waSkinDesc->getWidgetHeight());
}


void WaStereo::paintEvent ( QPaintEvent * paintEvent ) {

  QPainter paint;

  int stereo=getStereo();
  if (stereo) {
    paint.begin( this );
    paint.drawPixmap(0,0,*stereoPixmap);
    paint.end();  
  } else {
    paint.begin( this );
    paint.drawPixmap(0,0,*noStereoPixmap);
    paint.end();  
  }

}


void WaStereo::setStereo(int lStereo) {
  this->lStereo=lStereo;
  pixmapChange();
}

int WaStereo::getStereo() {
  return lStereo;
}


void WaStereo::pixmapChange() {

  stereoPixmap=waSkinModell->get(_WA_SKIN_MONOSTER_STEREO_TRUE);
  noStereoPixmap=waSkinModell->get(_WA_SKIN_MONOSTER_STEREO_FALSE);

  repaint(false);
}
     
#include "waStereo.moc"
