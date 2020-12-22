/*
  Winamp Skin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */

#include <kconfig.h>
#include <qstringlist.h>
#include <kglobal.h>
#include <kstddirs.h>

#include "waSkin.h"


WaSkin::WaSkin(QString loadDir,QWidget *parent, const char *name) 
  : GuiPlugin(parent,name) { 

  
  waSkinModell=new WaSkinModell();

  
  loadSkin(loadDir);

  createButtons();
  createHighLevelElements();
  sizeButtons();
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());
}


WaSkin::~WaSkin() {

}

void WaSkin::loadSkin(QString newSkinDir) {
  if (skinDir != newSkinDir) {
    WaSkinOperation::loadSkinSet(newSkinDir,waSkinModell);
    skinDir=newSkinDir;
  }

}


void WaSkin::setSong(QString song) {
  waInfo->setSong(song);
}




int  WaSkin::getSkinId() {
  return __SKINID_WINAMP;
}



QSize WaSkin::sizeHint() const {
  WaSkinDesc* waSkinDesc=waSkinModell->getWaSkinDesc(_WA_SKIN_MAIN);
  int w=waSkinDesc->getWidth();
  int h=waSkinDesc->getHeight();
  return QSize(w,h);
}


void WaSkin::paintEvent (QPaintEvent* paintEvent) {
  QPixmap* pixmapCurrent=waSkinModell->get(_WA_SKIN_MAIN);
  QPainter paint;
  paint.begin( this );
  paint.drawPixmap ( 0, 0, *pixmapCurrent);
  paint.end();
}


