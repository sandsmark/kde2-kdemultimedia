/*
  Modell for winamp skins
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#include <waSkinModell.h>



WaSkinModell::WaSkinModell() {
  
  WaSkinOperation::makeEmptySkinModell(this);
  skinDir="";
}


WaSkinModell::~WaSkinModell() {
}


void WaSkinModell::load(int id,QPixmap* pixmap) {
  int lEmit;

  lEmit=emitChangeEvent(id);
  if (lEmit == true) {
    waSkinDesc[id].setPixmap(pixmap);
  }

}


int WaSkinModell::emitChangeEvent(int id) {
  int back=true;

  switch(id) {
  case _WA_SKIN_MAIN: {
    emit(event_WA_SKIN_MAIN());
    break;
  } 
  case _WA_SKIN_CBUTTONS_PREV_NORM: {
    emit(event_WA_SKIN_CBUTTONS_PREV_NORM());
    break;
  }
  case _WA_SKIN_CBUTTONS_PREV_PRES: {
    emit(event_WA_SKIN_CBUTTONS_PREV_PRES());
    break;
  }
  case _WA_SKIN_CBUTTONS_PLAY_NORM: {
    emit(event_WA_SKIN_CBUTTONS_PLAY_NORM());
    break;
  }
  case _WA_SKIN_CBUTTONS_PLAY_PRES: {
    emit(event_WA_SKIN_CBUTTONS_PLAY_PRES());
    break;
  }
  case _WA_SKIN_CBUTTONS_PAUSE_NORM: {
    emit(event_WA_SKIN_CBUTTONS_PAUSE_NORM());
    break;
  }
  case _WA_SKIN_CBUTTONS_PAUSE_PRES: {
    emit(event_WA_SKIN_CBUTTONS_PAUSE_PRES());
    break;
  }
  case _WA_SKIN_CBUTTONS_STOP_NORM: {
    emit(event_WA_SKIN_CBUTTONS_STOP_NORM());
    break;
  }
  case _WA_SKIN_CBUTTONS_STOP_PRES: {
    emit(event_WA_SKIN_CBUTTONS_STOP_PRES());
    break;
  }
  case _WA_SKIN_CBUTTONS_NEXT_NORM: {
    emit(event_WA_SKIN_CBUTTONS_NEXT_NORM());
    break;
  }
  case _WA_SKIN_CBUTTONS_NEXT_PRES: {
    emit(event_WA_SKIN_CBUTTONS_NEXT_PRES());
    break;
  }
  case _WA_SKIN_CBUTTONS_EJECT_NORM: {
    emit(event_WA_SKIN_CBUTTONS_EJECT_NORM());
    break;
  }
  case _WA_SKIN_CBUTTONS_EJECT_PRESS: {
    emit(event_WA_SKIN_CBUTTONS_EJECT_PRESS());
    break;
  }
  case _WA_SKIN_MONOSTER_STEREO_TRUE: {
    emit(event_WA_SKIN_MONOSTER_STEREO_TRUE());
    break;
  }
  case _WA_SKIN_MONOSTER_STEREO_FALSE: {
    emit(event_WA_SKIN_MONOSTER_STEREO_FALSE());
    break;
  }
  case _WA_SKIN_MONOSTER_MONO_TRUE: {
    emit(event_WA_SKIN_MONOSTER_MONO_TRUE());
    break;
  }
  case _WA_SKIN_MONOSTER_MONO_FALSE: {
    emit(event_WA_SKIN_MONOSTER_MONO_FALSE());
    break;
  }
  case _WA_SKIN_NUMBERS: {
    emit(event_WA_SKIN_NUMBERS());
    break;
  }
  case _WA_SKIN_NUMBERS_MINUS: {
    emit(event_WA_SKIN_NUMBERS_MINUS());
    break;
  } 
  case _WA_SKIN_SHUFREP_REPEAT_NOT_SET_NORM: {
    emit(event_WA_SKIN_SHUFREP_REPEAT_NOT_SET_NORM());
    break;
  }
  case _WA_SKIN_SHUFREP_REPEAT_NOT_SET_PRES: {
    emit(event_WA_SKIN_SHUFREP_REPEAT_NOT_SET_PRES());
    break;
  }
  case _WA_SKIN_SHUFREP_REPEAT_SET_NORM: {
    emit(event_WA_SKIN_SHUFREP_REPEAT_SET_NORM());
    break;
  }
  case _WA_SKIN_SHUFREP_REPEAT_SET_PRES: {
    emit(event_WA_SKIN_SHUFREP_REPEAT_SET_PRES());
    break;
  }
  case _WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_NORM: {
    emit(event_WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_NORM());
    break;
  }
  case _WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_PRES: {
    emit(event_WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_PRES());
    break;
  }
  case _WA_SKIN_SHUFREP_SHUFFLE_SET_NORM: {
    emit(event_WA_SKIN_SHUFREP_SHUFFLE_SET_NORM());
    break;
  }
  case _WA_SKIN_SHUFREP_SHUFFLE_SET_PRES: {
    emit(event_WA_SKIN_SHUFREP_SHUFFLE_SET_PRES());
    break;
  }
  case _WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_NORM: {
    emit(event_WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_NORM());
    break;
  }
  case _WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_PRES: {
    emit(event_WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_PRES());
    break;
  }
  case _WA_SKIN_SHUFREP_PLAYLIST_SET_NORM: {
    emit(event_WA_SKIN_SHUFREP_PLAYLIST_SET_NORM());
    break;
  }
  case _WA_SKIN_SHUFREP_PLAYLIST_SET_PRES: {
    emit(event_WA_SKIN_SHUFREP_PLAYLIST_SET_PRES());
    break;
  }
  case _WA_SKIN_SHUFREP_EQ_SET_NORM: {
    emit(event_WA_SKIN_SHUFREP_EQ_SET_NORM());
    break;
  }
  case _WA_SKIN_SHUFREP_EQ_SET_PRES: {
    emit(event_WA_SKIN_SHUFREP_EQ_SET_PRES());
    break;
  }
  case _WA_SKIN_SHUFREP_EQ_NOT_SET_NORM: {
    emit(event_WA_SKIN_SHUFREP_EQ_NOT_SET_NORM());
    break;
  }
  case _WA_SKIN_SHUFREP_EQ_NOT_SET_PRES: {
    emit(event_WA_SKIN_SHUFREP_EQ_NOT_SET_PRES());
    break;
  }
  case _WA_SKIN_TEXT: {
    emit(event_WA_SKIN_TEXT());
    break;
  }
  case _WA_SKIN_VOLUME_BAR: {
    emit(event_WA_SKIN_VOLUME_BAR());
    break;
  }
  case _WA_SKIN_VOLUME_BAR_ALL_BARS: {
    break;
  }
  case _WA_SKIN_VOLUME_SLIDER_NORM: {
    emit(event_WA_SKIN_VOLUME_SLIDER_NORM());
    break;
  }
  case _WA_SKIN_VOLUME_SLIDER_PRES: {
    emit(event_WA_SKIN_VOLUME_SLIDER_PRES());
    break;
  }
  case _WA_SKIN_BALANCE_BAR: {
    emit(event_WA_SKIN_BALANCE_BAR());
    break;
  }
  case _WA_SKIN_BALANCE_SLIDER_NORM: {
    emit(event_WA_SKIN_BALANCE_SLIDER_NORM());
    break;
  }
  case _WA_SKIN_BALANCE_SLIDER_PRES: {
    emit(event_WA_SKIN_BALANCE_SLIDER_PRES());
    break;
  }
  case _WA_SKIN_POS_BAR: {
    emit(event_WA_SKIN_POS_BAR());
    break;
  }
  case _WA_SKIN_POS_BAR_SLIDER_NORM: {
    emit(event_WA_SKIN_POS_BAR_SLIDER_NORM());
    break;
  }
  case _WA_SKIN_POS_BAR_SLIDER_PRES: {
    emit(event_WA_SKIN_POS_BAR_SLIDER_PRES());
    break;
  }
  case _WA_SKIN_PLAYPAUS_PLAY: {
    emit(event_WA_SKIN_PLAYPAUS_PLAY());
    break;
  }
  case _WA_SKIN_PLAYPAUS_PAUSE: {
    emit(event_WA_SKIN_PLAYPAUS_PAUSE());
    break;
  }
  case _WA_SKIN_PLAYPAUS_STOP: {
    emit(event_WA_SKIN_PLAYPAUS_STOP());
    break;
  }
  case _WA_SKIN_TITLE: {
        emit(event_WA_SKIN_TITLE());
        break;
  }
  case _WA_SKIN_TITLE_SHADE_PRES: {
        emit(event_WA_SKIN_TITLE_SHADE_PRES());
  }
  case _WA_SKIN_TITLE_SHADE_NORM: {
        emit (event_WA_SKIN_TITLE_SHADE_NORM());
  }

  default:
    back=false;
  }
  return back;
}


QPixmap* WaSkinModell::get(int id) {
  QPixmap* back;
  if ((id < 0) || (id >= _WA_SKIN_ENTRIES)) {
    return NULL;
  }
  back=waSkinDesc[id].getPixmap();
  return back;
}

  

WaSkinDesc* WaSkinModell::getWaSkinDesc(int id) {
  if ((id < 0) || (id >= _WA_SKIN_ENTRIES)) {
    return NULL;
  }
  WaSkinDesc* back=&(waSkinDesc[id]);
  return back;
}


WaSkinDesc* WaSkinModell::getWaSkinMapDesc(int id) {
  if ((id < 0) || (id >= _WA_MAPPING_ENTRIES)) {
    return NULL;
  }
  WaSkinDesc* back=&(waSkinMap[id]);
  return back;
}



QPixmap* WaSkinModell::get(int id,int argument) {

  switch (id) {
  case _WA_SKIN_POS_BAR : {
    QPixmap* dest=get(_WA_SKIN_POS_BAR);
    return dest;
    break;
  }    
  case _WA_SKIN_VOLUME_BAR : {
    int percent=argument;
    QPixmap* allBars=get(_WA_SKIN_VOLUME_BAR_ALL_BARS);
    QPixmap* dest=get(_WA_SKIN_VOLUME_BAR);
    int nBar=(int)((float)percent*(float)28/(float)100);
    bitBlt(dest,0,0,allBars,0,15*nBar,68,13);
    return dest;
    break;
  }    
  case _WA_SKIN_BALANCE_BAR : {
    int percent=argument;
    QPixmap* allBars=get(_WA_SKIN_VOLUME_BAR_ALL_BARS);
    QPixmap* dest=get(_WA_SKIN_BALANCE_BAR);
    if (percent < 0) {
      percent=-1*percent;
    }
      
    int nBar=(int)((float)percent*(float)28/(float)100);
    bitBlt(dest,0,0,allBars,0,15*nBar,68,13);
    return dest;
    break;
  }    
  default: {

  }
  }
  QPixmap* dest=get(_WA_SKIN_VOLUME_BAR);
  return dest;
}


void WaSkinModell::getNumber(int id,char cNumber,QPixmap* dest) {

  switch (id) {
  case _WA_SKIN_NUMBERS : {
    if (cNumber=='-') {
      QPixmap* pix=get(_WA_SKIN_NUMBERS_MINUS);
      dest->resize(pix->width(),pix->height());
      bitBlt(dest,0,0,pix);
      return;
    }
    // number
    QPixmap* pix=get(_WA_SKIN_NUMBERS);
    dest->resize(9,13);
    // empty number ?
    if (cNumber==' ') {
      bitBlt(dest,0,0,pix,10*9,0,9,13);
      return;
    }
    // ordinary number:
    int number=cNumber-'0';
    if ((number < 0) || (number > 9)) {
      return;
    }
    bitBlt(dest,0,0,pix,number*9,0,9,13);
    return;
    break;
  }
  case _WA_SKIN_TEXT: {
    getText(cNumber,dest);
    break;
  }
  
  default: {

  }
  }
  return ;
}



void WaSkinModell::getText(char zeichen,QPixmap* dest) {
  QPixmap* pix=get(_WA_SKIN_TEXT);
  dest->resize(5,6);
  if (('A' <= zeichen) && (zeichen <= 'Z')) {
    bitBlt(dest,0,0,pix,(zeichen-'A')*5,0,5,6);
    return;
  }
  if (('a' <= zeichen) && (zeichen <= 'z')) {
    bitBlt(dest,0,0,pix,(zeichen-'a')*5,0,5,6);
    return;
  }
  if (('0' <= zeichen) && (zeichen <= '9')) {
    bitBlt(dest,0,0,pix,(zeichen-'0')*5,6,5,6);
    return;
  }  
  if ('"' == zeichen) {
    bitBlt(dest,0,0,pix,27*5,0,5,6);
    return;
  }
  if ('@' == zeichen) {
    bitBlt(dest,0,0,pix,28*5,0,5,6);
    return;
  }


  if ('.' == zeichen) {
    bitBlt(dest,0,0,pix,11*5,6,5,6);
    return;
  }
  if (':' == zeichen) {
    bitBlt(dest,0,0,pix,12*5,6,5,6);
    return;
  }  
  if ('(' == zeichen) {
    bitBlt(dest,0,0,pix,13*5,6,5,6);
    return;
  }  
  if (')' == zeichen) {
    bitBlt(dest,0,0,pix,14*5,6,5,6);
    return;
  }          
  if ('-' == zeichen) {
    bitBlt(dest,0,0,pix,15*5,6,5,6);
    return;
  }          
  if ('`' == zeichen) {
    bitBlt(dest,0,0,pix,16*5,6,5,6);
    return;
  } 
  if ('!' == zeichen) {
    bitBlt(dest,0,0,pix,17*5,6,5,6);
    return;
  } 
  if ('_' == zeichen) {
    bitBlt(dest,0,0,pix,18*5,6,5,6);
    return;
  } 
  if ('+' == zeichen) {
    bitBlt(dest,0,0,pix,19*5,6,5,6);
    return;
  } 
  if ('\\' == zeichen) {
    bitBlt(dest,0,0,pix,20*5,6,5,6);
    return;
  }  
  if ('/' == zeichen) {
    bitBlt(dest,0,0,pix,21*5,6,5,6);
    return;
  }  
  if ('[' == zeichen) {
    bitBlt(dest,0,0,pix,22*5,6,5,6);
    return;
  }  
  if (']' == zeichen) {
    bitBlt(dest,0,0,pix,23*5,6,5,6);
    return;
  } 
  if ('^' == zeichen) {
    bitBlt(dest,0,0,pix,24*5,6,5,6);
    return;
  } 
  if ('&' == zeichen) {
    bitBlt(dest,0,0,pix,25*5,6,5,6);
    return;
  } 
  if ('%' == zeichen) {
    bitBlt(dest,0,0,pix,26*5,6,5,6);
    return;
  } 
  if (',' == zeichen) {
    bitBlt(dest,0,0,pix,27*5,6,5,6);
    return;
  } 
  if ('=' == zeichen) {
    bitBlt(dest,0,0,pix,28*5,6,5,6);
    return;
  } 
  if ('$' == zeichen) {
    bitBlt(dest,0,0,pix,29*5,6,5,6);
    return;
  } 
  if ('#' == zeichen) {
    bitBlt(dest,0,0,pix,30*5,6,5,6);
    return;
  } 
  if (('ä' == zeichen) || ('Ä' == zeichen)) {
    bitBlt(dest,0,0,pix,0*5,12,5,6);
    return;
  } 
  if (('ö' == zeichen) || ('Ö' == zeichen)) {
    bitBlt(dest,0,0,pix,1*5,12,5,6);
    return;
  } 
  if (('ä' == zeichen) || ('Ä' == zeichen)) {
    bitBlt(dest,0,0,pix,2*5,12,5,6);
    return;
  } 
  if ('?' == zeichen) {
    bitBlt(dest,0,0,pix,3*5,12,5,6);
    return;
  }     
  if ('*' == zeichen) {
    bitBlt(dest,0,0,pix,4*5,12,5,6);
    return;
  }   
  // default back is space char
  bitBlt(dest,0,0,pix,(10*5),12,5,6);

}

    

void WaSkinModell::setSkinDirectory(QString dir) {
  skinDir=dir;
}


QString WaSkinModell::getSkinDirectory() {
  return skinDir;
}


#include "waSkinModell.moc"
