/*
  balanceslider for winamp skins
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waBalanceSlider.h>


WaBalanceSlider::WaBalanceSlider(WaSkinModell* waSkinModell,
				 QWidget *parent, const char *name) :
  QWidget(parent,name) {

  this->waSkinModell=waSkinModell;
  WaSkinDesc* waSkinDesc=
    waSkinModell->getWaSkinMapDesc(_WA_MAPPING_BALANCE_BAR);
  setGeometry((*waSkinDesc->getGeometry()));
}


WaBalanceSlider::~WaBalanceSlider() {
}


void WaBalanceSlider::buildGui() {
  ws = new WaSlider(waSkinModell,_WA_MAPPING_BALANCE_BAR,
		    _WA_MAPPING_BALANCE_SLIDER,
		    parentWidget());
  ws->setRange(-100,100);
  ws->setPixmapSliderButtonUp(SIGNAL(event_WA_SKIN_BALANCE_SLIDER_NORM()),
			      _WA_SKIN_BALANCE_SLIDER_NORM);
  ws->setPixmapSliderButtonDown(SIGNAL(event_WA_SKIN_BALANCE_SLIDER_PRES()),
				_WA_SKIN_BALANCE_SLIDER_PRES);
  ws->setPixmapSliderBar(SIGNAL(event_WA_SKIN_BALANCE_BAR()),
			 _WA_SKIN_BALANCE_BAR);

  sizeHint();
  ws->setValue(-100);
  connect(ws,SIGNAL(valueChanged(int)),this,SIGNAL(balanceSetValue(int)));

}

QSize WaBalanceSlider::sizeHint() const {
  return ws->sizeHint();
}

void WaBalanceSlider::paintEvent ( QPaintEvent * paintEvent ) {
  // noting
}

  
void WaBalanceSlider::setBalanceValue(int val) {
  int currVal=ws->value();
  if (currVal != val) {
    ws->setValue(val);
  }
}
#include "waBalanceSlider.moc"
