/*
  a GUI for a spectrum analyser
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef _GUISPECTRUMANALYSER_H
#define _GUISPECTRUMANALYSER_H

#include <iostream.h>
#include <qpainter.h>
#include <qtimer.h> 
#include <qpixmap.h> 
#include <qwidget.h>
#include <kpixmap.h>
#include <qarray.h>
#include <qlist.h>
#include <qcolor.h>

#include <spectrumImage.h>
#include <math.h>
#include <plugin.h>


class GuiSpectrumAnalyser : public QWidget, public MonoFFTScope {
  Q_OBJECT

 public:
  GuiSpectrumAnalyser(QWidget *parent=0, const char *name=0, int refresh = 25);

  QSize sizeHint() const;
  void mousePressEvent ( QMouseEvent* mouseEvent);
  void scopeEvent(float *, int);
  void reload();

 private:
  QColor mFGColor;
  QColor mBGColor;
  int mBarWidth;
  QArray<QPixmap *> bars;
  QList<QPixmap> memoryManager;
};
#endif

// TODO: fix this
// See http://derkarl.org/s-and-s.html for details
// vim: ts=2 et
