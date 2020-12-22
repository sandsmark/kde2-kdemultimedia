/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WASTATUS_H
#define __WASTATUS_H

#include <waSkinModell.h>
#include <qpainter.h>
#include <qwidget.h>

#define _STATUS_PLAYING 1
#define _STATUS_STOPPED 2
#define _STATUS_PAUSED  3


class WaStatus : public QWidget {
 Q_OBJECT

  QPixmap* playPixmap;
  QPixmap* pausePixmap;
  QPixmap* stopPixmap;
 
  WaSkinModell* waSkinModell;
  int nStatus;

 public:
   WaStatus(WaSkinModell* waSkinModell,QWidget* parent=0,char* name=0);
   ~WaStatus();

   void setStatus(int status);
   int getStatus();

 public slots:
  QSize sizeHint() const;
  void paintEvent ( QPaintEvent * paintEvent );

 private slots:
  void pixmapChange();
     
};
#endif
