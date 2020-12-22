/*
  Winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __WASKIN_H
#define __WASKIN_H
#include <waJumpSlider.h>
#include <waVolumeSlider.h>
#include <waBalanceSlider.h>

#include <waSkinModell.h>
#include <waSkinOperation.h>


#include <qlayout.h>
#include <qwidget.h>
#include <qpainter.h>
#include <waButton.h>
#include <waDigit.h>
#include <waBPS.h>
#include <waFreq.h>
#include <waInfo.h>
#include <waStatus.h>
#include <waStereo.h>
#include <waMono.h>
#include <waTitleBar.h>
#include "guiSpectrumAnalyser.h"

#include "guiPlugin.h"


#define __SKINID_WINAMP       1

class WaSkin : public GuiPlugin {
  Q_OBJECT

   WaSkinModell* waSkinModell;
   QString skinDir;

 public:
  WaSkin(QString skinDir,QWidget *parent=0, const char *name=0 );
  ~WaSkin();

  int  getSkinId();

  void loadSkin(QString skinDir);
  void setSong(QString song);
  void setRepeat(int val);
  void setShuffle(int val);
  void setPlaylist(int val);
  void setBPS(int val);
  void setFreq(int val);

  void setJumpValue(int val);
  void setJumpRange(int val);
  void setVolumeValue(int val);
  void setBalanceValue(int val);

  void setStatus(int val);
  void setStereo(int val);
  void setTime(QString timeString);
  void setSpectrum(float floatArray[75]);

  QSize sizeHint() const;
  void paintEvent (QPaintEvent* paintEvent);
   GuiSpectrumAnalyser* guiSpectrumAnalyser;


 private slots:
   void pixmapChange();

 private:
   void createButtons();
   void createHighLevelElements();
   void sizeButtons();

   WaButton* prev;
   WaButton* play;
   WaButton* pause;
   WaButton* stop;
   WaButton* next;
   WaButton* eject;
   WaButton* shuffle;
   WaButton* repeat;
   WaButton* playlist;
   WaButton* eq;
   WaButton *titleshade;

   WaJumpSlider* waJumpSlider;
   WaVolumeSlider* waVolumeSlider;
   WaBalanceSlider* waBalanceSlider;
   WaDigit* waDigit;
   WaBPS* waBPS;
   WaFreq* waFreq;
   WaInfo* waInfo;
   WaStatus* waStatus;
   WaStereo* waStereo;
   WaMono* waMono;
   WaTitleBar *waTitleBar;
};
#endif
