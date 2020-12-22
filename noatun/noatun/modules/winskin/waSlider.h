/*
  standard Slider for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WASLIDER_H
#define __WASLIDER_H

#include <qslider.h>
#include <waSkinModell.h>
#include <waButton.h>
#include <qpainter.h>



class WaSlider : public QSlider {
 Q_OBJECT

  WaSkinModell* waSkinModell;
  WaButton* sliderButton;
  int sliderBarId;
  int sliderBarMapId;
  int lDraging;
  QPoint pressPoint;

 public:
   WaSlider(WaSkinModell* waSkinModell,int sliderBarMapId,int sliderId,
	    QWidget* parent=0,char* name=0);
   ~WaSlider();

   QWidget* getButtonWidget();
 public slots:
   void setPixmapSliderButtonDown(const char* signal,int pixId);
   void setPixmapSliderButtonUp(const char* signal,int pixId);

   void setPixmapSliderBar(const char* signal,int pixId);
   QSize sizeHint() const;
   void paintEvent (QPaintEvent* paintEvent);

 private slots:
   void pixmapChange();
   void sliderPressEvent(QMouseEvent* e);
   void sliderReleasedEvent();
   void sliderMoveEvent (QMouseEvent* e);
   void updateSliderPos(int value);

 private:
   int pixel2Value(int xpos);
   int value2Pixel(int value);

};
#endif




