// userinterface.h
//
// Copyright (C) 2000, 2001 Neil Stevens <multivac@fcmail.com>
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

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <plugin.h>
#include <noatunapp.h>
#include <kmainwindow.h>

class KAction;
class KPopupMenu;
class KStatusBar;
class KToggleAction;
class Player;
class QHBox;
class QLCDNumber;
class QSlider;
class L33tSlider;

/**
 * @short Main window class
 * @author Neil Stevens <multivac@fcmail.com>
 * @author Charles Samuels <charles@kde.org>
 */
class Excellent : public KMainWindow, public UserInterface
{
Q_OBJECT
NOATUNPLUGIND
public:
	Excellent();
	virtual ~Excellent();
	void load(const QString& url);

protected:
	virtual void closeEvent(QCloseEvent *);
	virtual void showEvent(QShowEvent *);
	virtual void hideEvent(QHideEvent *);
	virtual void dragEnterEvent(QDragEnterEvent *);
	virtual void dropEvent(QDropEvent *);

protected:
	QSlider *seeker() const { return mSeeker; }

public slots:
	void slotPlaying();
	void slotStopped();
	void slotPaused();

	void slotTimeout();
	void sliderMoved(int seconds);
	void changeLoopType(int t);
	void skipToWrapper(int second);

signals:
	void skipTo( int ); // emitted by skipToWrapper()

private slots:
	void showToolbar(void);
	void showVolumeControl(void);
	void showMenubar(void);
	void changeStatusbar(const QString& text);
	void changeCaption(const QString& text);
	void changeVolume(int);

	void handleLengthString(const QString &text);

private:
	void growVolumeControl(void);
	void shrinkVolumeControl(void);
	
	QHBox *mainFrame;

	KAction *mLoop;
	KToggleAction *mToolbar, *mVolume, *mMenubar;
	L33tSlider *mVolumeControl;
	QSlider *mSeeker;
	QLabel *mName;
	QLCDNumber *mElapsed, *mTotal;
	KPopupMenu *mTrayMenu;
};

#endif
