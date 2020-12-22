/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WAINFO_H
#define __WAINFO_H

#include <waSkinModell.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qwidget.h>

class WaInfo : public QWidget {
 Q_OBJECT

  QPixmap* completePixmap;
  QPixmap* buildPixmap;
  WaSkinModell* waSkinModell;
  QString text;
  QTimer* timer;
  int xScrollPos;
  int xScrollDirection;


 public:
   WaInfo(WaSkinModell* waSkinModell,QWidget* parent=0,char* name=0);
   ~WaInfo();

   void setSong(QString song);
   QString getSong();


 public slots:
  QSize sizeHint() const;
  void paintEvent ( QPaintEvent * paintEvent );

 private slots:
  void pixmapChange();
  void timeEvent();
  void scrollerSetup();
  


};
#endif

