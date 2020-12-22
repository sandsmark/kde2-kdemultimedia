/*
  jumpslider for winamp skins
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __WAJUMPSLIDER_H
#define __WAJUMPSLIDER_H

#include <waSlider.h>


class WaJumpSlider : public QWidget {
  Q_OBJECT

  WaSkinModell* waSkinModell;
  WaSlider* ws;

 public:
  WaJumpSlider(WaSkinModell* waSkinModell,
	       QWidget *parent=0, const char *name=0);
  ~WaJumpSlider();
  void buildGui();
  QSize sizeHint() const;
  
  void paintEvent ( QPaintEvent * paintEvent );

  void setJumpRange(int val);
  void setJumpValue(int val);

 signals:
   void jump(int seconds);
   void jumpSliderPressed(); 
   void jumpSliderReleased();

 private slots:  
   void sliderReleased();


};


#endif


