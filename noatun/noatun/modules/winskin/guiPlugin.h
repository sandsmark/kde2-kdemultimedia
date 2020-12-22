/*
  here we define the signals a playerplugin can define
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */





#ifndef __GUIPLUGIN_H
#define __GUIPLUGIN_H

#include <qwidget.h>

/**
   This is the plugin standard for new GUIs of kmpg(anyone
   do a "panel-docked-gui?"). We have a few standard signals 
   which the Plugin can emit.
   But the gui as well needs input which it should show
   on the screen as well!
   The guiPlugin gets these necessary runtime informations like
   currentTime,status of playing (paused,stopped,...) from
   the standardgraph.
   <p>
   How things work:
   <p>
   You build your own gui in the constructor, like playbuttons,
   jumpsliders, etc..
   Then in the construction process sometimes the method
   attachTo(..) is called. This is the time for the plugin
   to grap all these nice signals which are important for your gui.
   <p>
   The detach call removes the connection to the stream.
   <p>
   It is very usefull to use the underlying widgets for time,state etc,
   and only overwrite the "look and feel" of it, because these devices
   already know how to attach/detach. The KDE skin, and
   the winamp skin uses them and I think there is no
   reason not use this "basicWidgetSet", but you can write, nevertheless,
   your own.
*/
class GuiPlugin : public QWidget {
 Q_OBJECT

  public:
   GuiPlugin(QWidget *parent=0, const char *name=0);
   ~GuiPlugin();

   virtual int  getSkinId()=0;

   // here you get the current song
   virtual void setSong(QString song)=0;
   virtual void setRepeat(int val)=0;
   virtual void setShuffle(int val)=0;
   virtual void setPlaylist(int val)=0;
   virtual void setBPS(int val)=0;
   virtual void setFreq(int val)=0;

   virtual void setJumpValue(int val)=0;
   virtual void setVolumeValue(int val)=0;
   virtual void setBalanceValue(int val)=0;
   
   virtual void setStatus(int val)=0;
   virtual void setStereo(int val)=0;
   virtual void setTime(QString timeString)=0;
   virtual void setSpectrum(float floatArray[75])=0;

  
 signals:
   void repeatClickedEvent(int val);
   void shuffleClickedEvent(int val);
   void playlistClickedEvent(int val);

   void playCurrentEvent();
   void playNextEvent();
   void playPrevEvent();
   void playStopEvent();
   void playPauseEvent();
   void ejectEvent();

   // seek bar
   void jump(int second);
   void jumpSliderPressed(); 
   void jumpSliderReleased();

   // balance
   void balanceSetValue(int val);
   // volume
   void volumeSetValue(int val);

};
#endif




