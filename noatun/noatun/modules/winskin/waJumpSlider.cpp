/*
  jumpslider for winamp skins
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waJumpSlider.h>


WaJumpSlider::WaJumpSlider(WaSkinModell* waSkinModell,
			   QWidget *parent, const char *name) :
  QWidget(parent,name) {

  this->waSkinModell=waSkinModell;
  WaSkinDesc* waSkinDesc=
    waSkinModell->getWaSkinMapDesc(_WA_MAPPING_POS_BAR);
  setGeometry((*waSkinDesc->getGeometry()));
}


WaJumpSlider::~WaJumpSlider() {
}


void WaJumpSlider::buildGui() {
  ws = new WaSlider(waSkinModell,_WA_MAPPING_POS_BAR,
		    _WA_MAPPING_POS_BAR_SLIDER,
		    parentWidget());
  ws->setPixmapSliderButtonUp(SIGNAL(event_WA_SKIN_POS_BAR_SLIDER_NORM()),
			      _WA_SKIN_POS_BAR_SLIDER_NORM);
  ws->setPixmapSliderButtonDown(SIGNAL(event_WA_SKIN_POS_BAR_SLIDER_PRES()),
				_WA_SKIN_POS_BAR_SLIDER_PRES);
  ws->setPixmapSliderBar(SIGNAL(event_WA_SKIN_POS_BAR()),
			 _WA_SKIN_POS_BAR);
  ws->setRange(0,100);
  ws->setValue( 0 );


  connect(ws,SIGNAL(sliderPressed()),SIGNAL(jumpSliderPressed()));
  connect(ws,SIGNAL(sliderReleased()),SLOT(sliderReleased()));
}

QSize WaJumpSlider::sizeHint() const {
  return ws->sizeHint();
}

void WaJumpSlider::paintEvent ( QPaintEvent * paintEvent ) {
  // noting
}


void WaJumpSlider::sliderReleased() {
  int val=ws->value();
  emit(jump(val));
  emit(jumpSliderReleased());
}

void WaJumpSlider::setJumpRange(int val) {
 ws->setRange(0,val);
}

void WaJumpSlider::setJumpValue(int val) {
  int currVal=ws->value();
  if (currVal != val) {
    ws->setValue(val);
  }
}

#include "waJumpSlider.moc"
