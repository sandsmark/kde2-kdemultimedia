/*
  generic type for describing skins.
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <waSkinDesc.h>

WaSkinDesc::WaSkinDesc() {
  pixmap=new QPixmap();
}


WaSkinDesc::~WaSkinDesc() {
  delete pixmap;
}


void WaSkinDesc::setFilename(QString fname) {
  this->filename=fname;
}


void WaSkinDesc::setPosId(int posId) {
  this->posId=posId;
}


void WaSkinDesc::setX(int x) {
  this->x=x;
}


void WaSkinDesc::setY(int y) {
  this->y=y;
}


void WaSkinDesc::setWidth(int width) {
  this->width=width;
}


void WaSkinDesc::setHeight(int height) {
  this->height=height;
}


void WaSkinDesc::setPixmap(QPixmap* apix) {
  pixmap->resize(apix->width(),apix->height());
  bitBlt(pixmap,0,0,apix);
}


QString WaSkinDesc::getFilename() {
  return filename;
}


int WaSkinDesc::getPosId() {
  return posId;
}


int WaSkinDesc::getX() {
  return x;
}


int WaSkinDesc::getY() {
  return y;
}


int WaSkinDesc::getWidth() {
  return width;
}


int WaSkinDesc::getHeight() {
  return height;
}


QPixmap* WaSkinDesc::getPixmap() {
  return pixmap;
}


int WaSkinDesc::getWidgetId() {
  return widgetId;
}


int WaSkinDesc::getWidgetX() {
  return widgetX;
}


int WaSkinDesc::getWidgetY() {
  return widgetY;
}


int WaSkinDesc::getWidgetWidth() {
  return widgetWidth;
}


int WaSkinDesc::getWidgetHeight() {
  return widgetHeight;
}


void WaSkinDesc::setWidgetId(int posId) {
  this->widgetId=posId;
}


void WaSkinDesc::setWidgetX(int x) {
  this->widgetX=x;
}


void WaSkinDesc::setWidgetY(int y) {
  this->widgetY=y;
}


void WaSkinDesc::setWidgetWidth(int width) {
  this->widgetWidth=width;
}


void WaSkinDesc::setWidgetHeight(int height) {
  this->widgetHeight=height;
}

void WaSkinDesc::print() {
#ifdef COUT_MESS
  cout << "WaSkinDesc [START]:"<<getFilename().latin1()<<endl;
  cout << "posID:"<<getPosId()<<endl;
  cout << "x in file:"<<getX()<<endl;
  cout << "y in file:"<<getY()<<endl;
  cout << "w in file:"<<getWidth()<<endl;
  cout << "h in file:"<<getHeight()<<endl;
  cout << "id:"<<getWidgetId()<<endl;
  cout << "widgetX:"<<getWidgetX()<<endl;
  cout << "widgetY:"<<getWidgetY()<<endl;
  cout << "widgetH:"<<getWidgetWidth()<<endl;
  cout << "widgetW:"<<getWidgetHeight()<<endl;
  cout << "WaSkinDesc [END]"<<endl;
#endif
}
  
    

const QRect* WaSkinDesc::getGeometry() {
  geometry.setX(widgetX);
  geometry.setY(widgetY);
  geometry.setWidth(widgetWidth);
  geometry.setHeight(widgetHeight);
  return &geometry;
}
 
  
 
		
#include "waSkinDesc.moc"
