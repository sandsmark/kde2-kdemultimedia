/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WABUTTON_H
#define __WABUTTON_H

#include <waSkinModell.h>
#include <qpushbutton.h>
#include <qpainter.h>

/**
   WaButton does a lot of stuff. It is used as a slider in slidebars,
   as a togglebuttn for playlist,shuffle,repeat and as normal
   clicks buttons with pixmaps.
   Clean solutions should beter split all this into different
   classes, but winamp skins do not change very much, thus
   this hack is ok (for winamp skins)
*/


class WaButton : public QPushButton {
 Q_OBJECT

  int nUpId;
  int nDownId;
  int nDownIdSelected;
  int nUpIdSelected;
  
  WaSkinModell* waSkinModell;
  int lSelected;
  QPoint currentLocation;
  int mapId;
  int lmove;

 public:
   WaButton(WaSkinModell* waSkinModell,int mapId,
	    QWidget* parent=0,char* name=0);
   ~WaButton();

 public slots:
   void setPixmapDown(const char* signal,int pixId);
   void setPixmapUp(const char* signal,int pixId);
   void setPixmapUpSelected(const char* signal,int pixId);
   void setPixmapDownSelected(const char* signal,int pixId);

   int getSelected();
   void setSelected(int lSelect);
   

   QSize sizeHint() const;
   // for buttons used as slider:
   QPoint getCurrentLocation();
   void setCurrentLocation(QPoint point);
   void setMove(int lmove);
   int getMove();
   void setToggleFlag(int flag);

   void paintEvent (QPaintEvent* paintEvent);
 signals:
   void sliderMoveEvent(QMouseEvent* e);
   void sliderPressEvent(QMouseEvent* e);
   void setSelectedEvent(int val);
   
 private slots:
   void pixmapChange();
   void mouseMoveEvent (QMouseEvent* e);
   void mousePressEvent (QMouseEvent* e);
   void doToggle(bool val);

     
};
#endif
