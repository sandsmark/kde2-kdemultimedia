/*
  jumpslider for winamp skins
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waVolumeSlider.h>


WaVolumeSlider::WaVolumeSlider(WaSkinModell* waSkinModell,
			       QWidget *parent, const char *name) :
  QWidget(parent,name) {

  this->waSkinModell=waSkinModell;
 WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_VOLUME_BAR);
  setGeometry((*waSkinDesc->getGeometry()));
}


WaVolumeSlider::~WaVolumeSlider() {
}


void WaVolumeSlider::buildGui() {
  ws = new WaSlider(waSkinModell,_WA_MAPPING_VOLUME_BAR,
		    _WA_MAPPING_VOLUME_SLIDER,
		    parentWidget());
  ws->setPixmapSliderButtonUp(SIGNAL(event_WA_SKIN_VOLUME_SLIDER_NORM()),
			      _WA_SKIN_VOLUME_SLIDER_NORM);
  ws->setPixmapSliderButtonDown(SIGNAL(event_WA_SKIN_VOLUME_SLIDER_PRES()),
				_WA_SKIN_VOLUME_SLIDER_PRES);
  ws->setPixmapSliderBar(SIGNAL(event_WA_SKIN_VOLUME_BAR()),
			 _WA_SKIN_VOLUME_BAR);

  sizeHint();

  connect(ws,SIGNAL(valueChanged(int)),this,SIGNAL(volumeSetValue(int)));
}

QSize WaVolumeSlider::sizeHint() const {
  return ws->sizeHint();
}

void WaVolumeSlider::paintEvent ( QPaintEvent * paintEvent ) {
  // nothing
}

  
void WaVolumeSlider::setVolumeValue(int val) {
  int currVal=ws->value();
  if (currVal != val) {
    ws->setValue(val);
  }
}


#include "waVolumeSlider.moc"