void WaSkin::createButtons() {
  prev=new WaButton(waSkinModell,_WA_MAPPING_CBUTTONS_PREV,this);
  play=new WaButton(waSkinModell,_WA_MAPPING_CBUTTONS_PLAY,this);
  pause=new WaButton(waSkinModell,_WA_MAPPING_CBUTTONS_PAUSE,this);
  stop=new WaButton(waSkinModell,_WA_MAPPING_CBUTTONS_STOP,this);
  next=new WaButton(waSkinModell,_WA_MAPPING_CBUTTONS_NEXT,this);
  eject=new WaButton(waSkinModell,_WA_MAPPING_CBUTTONS_EJECT,this);
  shuffle=new WaButton(waSkinModell,_WA_MAPPING_SHUFFLE,this);
  repeat=new WaButton(waSkinModell,_WA_MAPPING_REPEAT,this);
  playlist=new WaButton(waSkinModell,_WA_MAPPING_PLAYLIST,this);
  eq=new WaButton(waSkinModell,_WA_MAPPING_EQ,this);

        titleshade = new WaButton(waSkinModell, _WA_MAPPING_TITLE_SHADE, this);
        titleshade->setPixmapUp(SIGNAL(event_WA_SKIN_TITLE_SHADE_NORM()), _WA_SKIN_TITLE_SHADE_NORM);
	titleshade->setPixmapDown(SIGNAL(event_WA_SKIN_TITLE_SHADE_PRES()), _WA_SKIN_TITLE_SHADE_PRES);
        connect(titleshade,SIGNAL(pressed()),this,SIGNAL(shadeEvent()));

  shuffle->setToggleFlag(true);
  repeat->setToggleFlag(true);
  playlist->setToggleFlag(true);
  connect(shuffle,SIGNAL(setSelectedEvent(int)),
	  this,SIGNAL(shuffleClickedEvent(int)));

  connect(repeat,SIGNAL(setSelectedEvent(int)),
	  this,SIGNAL(repeatClickedEvent(int)));

  connect(playlist,SIGNAL(setSelectedEvent(int)),
	  this,SIGNAL(playlistClickedEvent(int)));

  prev->setPixmapUp(SIGNAL(event_WA_SKIN_CBUTTONS_PREV_NORM()),
                    _WA_SKIN_CBUTTONS_PREV_NORM);
  prev->setPixmapDown(SIGNAL(event_WA_SKIN_CBUTTONS_PREV_PRES()),
		      _WA_SKIN_CBUTTONS_PREV_PRES);
  connect(prev,SIGNAL(pressed()),this,SIGNAL(playPrevEvent()));

  play->setPixmapUp(SIGNAL(event_WA_SKIN_CBUTTONS_PLAY_NORM()),
                     _WA_SKIN_CBUTTONS_PLAY_NORM);
  play->setPixmapDown(SIGNAL(event_WA_SKIN_CBUTTONS_PLAY_PRES()),
		     _WA_SKIN_CBUTTONS_PLAY_PRES);
  connect(play,SIGNAL(pressed()),this,SIGNAL(playCurrentEvent()));


  
  
  pause->setPixmapUp(SIGNAL(event_WA_SKIN_CBUTTONS_PAUSE_NORM()),
                    _WA_SKIN_CBUTTONS_PAUSE_NORM);
  pause->setPixmapDown(SIGNAL(event_WA_SKIN_CBUTTONS_PAUSE_PRES()),
		      _WA_SKIN_CBUTTONS_PAUSE_PRES);
  connect(pause,SIGNAL(pressed()),this,SIGNAL(playPauseEvent()));



  stop->setPixmapUp(SIGNAL(event_WA_SKIN_CBUTTONS_STOP_NORM()),
                    _WA_SKIN_CBUTTONS_STOP_NORM);
  stop->setPixmapDown(SIGNAL(event_WA_SKIN_CBUTTONS_STOP_PRES()),
		      _WA_SKIN_CBUTTONS_STOP_PRES);
  connect(stop,SIGNAL(pressed()),this,SIGNAL(playStopEvent()));



  next->setPixmapUp(SIGNAL(event_WA_SKIN_CBUTTONS_NEXT_NORM()),
                    _WA_SKIN_CBUTTONS_NEXT_NORM);
  next->setPixmapDown(SIGNAL(event_WA_SKIN_CBUTTONS_NEXT_PRES()),
		     _WA_SKIN_CBUTTONS_NEXT_PRES);
  connect(next,SIGNAL(pressed()),this,SIGNAL(playNextEvent()));


  eject->setPixmapUp(SIGNAL(event_WA_SKIN_CBUTTONS_EJECT_NORM()),
                    _WA_SKIN_CBUTTONS_EJECT_NORM);
  eject->setPixmapDown(SIGNAL(event_WA_SKIN_CBUTTONS_EJECT_PRESS()),
		      _WA_SKIN_CBUTTONS_EJECT_PRESS);
  connect(eject,SIGNAL(pressed()),this,SIGNAL(ejectEvent()));



  shuffle->setPixmapUp(SIGNAL(event_WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_NORM()),
		       _WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_NORM);
  shuffle->setPixmapDown(SIGNAL(event_WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_PRES()),
		     _WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_PRES);
  shuffle->setPixmapUpSelected(
           SIGNAL(event_WA_SKIN_SHUFREP_SHUFFLE_SET_NORM()),
	   _WA_SKIN_SHUFREP_SHUFFLE_SET_NORM);
  shuffle->setPixmapDownSelected(
	   SIGNAL(event_WA_SKIN_SHUFREP_SHUFFLE_SET_PRES()),
	   _WA_SKIN_SHUFREP_SHUFFLE_SET_PRES);
  shuffle->setSelected(true);

  repeat->setPixmapUp(SIGNAL(event_WA_SKIN_SHUFREP_REPEAT_NOT_SET_NORM()),
                     _WA_SKIN_SHUFREP_REPEAT_NOT_SET_NORM);
  repeat->setPixmapDown(SIGNAL(event_WA_SKIN_SHUFREP_REPEAT_NOT_SET_PRES()),
		     _WA_SKIN_SHUFREP_REPEAT_NOT_SET_PRES );
  repeat->setPixmapUpSelected(
          SIGNAL(event_WA_SKIN_SHUFREP_REPEAT_SET_NORM()),
          _WA_SKIN_SHUFREP_REPEAT_SET_NORM);
  repeat->setPixmapDownSelected(
          SIGNAL(event_WA_SKIN_SHUFREP_REPEAT_SET_PRES()),
	  _WA_SKIN_SHUFREP_REPEAT_SET_PRES );


  eq->setPixmapUp(SIGNAL(event_WA_SKIN_SHUFREP_EQ_NOT_SET_NORM()),
		  _WA_SKIN_SHUFREP_EQ_NOT_SET_NORM);
  eq->setPixmapDown(SIGNAL(event_WA_SKIN_SHUFREP_EQ_NOT_SET_PRES()),
		    _WA_SKIN_SHUFREP_EQ_NOT_SET_PRES );

  

  playlist->setPixmapUp(SIGNAL(event_WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_NORM()),
			_WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_NORM);
 playlist->setPixmapDown(SIGNAL(event_WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_PRES())
			 ,_WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_PRES);

  playlist->setPixmapUpSelected(
	    SIGNAL(event_WA_SKIN_SHUFREP_PLAYLIST_SET_NORM()),
	    _WA_SKIN_SHUFREP_PLAYLIST_SET_NORM);
 playlist->setPixmapDownSelected(
	    SIGNAL(event_WA_SKIN_SHUFREP_PLAYLIST_SET_PRES())
	    ,_WA_SKIN_SHUFREP_PLAYLIST_SET_PRES);

}



void WaSkin::sizeButtons() {
  prev->setFixedSize(prev->sizeHint());
  play->setFixedSize(play->sizeHint());
  pause->setFixedSize(pause->sizeHint());
  stop->setFixedSize(stop->sizeHint());
  next->setFixedSize(next->sizeHint());
  eject->setFixedSize(eject->sizeHint());
  shuffle->setFixedSize(shuffle->sizeHint());
  repeat->setFixedSize(repeat->sizeHint());
  playlist->setFixedSize(playlist->sizeHint());
  eq->setFixedSize(eq->sizeHint());
  titleshade->setFixedSize(titleshade->sizeHint());

}




