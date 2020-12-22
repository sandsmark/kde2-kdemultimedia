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

#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <plugin.h>
#include <noatunapp.h>
#include <kmainwindow.h>

class KAction;
class KPopupMenu;
class KToggleAction;
class KitSystemTray;
class Player;

class NoatunSystray : public KMainWindow, public Plugin
{
Q_OBJECT
public:
	enum Blink { BlinkIcon = 0, ShowIcon = 1, Neither = 2 };
	NoatunSystray();
	virtual ~NoatunSystray();
	void load(const QString& url);
	virtual void init(void);

	void setBlink(Blink);
	void setTip(bool);

	static NoatunSystray *jasonkb;

protected:
	virtual void closeEvent(QCloseEvent *);

public slots:
	void slotPlaying();
	void slotStopped();
	void slotPaused();

	void changeLoopType(int t);

private slots:
	void changeTray(const QPixmap &);

	void blinkTimer(void);

private:
	QPixmap *trayDefault, *trayStatus;
	bool showingTrayStatus;
	Blink blink;
	KitSystemTray *mTray;

	void setTipText(QString);
	bool tip;
	QString tipText;

	KAction *mBack, *mStop, *mPlay, *mForward, *mLoop;
	KToggleAction *mPlaylist;
	KPopupMenu *menu;
	KXMLGUIFactory *factory;
};

#endif
