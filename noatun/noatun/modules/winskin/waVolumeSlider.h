/*
  jumpslider for winamp skins
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __WAVOLUMESLIDER_H
#define __WAVOLUMESLIDER_H

#include <waSlider.h>


class WaVolumeSlider : public QWidget {
  Q_OBJECT

  WaSkinModell* waSkinModell;
  WaSlider* ws;


 public:
  WaVolumeSlider(WaSkinModell* waSkinModell,
	       QWidget *parent=0, const char *name=0);
  ~WaVolumeSlider();
  void buildGui();
  QSize sizeHint() const;
  
  
  void paintEvent ( QPaintEvent * paintEvent );

  void setVolumeValue(int val);

  
 signals:
  void volumeSetValue(int val);

  

};


#endif