void WaSkin::createHighLevelElements() {
        KConfig *config = KGlobal::config();
        config->setGroup("Winskin");
	int refreshRate = config->readNumEntry("ScopeFPS", 2);
  WaSkinDesc* waSkinDesc;
        if (refreshRate == 0)
        {
	        // Wants 10 FPS
                guiSpectrumAnalyser = new GuiSpectrumAnalyser(this, "WinSkin", 100);
        }
        if (refreshRate == 1)
        {
	        // Wants 25 FPS
                guiSpectrumAnalyser = new GuiSpectrumAnalyser(this, "WinSkin", 40);
        }
        if (refreshRate == 2)
        {
	        // Wants 40 FPS
                guiSpectrumAnalyser = new GuiSpectrumAnalyser(this,"WinSkin" ,25);
        }
        if (refreshRate == 3)
        {
	        // Wants ~ 55 FPS
                guiSpectrumAnalyser = new GuiSpectrumAnalyser(this, "WinSkin", 18);
        }
        if (refreshRate == 4)
        {
	        // Wants ~ 70 FPS
                guiSpectrumAnalyser = new GuiSpectrumAnalyser(this, "WinSkin", 14);
        }

  waSkinDesc=waSkinModell->getWaSkinMapDesc(_WA_MAPPING_ANALYSER);
  guiSpectrumAnalyser->setGeometry((*waSkinDesc->getGeometry()));

  waJumpSlider= new WaJumpSlider(waSkinModell,this);
  waJumpSlider->buildGui();
  waJumpSlider->setFixedSize(waJumpSlider->sizeHint());


  waVolumeSlider= new WaVolumeSlider(waSkinModell,this);
  waVolumeSlider->buildGui();
  waVolumeSlider->setFixedSize(waVolumeSlider->sizeHint());
  connect(waVolumeSlider,SIGNAL(volumeSetValue(int)),
	  this,SIGNAL(volumeSetValue(int)));

  waBalanceSlider= new WaBalanceSlider(waSkinModell,this);
  waBalanceSlider->buildGui();
  waBalanceSlider->setFixedSize(waBalanceSlider->sizeHint());
  connect(waBalanceSlider,SIGNAL(balanceSetValue(int)),
	  this,SIGNAL(balanceSetValue(int)));
 

  waDigit= new WaDigit(waSkinModell,this);
  waDigit->setFixedSize(waDigit->sizeHint());


  waBPS= new WaBPS(waSkinModell,this);
  waBPS->setFixedSize(waBPS->sizeHint());

  waFreq= new WaFreq(waSkinModell,this);
  waFreq->setFixedSize(waFreq->sizeHint());

  waInfo= new WaInfo(waSkinModell,this);
  waInfo->setFixedSize(waInfo->sizeHint());

  waStatus= new WaStatus(waSkinModell,this);
  waStatus->setFixedSize(waStatus->sizeHint());

  waStereo= new WaStereo(waSkinModell,this);
  waStereo->setFixedSize(waStereo->sizeHint());

  waMono= new WaMono(waSkinModell,this);
  waMono->setFixedSize(waMono->sizeHint());

  waTitleBar = new WaTitleBar(waSkinModell, this);
  waTitleBar->setFixedSize(waTitleBar->sizeHint());

  connect(waJumpSlider,SIGNAL(jump(int)),this,SIGNAL(jump(int)));
  connect(waJumpSlider,SIGNAL(jumpSliderPressed()),
	  this,SIGNAL(jumpSliderPressed()));
  connect(waJumpSlider,SIGNAL(jumpSliderReleased()),
	  this,SIGNAL(jumpSliderReleased()));

  // now a few connections to the modell
  connect(waSkinModell,SIGNAL(event_WA_SKIN_MAIN()),
	  this,SLOT(pixmapChange()));

}




void WaSkin::pixmapChange() {
  repaint(false);
}




void WaSkin::setRepeat(int val) {
  repeat->setSelected(val);
}


void WaSkin::setShuffle(int val) {
  shuffle->setSelected(val);
}


void WaSkin::setPlaylist(int val) {
  playlist->setSelected(val);
}

  
void WaSkin::setBPS(int val) {
  waBPS->setBPS(val);
}


void WaSkin::setFreq(int val) {
  waFreq->setFreq(val);
}
 
void WaSkin::setJumpValue(int val) {
  waJumpSlider->setJumpValue(val);
}

void WaSkin::setJumpRange(int val) {
  waJumpSlider->setJumpRange(val);
}

void WaSkin::setVolumeValue(int val) {
  waVolumeSlider->setVolumeValue(val);
}


void WaSkin::setBalanceValue(int val) {
  waBalanceSlider->setBalanceValue(val);
}
 

void WaSkin::setStatus(int val) {
  waStatus->setStatus(val);
}

void WaSkin::setStereo(int val) {
  if (val == true) {
    waStereo->setStereo(true);
    waMono->setMono(false);
  } else {
    waStereo->setStereo(false);
    waMono->setMono(true);
  }
}


void WaSkin::setTime(QString timeString) {
  waDigit->setTime(timeString);
}


void WaSkin::setSpectrum(float floatArray[75]) {
  // What is this thing? - (Name redacted due to shame)
  // guiSpectrumAnalyser->setValues(floatArray);
}
#include "waSkin.moc"
