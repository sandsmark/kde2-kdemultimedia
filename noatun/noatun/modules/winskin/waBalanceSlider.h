/*
  balanceslider for winamp skins
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __WABALANCESLIDER_H
#define __WABALANCESLIDER_H

#include <waSlider.h>

class WaBalanceSlider : public QWidget {
  Q_OBJECT

  WaSkinModell* waSkinModell;
  WaSlider* ws;


 public:
  WaBalanceSlider(WaSkinModell* waSkinModell,
		  QWidget *parent=0, const char *name=0);
  ~WaBalanceSlider();
  void buildGui();
  QSize sizeHint() const;

  
  void paintEvent ( QPaintEvent * paintEvent );

  void setBalanceValue(int val);

 signals:
   void balanceSetValue(int val);
  

};


#endif


