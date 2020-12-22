/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WAMONO_H
#define __WAMONO_H

#include <waSkinModell.h>
#include <qpainter.h>
#include <qwidget.h>


class WaMono : public QWidget {
 Q_OBJECT

  QPixmap* noMonoPixmap;
  QPixmap* monoPixmap;
 
  WaSkinModell* waSkinModell;
  int lMono;

 public:
   WaMono(WaSkinModell* waSkinModell,QWidget* parent=0,char* name=0);
   ~WaMono();

   void setMono(int lMono);
   int getMono();


 public slots:
  QSize sizeHint() const;
  void paintEvent ( QPaintEvent * paintEvent );

 private slots:
  void pixmapChange();
     
};
#endif
