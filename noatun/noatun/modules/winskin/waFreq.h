/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WAFREQ_H
#define __WAFREQ_H

#include <waSkinModell.h>
#include <qpainter.h>
#include <qwidget.h>

class WaFreq : public QWidget {
 Q_OBJECT

  QPixmap* completePixmap;
  QPixmap* buildPixmap;
  WaSkinModell* waSkinModell;
  QString freq;

 public:
   WaFreq(WaSkinModell* waSkinModell,QWidget* parent=0,char* name=0);
   ~WaFreq();

   void setFreq(int val);
   int getFreq();

 public slots:
  void setSongEvent();
  QSize sizeHint() const;
  void paintEvent ( QPaintEvent * paintEvent );

 private slots:
  void pixmapChange();
     
};
#endif

