// systray.h
//
// Copyright (C) 2000 Neil Stevens <multivac@fcmail.com>
// Copyright (C) 1999 Charles Samuels <charles@kde.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
// Except as contained in this notice, the name(s) of the author(s) shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from the author(s).

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <effects.h>
#include <noatunapp.h>
#include "noatunpref.h"
#include "player.h"
#include "systray.h"
#include "kitsystemtray.h"
#include "noatunstdaction.h"
#include "cmodule.h"

#include <kaction.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpixmapeffect.h>
#include <kpopupmenu.h>
#include <kstdaction.h>
#include <qbitmap.h>
#include <qtimer.h>
#include <qtooltip.h>

NoatunSystray *NoatunSystray::jasonkb = 0;

NoatunSystray::NoatunSystray()
	: KMainWindow()
	, Plugin()
{
	hide();
	
	KStdAction::quit(napp, SLOT(quit()), actionCollection());
	KStdAction::open(napp, SLOT(fileOpen()), actionCollection());
	KStdAction::preferences(napp, SLOT(preferences()), actionCollection());
	mBack = NoatunStdAction::back(actionCollection(), "back");
	mStop = NoatunStdAction::stop(actionCollection(), "stop");
	mPlay = NoatunStdAction::playpause(actionCollection(), "play");
	mForward = NoatunStdAction::forward(actionCollection(), "forward");
	mPlaylist = NoatunStdAction::playlist(actionCollection(), "show_playlist");
	mLoop = new KAction(i18n("L&oop Style"), 0,napp->player(), SLOT(loop()), actionCollection(), "loop_style");
	NoatunStdAction::effects(actionCollection(), "effects");
	NoatunStdAction::equalizer(actionCollection(), "equalizer");

	createGUI("systrayui.rc");

	mTray = new KitSystemTray("tray", this);
	mTray->show();
	trayDefault = new QPixmap(SmallIcon("noatuntray"));
	trayStatus = new QPixmap(SmallIcon("noatuntraystop"));
	mTray->changeTitle(*trayDefault, "Noatun");
	showingTrayStatus = false;

	QTimer *t = new QTimer(this);
	connect(t, SIGNAL(timeout(void)), this, SLOT(blinkTimer(void)) );
	t->start(1000);

	connect(napp->player(), SIGNAL(playing()), this, SLOT(slotPlaying()));
	connect(napp->player(), SIGNAL(stopped()), this, SLOT(slotStopped()));
	connect(napp->player(), SIGNAL(paused()), this, SLOT(slotPaused()));
	connect(napp->player(), SIGNAL(loopTypeChange(int)), this, SLOT(changeLoopType(int)));
	napp->player()->handleButtons();

	changeLoopType(Player::None);

	jasonkb = this;
}

NoatunSystray::~NoatunSystray()
{
	delete trayStatus;
	delete trayDefault;
	napp->player()->showInterfaces();
}

void NoatunSystray::init(void)
{
	YHModule *module = new YHModule(this);
	blinkTimer();
}

void NoatunSystray::setBlink(Blink b)
{
	blink = b;
	blinkTimer();
}

void NoatunSystray::setTip(bool b)
{
	tip = b;
	if(tip)
		QToolTip::add(mTray, tipText);
	else
		QToolTip::remove(mTray);
}

void NoatunSystray::closeEvent(QCloseEvent*)
{
	unload();
}

void NoatunSystray::slotPlaying()
{
	mStop->setEnabled(true);
	changeTray(SmallIcon("noatuntrayplay"));
	setTipText(napp->player()->current()->title());
}

void NoatunSystray::slotStopped()
{
	if(!napp->player()->current()) return;
	changeTray(SmallIcon("noatuntraystop"));
	setTipText(i18n("Stopped"));
}

void NoatunSystray::slotPaused()
{
	changeTray(SmallIcon("noatuntraypause"));
	setTipText(i18n("%1 - Paused").arg(napp->player()->current()->title()));
}

void NoatunSystray::changeLoopType(int t)
{
	switch (t)
	{
	case Player::None:
		mLoop->setIconSet(QIconSet(SmallIcon("nloopnone")));
		mLoop->setText(i18n("No looping"));
		break;
	case Player::Song:
		mLoop->setIconSet(QIconSet(SmallIcon("nloopsong")));
		mLoop->setText(i18n("Song looping"));
		break;
	case Player::Playlist:
		mLoop->setIconSet(QIconSet(SmallIcon("nloopplaylist")));
		mLoop->setText(i18n("Playlist looping"));
	}
}

void NoatunSystray::changeTray(const QPixmap &pm)
{
	delete trayStatus;
	trayStatus = new QPixmap(pm);
	if(showingTrayStatus) blinkTimer();
}

void NoatunSystray::blinkTimer(void)
{
	switch((int)blink)
	{
	case 0:
		showingTrayStatus ^= true;
		break;
	case 1:
		showingTrayStatus = true;
		break;
	case 2:
		showingTrayStatus = false;
		break;
	}

	if(showingTrayStatus)
		mTray->setPixmap(*trayStatus);
	else
		mTray->setPixmap(*trayDefault);
}

void NoatunSystray::setTipText(QString text)
{
	tipText = text;
	if(tip) QToolTip::add(mTray, tipText);
}

#include "systray.moc"
