/*
  generic type for describing skins.
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef _SKINDESCRIPTION_H
#define _SKINDESCRIPTION_H

#include <qpixmap.h>
#include <qstring.h>
#include <qobject.h>
#include <iostream>

typedef struct {
  const char* filename;
  int posId;
  int x;
  int y;
  int width;
  int height;
} SkinDesc;


class WaSkinDesc : public QObject {
 Q_OBJECT
  
  QString filename;
  int posId;
  int x;
  int y;
  int width;
  int height;
  QPixmap* pixmap;

  int widgetId;
  int widgetX;
  int widgetY;
  int widgetWidth;
  int widgetHeight;
  QRect geometry;
  

 public:
  WaSkinDesc();
  ~WaSkinDesc();

  //
  // The mapping from bmp file to local pixmap
  //

  void setFilename(QString filename);
  void setPosId(int posId);
  void setX(int x);
  void setY(int y);
  void setWidth(int width);
  void setHeight(int height);
  void setPixmap(QPixmap* pixmap);
  const QRect* getGeometry();

  QString getFilename();
  int getPosId();
  int getX();
  int getY();
  int getWidth();
  int getHeight();
  QPixmap* getPixmap();

  //
  // Now the mapping from local pixmap to the GUI object
  //


  int getWidgetId();
  int getWidgetX();
  int getWidgetY();
  int getWidgetWidth();
  int getWidgetHeight();  

  void setWidgetId(int posId);
  void setWidgetX(int x);
  void setWidgetY(int y);
  void setWidgetWidth(int width);
  void setWidgetHeight(int height);

  void print();

};
#endif
