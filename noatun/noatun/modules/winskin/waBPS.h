/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WABPS_H
#define __WABPS_H

#include <waSkinModell.h>
#include <qpainter.h>
#include <qwidget.h>


class WaBPS : public QWidget {
 Q_OBJECT

  QPixmap* completePixmap;
  QPixmap* buildPixmap;
  WaSkinModell* waSkinModell;
  QString bps;

 public:
   WaBPS(WaSkinModell* waSkinModell,QWidget* parent=0,char* name=0);
   ~WaBPS();

   void setBPS(int bps);
   int getBPS();


 public slots:
  void setSongEvent();
  QSize sizeHint() const;
  void paintEvent (QPaintEvent * paintEvent);

 private slots:
  void pixmapChange();
     
};
#endif
