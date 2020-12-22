/*
  Modell for winamp skins
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __WASKINMODELL_H
#define __WASKINMODELL_H

#include <iostream.h>
#include <qstring.h>
#include <waSkins.h>
#include <waSkinDesc.h>
#include <qobject.h>
#include <waSkinOperation.h>

class WaSkinModell : public QObject {
 Q_OBJECT

  WaSkinDesc waSkinDesc[_WA_SKIN_ENTRIES];
  WaSkinDesc waSkinMap[_WA_MAPPING_ENTRIES];
  QString skinDir;

 public:
  WaSkinModell();
  ~WaSkinModell();
  
  void load(int id,QPixmap* pixmap);
  QPixmap* get(int id);
  QPixmap* get(int id,int argument);
  void getNumber(int id,char number,QPixmap* dest);

  int emitChangeEvent(int id);
  WaSkinDesc* getWaSkinDesc(int id);
  WaSkinDesc* getWaSkinMapDesc(int id);
  void setSkinDirectory(QString dir);
  QString getSkinDirectory();


 signals:
 
  void event_WA_SKIN_MAIN();
  void event_WA_SKIN_CBUTTONS_PREV_NORM();
  void event_WA_SKIN_CBUTTONS_PREV_PRES();
  void event_WA_SKIN_CBUTTONS_PLAY_NORM();
  void event_WA_SKIN_CBUTTONS_PLAY_PRES();
  void event_WA_SKIN_CBUTTONS_PAUSE_NORM();
  void event_WA_SKIN_CBUTTONS_PAUSE_PRES();
  void event_WA_SKIN_CBUTTONS_STOP_NORM();
  void event_WA_SKIN_CBUTTONS_STOP_PRES();
  void event_WA_SKIN_CBUTTONS_NEXT_NORM();
  void event_WA_SKIN_CBUTTONS_NEXT_PRES();
  void event_WA_SKIN_CBUTTONS_EJECT_NORM();
  void event_WA_SKIN_CBUTTONS_EJECT_PRESS();
  
  void event_WA_SKIN_MONOSTER_STEREO_TRUE();
  void event_WA_SKIN_MONOSTER_STEREO_FALSE();
  void event_WA_SKIN_MONOSTER_MONO_TRUE();
  void event_WA_SKIN_MONOSTER_MONO_FALSE();
  
  void event_WA_SKIN_NUMBERS();
  void event_WA_SKIN_NUMBERS_MINUS();

  void event_WA_SKIN_SHUFREP_REPEAT_NOT_SET_NORM();
  void event_WA_SKIN_SHUFREP_REPEAT_NOT_SET_PRES();
  void event_WA_SKIN_SHUFREP_REPEAT_SET_NORM();
  void event_WA_SKIN_SHUFREP_REPEAT_SET_PRES();
  void event_WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_NORM();
  void event_WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_PRES();
  void event_WA_SKIN_SHUFREP_SHUFFLE_SET_NORM();
  void event_WA_SKIN_SHUFREP_SHUFFLE_SET_PRES();
  void event_WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_NORM();
  void event_WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_PRES();
  void event_WA_SKIN_SHUFREP_PLAYLIST_SET_NORM();
  void event_WA_SKIN_SHUFREP_PLAYLIST_SET_PRES();
  void event_WA_SKIN_SHUFREP_EQ_SET_NORM();
  void event_WA_SKIN_SHUFREP_EQ_SET_PRES();
  void event_WA_SKIN_SHUFREP_EQ_NOT_SET_NORM();
  void event_WA_SKIN_SHUFREP_EQ_NOT_SET_PRES();

  void event_WA_SKIN_TEXT();
  
  void event_WA_SKIN_VOLUME_BAR();
  void event_WA_SKIN_VOLUME_SLIDER_NORM();
  void event_WA_SKIN_VOLUME_SLIDER_PRES();
 
  void event_WA_SKIN_BALANCE_BAR();
  void event_WA_SKIN_BALANCE_SLIDER_NORM();
  void event_WA_SKIN_BALANCE_SLIDER_PRES();
   
  void event_WA_SKIN_POS_BAR();
  void event_WA_SKIN_POS_BAR_SLIDER_NORM();
  void event_WA_SKIN_POS_BAR_SLIDER_PRES();
  
  void event_WA_SKIN_PLAYPAUS_PLAY();
  void event_WA_SKIN_PLAYPAUS_PAUSE();
  void event_WA_SKIN_PLAYPAUS_STOP();
  void event_WA_SKIN_TITLE();
  void event_WA_SKIN_TITLE_SHADE_PRES();
  void event_WA_SKIN_TITLE_SHADE_NORM();

 private:
  void getText(char text,QPixmap* dest);


};
#endif
