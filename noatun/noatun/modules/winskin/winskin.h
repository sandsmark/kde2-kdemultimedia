/*
 * nwaPlugin.h
 *
 * This file is distributed under the terms of the Artistic License
 *
 * Copyright (C) 1999 Charles Samuels <charles@kde.org>
 * Copyright (C) 2000 Neil Stevens <multivac@fcmail.com>
 */

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <plugin.h>
#include <noatunapp.h>
#include <kmainwindow.h>

#include "waSkin.h"
#include "player.h"

#define _STATUS_PLAYING 1
#define _STATUS_STOPPED 2
#define _STATUS_PAUSED  3

class	KPopupMenu;

class winskin : public  WaSkin, public UserInterface
{
	Q_OBJECT
	NOATUNPLUGIND
public:
	winskin();
	virtual ~winskin();
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
        virtual void mouseMoveEvent(QMouseEvent*);
	void showContextMenu(const QPoint &);
	void showContextMenu();
        GuiSpectrumAnalyser *gsa;

public slots:
	void fileOpen();
	void quit();
	void preferences();
	void effects();
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);	
	void repeatClickedEvent(int val);
	void shuffleClickedEvent(int val);
	void playlistClickedEvent(int val);
	
	void playCurrentEvent();
	void shadeEvent();
	void playNextEvent();
	void playPrevEvent();
	void playStopEvent();
	void playPauseEvent();
	void ejectEvent();
	void jump(int second);
	void jumpSliderPressed();
	void jumpSliderReleased();
	void jumpValueChanged(int val);
	void balanceSetValue(int val);
	void volumeSetValue(int val);
	
	void slotPlaying();
	void slotStopped();
	void slotPaused();
	void timetick();
	void loopChange(int loopType);
	void playlistShown();
	void playlistHidden();
	void newSongLen(int min, int sec);
	void newSong();
	void songModified();
        void configureDialog();
private:
	int		mLength;
	winskin		*mWaSkin;
	bool		mJumpPressed;
	bool		i_b_move;
	QPoint		i_point_lastPos;
	QPoint		i_point_dragStart;
//	KPopupMenu 	*mContextMenu; Also away

};

#endif
