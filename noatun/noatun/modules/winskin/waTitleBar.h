/*
  standard Button for winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WATITLEBAR_H
#define __WATITLEBAR_H

#include <waSkinModell.h>
#include <qpainter.h>
#include <qwidget.h>

class WaTitleBar : public QWidget {
 Q_OBJECT

  QPixmap *titlePixmap;

  WaSkinModell* waSkinModell;
 public:
   WaTitleBar(WaSkinModell* waSkinModell,QWidget* parent=0,char* name=0);
   ~WaTitleBar();

private:
        void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);

        bool moving;
	QWidget *waParent;
	QPoint mLastPos;
	QPoint mDragStart;

 public slots:
  QSize sizeHint() const;
  void paintEvent ( QPaintEvent * paintEvent );

 private slots:
  void pixmapChange();

};
#endif
