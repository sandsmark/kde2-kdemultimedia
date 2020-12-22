/*
  The digit for the time
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WADIGIT_H
#define __WADIGIT_H

#include <waSkinModell.h>
#include <qpushbutton.h>
#include <qpainter.h>


class WaDigit : public QWidget {
 Q_OBJECT

  int nDigitId;
  
  WaSkinModell* waSkinModell;
  QPixmap* digit1;
  QPixmap* digit2;
  QPixmap* digit3;
  QPixmap* digit4;
  QPixmap* minus;
  QPixmap* background;
  QString timeString;

 public:
   WaDigit(WaSkinModell* waSkinModell,QWidget* parent=0,char* name=0);
   ~WaDigit();

   void setTime(QString time);
   QString getTime();

 public slots:
   QSize sizeHint() const;
   // for buttons used as slider:
   void paintEvent (QPaintEvent* paintEvent);
   
 private slots:
   void pixmapChange();

 private:
   void reloadBackground();     
};
#endif
