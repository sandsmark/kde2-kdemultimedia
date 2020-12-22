/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WASTEREO_H
#define __WASTEREO_H

#include <waSkinModell.h>
#include <qpainter.h>
#include <qwidget.h>

class WaStereo : public QWidget {
 Q_OBJECT

  QPixmap* noStereoPixmap;
  QPixmap* stereoPixmap;
 
  WaSkinModell* waSkinModell;
  int lStereo;

 public:
   WaStereo(WaSkinModell* waSkinModell,QWidget* parent=0,char* name=0);
   ~WaStereo();

   void setStereo(int lStereo);
   int getStereo();


 public slots:
  QSize sizeHint() const;
  void paintEvent ( QPaintEvent * paintEvent );

 private slots:
  void pixmapChange();
     
};
#endif
