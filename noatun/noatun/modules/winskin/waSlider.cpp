/*
  standard Slider for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waSlider.h>


WaSlider::WaSlider(WaSkinModell* waSkinModell,int sliderBarMapId,int sliderId,
		   QWidget* parent,char* name):QSlider(parent,name) {
  this->waSkinModell=waSkinModell;
  this->sliderBarMapId=sliderBarMapId;
  sliderButton=new WaButton(waSkinModell,sliderId,this);
  sliderButton->setCurrentLocation(QPoint(0,0));
  sliderButton->setMove(true);
  connect(sliderButton,SIGNAL(sliderPressEvent(QMouseEvent*)),
	  this,SLOT(sliderPressEvent(QMouseEvent*)));
  connect(sliderButton,SIGNAL(released()),this,SLOT(sliderReleasedEvent()));
  connect(sliderButton,SIGNAL(sliderMoveEvent(QMouseEvent*)),
	  this,SLOT(sliderMoveEvent(QMouseEvent*)));
  connect(this,SIGNAL(valueChanged(int)),this,SLOT(updateSliderPos(int)));
  lDraging=false;
  setRange(0,100);
  sliderButton->setFixedSize(sliderButton->sizeHint());
  sliderButton->setCurrentLocation(QPoint(0,0));

  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinMapDesc(sliderBarMapId);
  setGeometry((*waSkinDesc->getGeometry()));

  

} 


WaSlider::~WaSlider() {
}



QWidget* WaSlider::getButtonWidget() {
  return sliderButton;
}


void WaSlider::setPixmapSliderButtonDown(const char* signal,int pixId) {
  sliderButton->setPixmapDown(signal,pixId);
}


void WaSlider::setPixmapSliderButtonUp(const char* signal,int pixId) {
  sliderButton->setPixmapUp(signal,pixId);
}


void WaSlider::setPixmapSliderBar(const char* signal,int pixId) {
  connect(waSkinModell,signal,this,SLOT(pixmapChange()));
  this->sliderBarId=pixId;
  QPixmap* pixmapCurrent=waSkinModell->get(sliderBarId,value());
  setBackgroundPixmap(*pixmapCurrent); 
}
  

QSize WaSlider::sizeHint() const {
  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinMapDesc(sliderBarMapId);
  return QSize(waSkinDesc->getWidgetWidth(),waSkinDesc->getWidgetHeight());
}



void WaSlider::pixmapChange() {
  QPixmap* pixmapCurrent=waSkinModell->get(sliderBarId,value());
  setBackgroundPixmap(*pixmapCurrent);
}



void WaSlider::paintEvent(QPaintEvent* paintEvent) {
}


void WaSlider::sliderMoveEvent (QMouseEvent* e ) {
  if (lDraging == false) {
    return;
  }
  QPoint point(mapToGlobal(QPoint(0,0)));
  QPoint globalPos=mapToGlobal(e->pos());
  QPoint currentLocation=sliderButton->getCurrentLocation();

  int newX=globalPos.x()-point.x()-pressPoint.x();
  newX=currentLocation.x()+newX;
  if (newX < 0) {
    newX=0;
  }
  QSize sliderSize=sliderButton->sizeHint();
  QSize size=sizeHint();
  if (newX > size.width()-sliderSize.width()) {
    newX= size.width()-sliderSize.width();
  }
  int value=pixel2Value(newX);
  setValue(value);
  pixmapChange();
}


void WaSlider::updateSliderPos(int value) {
  if (value > maxValue()) {
    value=maxValue();
  }
  if (value < minValue()) {
    value = minValue();
  }
  QPoint currentLocation=sliderButton->getCurrentLocation();
  int pixelPos=value2Pixel(value);
  currentLocation.setX(pixelPos);

  sliderButton->setCurrentLocation(currentLocation);
  pixmapChange();
  sliderButton->repaint(false);  
}


void WaSlider::sliderPressEvent(QMouseEvent* e) {
  pressPoint.setX(e->x());
  lDraging=true;
  emit(sliderPressed());
}

void WaSlider::sliderReleasedEvent() {
  lDraging=false;
  emit(sliderReleased());
}


int WaSlider::pixel2Value(int xpos) {
  QSize size=sizeHint();
  QSize sliderSize=sliderButton->sizeHint();
  float min=(float)minValue();
  float max=(float)maxValue();
  float fmin=min;
  float fmax=max;
  // under sun fabs(..) seems to be wrong
  // they cant get a simple fabs to work ? 
  // maybe I'm doing something wrong.
  // heres the workaround:(obviously correct)
  if (min < 0) {
    fmin=-1*fmin;
  }
  if (max < 0) {
    fmax=-1*fmax;
  }
  
  float valuerange=fmin+fmax;
  float pixelrange=(float)(size.width()-sliderSize.width());
  float anstieg=valuerange/pixelrange;
  float value=(float)xpos*anstieg+min;
  return (int)value;
}


int WaSlider::value2Pixel(int value) {
  QSize size=sizeHint();
  QSize sliderSize=sliderButton->sizeHint();
  float min=(float)minValue();
  float max=(float)maxValue();
  float fmin=min;
  float fmax=max;
  if (min < 0) {
    fmin=-1*fmin;
  }
  if (max < 0) {
    fmax=-1*fmax;
  }
  float valuerange=fmin+fmax;
  float verhaeltnis=fmin/valuerange;  
  float pixelrange=(float)(size.width()-sliderSize.width());
  float zeropoint=verhaeltnis*pixelrange;
  float anstieg=pixelrange/valuerange;

  float pixel=(float)value*anstieg+zeropoint; 
  return (int)pixel;
}



#include "waSlider.moc"
