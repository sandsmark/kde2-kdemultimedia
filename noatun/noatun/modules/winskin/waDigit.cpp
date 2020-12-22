/*
  The digit for the time
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waDigit.h>




WaDigit::WaDigit(WaSkinModell* waSkinModell,QWidget* parent,char* name):
 QWidget(parent,name) {
  this->waSkinModell=waSkinModell;
  
  connect(waSkinModell,
	  SIGNAL(event_WA_SKIN_NUMBERS()),this,SLOT(pixmapChange()));


  digit1=new QPixmap();
  digit2=new QPixmap();
  digit3=new QPixmap();
  digit4=new QPixmap();
  minus=new QPixmap();
  background=new QPixmap();

  reloadBackground();
}

WaDigit::~WaDigit() {
  delete digit1;
  delete digit2;
  delete digit3;
  delete digit4;
  delete minus;
  delete background;
}


QSize WaDigit::sizeHint() const {
  return QSize(63,13);
}


void WaDigit::setTime(QString time) {
  timeString=time;
  pixmapChange();
}


QString WaDigit::getTime() {
  return timeString;
}

void WaDigit::paintEvent (QPaintEvent* paintEvent) {
  const char* timeString=getTime().latin1();
  int len=strlen(timeString);
  char cMinus=' ';
  char cDigit1=' ';
  char cDigit2=' ';
  char cDigit3=' ';
  char cDigit4=' ';
  
  if (len == 0) {
    return;
  }
  cMinus='-';
  int match=sscanf(timeString,"-%c%c:%c%c",
		   &cDigit1,&cDigit2,&cDigit3,&cDigit4);
  if (match != 4) {
    cMinus='-';
    cDigit1=' ';
    match=sscanf(timeString,"-%c:%c%c",&cDigit2,&cDigit3,&cDigit4);
    match++;
  }

  if (match != 4) {
    cMinus=' ';
    cDigit1=' ';
    match=sscanf(timeString,"%c:%c%c",&cDigit2,&cDigit3,&cDigit4);
    match++;
  }
  if (match != 4) {
    cMinus=' ';
    match=sscanf(timeString,"%c%c:%c%c",&cDigit1,&cDigit2,&cDigit3,&cDigit4);
  }

  if (match != 4) {
    return;
  }
    
  waSkinModell->getNumber(_WA_SKIN_NUMBERS,cMinus,minus);
  waSkinModell->getNumber(_WA_SKIN_NUMBERS,cDigit1,digit1);
  waSkinModell->getNumber(_WA_SKIN_NUMBERS,cDigit2,digit2);
  waSkinModell->getNumber(_WA_SKIN_NUMBERS,cDigit3,digit3);
  waSkinModell->getNumber(_WA_SKIN_NUMBERS,cDigit4,digit4);


  QPainter paint;
  WaSkinDesc* mapDesc;
  int x=40;
  int y=26;
  move(x,y);
  paint.begin(this);
  paint.drawPixmap(0,0,*background);
  mapDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_MINUS);
  paint.drawPixmap(mapDesc->getWidgetX()-x,mapDesc->getWidgetY()-y,
		   *minus);

  mapDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_DIGIT_1);
  paint.drawPixmap(mapDesc->getWidgetX()-x,mapDesc->getWidgetY()-y,
		   *digit1);

  mapDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_DIGIT_2);
  paint.drawPixmap(mapDesc->getWidgetX()-x,mapDesc->getWidgetY()-y,
		   *digit2);
  
  mapDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_DIGIT_3);
  paint.drawPixmap(mapDesc->getWidgetX()-x,mapDesc->getWidgetY()-y,
		   *digit3);
  
  mapDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_DIGIT_4);
  paint.drawPixmap(mapDesc->getWidgetX()-x,mapDesc->getWidgetY()-y,
		   *digit4);
  paint.end();  
  
  
}
    

void WaDigit::pixmapChange() {
  reloadBackground();
  repaint(false);
}

void WaDigit::reloadBackground() {
  QPixmap* pix=waSkinModell->get(_WA_SKIN_MAIN);
  QSize size=sizeHint();
  background->resize(size.width(),size.height());
  bitBlt(background,0,0,pix,40,26,size.width(),size.height());
  setBackgroundPixmap(*background);
}

#include "waDigit.moc"
